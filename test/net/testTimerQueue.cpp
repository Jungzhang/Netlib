#include <iostream>
#include "../../net/EventLoop.h"
#include "../../base/TimeStamp.h"

int count = 0;
Netlib::EventLoop *g_ptr;

void print(const char *msg) {
    printf("%s  时间：%s\n", msg, Netlib::TimeStamp::now().toString().c_str());
    ++count;
    if (count == 20) {
        g_ptr->quit();
    }
}

int main() {

    Netlib::EventLoop eventLoop(-1);
    g_ptr = &eventLoop;
    printf("时间：%s\n", Netlib::TimeStamp::now().toString().c_str());
    eventLoop.runAfter(1, std::bind(print, "1秒"));
    eventLoop.runAfter(2, std::bind(print, "2秒"));
    eventLoop.runEvery(1, std::bind(print, "Every1"));

    eventLoop.loop();

    print("main");

    sleep(1);

    return 0;
}