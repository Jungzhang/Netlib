/*************************************************************************
	> File Name: testLogLib.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description: 测试多线程异步日志库
 ************************************************************************/
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <atomic>
#include "../../base/Logger.h"
#include "../../net/EventLoop.h"

Netlib::Logger logTest;
const char *data = "This is test";
std::atomic_bool isRunig;
Netlib::EventLoop *g_ptr;

void testFunc() {
    while (isRunig) {
        logTest.append(data, strlen(data), Netlib::Logger::LogLevel::INFO);
    }
}

void stop() {
    isRunig = false;
    g_ptr->quit();
}

int main(int argc, char *argv[]) {
    logTest.setLogFileInfo("Test");
    Netlib::EventLoop eventLoop(-1);
    g_ptr = &eventLoop;
    isRunig = true;
    eventLoop.runAfter(1, std::bind(stop));
    std::thread t1(testFunc);
    eventLoop.loop();

    t1.join();

    return EXIT_SUCCESS;
}
