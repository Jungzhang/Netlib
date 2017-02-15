/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMERQUEUE_H
#define NETLIB_TIMERQUEUE_H

#include <set>
#include <memory>
#include "Callbacks.h"
#include "../base/TimeStamp.h"
#include "Channel.h"
#include "Timer.h"

namespace Netlib {

    class EventLoop;
    class Timer;
    class TimerId;

    class TimerQueue {

    public:
        TimerQueue(const TimerQueue &) = delete;
        TimerQueue &operator=(const TimerQueue &) = delete;

        TimerQueue(EventLoop *loop);
        ~TimerQueue();

        // 撤销定时任务
//        void canel(TimerId timerId);
        // 添加定时任务
        TimerId addTimer(const TimerCallback &cb, TimeStamp when, double interval);

    private:
        typedef std::pair<TimeStamp, Timer *>  Entry;
        typedef std::set<Entry> TimerList;

        // 分发事件
        void handleRead();
        // 从timers中删除已到期的timer,并返回出来
        std::vector<Entry> getExpired(TimeStamp now);
        void reset(const std::vector<Entry> &expired, TimeStamp now);

        bool insert(Timer *timer);

        EventLoop *loop_;
        const int timerfd_;
        Channel timerfdChannel_;
        TimerList timers_;

    };
}

#endif //NETLIB_TIMERQUEUE_H
