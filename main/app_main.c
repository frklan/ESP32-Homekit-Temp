
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <esp_log.h>

#include <driver/adc.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>

#include <hap_fw_upgrade.h>
#include <iot_button.h>

#include <app_wifi.h>
#include <app_hap_setup_payload.h>

#include <Thermistor_interop.h>

#define SENSOR_TASK_PRIORITY 1
#define SENSOR_TASK_STACKSIZE 4 * 1024
#define TAG "TTS42"

static void sensor_send_update(void *arg) {
  hap_char_t* hc;
  if(arg != NULL) {
    hc = (hap_char_t*) arg;
  } else {
    hap_serv_t *hs = hap_acc_get_serv_by_uuid(hap_get_first_acc(), HAP_SERV_UUID_TEMPERATURE_SENSOR);
    hc = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_CURRENT_TEMPERATURE);
  }

  hap_val_t new_val;
  new_val.f = get_sensor_reading(ADC1_CHANNEL_6);

  ESP_LOGI(TAG, "%f°C", new_val.f);
  hap_char_update_val(hc, &new_val);
}


void sensor_update_task(void* arg) {
  const TickType_t delay = 15000 / portTICK_PERIOD_MS;

  while(1) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    sensor_send_update(NULL);

    vTaskDelayUntil(&xLastWakeTime, delay);
  }
}


void sensor_init_hw() {
  init_sensor(ADC1_CHANNEL_6);
}

#define BUTTON_GPIO GPIO_NUM_0
void sensor_init_services() {
  button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LOW);
  iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, sensor_send_update, NULL);

  xTaskCreate(sensor_update_task, "Sensor auto update task", SENSOR_TASK_STACKSIZE, NULL, SENSOR_TASK_PRIORITY, NULL);
}


/* ************************* */
/* Callbacks for HAP service */
/* ************************* */


static int sensor_identify(hap_acc_t *ha) {
  ESP_LOGI(TAG, "I'm here!");
  return HAP_SUCCESS;
}

static int sensor_read(hap_char_t *hc, hap_status_t *status_code, void *serv_priv, void *read_priv) {
  const char* type_uuid = hap_char_get_type_uuid(hc);
  hap_char_format_t format = hap_char_get_format(hc);
  ESP_LOGI(TAG, "Got read request for %s, Format: %d", type_uuid, format);
  
  if(!strcmp(type_uuid, HAP_CHAR_UUID_CURRENT_TEMPERATURE)) { // type= 0x11, Format == 7 (HAP_CHAR_FORMAT_FLOAT)
    hap_val_t new_val = {
      .f = get_sensor_reading(ADC1_CHANNEL_6)
    };

    hap_char_update_val(hc, &new_val);
    *status_code = HAP_STATUS_SUCCESS;
  } else {
    *status_code = HAP_STATUS_RES_ABSENT;
  }
  
  return HAP_SUCCESS;
}

static void hap_event_handler(void *arg, esp_event_base_t event_base, int event, void *data) {
  switch (event) {
    case HAP_EVENT_PAIRING_STARTED:
      ESP_LOGI(TAG, "--~=>> Pairing Started");
      break;
    case HAP_EVENT_PAIRING_ABORTED:
      ESP_LOGI(TAG, "--~=>> Pairing Aborted");
      break;
    case HAP_EVENT_CTRL_PAIRED:
      ESP_LOGI(TAG, "--~=>> Controller %s Paired. Controller count: %d",
              (char *)data, hap_get_paired_controller_count());
      break;
    case HAP_EVENT_CTRL_UNPAIRED:
      ESP_LOGI(TAG, "--~=>> Controller %s Removed. Controller count: %d",
              (char *)data, hap_get_paired_controller_count());
      break;
    case HAP_EVENT_CTRL_CONNECTED:
      ESP_LOGI(TAG, "--~=>> Controller %s Connected", (char *)data);
      
      break;
    case HAP_EVENT_CTRL_DISCONNECTED:
      ESP_LOGI(TAG, "--~=>> Controller %s Disconnected", (char *)data);
      break;
    case HAP_EVENT_ACC_REBOOTING: {
      char *reason = (char *)data;
      ESP_LOGI(TAG, "--~=>> Accessory Rebooting (Reason: %s)", reason ? reason : "null");
      break;
    }
    default:
      /* Silently ignore unknown events */
      break;
  }
}


/* ******* */

static void sensor_thread_entry(void *p) {
  hap_acc_t *accessory;
  hap_serv_t *service;

  /* Initialize the HAP core */
  hap_init(HAP_TRANSPORT_WIFI);

  /* Initialise the mandatory parameters for Accessory which will be added as
  *  the mandatory services internally
  */
  hap_acc_cfg_t cfg = {
      .name = "Thermistor Temperature Sensor",
      .manufacturer = "yellowfortyfour.com",
      .model = TAG,
      .serial_num = "1",
      .fw_rev = "0.1.0",
      .hw_rev = "0.1.0",
      .pv = "1.1",
      .identify_routine = sensor_identify,
      .cid = HAP_CID_SENSOR,
  };

  /* Create accessory object */
  accessory = hap_acc_create(&cfg);
  if(!accessory) {
    ESP_LOGE(TAG, "Failed to create accessory");
    return;
  }

  uint8_t product_data[] = {'E', 'S', 'P', '3', '2', 'H', 'A', 'P'};
  hap_acc_add_product_data(accessory, product_data, sizeof(product_data));

  service = hap_serv_temperature_sensor_create(0);
  if(!service) {
    ESP_LOGE(TAG, "Failed to create sensor service");
    return;
  }

  // Add extra services here:
  //hap_serv_add_char(service, hap_char_xxxx_create(...));

  hap_serv_set_read_cb(service, sensor_read);
  hap_acc_add_serv(accessory, service);
  hap_add_accessory(accessory);

  esp_event_handler_register(HAP_EVENT, ESP_EVENT_ANY_ID, &hap_event_handler, NULL);

  /* HW init start */
  sensor_init_hw();
  
  /* HW init done */

  hap_set_setup_code(CONFIG_EXAMPLE_SETUP_CODE);
  /* Unique four character Setup Id. Default: ES32 */
  hap_set_setup_id(CONFIG_EXAMPLE_SETUP_ID);

  app_hap_setup_payload(CONFIG_EXAMPLE_SETUP_CODE, CONFIG_EXAMPLE_SETUP_ID, false, cfg.cid);

  hap_enable_mfi_auth(HAP_MFI_AUTH_HW);
  app_wifi_init();
  hap_start();
  app_wifi_start(portMAX_DELAY);

  // Sensor (and HAP) is now running - init auto update and user triggerd services
  sensor_init_services();

  vTaskDelete(NULL);
}

void app_main() {
  xTaskCreate(sensor_thread_entry, "Sensor HAP startup task", SENSOR_TASK_STACKSIZE, NULL, SENSOR_TASK_PRIORITY, NULL);
}

