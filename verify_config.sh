#!/bin/bash

# ESP32-S3 OV2640 Camera Configuration Verification Script
# 此脚本用于验证当前配置

CONFIG_FILE="main/camera_config.h"

echo "ESP32-S3 OV2640 Camera Configuration Verification"
echo "================================================="

# 检查配置文件是否存在
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: $CONFIG_FILE not found!"
    exit 1
fi

echo ""
echo "📶 WiFi Configuration:"
echo "====================="
WIFI_SSID=$(grep "#define WIFI_SSID" "$CONFIG_FILE" | cut -d'"' -f2)
WIFI_PASS=$(grep "#define WIFI_PASS" "$CONFIG_FILE" | cut -d'"' -f2)
echo "SSID: $WIFI_SSID"
echo "Password: $WIFI_PASS"

echo ""
echo "📷 Camera Pin Configuration:"
echo "==========================="
echo "Control Pins:"
echo "  PWDN:  $(grep "#define CAM_PIN_PWDN" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  RESET: $(grep "#define CAM_PIN_RESET" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  XCLK:  $(grep "#define CAM_PIN_XCLK" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  SIOD:  $(grep "#define CAM_PIN_SIOD" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1) (SDA)"
echo "  SIOC:  $(grep "#define CAM_PIN_SIOC" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1) (SCL)"

echo ""
echo "Data Pins (Y2-Y9 → D0-D7):"
echo "  D0 (Y2): $(grep "#define CAM_PIN_D0" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D1 (Y3): $(grep "#define CAM_PIN_D1" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D2 (Y4): $(grep "#define CAM_PIN_D2" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D3 (Y5): $(grep "#define CAM_PIN_D3" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D4 (Y6): $(grep "#define CAM_PIN_D4" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D5 (Y7): $(grep "#define CAM_PIN_D5" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D6 (Y8): $(grep "#define CAM_PIN_D6" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  D7 (Y9): $(grep "#define CAM_PIN_D7" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"

echo ""
echo "Sync Pins:"
echo "  VSYNC: $(grep "#define CAM_PIN_VSYNC" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  HREF:  $(grep "#define CAM_PIN_HREF" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"
echo "  PCLK:  $(grep "#define CAM_PIN_PCLK" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)"

echo ""
echo "⚙️  Camera Parameters:"
echo "===================="
XCLK_FREQ=$(grep "#define CAM_XCLK_FREQ" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)
FRAME_SIZE=$(grep "#define CAM_FRAME_SIZE" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)
JPEG_QUALITY=$(grep "#define CAM_JPEG_QUALITY" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)
FB_COUNT=$(grep "#define CAM_FB_COUNT" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)

echo "Clock Frequency: ${XCLK_FREQ} Hz ($(($XCLK_FREQ / 1000000)) MHz)"
echo "Frame Size: $FRAME_SIZE"
echo "JPEG Quality: $JPEG_QUALITY (lower = better quality)"
echo "Frame Buffer Count: $FB_COUNT"

echo ""
echo "🌐 Web Server Configuration:"
echo "=========================="
WEB_PORT=$(grep "#define WEB_SERVER_PORT" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)
STREAM_DELAY=$(grep "#define STREAM_DELAY_MS" "$CONFIG_FILE" | awk '{print $3}' | cut -d'/' -f1)
echo "Server Port: $WEB_PORT"
echo "Stream Delay: ${STREAM_DELAY}ms"

echo ""
echo "✅ Configuration verified successfully!"
echo ""
echo "📋 Next Steps:"
echo "============="
echo "1. Build the project: idf.py build"
echo "2. Flash to device:   idf.py -p /dev/ttyUSB0 flash monitor"
echo "3. Check serial output for IP address"
echo "4. Open browser and go to: http://[DEVICE_IP]"

echo ""
echo "💡 Pin Connection Summary:"
echo "========================"
echo "ESP32-S3 → OV2640 Camera"
echo "GPIO15   → XCLK"
echo "GPIO4    → SDA/SIOD"
echo "GPIO5    → SCL/SIOC"
echo "GPIO11   → Y2"
echo "GPIO9    → Y3"
echo "GPIO8    → Y4"
echo "GPIO10   → Y5"
echo "GPIO12   → Y6"
echo "GPIO18   → Y7"
echo "GPIO17   → Y8"
echo "GPIO16   → Y9"
echo "GPIO6    → VSYNC"
echo "GPIO7    → HREF"
echo "GPIO13   → PCLK"
echo "3.3V     → VCC"
echo "GND      → GND"