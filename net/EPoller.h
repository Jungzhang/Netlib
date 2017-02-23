/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_EPOLLER_H
#define NETLIB_EPOLLER_H

#include <vector>
#include <map>
#include <sys/epoll.h>
#include "EventLoop.h"

namespace Netlib {



    class Channel;

    class EPoller {
    public:
        typedef std::vector<Channel *> ChannelList;
        EPoller(const EPoller &) = delete;
        EPoller &operator=(const EPoller &) = delete;

        EPoller(EventLoop *loop);
        ~EPoller();

        TimeStamp poll(int timeoutMs, ChannelList *activeChannels);
        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        void assertInLoopThread();

    private:
        static const int kInitEventListSize = 16;

        void fillActiveChannels(int numEvents, ChannelList *activeChannels) const ;
        void update(int operation, Channel *channel);

        typedef std::vector<struct epoll_event> EventList;
        typedef std::map<int, Channel *> ChannelMap;

    private:
        EventLoop *ownerLoop_;      // 所属的事件循环
        int epollfd_;               // epoll的fd
        EventList events_;          // epoll_wait返回的活动fd列表
        ChannelMap channels_;       // fd和Channel的映射
    };
}

#endif //NETLIB_EPOLLER_H
