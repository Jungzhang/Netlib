/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_EVENTLOOPTHREAD_H
#define NETLIB_EVENTLOOPTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

namespace Netlib{

    // 前向声明
    class EventLoop;

    class EventLoopThread {
    public:
        // 不可复制类
        EventLoopThread(const EventLoopThread&) = delete;
        EventLoopThread &operator=(const EventLoopThread&) = delete;
        // 构造函数
        EventLoopThread();
        // 析构函数
        ~EventLoopThread();
        // 启动事件循环
        EventLoop *startLoop();

        // 设置超时
        void setTimeoutMs(int ms) {
            ms_ = ms;
        }

    private:
        // 线程函数
        void threadFunc(int ms);

    private:
        EventLoop *loop_;
        bool exiting_;
        std::unique_ptr<std::thread > thread_;
        std::mutex mutex_;
        std::condition_variable_any cond_;
        int ms_ = 1000;
    };
}

#endif //NETLIB_EVENTLOOPTHREAD_H
