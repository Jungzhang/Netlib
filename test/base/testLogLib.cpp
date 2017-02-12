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

Netlib::Logger logTest;
const char *data = "This is test";
std::atomic_bool isRunig ;

void testFunc() {
    while (true) {
        if (!isRunig) {
            break;
        }
        logTest.append(data, strlen(data), Netlib::Logger::LogLevel::INFO);
    }
}

int main(int argc, char *argv[]) {
    logTest.setLogFileInfo("Test");
    isRunig = true;
    std::thread t1(testFunc);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    isRunig = false;
    t1.join();

    return EXIT_SUCCESS;
}
