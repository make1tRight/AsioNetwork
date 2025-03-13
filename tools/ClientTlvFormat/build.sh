#!/bin/bash

CXX=g++
CXXFLAGS="-std=c++11"

JSONCPP_INCLUDE_PATH="/usr/include/jsoncpp"

SRC="client.cpp"
OUT="client"

# compile
${CXX} ${CXXFLAGS} -I${JSONCPP_INCLUDE_PATH} -o ${OUT} ${SRC} -ljsoncpp

# check 
if [ $? -eq 0 ]; then
    echo "compiled successufully ./${OUT}"
else
    echo "failed to compiled"
    exit 1
fi
