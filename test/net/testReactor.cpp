/*************************************************************************
	> File Name: testReactor.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description: 测试EventLoop/Channel/Poller类构成的Reactor模型
 ************************************************************************/
#include <iostream>
#include "../../net/EventLoop.h"
#include "../../net/Channel.h"
#include <cstdlib>
#include <unistd.h>

Netlib::EventLoop *g_loop;

void readEvent(char *buf) {
    printf("readEvent = %p\n", buf);
    read(1, buf, 256);
    printf("可读数据是：%s\n", buf);
    g_loop->quit();
}

int main(int argc, char *argv[])
{
    char buf[256] = {0,0,0};
    printf("main = %p\n", buf);
    Netlib::EventLoop loop(-1);
    g_loop = &loop;
    Netlib::Channel channel(&loop, 1);
    channel.setReadCallback(std::bind(readEvent, buf));
    channel.enableReading();
    loop.loop();

    return EXIT_SUCCESS;
}