#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "camera_config.h"

static const char *TAG = "camera_httpd";

// FreeRTOS事件组位
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    .xclk_freq_hz = CAM_XCLK_FREQ,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = CAM_PIXEL_FORMAT,
    .frame_size = CAM_FRAME_SIZE,

    .jpeg_quality = CAM_JPEG_QUALITY,
    .fb_count = CAM_FB_COUNT,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 WIFI_SSID, WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 WIFI_SSID, WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

esp_err_t init_camera(void)
{
    // 初始化摄像头
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return err;
    }

    // 获取摄像头传感器
    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV2640_PID) {
        ESP_LOGI(TAG, "OV2640 camera detected!");
        // 初始设置一些参数
        s->set_vflip(s, 1);       // 垂直翻转
        s->set_brightness(s, 0);  // 亮度 (-2到2)
        s->set_saturation(s, 0);  // 饱和度 (-2到2)
    } else {
        ESP_LOGW(TAG, "Camera PID=0x%02x not recognized", s->id.PID);
    }

    return ESP_OK;
}

static esp_err_t capture_handler(httpd_req_t *req)
{
    camera_fb_t *pic = esp_camera_fb_get();
    if (!pic) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_send(req, (const char *)pic->buf, pic->len);

    esp_camera_fb_return(pic);
    ESP_LOGI(TAG, "JPG: %uB", (uint32_t)(pic->len));
    return ESP_OK;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[64];

    res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        } else {
            if (fb->width > 400) {
                if (fb->format != PIXFORMAT_JPEG) {
                    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    if (!jpeg_converted) {
                        ESP_LOGE(TAG, "JPEG compression failed");
                        res = ESP_FAIL;
                    }
                } else {
                    _jpg_buf_len = fb->len;
                    _jpg_buf = fb->buf;
                }
            }
        }
        if (res == ESP_OK) {
            size_t hlen = snprintf((char *)part_buf, 64, "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, "\r\n--" STREAM_BOUNDARY "\r\n", strlen("\r\n--" STREAM_BOUNDARY "\r\n"));
        }
        if (fb) {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        } else if (_jpg_buf) {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK) {
            break;
        }
        // 控制帧率
        vTaskDelay(STREAM_DELAY_MS / portTICK_PERIOD_MS);
    }
    return res;
}

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    
    char html_response[] = "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>ESP32-S3 Camera Stream</title>\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "<style>\n"
        "body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; background-color: #f0f0f0; }\n"
        ".container { max-width: 800px; margin: 0 auto; background: white; border-radius: 10px; padding: 20px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }\n"
        "h1 { color: #333; margin-bottom: 20px; }\n"
        ".stream-container { margin: 20px 0; }\n"
        "img { max-width: 100%; height: auto; border: 2px solid #ddd; border-radius: 8px; }\n"
        ".buttons { margin: 20px 0; }\n"
        "button { background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 4px; }\n"
        "button:hover { background-color: #45a049; }\n"
        ".info { background-color: #e7f3ff; border: 1px solid #b3d9ff; border-radius: 4px; padding: 10px; margin: 10px 0; }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<h1>ESP32-S3 OV2640 Camera Stream</h1>\n"
        "<div class=\"info\">\n"
        "<p>实时视频流正在显示。如果看不到图像，请确保设备已正确连接并刷新页面。</p>\n"
        "</div>\n"
        "<div class=\"stream-container\">\n"
        "<img id=\"stream\" src=\"/stream\" onerror=\"this.src='/stream?t='+(new Date()).getTime()\">\n"
        "</div>\n"
        "<div class=\"buttons\">\n"
        "<button onclick=\"capturePhoto()\">拍摄照片</button>\n"
        "<button onclick=\"refreshStream()\">刷新视频流</button>\n"
        "</div>\n"
        "</div>\n"
        "<script>\n"
        "function capturePhoto() {\n"
        "    window.open('/capture', '_blank');\n"
        "}\n"
        "function refreshStream() {\n"
        "    var img = document.getElementById('stream');\n"
        "    img.src = '/stream?t=' + (new Date()).getTime();\n"
        "}\n"
        "// 自动重连机制\n"
        "document.getElementById('stream').onerror = function() {\n"
        "    setTimeout(function() {\n"
        "        refreshStream();\n"
        "    }, 3000);\n"
        "};\n"
        "</script>\n"
        "</body>\n"
        "</html>";
    
    return httpd_resp_send(req, html_response, strlen(html_response));
}

void start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = WEB_SERVER_PORT;

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Web server started successfully");

        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &index_uri);

        httpd_uri_t capture_uri = {
            .uri       = "/capture",
            .method    = HTTP_GET,
            .handler   = capture_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &capture_uri);

        httpd_uri_t stream_uri = {
            .uri       = "/stream",
            .method    = HTTP_GET,
            .handler   = stream_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &stream_uri);

    } else {
        ESP_LOGE(TAG, "Error starting web server!");
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP32-S3 OV2640 Camera Web Server starting...");

    // 初始化摄像头
    ESP_LOGI(TAG, "Initializing camera...");
    if (init_camera() != ESP_OK) {
        ESP_LOGE(TAG, "Camera initialization failed!");
        return;
    }
    ESP_LOGI(TAG, "Camera initialized successfully");

    // 连接WiFi
    ESP_LOGI(TAG, "Connecting to WiFi...");
    wifi_init_sta();

    // 启动Web服务器
    ESP_LOGI(TAG, "Starting web server...");
    start_webserver();

    ESP_LOGI(TAG, "Setup completed! Connect to the web interface to view the camera stream.");
    ESP_LOGI(TAG, "Note: Please update WiFi credentials in the code: WIFI_SSID and WIFI_PASS");
}
