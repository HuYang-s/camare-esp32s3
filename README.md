# ESP32-S3 + OV2640 摄像头 Web 流媒体示例

本工程基于 ESP-IDF（≥ v5.4）与 `espressif/esp32-camera` 组件，实现：
- 启动 OV2640 摄像头采集（JPEG）
- 连接 Wi‑Fi（STA）
- 内置 HTTP 服务器，支持：
  - `/` 简单预览页面
  - `/jpg` 抓拍单张 JPEG
  - `/stream` MJPEG 实时视频流（可用参数调分辨率与画质）

已针对 ESP32‑S3 + 8MB PSRAM + 16MB Flash 优化，默认使用 PSRAM 帧缓冲、MJPEG 流式输出、Wi‑Fi 吞吐优化与摄像头抓取最新帧（低延迟）。

---

## 硬件准备
- ESP32‑S3 开发板（16MB Flash，8MB PSRAM）
- OV2640 摄像头模组（DVP 接口）
- 供电与线材

### 引脚连接（工程默认）
- XCLK: GPIO15
- SIOD(SDA): GPIO4
- SIOC(SCL): GPIO5
- D0..D7: GPIO11, 9, 8, 10, 12, 18, 17, 16（按 D0→D7）
- VSYNC: GPIO6
- HREF: GPIO7
- PCLK: GPIO13
- PWDN: 未接（-1）
- RESET: 未接（-1）

如你的模组脚位不同，可在 menuconfig 修改（见下）。

---

## 工程结构
- `main/shexiang-ov2640.c`：摄像头初始化、Wi‑Fi、HTTP 服务器、`/jpg` 与 `/stream` 实现
- `main/Kconfig.projbuild`：Wi‑Fi 与摄像头引脚/参数的 menuconfig 选项
- `main/idf_component.yml`：声明依赖 `espressif/esp32-camera`（会自动获取）
- `managed_components/`：组件管理器缓存的第三方组件（如 `espressif__esp32-camera`、`espressif__esp_jpeg`）
- `sdkconfig`：已启用 16MB Flash 与 PSRAM（可依据你的板卡再调整）

---

## 开发环境
推荐使用 ESP‑IDF v5.4.x（例：v5.4.2）。

### 安装（本机）
```bash
# 克隆并安装（以 v5.4.2 为例）
git clone -b v5.4.2 --depth=1 https://github.com/espressif/esp-idf.git ~/esp-idf
cd ~/esp-idf
./install.sh esp32s3
# 导出环境（每个新终端都需要）
. ~/esp-idf/export.sh
```

### 可选：使用官方 Docker
```bash
docker pull espressif/idf:release-v5.4
# 构建
docker run --rm -it -v /absolute/path/to/project:/project -w /project espressif/idf:release-v5.4 \
  bash -lc "idf.py set-target esp32s3 && idf.py build"
# 烧录+监视（映射串口）
docker run --rm -it --device=/dev/ttyUSB0 -v /absolute/path/to/project:/project -w /project \
  espressif/idf:release-v5.4 bash -lc "idf.py -p /dev/ttyUSB0 flash monitor"
```

---

## 快速开始
```bash
cd <本工程目录>
idf.py set-target esp32s3
idf.py menuconfig
```

在 menuconfig 中设置：
- Application Configuration：
  - WiFi SSID / WiFi Password（默认已写入示例，可修改为你的 Wi‑Fi）
- Camera Configuration (OV2640 on ESP32-S3)：
  - 各引脚（如与默认不同）
  - XCLK 频率（默认 20MHz）
  - JPEG 质量（数值越小画质越好、数据越大；建议 10~30）
  - 帧缓冲数量（PSRAM 建议 3）
- Component config → ESP PSRAM：
  - Enable external SPIRAM：开启
  - PSRAM Mode：建议 Octal（若不稳定，可改 Quad）
  - PSRAM Speed：建议 80MHz（若不稳定，可改 40MHz）
  - 可根据需要开启 Fetch instructions/RODATA from PSRAM（若不稳定可关闭）
- Serial flasher config：
  - Flash size：设置为 16MB（与你的板卡一致）

构建、烧录与监视：
```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

串口日志中会显示获取到的 IP，浏览器访问：
- 首页：`http://<设备IP>/`
- 单帧：`http://<设备IP>/jpg`
- 流媒体：`http://<设备IP>/stream`

---

## Web 接口与参数
- `/`：简单预览页面（内置 `<img src="/stream">`）
- `/jpg`：抓拍 JPEG
- `/stream`：MJPEG 实时流
  - 查询参数：
    - `fs`：分辨率，如 `QVGA` / `VGA` / `SVGA` / `XGA`
    - `q`：JPEG 质量，范围 5..63（数值越小图像越清晰、数据越大）
  - 示例：
    - 更流畅：`/stream?fs=QVGA&q=22`
    - 清晰优先：`/stream?fs=VGA&q=12`

---

## 性能与流畅度建议
- 强烈建议启用 PSRAM，并将帧缓冲数设为 3（默认已设），抓取模式使用“最新帧”（已实现）
- Wi‑Fi：已关闭省电，启用 11n 与 40MHz 带宽（AP 需支持）；尽量靠近路由器、避免同信道干扰
- 带宽不足或浏览器卡顿时：
  - 降低分辨率（`QVGA`）
  - 提高 `q` 数值（如 20~30，意味着压缩更强、码率更低）
- 若画面倒置/镜像：在代码中可使用 `sensor_t` 的 `set_vflip`/`set_hmirror` 调整（`main/shexiang-ov2640.c` 已预留）

---

## 常见问题（FAQ）
- 日志提示：`W (...) ov2640_app: PSRAM not enabled. Using QVGA and single framebuffer`
  - 说明未启用 PSRAM 或配置未生效。请进入 menuconfig：
    - Component config → ESP PSRAM：启用 SPIRAM，并根据板卡选择 Mode（Oct/Quad）与 Speed（80/40MHz）
    - Serial flasher config：确保 Flash Size 配置与硬件一致
    - 重新构建、烧录
- 启动异常/不稳定：
  - 将 PSRAM Mode 从 Octal 降为 Quad，Speed 从 80MHz 降为 40MHz
  - 关闭 Fetch instructions/RODATA from PSRAM
- 仍有卡顿：
  - 使用 `/stream?fs=QVGA&q=25` 进一步降低带宽
  - 确保路由器 2.4GHz 环境的信道与带宽设置合理（开启 40MHz 但注意干扰与法规限制）

---

## 自定义与扩展
- 修改默认引脚/参数：
  - `idf.py menuconfig` → Camera Configuration (OV2640 on ESP32-S3)
- 修改页面或新增接口：
  - HTTP 服务器与处理器位于 `main/shexiang-ov2640.c`
- 第三方组件：
  - 通过 `main/idf_component.yml` 自动拉取 `espressif/esp32-camera` 与相关依赖

---

## 许可证
- 本示例仅用于学习与参考，请根据你的使用场景选择合适的许可证与合规模块。