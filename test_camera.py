#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ESP32-S3 OV2640 摄像头测试脚本
用于测试摄像头的基本功能和网络连接
"""

import requests
import time
import sys
from urllib.parse import urlparse

def test_web_interface(ip_address):
    """测试Web界面是否可访问"""
    print(f"测试Web界面: http://{ip_address}")
    
    try:
        response = requests.get(f"http://{ip_address}", timeout=5)
        if response.status_code == 200:
            print("✅ Web界面访问成功")
            return True
        else:
            print(f"❌ Web界面访问失败，状态码: {response.status_code}")
            return False
    except requests.exceptions.RequestException as e:
        print(f"❌ Web界面访问失败: {e}")
        return False

def test_camera_capture(ip_address):
    """测试拍照功能"""
    print(f"测试拍照功能: http://{ip_address}/capture")
    
    try:
        response = requests.get(f"http://{ip_address}/capture", timeout=10)
        if response.status_code == 200:
            content_type = response.headers.get('Content-Type', '')
            if 'image/jpeg' in content_type:
                print(f"✅ 拍照成功，图像大小: {len(response.content)} bytes")
                return True
            else:
                print(f"❌ 拍照失败，返回类型: {content_type}")
                return False
        else:
            print(f"❌ 拍照失败，状态码: {response.status_code}")
            return False
    except requests.exceptions.RequestException as e:
        print(f"❌ 拍照失败: {e}")
        return False

def test_video_stream(ip_address):
    """测试视频流功能"""
    print(f"测试视频流: http://{ip_address}/stream")
    
    try:
        response = requests.get(f"http://{ip_address}/stream", timeout=5, stream=True)
        if response.status_code == 200:
            content_type = response.headers.get('Content-Type', '')
            if 'multipart/x-mixed-replace' in content_type:
                print("✅ 视频流连接成功")
                return True
            else:
                print(f"❌ 视频流类型错误: {content_type}")
                return False
        else:
            print(f"❌ 视频流连接失败，状态码: {response.status_code}")
            return False
    except requests.exceptions.RequestException as e:
        print(f"❌ 视频流连接失败: {e}")
        return False

def save_test_image(ip_address, filename="test_capture.jpg"):
    """保存测试图像"""
    print(f"保存测试图像: {filename}")
    
    try:
        response = requests.get(f"http://{ip_address}/capture", timeout=10)
        if response.status_code == 200:
            with open(filename, 'wb') as f:
                f.write(response.content)
            print(f"✅ 测试图像已保存: {filename}")
            return True
        else:
            print(f"❌ 保存图像失败，状态码: {response.status_code}")
            return False
    except requests.exceptions.RequestException as e:
        print(f"❌ 保存图像失败: {e}")
        return False

def main():
    """主函数"""
    print("=" * 50)
    print("ESP32-S3 OV2640 摄像头功能测试")
    print("=" * 50)
    
    # 获取IP地址
    if len(sys.argv) > 1:
        ip_address = sys.argv[1]
    else:
        ip_address = input("请输入ESP32-S3的IP地址: ").strip()
    
    if not ip_address:
        print("❌ 未提供IP地址")
        return
    
    # 验证IP地址格式
    try:
        parsed = urlparse(f"http://{ip_address}")
        if not parsed.hostname:
            print("❌ IP地址格式无效")
            return
    except Exception:
        print("❌ IP地址格式无效")
        return
    
    print(f"\n开始测试ESP32-S3摄像头系统...")
    print(f"目标IP地址: {ip_address}")
    print("-" * 50)
    
    # 测试各项功能
    tests = [
        ("Web界面", test_web_interface),
        ("拍照功能", test_camera_capture),
        ("视频流", test_video_stream),
    ]
    
    results = {}
    for test_name, test_func in tests:
        print(f"\n测试 {test_name}...")
        results[test_name] = test_func(ip_address)
        time.sleep(1)  # 避免请求过快
    
    # 保存测试图像
    print(f"\n保存测试图像...")
    save_test_image(ip_address)
    
    # 显示测试结果
    print("\n" + "=" * 50)
    print("测试结果汇总")
    print("=" * 50)
    
    all_passed = True
    for test_name, result in results.items():
        status = "✅ 通过" if result else "❌ 失败"
        print(f"{test_name}: {status}")
        if not result:
            all_passed = False
    
    print("-" * 50)
    if all_passed:
        print("🎉 所有测试通过！摄像头系统工作正常")
        print("\n下一步操作:")
        print("1. 在浏览器中访问 http://" + ip_address)
        print("2. 查看实时视频流")
        print("3. 测试拍照功能")
    else:
        print("⚠️  部分测试失败，请检查系统配置")
        print("\n故障排除建议:")
        print("1. 确认ESP32-S3已连接到WiFi")
        print("2. 检查摄像头硬件连接")
        print("3. 查看串口输出日志")
        print("4. 确认防火墙设置")
    
    print("\n测试完成！")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n测试被用户中断")
    except Exception as e:
        print(f"\n测试过程中发生错误: {e}")
        sys.exit(1)