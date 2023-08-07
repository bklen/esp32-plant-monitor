/* 
	DHT22 temperature sensor driver
*/

#ifndef DHT22_H_
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =====================================

void 	    setDHTgpio(uint32_t gpio);
void 	    errorHandler(int32_t response);
int32_t 	readDHT();
float 	    getHumidity();
float 	    getTemperature();
float 	    getTemperatureF();
int32_t 	getSignalLevel(int32_t usTimeOut, bool state );

#endif