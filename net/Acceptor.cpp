/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "Acceptor.h"
#include "SocketOps.h"
#include "EventLoop.h"
#include "InetAddress.h"

Netlib::Acceptor::Acceptor(Netlib::EventLoop *loop, const Netlib::InetAddress &listenAddr)
        : loop_(loop), acceptChannel_(loop, acceptSocket_.fd()), listenning_(false)
        , acceptSocket_(sockets::createNonblockingOrDie()) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Netlib::Acceptor::setNewConnectionCallback(const Netlib::Acceptor::NewConnectionCallback &cb) {
    newConnectionCallback_ = cb;
}

bool Netlib::Acceptor::listenning() const {
    return listenning_;
}

void Netlib::Acceptor::listen() {
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Netlib::Acceptor::handleRead() {
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);
    int connd = acceptSocket_.accept(&peerAddr);
    if (connd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connd, peerAddr);
        } else {
            sockets::close(connd);
        }
    }
}
