/*
 * xuart.h
 *
 *  Created on: Nov 22, 2018
 *      Author: root
 */

#ifndef MAIN_XUART_H_
#define MAIN_XUART_H_


void xuart_init();
void xuart_loop();
esp_err_t xuart_writestr();

int xuart_echoint(int n);

#endif /* MAIN_XUART_H_ */
