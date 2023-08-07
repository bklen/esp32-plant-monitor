# ESP32 Plant Monitor
ESP32 Plant monitoring system that publishes sensor data to MQTT.

This custom plant monitor utilizes the ESP32 running FreeRTOS to connect to WI-FI and publish temperature, humidity, and soil moisture levels to an MQTT broker.

v1.0  
Created: on August 6 2023  
Author: bklen  

## Dependencies
MQTT broker
  * [Publicly available MQTT server/broker](https://test.mosquitto.org/)
  * [Private MQTT broker](https://github.com/home-assistant/addons/blob/master/mosquitto/DOCS.md) (Home Assistant integration)
    * https://www.home-assistant.io/integrations/mqtt/

Optional Tools
  * [mqttx](https://mqttx.app/)
    * MQTT client used for testing
  * [Studio Code Server](https://github.com/hassio-addons/addon-vscode) (Home Assistant integration)
    * Allows you to edit your Home Assistant configuration directly from your web browser

## Parts List
### Parts
  * 1x [ESP32](https://a.co/d/2KYmtGQ)
     * [Pinout](https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-30-GPIOs-Copy.png?quality=100&strip=all&ssl=1)
  * 1x [DHT22/AM2302 Digital Temperature And Humidity Sensor](https://a.co/d/7EfM90X)
  * 1x [Capacitive Soil Moisture Sensor](https://a.co/d/dryRND9)

## Step 1: Wire Prototype + Calibrate Soil Moisture Sensor
![moisture_sensor_dry_calibration](https://github.com/bklen/esp32-plant-monitor/assets/6707864/0384db8d-674f-4a84-a8d5-9855474cc3c2)
  1. Connect the analog output from the soil moisture sensor to pin 33(ADC1 CH5)
  2. Connect the data out from the DHT22 to GPIO 32

Note ([From the ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html))
- "ADC2 is used by the Wi-Fi driver. Therefore the application can only use ADC2 when the Wi-Fi driver has not started."
