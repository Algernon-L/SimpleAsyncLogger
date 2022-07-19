cmake_minimum_required(VERSION 2.6)
project(mylogger)

# Simplelog最终编译成so动态库 设置动态库的路径 放置项目根目录的lib文件夹下面
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置调试信息  以及启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE cpp_files 
${PROJECT_SOURCE_DIR}/mylogger/*.cpp)

# 编译动态库
add_library(mylogger SHARED ${cpp_files})