#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_http_server.h>
#include <esp_camera.h>
#include <esp_timer.h>
#include <img_converters.h>

// WiFi配置
#define WIFI_SSID      "YOUR_WIFI_SSID"      // 请修改为您的WiFi名称
#define WIFI_PASS      "YOUR_WIFI_PASSWORD"  // 请修改为您的WiFi密码
#define MAXIMUM_RETRY  5

// 摄像头引脚定义 (ESP32-S3)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

static const char *TAG = "OV2640_CAMERA";
static int s_retry_num = 0;
static httpd_handle_t stream_httpd = NULL;
static httpd_handle_t camera_httpd = NULL;

// 摄像头配置
static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,        // 20MHz XCLK
    .ledc_channel = LEDC_CHANNEL_0,
    .ledc_timer = LEDC_TIMER_0,
    .pixel_format = PIXFORMAT_JPEG,  // JPEG格式
    .frame_size = FRAMESIZE_VGA,     // 640x480分辨率
    .jpeg_quality = 12,              // JPEG质量 (0-63, 越低质量越好)
    .fb_count = 2,                   // 帧缓冲区数量
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

// WiFi事件处理
static void event_handler(void* arg, esp_event_base_t event_base,
                         int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "重试连接到AP");
        } else {
            ESP_LOGE(TAG, "连接到AP失败");
        }
        ESP_LOGI(TAG, "连接到AP");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "获取到IP地址:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
    }
}

// 初始化WiFi
static void wifi_init_sta(void)
{
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
                .pmf_required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta完成");
}

// 初始化摄像头
static esp_err_t init_camera(void)
{
    // 初始化摄像头
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "摄像头初始化失败: 0x%x", err);
        return err;
    }
    
    ESP_LOGI(TAG, "摄像头初始化成功");
    
    // 获取摄像头传感器
    sensor_t * s = esp_camera_sensor_get();
    if (s) {
        // 设置摄像头参数
        s->set_brightness(s, 0);     // 亮度 (-2 to 2)
        s->set_contrast(s, 0);       // 对比度 (-2 to 2)
        s->set_saturation(s, 0);     // 饱和度 (-2 to 2)
        s->set_special_effect(s, 0); // 特效 (0 to 6)
        s->set_whitebal(s, 1);       // 自动白平衡 (0 = disable, 1 = enable)
        s->set_awb_gain(s, 1);       // 自动白平衡增益 (0 = disable, 1 = enable)
        s->set_wb_mode(s, 0);        // 白平衡模式 (0 to 4)
        s->set_exposure_ctrl(s, 1);  // 自动曝光 (0 = disable, 1 = enable)
        s->set_aec2(s, 0);           // 自动曝光控制 (0 = disable, 1 = enable)
        s->set_gain_ctrl(s, 1);      // 自动增益控制 (0 = disable, 1 = enable)
        s->set_agc_gain(s, 0);       // 设置增益 (0 to 30)
        s->set_gainceiling(s, (gainceiling_t)0);  // 增益上限
        s->set_bpc(s, 0);            // 坏像素校正 (0 = disable, 1 = enable)
        s->set_wpc(s, 1);            // 白像素校正 (0 = disable, 1 = enable)
        s->set_raw_gma(s, 1);        // 原始伽马 (0 = disable, 1 = enable)
        s->set_lenc(s, 1);           // 镜头校正 (0 = disable, 1 = enable)
        s->set_hmirror(s, 0);        // 水平镜像 (0 = disable, 1 = enable)
        s->set_vflip(s, 0);          // 垂直翻转 (0 = disable, 1 = enable)
        s->set_dcw(s, 1);            // 下采样 (0 = disable, 1 = enable)
        s->set_colorbar(s, 0);       // 彩色条 (0 = disable, 1 = enable)
    }
    
    return ESP_OK;
}

// 拍照并保存到SD卡 (可选功能)
static esp_err_t capture_photo(void)
{
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "摄像头获取帧失败");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "拍照成功! 图像大小: %d bytes", fb->len);
    
    // 这里可以将图像数据通过串口发送到电脑
    // 或者保存到SD卡，或者通过其他方式处理
    
    esp_camera_fb_return(fb);
    return ESP_OK;
}

// 视频流处理函数
static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char * part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "30");

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "摄像头获取帧失败");
            res = ESP_FAIL;
        } else {
            if (fb->format != PIXFORMAT_JPEG) {
                bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = NULL;
                if (!jpeg_converted) {
                    ESP_LOGE(TAG, "JPEG转换失败");
                    res = ESP_FAIL;
                }
            } else {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }
        
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)part_buf, 0);
        }
        if (res != ESP_OK) {
            break;
        }
        
        // 发送图像数据
        if (fb) {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        } else if (_jpg_buf) {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        
        if (res != ESP_OK) {
            break;
        }
        
        // 控制帧率
        vTaskDelay(pdMS_TO_TICKS(33)); // 约30fps
    }

    return res;
}

// 拍照处理函数
static esp_err_t capture_handler(httpd_req_t *req)
{
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;

    fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "摄像头获取帧失败");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
            ESP_LOGE(TAG, "JPEG转换失败");
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
    }

    res = httpd_resp_set_type(req, "image/jpeg");
    res |= httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    res |= httpd_resp_set_hdr(req, "Content-Length", _jpg_buf_len);
    res |= httpd_resp_send(req, (const char *)_jpg_buf, _jpg_buf_len);

    if (fb) {
        esp_camera_fb_return(fb);
        fb = NULL;
        _jpg_buf = NULL;
    } else if (_jpg_buf) {
        free(_jpg_buf);
        _jpg_buf = NULL;
    }

    return res;
}

// 主页处理函数
static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    const char* index_html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32-S3 OV2640 摄像头</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        .video-container { text-align: center; margin: 20px 0; }
        .video-stream { border: 2px solid #ddd; border-radius: 8px; max-width: 100%; }
        .controls { text-align: center; margin: 20px 0; }
        .btn { background-color: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; margin: 5px; font-size: 16px; }
        .btn:hover { background-color: #0056b3; }
        .btn:active { transform: translateY(1px); }
        .status { text-align: center; margin: 20px 0; padding: 10px; background-color: #e9ecef; border-radius: 5px; }
        .info { background-color: #d1ecf1; border: 1px solid #bee5eb; border-radius: 5px; padding: 15px; margin: 20px 0; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32-S3 OV2640 摄像头系统</h1>
        
        <div class="info">
            <h3>系统信息</h3>
            <p><strong>硬件:</strong> ESP32-S3 + OV2640摄像头</p>
            <p><strong>分辨率:</strong> 640x480 VGA</p>
            <p><strong>格式:</strong> MJPEG视频流</p>
            <p><strong>帧率:</strong> 约30fps</p>
        </div>

        <div class="video-container">
            <h3>实时视频流</h3>
            <img src="/stream" class="video-stream" alt="视频流" id="videoStream">
        </div>

        <div class="controls">
            <button class="btn" onclick="capturePhoto()">拍照</button>
            <button class="btn" onclick="refreshStream()">刷新视频流</button>
        </div>

        <div class="status" id="status">
            状态: 视频流运行中...
        </div>

        <div class="info">
            <h3>使用说明</h3>
            <p>1. 确保ESP32-S3已连接到WiFi网络</p>
            <p>2. 视频流会自动开始显示</p>
            <p>3. 点击"拍照"按钮可以拍摄单张照片</p>
            <p>4. 如果视频流中断，点击"刷新视频流"按钮</p>
        </div>
    </div>

    <script>
        function capturePhoto() {
            document.getElementById('status').innerHTML = '状态: 正在拍照...';
            const img = document.createElement('img');
            img.src = '/capture?' + new Date().getTime();
            img.onload = function() {
                const link = document.createElement('a');
                link.download = 'capture.jpg';
                link.href = img.src;
                link.click();
                document.getElementById('status').innerHTML = '状态: 照片已下载';
            };
            img.onerror = function() {
                document.getElementById('status').innerHTML = '状态: 拍照失败';
            };
        }

        function refreshStream() {
            document.getElementById('status').innerHTML = '状态: 正在刷新视频流...';
            const videoStream = document.getElementById('videoStream');
            videoStream.src = '/stream?' + new Date().getTime();
            document.getElementById('status').innerHTML = '状态: 视频流已刷新';
        }

        // 自动刷新状态
        setInterval(function() {
            const videoStream = document.getElementById('videoStream');
            if (videoStream.complete && videoStream.naturalHeight !== 0) {
                document.getElementById('status').innerHTML = '状态: 视频流运行正常';
            } else {
                document.getElementById('status').innerHTML = '状态: 视频流连接中...';
            }
        }, 5000);
    </script>
</body>
</html>
    )";
    
    return httpd_resp_send(req, index_html, strlen(index_html));
}

// 启动摄像头服务器
static esp_err_t start_camera_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;

    // 启动流媒体服务器
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_uri_t stream_uri = {
            .uri       = "/stream",
            .method    = HTTP_GET,
            .handler   = stream_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(stream_httpd, &stream_uri);
        ESP_LOGI(TAG, "流媒体服务器启动成功");
    }

    // 启动主服务器
    config.server_port += 1;
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(camera_httpd, &index_uri);

        httpd_uri_t capture_uri = {
            .uri       = "/capture",
            .method    = HTTP_GET,
            .handler   = capture_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        ESP_LOGI(TAG, "摄像头服务器启动成功");
    }

    return ESP_OK;
}

// 主函数
void app_main(void)
{
    ESP_LOGI(TAG, "启动ESP32-S3 OV2640摄像头系统...");
    
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化摄像头
    ESP_ERROR_CHECK(init_camera());
    
    // 初始化WiFi
    wifi_init_sta();
    
    // 等待WiFi连接
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    // 启动Web服务器
    ESP_ERROR_CHECK(start_camera_server());
    
    ESP_LOGI(TAG, "系统启动完成!");
    ESP_LOGI(TAG, "请在浏览器中访问ESP32-S3的IP地址来查看视频流");
    
    // 主循环
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        ESP_LOGI(TAG, "系统运行中...");
    }
}
