# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# 设置项目名称
project(shexiang-ov2640)

# 添加摄像头组件
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

# 包含ESP-IDF项目构建系统
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(shexiang-ov2640)
