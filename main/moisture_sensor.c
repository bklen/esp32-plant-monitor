/*
 * Capacitive soil moisture sensor driver for ESP32
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/adc.h"

#include "moisture_sensor.h"

// global defines
uint32_t moistureSensorADC = ADC1_CHANNEL_5;    // default moisture sensor ADC1 CH5, (pin 33)
int32_t moistureLevel = 0;


// set the adc for the moisture sensor, use ADC1
void setMoistureSensorADC(uint32_t adc)
{
	moistureSensorADC = adc;
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(moistureSensorADC, ADC_ATTEN_DB_11);
}

// get moisture percentage
int32_t getMoistureLevel() { return moistureLevel; }

// re-maps a number from one range to another
int32_t translate(double input, double input_end, double input_start, double output_start, double output_end)
{
    double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
    double output = output_start + slope * (input - input_start);
    return output;
}

// reads moisture level from sensor and returns it as a percentage from 0 - 100
void readMoistureSensor()
{
    moistureLevel = adc1_get_raw(ADC1_CHANNEL_5);
    moistureLevel = translate(moistureLevel, MOISTURE_SENSOR_CALLIBRATION_MIN, MOISTURE_SENSOR_CALLIBRATION_MAX, 0, 100);
}

void calibrate_soil_moisture_sensor()
{
    esp_log_level_set("*", ESP_LOG_NONE);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); // Pin 33   

    printf("....................................\n");
    printf("CALLIBRATION STEPS\n");
    printf("1 Totally dry sensor\n");
    printf("2 Note down max value\n");
    printf("3 Put the sensor in water\n");
    printf("4 Reset the device\n");
    printf("5 Note down min value\n");
    printf("6 Modify the program variables in moisture_sensor.h\n");
    printf("  MOISTURE_SENSOR_CALLIBRATION_MAX\n");
    printf("  MOISTURE_SENSOR_CALLIBRATION_MIN\n");
    printf("....................................\n");
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    int32_t val;
    while (true)
    {
        val = adc1_get_raw(ADC1_CHANNEL_5);
        printf("raw value is %d\n", val);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

    }
}