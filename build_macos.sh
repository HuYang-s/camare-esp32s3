#!/bin/bash

# ESP32-S3 OV2640 Camera Build Script for macOS ESP-IDF v5.4.2
# 此脚本专门针对macOS环境和ESP-IDF v5.4.2

echo "ESP32-S3 OV2640 Camera - macOS Build Script"
echo "==========================================="

# 检查ESP-IDF环境
if [ -z "$IDF_PATH" ]; then
    echo "❌ Error: ESP-IDF environment not set!"
    echo "Please run: . $HOME/esp/v5.4.2/esp-idf/export.sh"
    exit 1
fi

echo "✅ ESP-IDF environment detected: $IDF_PATH"

# 检查ESP-IDF版本
IDF_VERSION=$(idf.py --version 2>&1 | grep -o 'v[0-9]\+\.[0-9]\+\.[0-9]\+')
echo "📌 ESP-IDF Version: $IDF_VERSION"

# 清理构建目录
echo ""
echo "🧹 Cleaning previous build..."
echo "=============================="
rm -rf build managed_components

# 设置目标芯片
echo ""
echo "🎯 Setting target to ESP32-S3..."
echo "================================"
idf.py set-target esp32s3

# 构建项目
echo ""
echo "🔧 Building project..."
echo "====================="

if idf.py build; then
    echo ""
    echo "✅ Build successful!"
    echo ""
    echo "📋 Build Summary:"
    echo "================"
    if [ -f "build/shexiang-ov2640.bin" ]; then
        FIRMWARE_SIZE=$(stat -f%z "build/shexiang-ov2640.bin" 2>/dev/null || stat -c%s "build/shexiang-ov2640.bin" 2>/dev/null)
        echo "Firmware size: $FIRMWARE_SIZE bytes ($(($FIRMWARE_SIZE / 1024)) KB)"
        echo "Binary file: build/shexiang-ov2640.bin"
        echo "ELF file: build/shexiang-ov2640.elf"
    fi
    
    echo ""
    echo "🚀 Next Steps:"
    echo "============="
    echo "1. Connect ESP32-S3 device via USB"
    echo "2. Find the serial port (usually /dev/cu.* on macOS)"
    echo "3. Flash the firmware:"
    echo "   idf.py -p /dev/cu.usbserial-* flash monitor"
    echo "4. Watch serial output for WiFi connection and IP address"
    echo "5. Open browser and navigate to the device IP"
    
    echo ""
    echo "📱 Available serial ports:"
    ls /dev/cu.* 2>/dev/null | grep -E "(usbserial|wchusbserial)" | head -5 || echo "No USB serial ports found"
    
else
    echo ""
    echo "❌ Build failed!"
    echo ""
    echo "🔍 Troubleshooting:"
    echo "=================="
    echo "1. Ensure ESP-IDF v5.4.2 is properly installed"
    echo "2. Check that all dependencies are available"
    echo "3. Try: idf.py fullclean && idf.py build"
    echo "4. Check build logs in build/log/ directory"
    exit 1
fi