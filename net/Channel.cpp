/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "Channel.h"

Netlib::Channel::Channel(Netlib::EventLoop *loop, int fd) : eventLoop_(loop), fd_(fd), revents_(0), events_(0),
                                                            index_(-1) {}

void Netlib::Channel::handleEvent() {

    if (revents_ & (POLLNVAL | POLLERR)) {
        if (errorCallback_) errorCallback_;
    }

    if (revents_ & (POLLPRI | POLLIN)) {
        if (readCallback_) readCallback_;
    }

    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_;
    }
}

void Netlib::Channel::setReadCallback(const Netlib::Channel::EventCallback &cb) {
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
    events_ = revt;
}

bool Netlib::Channel::isNoneEvent() const {
    return events_ == kNoneEvent;
}

void Netlib::Channel::enableReading() {
    events_ |= kReadEvent;
}

void Netlib::Channel::enableWriting() {
    events_ |= kWriteEvent;
}

void Netlib::Channel::disableReading() {
    events_ &= ~kReadEvent;
}

void Netlib::Channel::disableWriting() {
    events_ &= ~kWriteEvent;
}

void Netlib::Channel::disableAll() {
    events_ = kNoneEvent;
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
    // TODO 后续实现
}
