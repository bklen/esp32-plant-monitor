# ESP32 Plant Monitor
ESP32 Plant monitoring system that publishes sensor data to MQTT.

This custom plant monitor utilizes the ESP32 running FreeRTOS to connect to WI-FI and publish temperature, humidity, and soil moisture levels to an MQTT broker.

v1.0  
Created: on August 6 2023  
Author: bklen  
![image](https://github.com/bklen/esp32-plant-monitor/assets/6707864/2e4c311b-9e9d-4f68-a219-b0023e522d44)

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

## Step 1: Wire Prototype
![moisture_sensor_dry_calibration](https://github.com/bklen/esp32-plant-monitor/assets/6707864/0384db8d-674f-4a84-a8d5-9855474cc3c2)
  1. Connect the analog output from the soil moisture sensor to pin 33(ADC1 CH5)
  2. Connect the data out from the DHT22 to GPIO 32

Note ([From the ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html))
- "ADC2 is used by the Wi-Fi driver. Therefore the application can only use ADC2 when the Wi-Fi driver has not started."

## Step 2: Flash the code + Calibrate Soil Moisture Sensor
  1. In the app_main() function in main.c, change the calibration boolean to true.  
    ![image](https://github.com/bklen/esp32-plant-monitor/assets/6707864/c2ff63b4-f97e-4e6d-b574-f7b62790fbaa)
  2. Flash the code onto the esp32 "idf.py flash monitor"
  3. Follow the steps printed to the ESP-IDF Terminal and edit the program variables in moisture_sensor.h  
     ### Dry
     ![image](https://github.com/bklen/esp32-plant-monitor/assets/6707864/fdf77546-e18a-4000-94de-56f6b72e100e)
     ### Wet
     ![image](https://github.com/bklen/esp32-plant-monitor/assets/6707864/74f3e667-6074-45e8-8d78-05b55aabfc30)
  4. Set the calibration boolean back to false
  5. Set the [project config](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html) parameters in Kconfig.projbuild
     * Wi-Fi SSID + Password
     * MQTT URI + Username and Password(if applicable)
  6. Flash the code onto the esp32
  7. Verify sensor data gets published to MQTT broker  
     ![image](https://github.com/bklen/esp32-plant-monitor/assets/6707864/3fbd3a8a-0111-409d-8505-5945d8ccf357)

## Step 3: (Optional) Integrate Plant Monitor with Home Assistant
  1. Edit Home Assitant configuration.yaml
     ```
     mqtt:
       sensor:
         - name: "Brandon Desk Plant Temperature"
           state_topic: "sensor/plantmonitor"
           icon: mdi:thermometer
           unit_of_measurement: "°F"
           value_template: "{{ value_json.temperature | float | round(1)}}"
         - name: "Brandon Desk Plant Moisture"
           state_topic: "sensor/plantmonitor"
           icon: mdi:water
           unit_of_measurement: "%"
           value_template: "{{ value_json.moisture }}"
         - name: "Brandon Desk Plant Humidity"
           state_topic: "sensor/plantmonitor"
           icon: mdi:water-percent
           unit_of_measurement: "%"
           value_template: "{{ value_json.humidity | float | round()}}"
     ```
  2. Edit Dashboard to add sensor data
     ```
     type: entities
     entities:
       - entity: sensor.brandon_desk_plant_moisture
         name: Plant Soil Moisture
       - entity: sensor.brandon_desk_plant_temperature
         name: Plant Temperature
       - entity: sensor.brandon_desk_plant_humidity
         name: Plant Humidity
     title: Brandon's Desk
     ```
     ![image](https://github.com/bklen/esp32-plant-monitor/assets/6707864/c275f738-0ce4-46a3-b3b6-4571bfce42e9)




