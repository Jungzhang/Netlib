/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <iostream>
#include <assert.h>
#include "Poller.h"
#include "EventLoop.h"

Netlib::Poller::Poller(Netlib::EventLoop *loop) : ownerLoop_(loop) { }

Netlib::Poller::~Poller() { }

Netlib::TimeStamp Netlib::Poller::poll(int timeoutMs, Netlib::Poller::ChannelList *activeChannels) {

    // 返回调用完成poll时的时间
    int numEvents = ::poll(pollFds_.data(), pollFds_.size(), timeoutMs);
    TimeStamp now(TimeStamp::now());

    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents < 0) {
        fprintf(stderr, "poll error in file %s %d lines\n", __FILE__, __LINE__);
    }

    return now;
}

void Netlib::Poller::updateChannel(Netlib::Channel *channel) {
    assertInLoopThread();
    // 如果是小于0则表示这是一个新的Channel对象,需要添加进map中
    if (channel->index() < 0) {
        // 如果是新的表明没有在map中,所以此处判断其要等于end
        assert(channels_.find(channel->fd()) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollFds_.push_back(pfd);
        int idx = static_cast<int> (pollFds_.size()) - 1;
        channel->setIndex(idx);
        channels_[pfd.fd] = channel;
    } else {
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<short >(pollFds_.size()));
        struct pollfd &pfd = pollFds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short >(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            // 置为负数,让poll忽略
            pfd.fd = -1;
        }
    }
}

void Netlib::Poller::assertInLoopThread() {
    ownerLoop_->assertInLoopThread();
}

void Netlib::Poller::fillActiveChannels(int numEvents, Netlib::Poller::ChannelList *activeChannels) const {
    // 遍历fds数组
    for (auto pfd = pollFds_.begin(); pfd != pollFds_.end() && numEvents; ++pfd) {
        // 如果该fd上有事件发生
        if (pfd->revents > 0) {
            --numEvents;
            auto ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            // 取出对应的Channel的地址
            Channel *channel = ch->second;
            activeChannels->push_back(channel);
        }
    }
}
