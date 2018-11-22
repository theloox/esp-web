/*
 * i2c.c
 *
 *  Created on: Nov 21, 2018
 *      Author: root
 */


#include <esp_system.h>
#include <esp_err.h>
#include "esp_log.h"

#include "driver/i2c.h"

#include   "xi2c.h"


#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */

static gpio_num_t i2c_gpio_sda = 18;
static gpio_num_t i2c_gpio_scl = 19;
static uint32_t i2c_frequency = 100000;
static i2c_port_t i2c_port = I2C_NUM_0;


static esp_err_t i2c_master_driver_initialize()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_gpio_sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = i2c_gpio_scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_frequency
    };
    return i2c_param_config(i2c_port, &conf);
}


esp_err_t xi2c_detect(char *response)
{
    i2c_master_driver_initialize();
    i2c_driver_install(i2c_port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    uint8_t address;

    int offs = 0;
    esp_err_t ret;
    uint8_t data;
    uint8_t chip = 1;
    for (int i = 0; i < 128; i ++) {
		fflush(stdout);
		address = i ;
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();

		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
		i2c_master_stop(cmd);
		ret = i2c_master_cmd_begin(i2c_port, cmd, 50 / portTICK_RATE_MS);

		if (ret == ESP_OK) {

			// get register WHO_AM_I_A 0x0f
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (chip << 1) | WRITE_BIT, ACK_CHECK_EN);
			i2c_master_write_byte(cmd, 0x0f, ACK_CHECK_EN);
			i2c_master_start(cmd);

			 i2c_master_write_byte(cmd, (chip << 1) | READ_BIT, ACK_CHECK_EN);
			 i2c_master_read_byte(cmd, &data, NACK_VAL);
			 i2c_master_stop(cmd);
			 ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
			if (ret == ESP_OK && data != 0) {
				offs += snprintf(response + offs, 512 - offs, "port: <a href=\"/?id=1\">%d</a> type:%s \n\n", i, "Acc") - 1;
			}

			// get register WHO_AM_I_M 0x4f
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (chip << 1) | WRITE_BIT, ACK_CHECK_EN);
			i2c_master_write_byte(cmd, 0x4f, ACK_CHECK_EN);
			i2c_master_start(cmd);

			 i2c_master_write_byte(cmd, (chip << 1) | READ_BIT, ACK_CHECK_EN);
			 i2c_master_read_byte(cmd, &data, NACK_VAL);
			 i2c_master_stop(cmd);
			 ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
			if (ret == ESP_OK && data != 0) {
				offs += snprintf(response + offs, 512 - offs, "port: <a href=\"/?id=1\">%d</a> type: %s\n\n", i, "Mag") - 1;
			}
		}

		i2c_cmd_link_delete(cmd);

    }

    i2c_driver_delete(i2c_port);

    if (offs == 0)
    {
    	return (1);
    }

    return 0;
}


#define RES_LEN 4096

esp_err_t xi2c_info(char *response, int id)
{

    uint8_t address = id;
    uint8_t chip = 1;

	int offs = 0;
	int len = 16;
	uint8_t data[32];

	i2c_master_driver_initialize();
	i2c_driver_install(i2c_port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, chip, ACK_CHECK_EN);
	i2c_master_start(cmd);

    i2c_master_write_byte(cmd, chip << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, len - 1, ACK_VAL);

    i2c_master_read_byte(cmd, data + len - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK) {
        for (int i = 0; i < len; i++) {
            offs += snprintf(response + offs, RES_LEN - offs, "0x%02x ", data[i] - 1);
            if ((i + 1) % 16 == 0) {
            	offs += snprintf(response + offs, RES_LEN - offs, "\r\n") - 1;
            }
        }
        if (len % 16) {
        	offs += snprintf(response + offs, RES_LEN - offs, "\r\n") - 1;
        }
    }
    else
    {
    	offs += snprintf(response + offs, RES_LEN - offs, "not found\r\n") - 1;
    }

    i2c_driver_delete(i2c_port);
    return 0;
}

esp_err_t xi2c_test(char *response)
{
	char res[1024];
	esp_err_t ret;

	ret= xi2c_detect(res);

	return (ret);
}
