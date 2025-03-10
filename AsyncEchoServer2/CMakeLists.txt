cmake_minimum_required(VERSION 3.12)
project(AsyncEchoServer2)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)


# 添加可执行文件和源文件
file(GLOB
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(AsyncEchoServer2 ${SOURCES})

# 包含头文件路径（包括其他目录）
target_include_directories(
    AsyncEchoServer2
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)