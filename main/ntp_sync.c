/*
 *  Copyright 2020-2021 Fredrik Andersson <fredrik.k.andersson@gmail.com>
 *
 *  This work is licensed under the CC BY-NC-SA 4.0 International License.
 * 
 */

#include "ntp_sync.h"

bool ntp_time_has_been_set = false;

int64_t get_current_epoch(void) {
  if(!ntp_time_has_been_set) {
    return 0;
  }

  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (int64_t)tv_now.tv_sec;
}

int ntp_time_init(const char* ntp_server) {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, ntp_server);
  sntp_init();

  // wait for time to be set
  int retries = 0;
  while(1) {
      sntp_sync_status_t sync_status = sntp_get_sync_status();
      if(sync_status == SNTP_SYNC_STATUS_COMPLETED) {
        break;
      } else if(retries >= CONFIG_NTP_SERVER_TIMEOUT) {
        return ESP_ERR_TIMEOUT;
      }
      retries++;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  ntp_time_has_been_set = true;
  return ESP_OK;
}
