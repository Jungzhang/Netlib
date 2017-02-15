/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMER_H
#define NETLIB_TIMER_H

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

        // 重启
        void restart(TimeStamp now);

    private:
        const TimerCallback callback_;  // 定时事件发生后的回调函数
        TimeStamp expiration_;  // 到期时间戳
        const double interval_;  // 间隔
        const bool repeat_;     // 是否重复
    };
}

#endif //NETLIB_TIMER_H
