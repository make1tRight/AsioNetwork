#!/bin/bash

# 编译参数
CXX=g++
SRC="BeastHttpServer.cpp"  # 假设你的代码保存为 server.cpp
OUT="server"

# 依赖库
BOOST_LIBS="-lboost_system -lboost_thread -lpthread"
JSONCPP_LIBS="-ljsoncpp"

# 编译命令
$CXX -std=c++20 $SRC -o $OUT $BOOST_LIBS $JSONCPP_LIBS

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "compiled successfulyy! ./server to start the server。"
else
    echo "failed to compile"
fi
