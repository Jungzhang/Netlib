/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_EVENTLOOP_H
#define NETLIB_EVENTLOOP_H

#include <atomic>
#include <signal.h>
#include <vector>
#include <memory>
#include <mutex>
#include "TimerId.h"
#include "Callbacks.h"
#include "../base/TimeStamp.h"

namespace Netlib {

    static int createEventfd();

    class Channel;
    class Poller;
    class TimerQueue;

    class EventLoop {
    public:

        // 删除复制构造函数
        EventLoop(const EventLoop &) = delete;
        EventLoop &operator=(const EventLoop &) = delete;

        // 从非IO线程挪进IO线程的任务
        typedef std::function<void ()> Functor;

        EventLoop(int ms  = 1000);
        ~EventLoop();
        void loop();
        void quit();

        // 返回poll返回的时刻
        TimeStamp pollReturnTime() const ;

        // 在IO线程中执行任务
        void runInLoop(const Functor &cb);
        // 对外界任务队列的操作
        void queueInLoop(const Functor &cb);

        // 断言此loop是否在本线程中
        void assertInLoopThread();
        // 返回loop是否在本线程中
        bool isInLoopThread() const ;
        // 获取本线程中的EventLoop对象指针
        static EventLoop *getEventLoopOfCurrentThread();

        // 定时器相关
        // 在指定的时间调用超时事件的回调函数
        TimerId runAt(const TimeStamp &time, const TimerCallback &cb);
        // 在等一段时间调用超时事件的回调函数
        TimerId runAfter(double delay, const TimerCallback &cb);
        // 以固定的时间间隔反复调用超时回调函数
        TimerId runEvery(double interval, const TimerCallback &cb);

        // 唤醒IO线程
        void wakeup();

        // 取消timer
        void canel(TimerId timerId);
        // 更新Channel
        void updateChannel(Channel *channel);
        // 删除Channel
        void removeChannel(Channel *channel);

    private:
        // 如果本EventLoop不在本线程则断言
        void abortNotLoopThread();
        // 唤醒后的事件回调
        void handleRead();
        // 执行任务
        void doPendingFunctors();

    private:
        // Poller中会用到
        typedef std::vector<Channel *> ChannelList;
        std::atomic_bool quit_;                     // 退出loop
        std::atomic_bool looping_;                  // 是否运行,原子变量
        const pthread_t threadId_;                  // 所属线程的ID
        TimeStamp pollReturnTime_;                  // poll返回的时间
        ChannelList activeChannels;                 // 保存有活动的Channel列表
        std::unique_ptr<Poller> poller_;            // 拥有的Poller对象
        std::unique_ptr<TimerQueue> timerQueue_;    // 拥有的定时器对象
        const int kPollTimeMs_;                     // Poller的超时时间
        std::atomic_bool callingPendingFunctors_;   // 是否正在调用从用户线程中挪过来的任务
        int wakeupFd_;                              // 唤醒IO线程的文件描述符
        std::unique_ptr<Channel> wakeupChannel_;    // 唤醒后的事件分发器
        std::mutex mutex_;                          // 保护pendingFunctors_的锁
        std::vector<Functor> pendingFunctors_;      // 从用户线程中挪进IO线程的任务
    };
}


#endif //NETLIB_EVENTLOOP_H
