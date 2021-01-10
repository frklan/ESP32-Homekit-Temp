idf_component_register(SRCS "web_api.c" "ntp_sync.c" "app_main.c" INCLUDE_DIRS "."
  REQUIRES 
    app_hap_setup_payload
    app_wifi button driver
    coap freertos
    esp_common
    esp_hap_core
    esp_hap_apple_profiles
    esp_hap_extras
    esp_hap_platform
    esp_http_server
    json_generator
    Thermistor protocol_examples_common)

