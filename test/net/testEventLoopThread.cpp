/*************************************************************************
	> File Name: testEventLoopThread.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description:
 ************************************************************************/
#include <iostream>
#include <zconf.h>
#include <thread>
#include "../../net/EventLoop.h"
#include "../../net/EventLoopThread.h"
#include "../../base/Thread.h"

void func() {
    printf("func(), pid = %d, tid = %ld\n", getpid(), Netlib::Thread::convertIdToInt(std::this_thread::get_id()));
}

int main() {

    printf("main(), pid = %d, tid = %ld\n", getpid(), Netlib::Thread::convertIdToInt(std::this_thread::get_id()));

    Netlib::EventLoopThread eventLoopThread;
    Netlib::EventLoop *eventLoop = eventLoopThread.startLoop();
    eventLoop->runInLoop(func);
    sleep(1);
    eventLoop->runAfter(2, func);
    sleep(3);
    eventLoop->quit();
    printf("exit");

    return 0;
}