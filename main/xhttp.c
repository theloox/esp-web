/*
 * i2c.c
 *
 *  Created on: Nov 21, 2018
 *      Author: root
 */

#include <string.h>

#include <esp_system.h>
#include <esp_err.h>

#include "xhttp.h"
#include "xi2c.h"
#include "xuart.h"


#define XHTTP_HEADER "\
<!doctype html>\n\
<html lang=\"en\">\n\
<head>\n\
  <meta charset=\"utf-8\">\n\
  <title>i2c test</title>\n\
  </head>\n\
<body>\
<pre>\n"

#define XHTTP_FOOTER "\
</pre>\
</body>\
</html>"


/**
 * @brief Default handler to list sensors
 *
 * Only returns to http
 */
esp_err_t xhttp_default(char *response)
{
	char res[2048];

	xi2c_detect(res);

	snprintf(response, 4096, "%s%s%s", XHTTP_HEADER, res, XHTTP_FOOTER);

	return(ESP_OK);
}

/** Handler to show info of a sensor and sends back to serial
 *
 * Sends back to http
 * Sends back to serial
 */
esp_err_t xhttp_info(char *response, int id)
{
	char res[2048];

	xi2c_info(res, id);

	snprintf(response, 4096, "%s%s%s", XHTTP_HEADER, res, XHTTP_FOOTER);

    xuart_writestr(res);

	return(ESP_OK);
}
