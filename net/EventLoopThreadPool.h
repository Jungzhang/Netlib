/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_EVENTLOOPPOOL_H
#define NETLIB_EVENTLOOPPOOL_H

#include <bits/unique_ptr.h>
#include <vector>

namespace Netlib {

    class EventLoop;
    class EventLoopThread;

    class EventLoopThreadPool {
    public:
        EventLoopThreadPool(const EventLoopThreadPool &) = delete;
        EventLoopThreadPool &operator=(const EventLoopThreadPool &) = delete;

        EventLoopThreadPool(EventLoop *baseLoop);
        ~EventLoopThreadPool();

        void setThreadNum(int numThreads);
        void start();
        EventLoop *getNextLoop();

    private:
        EventLoop *baseLoop_;
        bool started_;
        int numThreads_;
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop *> loops_;
    };
}

#endif //NETLIB_EVENTLOOPPOOL_H
