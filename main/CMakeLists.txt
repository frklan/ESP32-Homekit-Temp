idf_component_register(SRCS "app_main.c" INCLUDE_DIRS "."
  REQUIRES 
    app_hap_setup_payload
    app_wifi button driver
    coap freertos
    esp_hap_core
    esp_hap_apple_profiles
    esp_hap_extras
    Thermistor)

