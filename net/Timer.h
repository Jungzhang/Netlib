/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMER_H
#define NETLIB_TIMER_H

#include <atomic>
#include "Callbacks.h"
#include "../base/TimeStamp.h"

namespace Netlib{
    class Timer {
    public:
        // 不可复制
        Timer(const Timer &) = delete;
        Timer &operator=(const Timer &) = delete;

        Timer(const TimerCallback &cb, TimeStamp when, double interval);

        // 运行定时事件回调
        void run() const ;

        // 获得到期时间戳
        TimeStamp expiration() const ;

        // 获得是否重复
        bool repeat() const ;

        // 获得序号
        int64_t sequence();

        // 重启
        void restart(TimeStamp now);

    private:
        const TimerCallback callback_;  // 定时事件发生后的回调函数
        TimeStamp expiration_;  // 到期时间戳
        const double interval_;  // 间隔
        const bool repeat_;     // 是否重复
        const int64_t sequence_; // 标志位,区分timerId对象在地址相同但是时间不同的情况
        static std::atomic<int64_t > s_numberCreated_;
    };
}

#endif //NETLIB_TIMER_H
