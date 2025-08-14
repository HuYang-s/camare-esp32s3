# ESP32-S3 OV2640 摄像头网络视频流项目

本项目实现了ESP32-S3开发板与OV2640摄像头的集成，提供实时MJPEG视频流和Web界面。

## 功能特性

- ✅ OV2640摄像头驱动和初始化
- ✅ WiFi网络连接
- ✅ HTTP Web服务器
- ✅ 实时MJPEG视频流
- ✅ 响应式Web界面
- ✅ 单张照片拍摄功能
- ✅ 自动重连机制

## 硬件要求

### ESP32-S3开发板
- ESP32-S3-WROOM-1或ESP32-S3-WROOM-2
- 至少4MB Flash和8MB PSRAM

### OV2640摄像头模块
标准的ESP32-CAM开发板引脚配置：

| 功能 | GPIO | 说明 |
|------|------|------|
| XCLK | 15 | 时钟信号 |
| SIOD | 4 | I2C数据线(SDA) |
| SIOC | 5 | I2C时钟线(SCL) |
| D7 | 16 | 数据位7 |
| D6 | 17 | 数据位6 |
| D5 | 18 | 数据位5 |
| D4 | 12 | 数据位4 |
| D3 | 10 | 数据位3 |
| D2 | 8 | 数据位2 |
| D1 | 9 | 数据位1 |
| D0 | 11 | 数据位0 |
| VSYNC | 6 | 垂直同步 |
| HREF | 7 | 水平参考 |
| PCLK | 13 | 像素时钟 |

## 软件配置

### 开发环境
- ESP-IDF v4.4 或更高版本
- 支持ESP32-S3的工具链

### WiFi配置
在`main/shexiang-ov2640.c`文件中修改以下配置：

```c
#define WIFI_SSID "YOUR_WIFI_SSID"    // 替换为你的WiFi名称
#define WIFI_PASS "YOUR_WIFI_PASSWORD" // 替换为你的WiFi密码
```

### 摄像头参数配置
可以在`camera_config`结构体中调整以下参数：

- `frame_size`: 图像尺寸（FRAMESIZE_UXGA, FRAMESIZE_SVGA, FRAMESIZE_VGA等）
- `jpeg_quality`: JPEG质量（1-63，数字越小质量越高）
- `xclk_freq_hz`: 时钟频率（20MHz推荐）

## 构建和烧录

### 1. 克隆项目
```bash
git clone <project-url>
cd shexiang-ov2640
```

### 2. 配置ESP-IDF环境
```bash
. $IDF_PATH/export.sh
```

### 3. 配置项目
```bash
idf.py menuconfig
```

重要配置项：
- Component config → ESP32-specific → Support for external, SPI-connected RAM: 启用
- Component config → Camera configuration → OV2640 Support: 启用

### 4. 构建项目
```bash
idf.py build
```

### 5. 烧录到设备
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## 使用方法

### 1. 连接硬件
按照上述引脚配置连接ESP32-S3和OV2640摄像头。

### 2. 配置WiFi
修改代码中的WiFi凭据并重新编译烧录。

### 3. 启动设备
设备启动后，在串口监视器中会显示：
- 摄像头初始化状态
- WiFi连接状态
- 获取到的IP地址
- Web服务器启动状态

### 4. 访问Web界面
在浏览器中输入设备的IP地址（如：http://192.168.1.100），即可看到：
- 实时视频流
- 拍摄照片按钮
- 刷新视频流按钮

## API端点

- `GET /` - 主页面，显示视频流界面
- `GET /stream` - MJPEG视频流
- `GET /capture` - 拍摄单张照片

## 故障排除

### 摄像头无法初始化
1. 检查引脚连接是否正确
2. 确认摄像头模块是否兼容OV2640
3. 检查电源供应是否足够（建议使用外部电源）

### WiFi连接失败
1. 确认WiFi凭据是否正确
2. 检查WiFi信号强度
3. 确认路由器是否支持设备连接

### 视频流卡顿或质量差
1. 降低JPEG质量数值（提高压缩比）
2. 减小图像尺寸
3. 检查网络带宽

### 设备重启或内存不足
1. 确保启用了PSRAM支持
2. 降低图像质量和尺寸
3. 检查partitions.csv配置

## 性能优化建议

1. **图像质量 vs 性能**: 调整`jpeg_quality`参数平衡画质和传输速度
2. **帧率控制**: 修改`stream_handler`中的`vTaskDelay`值调整帧率
3. **内存管理**: 在高分辨率模式下建议增加`fb_count`到2以提高稳定性

## 技术规格

- **支持分辨率**: UXGA(1600x1200) 到 QQVGA(160x120)
- **图像格式**: JPEG
- **视频流格式**: MJPEG (multipart/x-mixed-replace)
- **网络协议**: HTTP/1.1
- **帧率**: 约10-15 FPS（取决于分辨率和网络条件）

## 许可证

本项目采用MIT许可证，详情请参见LICENSE文件。