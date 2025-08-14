#ifndef CAMERA_CONFIG_SAFE_H
#define CAMERA_CONFIG_SAFE_H

// 安全的摄像头配置 - 适用于内存受限环境
// 这个配置使用较小的分辨率和更少的内存

// WiFi配置
#define WIFI_SSID "bed_room_2.4G"
#define WIFI_PASS "Hdk4560.0"
#define WIFI_MAXIMUM_RETRY 5

// 摄像头引脚配置 (ESP32-S3 CAM)
#define CAM_PIN_PWDN    -1  // 功率关闭引脚，-1表示未使用
#define CAM_PIN_RESET   -1  // 复位引脚，-1表示未使用
#define CAM_PIN_XCLK    15  // 外部时钟
#define CAM_PIN_SIOD    4   // SDA
#define CAM_PIN_SIOC    5   // SCL

// 摄像头数据引脚 (Y2-Y9 对应 D0-D7)
#define CAM_PIN_D0      11  // Y2_GPIO_NUM
#define CAM_PIN_D1      9   // Y3_GPIO_NUM
#define CAM_PIN_D2      8   // Y4_GPIO_NUM
#define CAM_PIN_D3      10  // Y5_GPIO_NUM
#define CAM_PIN_D4      12  // Y6_GPIO_NUM
#define CAM_PIN_D5      18  // Y7_GPIO_NUM
#define CAM_PIN_D6      17  // Y8_GPIO_NUM
#define CAM_PIN_D7      16  // Y9_GPIO_NUM

// 摄像头控制引脚
#define CAM_PIN_VSYNC   6   // 垂直同步
#define CAM_PIN_HREF    7   // 水平参考
#define CAM_PIN_PCLK    13  // 像素时钟

// 安全的摄像头参数配置 - 最小内存使用
#define CAM_XCLK_FREQ   10000000  // 降低时钟频率到10MHz
#define CAM_PIXEL_FORMAT PIXFORMAT_JPEG  // 像素格式
#define CAM_FRAME_SIZE  FRAMESIZE_VGA    // 图像尺寸 (640x480 - 安全尺寸)
#define CAM_JPEG_QUALITY 20              // JPEG质量 (1-63, 较高压缩比)
#define CAM_FB_COUNT    1                // 帧缓冲区数量 (单缓冲)

// Web服务器配置
#define WEB_SERVER_PORT 80
#define STREAM_BOUNDARY "123456789000000000000987654321"
#define STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" STREAM_BOUNDARY

// 性能参数
#define STREAM_DELAY_MS 200  // 视频流帧间延迟 (更长延迟减少内存压力)

// 内存管理配置
#define CAMERA_MALLOC_PREFER_PSRAM  1  // 优先使用PSRAM
#define CAMERA_FB_IN_PSRAM         1  // 帧缓冲区在PSRAM中

/*
 * 安全图像尺寸选项（按内存使用量排序）:
 * FRAMESIZE_96X96,    // 96x96     (~9KB)
 * FRAMESIZE_QQVGA,    // 160x120   (~19KB)
 * FRAMESIZE_QCIF,     // 176x144   (~25KB)
 * FRAMESIZE_HQVGA,    // 240x176   (~42KB)
 * FRAMESIZE_240X240,  // 240x240   (~58KB)
 * FRAMESIZE_QVGA,     // 320x240   (~77KB)
 * FRAMESIZE_CIF,      // 400x296   (~118KB)
 * FRAMESIZE_HVGA,     // 480x320   (~154KB)
 * FRAMESIZE_VGA,      // 640x480   (~307KB) ← 当前选择
 * FRAMESIZE_SVGA,     // 800x600   (~480KB)
 * FRAMESIZE_XGA,      // 1024x768  (~786KB)
 * FRAMESIZE_HD,       // 1280x720  (~922KB)
 * FRAMESIZE_SXGA,     // 1280x1024 (~1.3MB)
 * FRAMESIZE_UXGA,     // 1600x1200 (~1.9MB)
 */

#endif // CAMERA_CONFIG_SAFE_H