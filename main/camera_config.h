#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

// WiFi配置
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define WIFI_MAXIMUM_RETRY 5

// 摄像头引脚配置 (ESP32-S3 CAM)
#define CAM_PIN_PWDN    -1  // 功率关闭引脚，-1表示未使用
#define CAM_PIN_RESET   -1  // 复位引脚，-1表示未使用
#define CAM_PIN_XCLK    15  // 外部时钟
#define CAM_PIN_SIOD    4   // SDA
#define CAM_PIN_SIOC    5   // SCL

// 摄像头数据引脚
#define CAM_PIN_D7      16
#define CAM_PIN_D6      17
#define CAM_PIN_D5      18
#define CAM_PIN_D4      12
#define CAM_PIN_D3      10
#define CAM_PIN_D2      8
#define CAM_PIN_D1      9
#define CAM_PIN_D0      11

// 摄像头控制引脚
#define CAM_PIN_VSYNC   6   // 垂直同步
#define CAM_PIN_HREF    7   // 水平参考
#define CAM_PIN_PCLK    13  // 像素时钟

// 摄像头参数配置
#define CAM_XCLK_FREQ   20000000  // 时钟频率 (Hz)
#define CAM_PIXEL_FORMAT PIXFORMAT_JPEG  // 像素格式
#define CAM_FRAME_SIZE  FRAMESIZE_UXGA   // 图像尺寸
#define CAM_JPEG_QUALITY 12              // JPEG质量 (1-63, 数字越小质量越高)
#define CAM_FB_COUNT    1                // 帧缓冲区数量

// Web服务器配置
#define WEB_SERVER_PORT 80
#define STREAM_BOUNDARY "123456789000000000000987654321"
#define STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" STREAM_BOUNDARY

// 性能参数
#define STREAM_DELAY_MS 100  // 视频流帧间延迟 (毫秒)

/*
 * 图像尺寸选项:
 * FRAMESIZE_96X96,    // 96x96
 * FRAMESIZE_QQVGA,    // 160x120
 * FRAMESIZE_QCIF,     // 176x144
 * FRAMESIZE_HQVGA,    // 240x176
 * FRAMESIZE_240X240,  // 240x240
 * FRAMESIZE_QVGA,     // 320x240
 * FRAMESIZE_CIF,      // 400x296
 * FRAMESIZE_HVGA,     // 480x320
 * FRAMESIZE_VGA,      // 640x480
 * FRAMESIZE_SVGA,     // 800x600
 * FRAMESIZE_XGA,      // 1024x768
 * FRAMESIZE_HD,       // 1280x720
 * FRAMESIZE_SXGA,     // 1280x1024
 * FRAMESIZE_UXGA,     // 1600x1200
 */

/*
 * JPEG质量说明:
 * 1-10:   高质量，大文件
 * 10-20:  中高质量
 * 20-30:  中等质量
 * 30-50:  中低质量
 * 50-63:  低质量，小文件
 */

#endif // CAMERA_CONFIG_H