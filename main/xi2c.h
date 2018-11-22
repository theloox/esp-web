/*
 * i2c.h
 *
 *  Created on: Nov 21, 2018
 *      Author: root
 */

#ifndef MAIN_XI2C_H_
#define MAIN_XI2C_H_

#include <esp_system.h>
#include <esp_err.h>


esp_err_t xi2c_detect(char *response);
esp_err_t xi2c_info(char *response, int id);



#endif /* MAIN_XI2C_H_ */
