idf_component_register(SRCS "shexiang-ov2640.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp32-camera nvs_flash esp_wifi esp_netif esp_http_server)
