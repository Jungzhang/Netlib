/*************************************************************************
	> File Name: testEventLoop.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description: 测试EventLoop
 ************************************************************************/
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include "../../net/EventLoop.h"
#include "../../base/Thread.h"

void threadFunc() {
    printf("pid = %d, tid = %ld\n", getpid(), Netlib::Thread::convertIdToInt(std::this_thread::get_id()));
    Netlib::EventLoop eventLoop;
    eventLoop.loop();
}

int main(int argc, char *argv[])
{
    std::thread t1(threadFunc);
    std::thread t2(threadFunc);
    std::thread t3(threadFunc);
    t1.join();
    t2.join();
    t3.join();

    return EXIT_SUCCESS;
}
