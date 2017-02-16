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
#include "TimerId.h"
#include "Callbacks.h"
#include "../base/TimeStamp.h"

namespace Netlib {
    class Channel;
    class Poller;
    class TimerQueue;

    class EventLoop {
    public:

        // 删除复制构造函数
        EventLoop(const EventLoop &) = delete;
        EventLoop &operator=(const EventLoop &) = delete;

        EventLoop(int ms  = 1000);
        ~EventLoop();
        void loop();
        void quit();

        // 断言此loop是否在本线程中
        void assertInLoopThread();
        // 返回loop是否在本线程中
        bool isInLoopThread() const ;
        // 获取本线程中的EventLoop对象指针
        static EventLoop *getEventLoopOfCurrentThread();

        // 定时器相关
        // 在指定的时间调用超时事件的回调函数
        TimerId runAt(const TimeStamp &time, const TimerCallback &cb);
        // 在等一段时间调用超时事件的回调函数
        TimerId runAfter(double delay, const TimerCallback &cb);
        // 以固定的时间间隔反复调用超时回调函数
        TimerId runEvery(double interval, const TimerCallback &cb);
        // 取消timer
        void canel(TimerId timerId);
        // 更新Channel
        void updateChannel(Channel *channel);

    private:
        // 如果本EventLoop不在本线程则断言
        void abortNotLoopThread();

    private:
        // Poller中会用到
        typedef std::vector<Channel *> ChannelList;
        std::atomic_bool quit_;                     // 退出loop
        std::atomic_bool looping_;                  // 是否运行,原子变量
        const pthread_t threadId_;                  // 所属线程的ID
        ChannelList activeChannels;                 // 保存有活动的Channel列表
        std::unique_ptr<Poller> poller_;            // 拥有的Poller对象
        std::unique_ptr<TimerQueue> timerQueue_;    // 拥有的定时器对象
        const int kPollTimeMs_;                     // Poller的超时时间
    };
}


#endif //NETLIB_EVENTLOOP_H
