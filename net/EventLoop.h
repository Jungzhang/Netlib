/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_EVENTLOOP_H
#define NETLIB_EVENTLOOP_H

#include <atomic>
#include <signal.h>
#include <vector>
#include <memory>

namespace Netlib {
    class Channel;
    class Poller;

    class EventLoop {
    public:

        // 删除复制构造函数
        EventLoop(const EventLoop &) = delete;
        EventLoop &operator=(const EventLoop &) = delete;

        EventLoop(int ms);
        ~EventLoop();
        void loop();
        void quit();

        // 断言此loop是否在本线程中
        void assertInLoopThread();
        // 返回loop是否在本线程中
        bool isInLoopThread() const ;
        // 获取本线程中的EventLoop对象指针
        static EventLoop *getEventLoopOfCurrentThread();

        void updateChannel(Channel *channel);

    private:
        // 如果本EventLoop不在本线程则断言
        void abortNotLoopThread();

    private:
        // Poller中会用到
        typedef std::vector<Channel *> ChannelList;
        std::atomic_bool quit_;         // 退出loop
        std::atomic_bool looping_;      // 是否运行,原子变量
        const pthread_t threadId_;      // 所属线程的ID
        ChannelList activeChannels;     // 保存有活动的Channel列表
        std::unique_ptr<Poller> poller_;
        const int kPollTimeMs_;
    };
}


#endif //NETLIB_EVENTLOOP_H
