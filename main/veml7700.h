/**
 * VEML7700 Light Sensor driver for integration with ESP-IDF framework.
 * 
 * @copyright:
 * Copyright (c) 2022, Kristijan Grozdanovski
 * All rights reserved.
 * 
 * @license:
 * BSD-3-Clause
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef veml7700_H_
#define veml7700_H_

#include "esp_err.h"

#define VEML7700_I2C_ADDR UINT8_C(0x10) /*!< Sensor slave I2C address */
#define LUX_FC_COEFFICIENT 0.092903     /*!< Multiplier coefficient for lux-fc conversion */
#define FC_LUX_COEFFICIENT 10.7639      /*!< Multiplier coefficient for fc-lux conversion */

/**
 * @brief Represents sensor and I2C device configuration.
 * 
 * @note Can contain calculated values for active settings such
 * as 'resolution' and 'maximum lux'.
 * 
 */
struct veml7700_config {
	uint16_t gain;				/*!< Sensor gain configuration */
	uint16_t integration_time;	/*!< Sensor integration time configuration */
	uint16_t persistance;		/*!< Last result persistance on-sensor configuration */
	uint16_t interrupt_enable;	/*!< Enable/disable interrupts */
	uint16_t shutdown;			/*!< Shutdown command configuration */
    float resolution;			/*!< Current resolution and multiplier */
    uint32_t maximum_lux;		/*!< Current maximum lux limit */
};

/**
 * @brief Opaque handle for one veml7700 device
 */
typedef struct veml7700_privdata_t veml7700_privdata_t;
typedef struct veml7700_privdata_t *veml7700_handle_t;

void 	set_i2c_gpio(uint32_t gpio_SDA, uint32_t gpi_SCL);
double 	get_lux_als();
double 	get_lux_white();
double 	get_fc_als();
double 	get_fc_white();
void 	read_veml7700();

/**
 * @brief Configure the ESP host as an I2C master device.
 * 
 */
void i2c_master_setup(void);

/**
 * @brief Initialize the sensor by configuring it with default settings.
 * 
 * @attention This function should be called only once and before any other
 * functions from this group.
 *
 * @param dev Pointer to a variable holding the device handle
 * @param i2c_master_num I2C port used by the master device
 * 
 * @return esp_err_t 
 */
esp_err_t veml7700_initialize(veml7700_handle_t *dev, int i2c_master_num);


/**
 * @brief Release any sensor data, freeing up the memory taken by veml7700_initialize.
 *
 * Call after you're done using the sensor, if applicable to your application.
 *
 * @param dev Device handle to free
 */
void veml7700_release(veml7700_handle_t dev);


/**
 * @brief Write the sensor configuration to the device.
 * 
 * @param configuration The configuration to be written.
 * 
 * @return esp_err_t 
 */
esp_err_t veml7700_set_config(veml7700_handle_t dev, struct veml7700_config *configuration);

/**
 * @brief Read the ALS data once.
 * 
 * @param dev Device handle to use
 * @param lux The ALS read result in lux.
 * 
 * @return esp_err_t 
 */
esp_err_t veml7700_read_als_lux(veml7700_handle_t dev, double* lux);

/**
 * @brief Read the ALS data once. Optimize resolution if neccessary.
 * 
 * @attention This function alters the sensor configuration as it sees fit.
 * 
 * @param dev Device handle to use
 * @param lux The ALS read result in lux.
 * 
 * @return esp_err_t 
 */
esp_err_t veml7700_read_als_lux_auto(veml7700_handle_t dev, double* lux);

/**
 * @brief Read the White light data once.
 * 
 * @param dev Device handle to use
 * @param lux The White light read result in lux.
 * 
 * @return esp_err_t 
 */
esp_err_t veml7700_read_white_lux(veml7700_handle_t dev, double* lux);

/**
 * @brief Read the White light data once. Optimize resolution if neccessary.
 * 
 * @attention This function alters the sensor configuration as it sees fit.
 * 
 * @param dev Device handle to use
 * @param lux The White light read result in lux.
 * 
 * @return esp_err_t 
 */
esp_err_t veml7700_read_white_lux_auto(veml7700_handle_t dev, double* lux);

/**
 * @brief Read the currently configured and used sensor resolution.
 * 
 * @param dev Device handle to use
 * @return float The resolution value.
 */
float veml7700_get_resolution(veml7700_handle_t dev);

#endif