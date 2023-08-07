/* 
	Capacitive soil moisture sensor driver
*/

#ifndef MOISTURE_SENSOR_H_
#define MOISTURE_SENSOR_H_

#define MOISTURE_SENSOR_CALLIBRATION_MAX 2800
#define MOISTURE_SENSOR_CALLIBRATION_MIN 1010

// == function prototypes ========================
void 	    setMoistureSensorADC(uint32_t adc);
void        readMoistureSensor();
int32_t     getMoistureLevel();
void        calibrate_soil_moisture_sensor();

#endif