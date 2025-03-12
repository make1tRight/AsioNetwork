cmake_minimum_required(VERSION 3.14)
project(GrpcExample)

set(CMAKE_CXX_STANDARD 20)

# 找到 Protobuf 和 gRPC
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(utf8_range REQUIRED)

# 服务器源文件
set(
    SERVER_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/GrpcServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/demo.grpc.pb.cc
)
add_executable(GrpcServer ${SERVER_SOURCES})

# 客户端源文件
set(
    CLIENT_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/GrpcClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/demo.grpc.pb.cc
)
add_executable(GrpcClient ${CLIENT_SOURCES})

# 包含头文件路径（包括其他目录）
target_include_directories(
    GrpcServer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${Protobuf_INCLUDE_DIRS}
)
# 包含头文件路径（包括其他目录）
target_include_directories(
    GrpcClient
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${Protobuf_INCLUDE_DIRS}
)

# 链接相关库
target_link_libraries(
    GrpcServer
    PRIVATE
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    utf8_range::utf8_range
    utf8_range::utf8_validity
)
# 链接相关库
target_link_libraries(
    GrpcClient
    PRIVATE
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    utf8_range::utf8_range
    utf8_range::utf8_validity
)





