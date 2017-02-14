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

__thread Netlib::EventLoop *t_loopInThisThread = 0;

namespace Netlib {

    EventLoop::EventLoop(int ms = 1000) : looping_(false), kPollTimeMs_(ms),
                                          threadId_(Thread::convertIdToInt(std::this_thread::get_id())) {
        if (t_loopInThisThread) {
            ::printf("已存在EventLoop\n");
            abort();
        } else {
            t_loopInThisThread = this;
        }
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
            poller_->poll(kPollTimeMs_, &activeChannels);
            // 遍历发生事件的Channel,并分发事件
            for (auto it = activeChannels.begin();  it < activeChannels.end(); ++it) {
                (*it)->handleEvent();
            }
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
        // FIXME 加上唤醒操作
    }

    void EventLoop::updateChannel(Channel *channel) {
        // 断言channel是否属于本EventLoop
        assert(channel->ownerLoop() == this);
        // 断言本EventLoop是否属于本线程
        assertInLoopThread();
        poller_->updateChannel(channel);
    }

}
