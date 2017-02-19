/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <thread>
#include <assert.h>
#include "Poller.h"
#include "EventLoop.h"
#include "../base/Thread.h"
#include "TimerQueue.h"
#include <mutex>
#include <sys/eventfd.h>

__thread Netlib::EventLoop *t_loopInThisThread = 0;

int ::Netlib::createEventfd() {
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    if (fd < 0) {
        fprintf(stderr, "eventfd create filed");
        abort();
    }

    return fd;
}

namespace Netlib {

    EventLoop::EventLoop(int ms) : looping_(false),
                                   kPollTimeMs_(ms),
                                   poller_(new Poller(this)),
                                   wakeupFd_(createEventfd()),
                                   timerQueue_(new TimerQueue(this)),
                                   wakeupChannel_(new Channel(this, wakeupFd_)),
                                   threadId_(Thread::convertIdToInt(std::this_thread::get_id())) {
        if (t_loopInThisThread) {
            ::printf("已存在EventLoop\n");
            abort();
        } else {
            t_loopInThisThread = this;
        }
        wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
        wakeupChannel_->enableReading();
    }

    EventLoop::~EventLoop() {
        assert(!looping_);
        t_loopInThisThread = nullptr;
    }

    void EventLoop::loop() {
        assert(!looping_);
        assertInLoopThread();
        looping_ = true;
        quit_ = false;

        while (!quit_) {
            activeChannels.clear();
            pollReturnTime_ = poller_->poll(kPollTimeMs_, &activeChannels);
            // 遍历发生事件的Channel,并分发事件
            for (auto it = activeChannels.begin(); it != activeChannels.end(); ++it) {
                (*it)->handleEvent(pollReturnTime_);
            }
            doPendingFunctors();
        }

        looping_ = false;
    }

    EventLoop *EventLoop::getEventLoopOfCurrentThread() {
        return t_loopInThisThread;
    }

    void EventLoop::assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotLoopThread();
        }
    }

    bool EventLoop::isInLoopThread() const {
        return threadId_ == Thread::convertIdToInt(std::this_thread::get_id());
    }

    void EventLoop::abortNotLoopThread() {
        ::printf("EventLoop is created in thread %ld but now in thread %ld", threadId_,
                 Thread::convertIdToInt(std::this_thread::get_id()));
        abort();
    }

    void EventLoop::quit() {
        quit_ = true;
        if (!isInLoopThread()) {
            wakeup();
        }
    }

    void EventLoop::updateChannel(Channel *channel) {
        // 断言channel是否属于本EventLoop
        assert(channel->ownerLoop() == this);
        // 断言本EventLoop是否属于本线程
        assertInLoopThread();
        poller_->updateChannel(channel);
    }

    TimerId EventLoop::runAt(const TimeStamp &time, const TimerCallback &cb) {
        return timerQueue_->addTimer(cb, time, 0.0);
    }

    TimerId EventLoop::runAfter(double delay, const TimerCallback &cb) {
        TimeStamp timeStamp(addTime(TimeStamp::now(), delay));
        return runAt(timeStamp, cb);
    }

    TimerId EventLoop::runEvery(double interval, const TimerCallback &cb) {
        TimeStamp timeStamp(addTime(TimeStamp::now(), interval));
        return timerQueue_->addTimer(cb, timeStamp, interval);
    }

    void EventLoop::canel(TimerId timerId) {
        //TODO 待实现
    }

    TimeStamp EventLoop::pollReturnTime() const {
        return pollReturnTime_;
    }

    void EventLoop::runInLoop(const EventLoop::Functor &cb) {
        // 如果传入的函数在IO中则直接执行,否则加入队列
        if (isInLoopThread()) {
            cb();
        } else {
            queueInLoop(cb);
        }
    }

    void EventLoop::queueInLoop(const EventLoop::Functor &cb) {
        // 先获得锁,再放入队列
        {
            std::lock_guard<std::mutex> locker(mutex_);
            pendingFunctors_.push_back(cb);
        }

        if (!isInLoopThread() || callingPendingFunctors_) {
            wakeup();
        }

    }

    void EventLoop::wakeup() {
        uint64_t one = 1;
        ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
        if (n != sizeof(one)) {
            fprintf(stderr, "wakeup write %ld bayes wakeFd file in %s %d lines", n, __FILE__, __LINE__);
        }
    }

    void EventLoop::handleRead() {
        uint64_t one = 1;
        ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
        if (n != sizeof(one)) {
            fprintf(stderr, "handleRead read %ld bayes wakeFd file in %s %d lines", n, __FILE__, __LINE__);
        }
    }

    void EventLoop::doPendingFunctors() {
        std::vector<Functor> functors;
        callingPendingFunctors_ = true;
        {
            std::lock_guard<std::mutex> locker(mutex_);
            functors.swap(pendingFunctors_);
        }

        for (size_t i = 0; i < functors.size(); ++i) {
            functors[i]();
        }
        callingPendingFunctors_ = false;
    }

    void EventLoop::removeChannel(Channel *channel) {
        assert(channel->ownerLoop() == this);
        assertInLoopThread();
        poller_->removeChannel(channel);
    }
}
