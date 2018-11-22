/*
 * xuart.c
 *
 *  Created on: Nov 22, 2018
 *      Author: root
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"


/**
 * @brief dummy to readinfo from serial and discardit
  * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

// 4 for the lsm303
#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

/**
 * @brief inits the uart
*/
void xuart_init()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

}

void xuart_loop_()
{
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Wait for more data
    }
}

/**
 * @brief main loop for reading data from uart and discard it
*/
void xuart_loop()
{
    xTaskCreate(xuart_loop_, "xuart_loop", 1024, NULL, 10, NULL);
}

/**
 * @brief writes a null terminated string to the uart
*/
esp_err_t xuart_writestr(char *str)
{
	int len;

	len = strlen(str);

	uart_write_bytes(UART_NUM_1, (const char *) str, len);

	return (0);
}



int xuart_echoint(int n)
{
	uint8_t buf[16];
	int len;

	xuart_init();

	len = snprintf((char *)buf, sizeof (buf), "%d", n);

	// Read data from the UART
    len = uart_read_bytes(UART_NUM_1, buf, len , 20 / portTICK_RATE_MS);
    // Write data back to the UART
    uart_write_bytes(UART_NUM_1, (const char *) buf, len);

    len = atoi((const char *)buf);

    return (len);
}
