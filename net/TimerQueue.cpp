/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description: 每次启动定时的只有队头的哪个元素,
*               超时后将所有超时元素都取出一并执行
**********************************************/

#include <sys/timerfd.h>
#include <strings.h>
#include <functional>
#include "TimerQueue.h"
#include "TimerId.h"
#include "EventLoop.h"

namespace Netlib {
    namespace NetlibTemp {
        // 创建timerfd
        int createTimerfd() {
            int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

            if (timefd < 0) {
                fprintf(stderr, "Failed create timerfd\n");
            }
            return timefd;
        }

        // 计算现在到超时还有多长时间
        struct timespec howMuchTimeFromNow(TimeStamp when) {
            int64_t microseconds = when.microSecondsSinceEpoch() - TimeStamp::now().microSecondsSinceEpoch();

            if (microseconds < 100) {
                microseconds = 100;
            }
            struct timespec ts;
            ts.tv_sec = static_cast<time_t > (microseconds / TimeStamp::kMicroSecondsPerSecond);
            ts.tv_nsec = static_cast<time_t > ((microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000);

            return ts;
        }

        // 重置timerfd,重置为从现在开始到超时时间还有多久,并启动定时
        void resetTimerfd(int timerfd, TimeStamp expiration) {
            struct itimerspec newValue;
            struct itimerspec oldValue;
            ::bzero(&newValue, sizeof(newValue));
            ::bzero(&oldValue, sizeof(oldValue));
            newValue.it_value = howMuchTimeFromNow(expiration);
            // 用来启动或关闭有fd指定的定时器,new_value:指定新的超时时间，
            // 设定new_value.it_value非零则启动定时器，否则关闭定时器，
            // 如果new_value.it_interval为0，则定时器只定时一次，即初始那次，否则之后每隔设定时间超时一次
            // old_value：不为null，则返回定时器这次设置之前的超时时间
            // flags：1代表设置的是绝对时间；为0代表相对时间
            int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
            if (ret) {
                fprintf(stderr, "timerfd_settime() in file %s %d lines.", __FILE__, __LINE__);
            }
        }

        // 读取超时次数(上次读取或者调用timerfd_settime到超时的次数)
        void readTimerfd(int timerfd, TimeStamp now) {
            uint64_t howmany;
            ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
            if (n != sizeof(howmany)) {
                fprintf(stderr, "TimerQueue::handleRead() reads %ld bytes instead of 8\n", howmany);
            }
        }
    }
}

Netlib::TimerQueue::TimerQueue(Netlib::EventLoop *loop)
        : loop_(loop), timerfd_(NetlibTemp::createTimerfd()), timerfdChannel_(loop, timerfd_), timers_() {
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

Netlib::TimerQueue::~TimerQueue() {
    ::close(timerfd_);

    for (auto &a : timers_) {
        delete a.second;
    }
}

Netlib::TimerId Netlib::TimerQueue::addTimer(const Netlib::TimerCallback &cb
        , Netlib::TimeStamp when, double interval) {
    Timer *timer = new Timer(cb, when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void Netlib::TimerQueue::addTimerInLoop(Netlib::Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        NetlibTemp::resetTimerfd(timerfd_, timer->expiration());
    }
}

std::vector<Netlib::TimerQueue::Entry> Netlib::TimerQueue::getExpired(Netlib::TimeStamp now) {
    assert(timers_.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer *> (UINTPTR_MAX));
    auto it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, std::back_inserter(expired));
    timers_.erase(timers_.begin(), it);

    for (auto &a : expired) {
        ActiveTimer timer(a.second, a.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1);
    }

    assert(timers_.size() == activeTimers_.size());
    return expired;
}

void Netlib::TimerQueue::reset(const std::vector<Netlib::TimerQueue::Entry> &expired, Netlib::TimeStamp now) {
    TimeStamp nextExpire;

    for (auto it = expired.begin(); it != expired.end(); ++it) {
        ActiveTimer timer(it->second, it->second->sequence());
        if (it->second->repeat() && (cancelingTimers_.find(timer) == cancelingTimers_.end())) {
            it->second->restart(now);
            insert(it->second);
        } else {
            delete it->second;
        }
    }

    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.vaild()) {
        NetlibTemp::resetTimerfd(timerfd_, nextExpire);
    }
}

bool Netlib::TimerQueue::insert(Netlib::Timer *timer) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();
    auto it = timers_.begin();

    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }

    {
        auto result = timers_.insert(std::make_pair(when, timer));
        assert(result.second);
    }
    {
        auto result = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
    }

    assert(timers_.size() == activeTimers_.size());

    return earliestChanged;
}

void Netlib::TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    TimeStamp now(TimeStamp::now());
    NetlibTemp::readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for (auto it = expired.begin(); it != expired.end(); ++it) {
        it->second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

void Netlib::TimerQueue::canel(Netlib::TimerId timerId) {
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void Netlib::TimerQueue::cancelInLoop(Netlib::TimerId timerId) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.value_, timerId.sequence_);
    auto it = activeTimers_.find(timer);

    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1);
        delete it->first;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(timer);
    }

    assert(timers_.size() == activeTimers_.size());
}
