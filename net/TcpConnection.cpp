/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <assert.h>
#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketOps.h"

namespace Netlib {
    TcpConnection::TcpConnection(EventLoop *loop, const std::string &name
            , int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr) : loop_(loop)
            , name_(name), state_(kConnecting), socket_(new Socket(sockfd))
            , channel_(new Channel(loop, sockfd)), localAddr_(localAddr), peerAddr_(peerAddr)
    {
        channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    }

    TcpConnection::~TcpConnection() { }

    EventLoop *TcpConnection::getLoop() const {
        return loop_;
    }

    const std::string &TcpConnection::name() const {
        return name_;
    }

    const InetAddress &TcpConnection::localAddress() const {
        return localAddr_;
    }

    const InetAddress &TcpConnection::peerAddress() const {
        return peerAddr_;
    }

    bool TcpConnection::connected() const {
        return state_ == kConnected;
    }

    void TcpConnection::setConnectionCallback(const ConnectionCallback &cb) {
        connectionCallback_ = cb;
    }

    void TcpConnection::setMessageCallback(const MessageCallback &cb) {
        messageCallback_ = cb;
    }

    void TcpConnection::connectEstablished() {
        loop_->assertInLoopThread();
        assert(state_  == kConnecting);
        setState(kConnected);
        channel_->enableReading();
        connectionCallback_(shared_from_this()); // 执行connectionCallback
    }

    void TcpConnection::setState(TcpConnection::StateE s) {
        state_ = s;
    }

    void TcpConnection::handleRead() {
        char buf[65536];
        ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));

        if (n > 0) {
            messageCallback_(shared_from_this(), buf, n);
        } else if (n == 0) {
            handleClose();
        } else {
            handleError();
        }
    }

    void TcpConnection::handleWrite() {
        //TODO
    }

    void TcpConnection::handleClose() {
        loop_->assertInLoopThread();
        assert(state_ == kConnected);
        channel_->disableAll();
        closeCallback_(shared_from_this());
    }

    void TcpConnection::handleError() {
        int err = sockets::getSocketError(socket_->fd());
        fprintf(stderr, "The sockfd is error, name : %s", name_.c_str());
    }

    void TcpConnection::setCloseCallback(const CloseCallback &cb) {
        closeCallback_ = cb;
    }

    void TcpConnection::connectDestroyed() {
        loop_->assertInLoopThread();
        assert(state_ == kConnected);
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
        loop_->removeChannel(channel_.get());
    }
}
