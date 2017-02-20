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
        channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
        channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
        channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
        channel_->serErrorCallback(std::bind(&TcpConnection::handleError, this));
        socket_->setKeepalive(true);
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

    void TcpConnection::handleRead(TimeStamp receiveTime) {

        int errnoSave;

        ssize_t n = inputBuffer_.readFd(socket_->fd(), &errnoSave);

        if (n > 0) {
            messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
        } else if (n == 0) {
            handleClose();
        } else {
            handleError();
        }
    }

    void TcpConnection::handleWrite() {
        loop_->assertInLoopThread();

        if (channel_->isWriting()) {
            ssize_t n = ::write(channel_->fd(), outBuffer_.peek(), outBuffer_.readableBytes());
            if (n > 0) {
                outBuffer_.retrieve(n);
                if (outBuffer_.readableBytes() == 0) {
                    channel_->disableWriting();
                    if (state_ == kDisConnecting) {
                        shutdownInLoop();
                    }
                } else {
                    printf("I am going to write more data\n");
                }
            } else {
                fprintf(stderr, "write filed\n");
            }
        } else {
            fprintf(stderr, "Connect id down, no more writing\n");
        }

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

    void TcpConnection::send(const std::string &message) {
        if (state_ == kConnected) {
            if (loop_->isInLoopThread()) {
                sendInLoop(message);
            } else {
                loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
            }
        }
    }

    void TcpConnection::shutdown() {
        if (state_ == kConnected) {
            setState(kDisConnecting);
            loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
        }
    }

    void TcpConnection::sendInLoop(const std::string &message) {
        loop_->assertInLoopThread();
        ssize_t nwrote = 0;
        if (!channel_->isWriting() && outBuffer_.readableBytes() == 0) {
            nwrote = ::write(channel_->fd(), message.data(), message.size());
            if (nwrote < 0) {
                nwrote = 0;
                if (errno != EWOULDBLOCK) {
                    fprintf(stderr, "发送失败");
                }
            }
        }

        assert(nwrote >= 0);
        if (nwrote < message.size()) {
            outBuffer_.append(message.data() + nwrote, message.size() - nwrote);
            if (!channel_->isWriting()) {
                channel_->enableWriting();
            }
        }
    }

    void TcpConnection::shutdownInLoop() {
        loop_->assertInLoopThread();
        if (!channel_->isWriting()) {
            socket_->shutdownWrite();
        }
    }

    void TcpConnection::setTcpNoDelay(bool on) {
        socket_->setTcpNoDelay(on);
    }

    void TcpConnection::setKeepalive(bool on) {
        socket_->setKeepalive(on);
    }
}
