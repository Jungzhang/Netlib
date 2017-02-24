/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "EventLoopThread.h"
#include "EventLoop.h"

Netlib::EventLoopThread::EventLoopThread() : loop_(nullptr), exiting_(false), mutex_() { }

Netlib::EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    loop_->quit();
    thread_->join();
}

Netlib::EventLoop *Netlib::EventLoopThread::startLoop() {
    thread_.reset(new std::thread(std::bind(&EventLoopThread::threadFunc, this, ms_)));
    {
        std::lock_guard<std::mutex> locker(mutex_);
        while (loop_ == nullptr) {
            cond_.wait(mutex_);
        }
    }

    return loop_;
}

void Netlib::EventLoopThread::threadFunc(int ms) {
    EventLoop loop;
    {
        std::lock_guard<std::mutex> locker(mutex_);
        loop.setTimeoutMs(ms);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop.loop();
}
