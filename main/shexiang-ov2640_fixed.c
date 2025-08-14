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

// 摄像头配置
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

static esp_err_t init_camera(void)
{
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    // 获取摄像头传感器并设置参数
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_vflip(s, 1);      // 垂直翻转
        s->set_hmirror(s, 1);    // 水平镜像
    }

    return ESP_OK;
}

// 修复的capture_handler函数 - 完全兼容macOS ESP-IDF v5.4.2
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

    // 使用 %zu 格式化 size_t 类型，兼容所有平台
    ESP_LOGI(TAG, "JPG: %zuB", pic->len);
    esp_camera_fb_return(pic);
    return ESP_OK;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    
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
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, "\r\n--" STREAM_BOUNDARY "\r\n", strlen("\r\n--" STREAM_BOUNDARY "\r\n"));
            }
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, "Content-Type: image/jpeg\r\n\r\n", strlen("Content-Type: image/jpeg\r\n\r\n"));
            }
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
            }
        }
        if (fb) {
            esp_camera_fb_return(fb);
            fb = NULL;
        }
        if (res != ESP_OK) {
            break;
        }
        vTaskDelay(STREAM_DELAY_MS / portTICK_PERIOD_MS);
    }
    return res;
}

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    
    // 现代化的响应式Web界面
    const char* html = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<title>ESP32-S3 摄像头</title>"
        "<style>"
        "body{font-family:Arial,sans-serif;margin:0;padding:20px;background:#f0f0f0;text-align:center}"
        ".container{max-width:800px;margin:0 auto;background:white;border-radius:10px;padding:20px;box-shadow:0 4px 6px rgba(0,0,0,0.1)}"
        "h1{color:#333;margin-bottom:30px}"
        "#stream{max-width:100%;height:auto;border:2px solid #ddd;border-radius:8px;margin:20px 0}"
        ".btn{background:#007bff;color:white;border:none;padding:12px 24px;margin:10px;border-radius:5px;cursor:pointer;font-size:16px;text-decoration:none;display:inline-block}"
        ".btn:hover{background:#0056b3}"
        ".btn:active{transform:translateY(1px)}"
        "@media (max-width:600px){.container{padding:10px;margin:10px}.btn{padding:10px 20px;font-size:14px}}"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<h1>🔴 ESP32-S3 实时摄像头</h1>"
        "<img id='stream' src='/stream' alt='摄像头画面'>"
        "<br>"
        "<a href='/capture' class='btn' target='_blank'>📸 拍照</a>"
        "<button class='btn' onclick='location.reload()'>🔄 刷新</button>"
        "</div>"
        "<script>"
        "document.getElementById('stream').onerror=function(){"
        "setTimeout(function(){location.reload()},5000);"
        "};"
        "</script>"
        "</body>"
        "</html>";
    
    return httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
}

void start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = WEB_SERVER_PORT;
    config.max_uri_handlers = 16;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        
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
    ESP_LOGI(TAG, "Initializing camera...");
    if (init_camera() != ESP_OK) {
        ESP_LOGE(TAG, "Camera initialization failed!");
        return;
    }
    ESP_LOGI(TAG, "Camera initialized successfully");

    ESP_LOGI(TAG, "Connecting to WiFi...");
    wifi_init_sta();

    ESP_LOGI(TAG, "Starting web server...");
    start_webserver();

    ESP_LOGI(TAG, "Setup completed! Connect to the web interface to view the camera stream.");
    ESP_LOGI(TAG, "WiFi: %s | Web Server: http://[device-ip]:%d", WIFI_SSID, WEB_SERVER_PORT);
}