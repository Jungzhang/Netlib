/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "Timer.h"

Netlib::Timer::Timer(const Netlib::TimerCallback &cb, Netlib::TimeStamp when, double interval)
        : callback_(cb), expiration_(when), interval_(interval), repeat_(interval > 0.0) { }

void Netlib::Timer::run() const {
    callback_();
}

Netlib::TimeStamp Netlib::Timer::expiration() const {
    return expiration_;
}

bool Netlib::Timer::repeat() const {
    return repeat_;
}

void Netlib::Timer::restart(Netlib::TimeStamp now) {
    if (repeat_) {
        expiration_ = addTime(now, interval_);   // 加上interval_秒
    } else {
        expiration_ = TimeStamp::invalid();      // 返回一个无效时间戳
    }
}
