cmake_minimum_required(VERSION 3.12)
project(AsyncEchoServer2)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(utf8_range REQUIRED)

# 添加可执行文件和源文件
file(GLOB
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

add_executable(AsyncEchoServer2 ${SOURCES})

# 包含头文件路径（包括其他目录）
target_include_directories(
    AsyncEchoServer2
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${Protobuf_INCLUDE_DIRS}
)

# 链接相关库
target_link_libraries(
    AsyncEchoServer2
    PRIVATE
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    utf8_range::utf8_range
    utf8_range::utf8_validity
)