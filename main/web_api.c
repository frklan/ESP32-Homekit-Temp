/*
 *  Copyright 2020-2021 Fredrik Andersson <fredrik.k.andersson@gmail.com>
 *
 *  This work is licensed under the CC BY-NC-SA 4.0 International License.
 * 
 */

#include <esp_log.h>
#include <esp_http_server.h>
#include <hap_platform_httpd.h>
#include <json_generator.h>

#include <Thermistor_interop.h>

#include "web_api.h"

#define TAG "TTS WEB API"

static esp_err_t get_api_v1_get_temp_handler(httpd_req_t* req) {
  const int BUFF_SIZE = 16;
  char *outbuff = NULL;
  outbuff = calloc(BUFF_SIZE, sizeof(char));

  if(!outbuff) {
    ESP_LOGE(TAG, "Could not allocate memory");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  json_gen_str_t jstr;
  json_gen_str_start(&jstr, outbuff, BUFF_SIZE, NULL, NULL);
  int err = json_gen_start_object(&jstr);
  err += json_gen_obj_set_float(&jstr, "c", get_sensor_reading(ADC1_CHANNEL_6));
  err += json_gen_end_object(&jstr);
  json_gen_str_end(&jstr);

  if(err == 0) {
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_set_type(req, HTTPD_TYPE_JSON);
    httpd_resp_send(req, outbuff, HTTPD_RESP_USE_STRLEN);
  } else {
    httpd_resp_send_500(req);
  }
  free(outbuff);
  return ESP_OK;
}

static esp_err_t get_api_v1_get_history_handler(httpd_req_t* req) {
  const int BUFF_SIZE = 8192;
  char *outbuff = NULL;
  outbuff = calloc(BUFF_SIZE, sizeof(char));

  if(!outbuff) {
    ESP_LOGE(TAG, "Could not allocate memory");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const int MAX_HIST_LEN = 192;
  struct sensor_history_t *h = NULL;
  h = calloc(MAX_HIST_LEN, sizeof(struct sensor_history_t));
  if(h == NULL) {
    httpd_resp_send_500(req);
    free(outbuff);
    return ESP_FAIL;
  }
  
  int hist_len = get_sensor_history(ADC1_CHANNEL_6, h, MAX_HIST_LEN);
  json_gen_str_t *jstr = malloc(sizeof(json_gen_str_t));
  if(jstr == NULL) {
    httpd_resp_send_500(req);
    free(h);
    free(outbuff);
    return ESP_FAIL;
  }

  json_gen_str_start(jstr, outbuff, BUFF_SIZE, NULL, NULL);
  int err = json_gen_start_object(jstr);
  err += json_gen_push_array(jstr, "h");

  for(int i = 0; i < hist_len; i++) {
    err += json_gen_start_object(jstr);
    err += json_gen_obj_set_int(jstr, "t", h[i].time);
    err += json_gen_obj_set_float(jstr, "c", h[i].reading);
    err += json_gen_end_object(jstr);
  }

  err += json_gen_pop_array(jstr);
  err += json_gen_end_object(jstr);
  json_gen_str_end(jstr);
  
  if(err == 0) {
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_set_type(req, HTTPD_TYPE_JSON);
    httpd_resp_send(req, outbuff, HTTPD_RESP_USE_STRLEN);
  } else {
    httpd_resp_send_500(req);
  }
  
  free(jstr);
  free(h);
  free(outbuff);
  return ESP_OK;
}

static struct httpd_uri uri_api_v1_get_temp = {
  .uri = "/api/v1/temp",
  .method = HTTP_GET,
  .handler = get_api_v1_get_temp_handler,
};

static struct httpd_uri uri_api_v1_get_history = {
  .uri = "/api/v1/hist",
  .method = HTTP_GET,
  .handler = get_api_v1_get_history_handler,
};

esp_err_t start_api_server() {
  /*
    I'd like to do:
      httpd_handle_t server = hap_platform_httpd_get_handle();
      if(server == NULL) {
        ESP_LOGE(TAG, "Error getting httpd");
        ESP_FAIL
      }
      httpd_register_uri_handler(server, &uri_api_v1);
    
    which looks correct but does not work. What am I missing here?
  */
  
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 3000;

  httpd_handle_t server = NULL;
  if(httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &uri_api_v1_get_temp);
    httpd_register_uri_handler(server, &uri_api_v1_get_history);
  } else {
    return ESP_FAIL;
  }
  return ESP_OK;
}
