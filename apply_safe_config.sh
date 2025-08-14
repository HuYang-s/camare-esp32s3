#!/bin/bash
# ESP32-S3 OV2640 摄像头安全配置应用脚本
# 这个脚本将应用内存友好的配置来解决帧缓冲区分配失败问题

echo "🛠️  应用ESP32-S3摄像头安全配置..."
echo "=================================="

# 备份当前配置
echo "📝 备份当前配置..."
cp main/camera_config.h main/camera_config.h.backup

# 应用安全配置
echo "🔄 应用安全配置..."
cp main/camera_config_safe.h main/camera_config.h

# 清理构建文件
echo "🧹 清理构建文件..."
rm -rf build managed_components

echo ""
echo "✅ 安全配置应用完成！"
echo ""
echo "📋 配置更改："
echo "   🔹 分辨率: UXGA (1600x1200) → VGA (640x480)"
echo "   🔹 JPEG质量: 15 → 20 (更高压缩)"
echo "   🔹 时钟频率: 20MHz → 10MHz"
echo "   🔹 帧缓冲: 双缓冲 → 单缓冲"
echo "   🔹 延迟: 100ms → 200ms"
echo "   ✅ 启用PSRAM支持"
echo ""
echo "🔧 修复的问题："
echo "   ✅ 帧缓冲区内存分配失败"
echo "   ✅ DMA配置失败"
echo "   ✅ 摄像头初始化失败"
echo ""
echo "🚀 现在可以重新构建："
echo "   idf.py build"
echo ""
echo "💡 如果问题仍然存在，可以尝试更小的分辨率："
echo "   - 编辑 main/camera_config.h"
echo "   - 将 FRAMESIZE_VGA 改为 FRAMESIZE_QVGA (320x240)"
echo "   - 或 FRAMESIZE_QQVGA (160x120) 用于最小内存使用"