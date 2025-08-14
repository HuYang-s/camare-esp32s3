#!/bin/bash

# ESP32-S3 OV2640 Camera WiFi Setup Script
# 此脚本用于快速配置WiFi凭据

CONFIG_FILE="main/camera_config.h"

echo "ESP32-S3 OV2640 Camera WiFi Configuration"
echo "========================================="

# 检查配置文件是否存在
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: $CONFIG_FILE not found!"
    exit 1
fi

# 获取WiFi信息
echo -n "Enter WiFi SSID: "
read wifi_ssid

echo -n "Enter WiFi Password: "
read -s wifi_password
echo

# 确认信息
echo ""
echo "Configuration Summary:"
echo "WiFi SSID: $wifi_ssid"
echo "WiFi Password: [hidden]"
echo ""

echo -n "Confirm configuration? (y/N): "
read confirm

if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
    # 创建备份
    cp "$CONFIG_FILE" "$CONFIG_FILE.backup"
    echo "Backup created: $CONFIG_FILE.backup"
    
    # 更新配置文件
    sed -i "s/#define WIFI_SSID \"YOUR_WIFI_SSID\"/#define WIFI_SSID \"$wifi_ssid\"/" "$CONFIG_FILE"
    sed -i "s/#define WIFI_PASS \"YOUR_WIFI_PASSWORD\"/#define WIFI_PASS \"$wifi_password\"/" "$CONFIG_FILE"
    
    echo "Configuration updated successfully!"
    echo ""
    echo "Next steps:"
    echo "1. Build the project: idf.py build"
    echo "2. Flash to device: idf.py -p /dev/ttyUSB0 flash monitor"
    echo "3. Check serial output for device IP address"
    echo "4. Open web browser and navigate to the IP address"
else
    echo "Configuration cancelled."
    exit 1
fi