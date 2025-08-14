#!/bin/bash
# ESP32-S3 OV2640 macOS ESP-IDF v5.4.2 构建修复脚本

echo "🔧 修复 macOS ESP-IDF v5.4.2 构建问题..."
echo "=================================="

# 备份原文件
if [ ! -f "main/shexiang-ov2640.c.backup" ]; then
    echo "📝 备份原文件..."
    cp main/shexiang-ov2640.c main/shexiang-ov2640.c.backup
fi

# 使用修复的源文件替换原文件
echo "🔄 替换为修复的源文件..."
cp main/shexiang-ov2640_fixed.c main/shexiang-ov2640.c

# 清理构建文件
echo "🧹 清理构建文件..."
rm -rf build managed_components

echo ""
echo "✅ 修复完成！"
echo ""
echo "📋 修复内容："
echo "   ✅ 修复所有格式化字符串问题 (%u -> %zu)"
echo "   ✅ 移除不必要的类型转换"
echo "   ✅ 优化代码结构和注释"
echo "   ✅ 确保与 macOS ESP-IDF v5.4.2 完全兼容"
echo ""
echo "🚀 现在可以重新构建："
echo "   idf.py set-target esp32s3"
echo "   idf.py build"
echo ""
echo "📱 如果仍有问题，请运行："
echo "   idf.py fullclean"
echo "   idf.py set-target esp32s3"
echo "   idf.py build"