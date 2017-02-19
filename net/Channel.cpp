/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <assert.h>
#include "EventLoop.h"
#include "Poller.h"

Netlib::Channel::Channel(Netlib::EventLoop *loop, int fd) : eventLoop_(loop), fd_(fd), revents_(0), events_(0),
                                                            index_(-1) {}

void Netlib::Channel::handleEvent(TimeStamp receiveTime) {

    eventHandling_ = true;

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & (POLLNVAL | POLLERR)) {
        if (errorCallback_) errorCallback_();
    }

    if (revents_ & (POLLPRI | POLLIN)) {
        if (readCallback_) readCallback_(receiveTime);
    }

    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }

    eventHandling_ = false;
}

void Netlib::Channel::setReadCallback(const Netlib::Channel::ReadEventCallback &cb) {
    readCallback_ = cb;
}

void Netlib::Channel::setWriteCallback(const Netlib::Channel::EventCallback &cb) {
    writeCallback_ = cb;
}

void Netlib::Channel::serErrorCallback(const Netlib::Channel::EventCallback &cb) {
    errorCallback_ = cb;
}

int Netlib::Channel::fd() const {
    return fd_;
}

int Netlib::Channel::events() const {
    return events_;
}

void Netlib::Channel::setRevents(int revt) {
    revents_ = revt;
}

bool Netlib::Channel::isNoneEvent() const {
    return events_ == kNoneEvent;
}

void Netlib::Channel::enableReading() {
    events_ |= kReadEvent;
    update();
}

void Netlib::Channel::enableWriting() {
    events_ |= kWriteEvent;
    update();
}

void Netlib::Channel::disableReading() {
    events_ &= ~kReadEvent;
    update();
}

void Netlib::Channel::disableWriting() {
    events_ &= ~kWriteEvent;
    update();
}

void Netlib::Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

int Netlib::Channel::index() {
    return index_;
}

void Netlib::Channel::setIndex(int idx) {
    index_ = idx;
}

Netlib::EventLoop *Netlib::Channel::ownerLoop() {
    return eventLoop_;
}

void Netlib::Channel::update() {
    eventLoop_->updateChannel(this);
}

Netlib::Channel::~Channel() {
    assert(!eventHandling_);
}

void Netlib::Channel::setCloseCallback(const Netlib::Channel::EventCallback &cb) {
    closeCallback_ = cb;
}
