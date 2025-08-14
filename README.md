# ESP32-S3 OV2640 摄像头网络视频流项目

[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.4.2-blue.svg)](https://github.com/espressif/esp-idf)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP32--S3-red.svg)](https://www.espressif.com/en/products/socs/esp32-s3)

本项目实现了ESP32-S3开发板与OV2640摄像头的集成，提供实时MJPEG视频流和现代化Web界面。经过完整的内存优化和稳定性改进，支持ESP-IDF v5.4.2。

## 🎯 功能特性

- ✅ **OV2640摄像头驱动** - 完整的摄像头初始化和配置
- ✅ **WiFi网络连接** - 自动连接和重连机制
- ✅ **HTTP Web服务器** - 轻量级HTTP服务器
- ✅ **实时MJPEG视频流** - 稳定的多部分HTTP视频流
- ✅ **响应式Web界面** - 现代化移动端友好设计
- ✅ **单张照片拍摄** - 高质量JPEG图像捕获
- ✅ **内存优化** - PSRAM支持和智能内存管理
- ✅ **多平台支持** - Linux/WSL/macOS兼容
- ✅ **完整错误处理** - 详细的诊断和恢复机制

## 🛠️ 硬件要求

### ESP32-S3开发板
- **处理器**: ESP32-S3-WROOM-1或ESP32-S3-WROOM-2
- **Flash**: 至少4MB
- **PSRAM**: 8MB (必需，用于摄像头缓冲)
- **GPIO**: 足够的可用引脚用于摄像头连接

### OV2640摄像头模块
经过验证的引脚映射配置：

| 功能 | GPIO | 摄像头引脚 | 说明 |
|------|------|------------|------|
| XCLK | 15 | XCLK | 时钟信号 (10MHz) |
| SIOD | 4 | SDA | I2C数据线 |
| SIOC | 5 | SCL | I2C时钟线 |
| D0 | 11 | Y2 | 数据位0 |
| D1 | 9 | Y3 | 数据位1 |
| D2 | 8 | Y4 | 数据位2 |
| D3 | 10 | Y5 | 数据位3 |
| D4 | 12 | Y6 | 数据位4 |
| D5 | 18 | Y7 | 数据位5 |
| D6 | 17 | Y8 | 数据位6 |
| D7 | 16 | Y9 | 数据位7 |
| VSYNC | 6 | VSYNC | 垂直同步 |
| HREF | 7 | HREF | 水平参考 |
| PCLK | 13 | PCLK | 像素时钟 |
| PWDN | - | - | 未使用 |
| RESET | - | - | 未使用 |

## 💻 开发环境

### 支持的ESP-IDF版本
- ✅ **ESP-IDF v5.4.2** (推荐，已测试)
- ✅ **ESP-IDF v5.x** (兼容)
- ⚠️ **ESP-IDF v4.x** (部分兼容，可能需要调整)

### 开发平台
- ✅ **Linux** (Ubuntu/Debian/CentOS)
- ✅ **macOS** (Intel/Apple Silicon)
- ✅ **Windows** (WSL推荐)

## ⚙️ 预配置设置

### WiFi配置
项目已预配置WiFi凭据：
```c
#define WIFI_SSID "bed_room_2.4G"     // WiFi名称
#define WIFI_PASS "Hdk4560.0"         // WiFi密码
```

### 摄像头配置 (优化后)
```c
#define CAM_FRAME_SIZE  FRAMESIZE_VGA    // 640x480 (内存友好)
#define CAM_JPEG_QUALITY 20              // 平衡质量与大小
#define CAM_FB_COUNT    1                // 单缓冲 (节省内存)
#define CAM_XCLK_FREQ   10000000         // 10MHz (稳定运行)
```

## 🚀 快速开始

### 1. 环境准备

#### Linux/WSL:
```bash
# 安装ESP-IDF v5.4.2
mkdir -p ~/esp
cd ~/esp
git clone -b v5.4.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh
```

#### macOS:
```bash
# 安装ESP-IDF v5.4.2
mkdir -p ~/esp
cd ~/esp
git clone -b v5.4.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh
```

### 2. 克隆项目
```bash
git clone <project-repository>
cd esp32s3-camera-streaming
```

### 3. 配置WiFi (可选)
```bash
# 使用快速配置脚本
./setup_wifi.sh

# 或手动编辑
vi main/camera_config.h
```

### 4. 构建和烧录

#### 自动构建 (推荐):

**Linux/WSL用户**:
```bash
./build_and_flash.sh
```

**macOS用户**:
```bash
./build_macos.sh
```

#### 手动构建:
```bash
# 设置目标
idf.py set-target esp32s3

# 构建项目
idf.py build

# 烧录固件 (替换为实际串口)
idf.py -p /dev/ttyUSB0 flash monitor
```

### 5. 验证配置
```bash
# 查看当前配置
./verify_config.sh
```

## 🌐 使用方法

### 1. 连接硬件
- 按照引脚映射表连接ESP32-S3和OV2640
- 确保电源供应稳定 (推荐外部3.3V电源)
- 使用短而质量好的杜邦线

### 2. 烧录固件
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

### 3. 查看启动日志
正常启动应该看到：
```
I (342) camera_httpd: Free heap before camera init: 283748 bytes
I (342) camera_httpd: PSRAM enabled, checking available PSRAM...
I (348) cam_hal: cam_init
I (352) camera_httpd: OV2640 camera detected!
I (356) camera_httpd: Camera sensor configured successfully
I (362) camera_httpd: Free heap after camera init: 276832 bytes
I (368) camera_httpd: Camera initialized successfully
I (375) camera_httpd: connected to ap SSID:bed_room_2.4G
I (382) camera_httpd: got ip:192.168.1.100
I (388) camera_httpd: Starting server on port: '80'
```

### 4. 访问Web界面
1. 从串口日志获取设备IP地址
2. 在浏览器中访问: `http://[设备IP地址]/`
3. 享受实时摄像头流！

## 🔗 API端点

| 端点 | 方法 | 功能 | 响应格式 |
|------|------|------|----------|
| `/` | GET | 主页面 | HTML |
| `/stream` | GET | MJPEG视频流 | multipart/x-mixed-replace |
| `/capture` | GET | 单次拍照 | image/jpeg |

### Web界面功能
- 🔴 **实时视频流** - 640x480分辨率
- 📸 **拍照按钮** - 捕获高质量JPEG
- 🔄 **刷新按钮** - 重新加载视频流
- 📱 **响应式设计** - 移动设备友好

## 🛠️ 故障排除

### 常见问题解决

#### 1. 摄像头内存分配失败
**错误信息**:
```
E (516) cam_hal: cam_dma_config(504): frame buffer malloc failed
E (516) cam_hal: cam_config(594): cam_dma_config failed
```

**解决方案**:
```bash
# 应用安全配置
./apply_safe_config.sh
idf.py build
```

#### 2. macOS构建错误
**错误信息**:
```
format '%u' expects argument of type 'unsigned int', but argument 6 has type 'long unsigned int'
```

**解决方案**:
```bash
# 使用macOS修复脚本
./fix_macos_build.sh
```

#### 3. ESP-IDF版本兼容性
**问题**: 不同ESP-IDF版本的兼容性问题

**解决方案**:
```bash
# 检查ESP-IDF版本
idf.py --version

# 推荐使用v5.4.2
cd $IDF_PATH
git checkout v5.4.2
git submodule update --init --recursive
./install.sh esp32s3
```

#### 4. WiFi连接失败
**检查项目**:
1. 确认WiFi凭据正确
2. 检查WiFi信号强度
3. 确认路由器支持设备连接

#### 5. 视频流卡顿或质量差
**优化建议**:
```c
// 编辑 main/camera_config.h
#define CAM_JPEG_QUALITY 25        // 提高压缩比
#define STREAM_DELAY_MS 300        // 增加延迟
#define CAM_FRAME_SIZE FRAMESIZE_QVGA  // 降低分辨率
```

### 完整故障排除指南

#### macOS ESP-IDF v5.4.2特定问题
查看详细指南：
```bash
cat TROUBLESHOOTING_MACOS.md
```

#### 内存问题完整解决方案
查看内存优化指南：
```bash
cat CAMERA_MEMORY_FIX_SUMMARY.md
```

## 📊 性能规格

### 当前配置性能
- **分辨率**: 640x480 (VGA)
- **帧率**: 5-10 FPS (取决于网络)
- **JPEG质量**: 中等 (质量20)
- **延迟**: ~200ms
- **内存使用**: ~307KB per frame
- **固件大小**: ~891KB

### 可调整的性能选项

#### 分辨率选项 (按内存使用排序)
```c
FRAMESIZE_96X96     // 96x96     (~9KB)
FRAMESIZE_QQVGA     // 160x120   (~19KB)
FRAMESIZE_QCIF      // 176x144   (~25KB)
FRAMESIZE_HQVGA     // 240x176   (~42KB)
FRAMESIZE_QVGA      // 320x240   (~77KB)
FRAMESIZE_VGA       // 640x480   (~307KB) ← 当前
FRAMESIZE_SVGA      // 800x600   (~480KB)
FRAMESIZE_XGA       // 1024x768  (~786KB)
FRAMESIZE_UXGA      // 1600x1200 (~1.9MB)
```

#### JPEG质量设置
```c
// 质量范围: 1-63 (数字越小质量越高，文件越大)
#define CAM_JPEG_QUALITY 10   // 高质量，大文件
#define CAM_JPEG_QUALITY 20   // 中等质量 ← 当前
#define CAM_JPEG_QUALITY 30   // 低质量，小文件
```

## 📁 项目结构

```
esp32s3-camera-streaming/
├── main/
│   ├── shexiang-ov2640.c          # 主程序源码
│   ├── camera_config.h            # 摄像头配置 (当前)
│   ├── camera_config_safe.h       # 安全配置选项
│   ├── idf_component.yml          # 组件依赖
│   └── CMakeLists.txt             # 构建配置
├── scripts/
│   ├── setup_wifi.sh              # WiFi配置脚本
│   ├── verify_config.sh           # 配置验证脚本
│   ├── build_and_flash.sh         # Linux构建脚本
│   ├── build_macos.sh             # macOS构建脚本
│   ├── apply_safe_config.sh       # 安全配置应用
│   └── fix_macos_build.sh         # macOS修复脚本
├── docs/
│   ├── TROUBLESHOOTING_MACOS.md   # macOS故障排除
│   ├── CAMERA_MEMORY_FIX_SUMMARY.md # 内存问题解决
│   └── BUILD_SUCCESS_ESP_IDF_v5.4.md # 构建成功报告
├── partitions.csv                 # 分区表配置
├── sdkconfig                      # ESP-IDF配置
├── CMakeLists.txt                 # 项目构建配置
└── README.md                      # 项目文档
```

## 🔧 高级配置

### 自定义摄像头参数
编辑 `main/camera_config.h`:
```c
// 摄像头时钟频率 (Hz)
#define CAM_XCLK_FREQ   10000000    // 10MHz (稳定) 或 20000000 (高性能)

// 图像格式
#define CAM_PIXEL_FORMAT PIXFORMAT_JPEG  // JPEG压缩

// 帧缓冲数量
#define CAM_FB_COUNT    1           // 单缓冲 (省内存) 或 2 (双缓冲)

// Web服务器端口
#define WEB_SERVER_PORT 80          // HTTP端口

// 视频流参数
#define STREAM_DELAY_MS 200         // 帧间延迟 (毫秒)
```

### PSRAM配置
项目已自动启用PSRAM，配置在 `sdkconfig`:
```ini
CONFIG_SPIRAM=y                    # 启用PSRAM
CONFIG_SPIRAM_USE_MALLOC=y         # malloc使用PSRAM
CONFIG_SPIRAM_SIZE=8388608         # 8MB PSRAM
```

### 网络配置
```c
// WiFi配置
#define WIFI_SSID "your_wifi_name"     # WiFi名称
#define WIFI_PASS "your_password"      # WiFi密码
#define WIFI_MAXIMUM_RETRY 5           # 最大重试次数
```

## 🎯 技术规格

### 系统要求
- **ESP32-S3**: 双核240MHz处理器
- **内存**: 512KB SRAM + 8MB PSRAM
- **Flash**: 4MB (最少), 8MB (推荐)
- **WiFi**: 802.11 b/g/n (2.4 GHz)

### 软件栈
- **RTOS**: FreeRTOS 10.4.3
- **TCP/IP**: LwIP
- **HTTP**: ESP HTTP Server
- **WiFi**: ESP WiFi 驱动
- **摄像头**: ESP32-Camera v2.1.2

### 协议支持
- **视频流**: MJPEG over HTTP
- **图像格式**: JPEG
- **网络**: HTTP/1.1
- **传输**: multipart/x-mixed-replace

## 📄 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🤝 贡献

欢迎贡献代码！请阅读贡献指南：

1. Fork 本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📞 支持

如果您遇到问题，请：

1. 查看 [故障排除指南](#故障排除)
2. 查看 [FAQ文档](docs/FAQ.md)
3. 搜索已有的 [Issues](../../issues)
4. 创建新的 [Issue](../../issues/new)

## 🚀 后续开发计划

- [ ] 更多摄像头型号支持 (OV3660, OV5640)
- [ ] 人脸检测和识别
- [ ] 移动侦测
- [ ] 录像功能
- [ ] Web配置界面
- [ ] HTTPS支持
- [ ] 多语言界面

---

**享受您的ESP32-S3摄像头项目！** 📸✨