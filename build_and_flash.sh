#!/bin/bash

# ESP32-S3 OV2640 Camera Build and Flash Script
# 此脚本用于构建项目并烧录到设备

echo "ESP32-S3 OV2640 Camera - Build and Flash"
echo "========================================"

# 检查ESP-IDF环境
if [ -z "$IDF_PATH" ]; then
    echo "❌ Error: ESP-IDF environment not set!"
    echo "Please run: . \$IDF_PATH/export.sh"
    exit 1
fi

echo "✅ ESP-IDF environment detected: $IDF_PATH"

# 获取串口端口
DEFAULT_PORT="/dev/ttyUSB0"
echo -n "Enter serial port [$DEFAULT_PORT]: "
read SERIAL_PORT

if [ -z "$SERIAL_PORT" ]; then
    SERIAL_PORT=$DEFAULT_PORT
fi

echo "📡 Using serial port: $SERIAL_PORT"

# 检查串口是否存在
if [ ! -e "$SERIAL_PORT" ]; then
    echo "⚠️  Warning: Serial port $SERIAL_PORT not found"
    echo "Available ports:"
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "No USB serial ports found"
    echo -n "Continue anyway? (y/N): "
    read CONTINUE
    if [ "$CONTINUE" != "y" ] && [ "$CONTINUE" != "Y" ]; then
        echo "Build cancelled."
        exit 1
    fi
fi

echo ""
echo "🔧 Building project..."
echo "====================="

# 构建项目
if idf.py build; then
    echo "✅ Build successful!"
else
    echo "❌ Build failed!"
    exit 1
fi

echo ""
echo "📋 Build Summary:"
echo "================"
# 显示固件大小信息
if [ -f "build/shexiang-ov2640.bin" ]; then
    FIRMWARE_SIZE=$(stat -c%s "build/shexiang-ov2640.bin")
    echo "Firmware size: $FIRMWARE_SIZE bytes ($(($FIRMWARE_SIZE / 1024)) KB)"
fi

echo ""
echo -n "Flash to device? (Y/n): "
read FLASH_CONFIRM

if [ "$FLASH_CONFIRM" = "n" ] || [ "$FLASH_CONFIRM" = "N" ]; then
    echo "Build completed. Firmware ready for manual flashing."
    echo "To flash manually: idf.py -p $SERIAL_PORT flash monitor"
    exit 0
fi

echo ""
echo "⚡ Flashing to device..."
echo "======================"

# 烧录到设备
if idf.py -p "$SERIAL_PORT" flash; then
    echo "✅ Flash successful!"
    echo ""
    echo "🖥️  Starting serial monitor..."
    echo "============================="
    echo "Device will restart automatically."
    echo "Look for WiFi connection status and IP address."
    echo "Press Ctrl+] to exit monitor."
    echo ""
    
    # 启动串口监视器
    idf.py -p "$SERIAL_PORT" monitor
else
    echo "❌ Flash failed!"
    echo ""
    echo "Troubleshooting:"
    echo "- Check if device is connected to $SERIAL_PORT"
    echo "- Try different USB cable"
    echo "- Hold BOOT button while connecting USB"
    echo "- Check device drivers"
    exit 1
fi