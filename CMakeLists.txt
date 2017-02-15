cmake_minimum_required(VERSION 3.5)
project(netlib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp base/StringPiece.h net/Buffer.cpp net/Buffer.h base/TimeStamp.cpp base/TimeStamp.h base/TimeZone.cpp base/TimeZone.h base/Date.cpp base/Date.h base/Logger.cpp base/Logger.h net/EventLoop.cpp net/EventLoop.h base/Thread.cpp base/Thread.h net/Channel.cpp net/Channel.h net/Poller.cpp net/Poller.h net/Timer.cpp net/Timer.h net/TimerId.h net/Callbacks.h)
add_executable(netlib ${SOURCE_FILES})