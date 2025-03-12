cmake_minimum_required(VERSION 3.12)
project(JsonServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(SERVER JsonServer)

find_package(jsoncpp REQUIRED)

# 添加可执行文件和源文件
file(GLOB
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(${SERVER} ${SOURCES})

# 包含头文件路径（包括其他目录）
target_include_directories(
    ${SERVER}
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# 链接相关库
target_link_libraries(
    ${SERVER}
    PRIVATE
    jsoncpp
)