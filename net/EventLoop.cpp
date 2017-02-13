/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <thread>
#include <assert.h>
#include <poll.h>
#include "EventLoop.h"
#include "../base/Thread.h"

__thread Netlib::EventLoop *t_loopInThisThread = 0;

namespace Netlib {

    namespace NetlibTemp {

    }

    EventLoop::EventLoop() : looping_(false), threadId_(Thread::convertIdToInt(std::this_thread::get_id())) {
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

        ::poll(nullptr, 0, 2 * 1000);

        ::printf("EventLoop is stop in %p\n", this);
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

}
