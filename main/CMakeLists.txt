idf_component_register(SRCS "shexiang-ov2640.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_event nvs_flash esp_http_server esp_timer espressif__esp32-camera espressif__esp_jpeg)
