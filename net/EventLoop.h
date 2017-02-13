/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_EVENTLOOP_H
#define NETLIB_EVENTLOOP_H

#include <atomic>
#include <signal.h>

namespace Netlib {
    class EventLoop {
    public:
        EventLoop();
        ~EventLoop();
        void loop();

        // 断言此loop是否在本线程中
        void assertInLoopThread();
        // 返回loop是否在本线程中
        bool isInLoopThread() const ;
        // 获取本线程中的EventLoop对象指针
        static EventLoop *getEventLoopOfCurrentThread();

    private:
        // 如果本EventLoop不在本线程则断言
        void abortNotLoopThread();

    private:
        std::atomic_bool looping_;  // 是否运行,原子变量
        const pthread_t threadId_;
    };
}


#endif //NETLIB_EVENTLOOP_H
