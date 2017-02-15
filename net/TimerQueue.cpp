/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <sys/timerfd.h>
#include <strings.h>
#include <assert.h>
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

        // 重置timerfd
        void resetTimerfd(int timerfd, TimeStamp expiration) {
            struct itimerspec newValue;
            struct itimerspec oldValue;
            ::bzero(&newValue, sizeof(newValue));
            ::bzero(&oldValue, sizeof(oldValue));
            newValue.it_value = howMuchTimeFromNow(expiration);
            int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
            if (ret) {
                fprintf(stderr, "timerfd_settime() in file %s %d lines.", __FILE__, __LINE__);
            }
        }

        void readTimerfd(int timerfd, TimeStamp now) {
            uint64_t howmany;
            ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
            fprintf(stdout, "TimerQueue::handleRead() %ld at %s\n", howmany, now.toString().c_str());
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

Netlib::TimerId Netlib::TimerQueue::addTimer(const Netlib::TimerCallback &cb, Netlib::TimeStamp when, double interval) {
    Timer *timer = new Timer(cb, when, interval);
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged) {
        NetlibTemp::resetTimerfd(timerfd_, timer->expiration());
    }
    return TimerId(timer);
}

std::vector<Netlib::TimerQueue::Entry> Netlib::TimerQueue::getExpired(Netlib::TimeStamp now) {
    std::vector<Entry> expired;
    Entry sentry =std::make_pair(now, reinterpret_cast<Timer *> (UINTPTR_MAX));
    auto it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, std::back_inserter(expired));
    timers_.erase(timers_.begin(), it);

    return expired;
}

void Netlib::TimerQueue::reset(const std::vector<Netlib::TimerQueue::Entry> &expired, Netlib::TimeStamp now) {
    TimeStamp nextExpire;

    for(auto it = expired.begin(); it != expired.end(); ++it) {
        if (it->second->repeat()) {
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
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();
    auto it = timers_.begin();

    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }

    std::pair<TimerList::iterator, bool> result = timers_.insert(std::make_pair(when, timer));
    assert(result.second);

    return earliestChanged;
}

void Netlib::TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    TimeStamp now(TimeStamp::now());
    NetlibTemp::readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    for (auto it = expired.begin(); it != expired.end(); ++it) {
        it->second->run();
    }

    reset(expired, now);
}
