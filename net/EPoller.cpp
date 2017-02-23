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

}

void Netlib::EPoller::fillActiveChannels(int numEvents, Netlib::EPoller::ChannelList *activeChannels) const {

}

void Netlib::EPoller::update(int operation, Netlib::Channel *channel) {
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        fprintf(stderr, "epoll_ctl op = %d, fd = %d\n", operation, fd);
        if (operation != EPOLL_CTL_DEL) {
            abort();
        }
    }
}
