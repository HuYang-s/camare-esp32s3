idf_component_register(SRCS "shexiang-ov2640.c"
                    INCLUDE_DIRS ".")

# 添加摄像头组件依赖
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
set(COMPONENTS main esp32-camera esp_http_server esp_wifi)
