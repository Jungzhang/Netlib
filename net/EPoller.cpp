/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <sys/epoll.h>
#include "EPoller.h"
#include "Channel.h"

namespace Netlib {
    const int kNew = -1;
    const int kAdded = 1;
    const int kDelete = 2;
}

Netlib::EPoller::EPoller(Netlib::EventLoop *loop)
        : ownerLoop_(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)), events_(kInitEventListSize){
    if (epollfd_ < 0) {
        fprintf(stderr, "epoll create filed\n");
        abort();
    }
}

Netlib::EPoller::~EPoller() {
    ::close(epollfd_);
}

Netlib::TimeStamp Netlib::EPoller::poll(int timeoutMs, Netlib::EPoller::ChannelList *activeChannels) {
    int numEvents = ::epoll_wait(epollfd_, events_.data(), static_cast<int>(events_.size()), timeoutMs);
    TimeStamp now(TimeStamp::now());

    if (numEvents < 0) {
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents < 0) {
        fprintf(stderr, "epoll filed\n");
    }

    return now;
}

void Netlib::EPoller::updateChannel(Netlib::Channel *channel) {
    assertInLoopThread();
    const int index = channel->index();
    if (index == kNew || index ==kDelete) {
        int fd = channel->fd();
        if (index == kNew) {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        } else {
            assert(channels_.find(fd) != channels_.end());
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDelete);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Netlib::EPoller::removeChannel(Netlib::Channel *channel) {
    assertInLoopThread();
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDelete);
    size_t n = channels_.erase(fd);
    assert(n == 1);

    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void Netlib::EPoller::assertInLoopThread() {
    ownerLoop_->assertInLoopThread();
}

void Netlib::EPoller::fillActiveChannels(int numEvents, Netlib::EPoller::ChannelList *activeChannels) const {
    assert(numEvents <= events_.size());
    for (int i = 0; i < numEvents; ++i) {
        Channel *channel = static_cast<Channel *> (events_[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
#endif
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void Netlib::EPoller::update(int operation, Netlib::Channel *channel) {
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = static_cast<uint32_t >(channel->events());
    event.data.ptr = channel;
    int fd = channel->fd();

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        fprintf(stderr, "epoll_ctl op = %d, fd = %d\n", operation, fd);
        if (operation != EPOLL_CTL_DEL) {
            abort();
        }
    }
}
