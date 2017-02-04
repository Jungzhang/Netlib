cmake_minimum_required(VERSION 3.5)
project(netlib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp base/StringPiece.h net/Buffer.cpp net/Buffer.h)
add_executable(netlib ${SOURCE_FILES})