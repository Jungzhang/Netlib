cmake_minimum_required(VERSION 3.5)
project(netlib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

set(SOURCE_FILES main.cpp base/StringPiece.h net/Buffer.cpp net/Buffer.h base/TimeStamp.cpp base/TimeStamp.h base/TimeZone.cpp base/TimeZone.h base/Date.cpp base/Date.h base/Logger.cpp base/Logger.h net/EventLoop.cpp net/EventLoop.h base/Thread.cpp base/Thread.h net/Channel.cpp net/Channel.h net/Poller.cpp net/Poller.h net/Timer.cpp net/Timer.h net/TimerId.h net/Callbacks.h net/TimerQueue.cpp net/TimerQueue.h net/EventLoopThread.cpp net/EventLoopThread.h net/Acceptor.cpp net/Acceptor.h net/Socket.cpp net/Socket.h net/InetAddress.cpp net/InetAddress.h net/SocketOps.cpp net/SocketOps.h net/TcpServer.cpp net/TcpServer.h net/TcpConnection.cpp net/TcpConnection.h)
add_executable(netlib ${SOURCE_FILES})