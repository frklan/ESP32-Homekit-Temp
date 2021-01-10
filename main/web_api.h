/*
 *  Copyright 2020-2021 Fredrik Andersson <fredrik.k.andersson@gmail.com>
 *
 *  This work is licensed under the CC BY-NC-SA 4.0 International License.
 * 
 */

#ifndef WEBAPIH
#define WEBAPIH

#include <esp_err.h>

/**
 * Starts and inits the API web server
 * 
 * @return ESP_OK on success
 * @return ESP_FAIL on failure
 */
esp_err_t start_api_server();

#endif
