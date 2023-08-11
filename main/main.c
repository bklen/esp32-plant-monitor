#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "mqtt_client.h"

#include "connect.h"
#include "veml7700.h"
#include "DHT22.h"
#include "moisture_sensor.h"

#define TAG "MQTT"
#define URI CONFIG_MQTT_URI
#define USERNAME CONFIG_MQTT_USERNAME
#define PASSWORD CONFIG_MQTT_PASSWORD
#define READING_QUEUE_SIZE 300
#define SENSOR_READ_DELAY 20000

xQueueHandle readingQueue;
TaskHandle_t taskHandle;

const uint32_t WIFI_CONNECTED = BIT1;
const uint32_t MQTT_CONNECTED = BIT2;
const uint32_t MQTT_PUBLISHED = BIT3;

void mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xTaskNotify(taskHandle, MQTT_CONNECTED, eSetValueWithOverwrite);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            xTaskNotify(taskHandle, MQTT_PUBLISHED, eSetValueWithOverwrite);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    mqtt_event_handler_cb(event_data);
}

void MQTTLogic(char* sensorReading)
{
    char data[READING_QUEUE_SIZE];
    uint32_t command = 0;
    esp_mqtt_client_config_t mqttConfig = {
        .uri = URI,
        .username = USERNAME,
        .password = PASSWORD};
    esp_mqtt_client_handle_t client = NULL;

  while (true)
  {
        xTaskNotifyWait(0, 0, &command, portMAX_DELAY);
        switch (command)
        {
            case WIFI_CONNECTED:
                client = esp_mqtt_client_init(&mqttConfig);
                esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
                esp_mqtt_client_start(client);
                break;

            case MQTT_CONNECTED:
                sprintf(data, "%s", sensorReading);
                //printf("sending data: %s", data);
                esp_mqtt_client_publish(client, "sensor/plantmonitor", data, strlen(data), 2, true);
                break;

            case MQTT_PUBLISHED:
                esp_mqtt_client_stop(client);
                esp_mqtt_client_destroy(client);
                esp_wifi_stop();
                return;

            default:
                break;
        }
    }
}

void OnConnected(void *params)
{
    while (true)
    {
        char sensorReading[READING_QUEUE_SIZE];
        if (xQueueReceive(readingQueue, &sensorReading, portMAX_DELAY))
        {
            ESP_ERROR_CHECK(esp_wifi_start());
            MQTTLogic(sensorReading);
        }
    }
}

void sensor_reader_task(void *params)
{
    // Initialize I2C for veml7700 sensor
    set_i2c_gpio(GPIO_NUM_21, GPIO_NUM_22);
    i2c_master_setup();

    setMoistureSensorADC(ADC1_CHANNEL_5);   // (GPIO33)
    setDHTgpio(GPIO_NUM_32);

    // initial delay for sensors to normalize
    vTaskDelay(20000 / portTICK_PERIOD_MS);
    
    while (true)
    {
		cJSON *root;
        root = cJSON_CreateObject();

        // read ambient light levels and add to JSON
        read_veml7700();
        cJSON_AddNumberToObject(root, "lux_als", get_lux_als());
        cJSON_AddNumberToObject(root, "fc_als", get_fc_als());
        cJSON_AddNumberToObject(root, "lux_white", get_lux_white());
        cJSON_AddNumberToObject(root, "fc_white", get_fc_white());
        
        // read moisture level and add to JSON
        readMoistureSensor();
        cJSON_AddNumberToObject(root, "moisture", getMoistureLevel());

        // read temperature + humidity and add to JSON
        uint32_t ret = readDHT();
        errorHandler(ret);

        
        cJSON_AddNumberToObject(root, "temperature", getTemperatureF());
        cJSON_AddNumberToObject(root, "humidity", getHumidity());

        char my_json_string[READING_QUEUE_SIZE];
        sprintf(my_json_string, "%s", cJSON_Print(root));
        xQueueSend(readingQueue, &my_json_string, 2000 / portTICK_PERIOD_MS);
        cJSON_Delete(root);

        vTaskDelay(SENSOR_READ_DELAY / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    bool calibration = false;

    if (calibration)
    {
        calibrate_soil_moisture_sensor();
    }
    else
    {
        readingQueue = xQueueCreate(sizeof(char), READING_QUEUE_SIZE);

        wifiInit();

        xTaskCreate(OnConnected, "OnConnected", 1024 * 5, NULL, 5, &taskHandle);
        xTaskCreate(sensor_reader_task, "sensor_reader_task", 1024 * 5, NULL, 5, NULL);
    }  
}