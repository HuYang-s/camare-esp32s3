#!/bin/bash

# ESP32-S3 OV2640 摄像头项目构建脚本

echo "=========================================="
echo "ESP32-S3 OV2640 摄像头项目构建脚本"
echo "=========================================="

# 检查ESP-IDF环境
if [ -z "$IDF_PATH" ]; then
    echo "错误: 未设置ESP-IDF环境变量"
    echo "请先运行: source $HOME/esp/esp-idf/export.sh"
    exit 1
fi

echo "ESP-IDF路径: $IDF_PATH"
echo "ESP-IDF版本: $(idf.py --version)"

# 清理之前的构建
echo "清理之前的构建..."
idf.py fullclean

# 配置项目
echo "配置项目..."
idf.py set-target esp32s3
idf.py menuconfig

# 构建项目
echo "构建项目..."
idf.py build

# 检查构建结果
if [ $? -eq 0 ]; then
    echo "=========================================="
    echo "构建成功!"
    echo "=========================================="
    echo ""
    echo "下一步操作:"
    echo "1. 将ESP32-S3连接到电脑"
    echo "2. 运行: idf.py -p [PORT] flash monitor"
    echo "3. 在浏览器中访问ESP32-S3的IP地址"
    echo ""
    echo "注意: 请先修改代码中的WiFi配置"
    echo ""
else
    echo "=========================================="
    echo "构建失败!"
    echo "=========================================="
    echo "请检查错误信息并修复问题"
    exit 1
fi