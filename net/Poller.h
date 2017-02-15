/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_POLLER_H
#define NETLIB_POLLER_H

#include <vector>
#include <map>
#include "Channel.h"
#include "../base/TimeStamp.h"

namespace Netlib {
    class Poller {
    public:
        // ChannelList
        typedef std::vector<Channel *> ChannelList;

        // 不可复制的类
        Poller(const Poller &) = delete;
        Poller &operator=(const Poller &) = delete;

        // 构造函数
        Poller(EventLoop *loop);

        // 析构函数
        ~Poller();

        // Poller的核心函数
        TimeStamp poll(int timeoutMs, ChannelList *activeChannels);

        // 改变注册事件的接口
        void updateChannel(Channel *channel);

        // 断言事件循环是否是在本线程中
        void assertInLoopThread();

    private:
        // 处理poll返回后有时间的返回的事情
        void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
        typedef std::vector<struct pollfd> PollFdList;  // poll的pollfd数组,使用时将vector转换为C数组
        typedef std::map<int, Channel *> ChannelMap;    // fd到Channel的映射

        EventLoop *ownerLoop_;
        PollFdList pollFds_;
        ChannelMap channels_;
    };
}

#endif //NETLIB_POLLER_H
