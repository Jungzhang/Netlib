/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <cassert>
#include <vector>
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

namespace Netlib {
    EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop) : baseLoop_(baseLoop)
            , started_(false), numThreads_(0), next_(0) { }

    EventLoopThreadPool::~EventLoopThreadPool() { }

    void EventLoopThreadPool::setThreadNum(int numThreads) {
        numThreads_ = numThreads;
    }

    void EventLoopThreadPool::start() {
        assert(!started_);
        baseLoop_->assertInLoopThread();

        started_ = true;

        // 创建EventLoopThread对象(相当于创建了EventLoop线程)
        for (int i = 0; i < numThreads_; ++i) {
            // 创建EvEntLoop线程对象
            std::unique_ptr<EventLoopThread> t(new EventLoopThread);
            t->setTimeoutMs(ms_);
            threads_.push_back(std::move(t));
            // 保存创建出来的EventLoop的指针
            loops_.push_back(threads_[threads_.size() - 1]->startLoop());
        }

    }

    EventLoop *EventLoopThreadPool::getNextLoop() {
        baseLoop_->assertInLoopThread();
        EventLoop *loop = baseLoop_;

        if (!loops_.empty()) {
            loop = loops_[next_];
            ++next_;
            if (static_cast<size_t >(next_) >= loops_.size()) {
                next_ = 0;
            }
        }
        return loop;
    }
}