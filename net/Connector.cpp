/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <functional>
#include <algorithm>
#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "SocketOps.h"

namespace Netlib {

    const int Connector::kMaxRetDelayMs;


    Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
            : loop_(loop),
              serverAddr_(serverAddr),
              connect_(false),
              state_(kDisconnected),
              retryDelayMs_(kInitRetryDelayMs) { }

    Connector::~Connector() {
        loop_->canel(timerId_);
        assert(!channel_);
    }

    void Connector::setNewConnectionCallback(const Connector::NewConnectionCallback &cb) {
        newConnectionCallback_ = cb;
    }

    void Connector::start() {
        connect_ = true;
        loop_->runInLoop(std::bind(&Connector::startInLoop, this));
    }

    void Connector::restart() {
        loop_->assertInLoopThread();
        setState(kDisconnected);
        retryDelayMs_ = kInitRetryDelayMs;
        connect_ = true;
        startInLoop();
    }

    void Connector::stop() {
        connect_ = false;
        loop_->canel(timerId_);
    }

    const InetAddress &Connector::serverAddress() const {
        return serverAddr_;
    }

    void Connector::setState(Connector::States s) {
        state_ = s;
    }

    void Connector::startInLoop() {
        loop_->assertInLoopThread();
        assert(state_ == kDisconnected);

        if (connect_) {
            connect();
        } else {
            fprintf(stderr, "do not connect\n");
        }
    }

    void Connector::connect() {
        int sockfd = sockets::createNonblockingOrDie();
        int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
        int saveError = (ret == 0) ? 0 : errno;

        switch (saveError) {
            case 0:             // 无错误发生
            case EINPROGRESS:   // 连接还在进行中
            case EINTR:         // 调用被打断
            case EISCONN:       // 已经连接
                connecting(sockfd);
                break;

            case EAGAIN:        // 本机port用完
            case EADDRINUSE:    // 地址已经使用
            case EADDRNOTAVAIL: // 不能分配请求的地址
            case ECONNREFUSED:  // 收到服务器端发送的RST断
            case ENETUNREACH:   // 目的不可达
                retry(sockfd);
                break;

            case EACCES:
            case EPERM:
            case EAFNOSUPPORT:
            case EALREADY:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:
                fprintf(stderr, "connect error in Connector::startInLoop");
                sockets::close(sockfd);
                break;

            default:
                fprintf(stderr, "Unexpected error in Connector::startInLoop");
                sockets::close(sockfd);
                break;
        }
    }

    void Connector::connecting(int sockfd) {
        setState(kConnecting);
        assert(!channel_);
        channel_.reset(new Channel(loop_, sockfd));
        channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
        channel_->setErrorCallback(std::bind(&Connector::handleError, this));
        channel_->enableWriting();
    }

    void Connector::handleWrite() {
        if (state_ == kConnecting) {
            int sockfd = removeAndResetChannel();
            int err = sockets::getSocketError(sockfd);

            if (err) {
                fprintf(stderr, "Connector::handleWrite error code %d\n", err);
            } else if (sockets::isSelfConnect(sockfd)) {
                fprintf(stderr, "Connect is self connected\n");
                retry(sockfd);
            } else {
                setState(kConnected);
                if (connect_) {
                    newConnectionCallback_(sockfd);
                } else {
                    ::close(sockfd);
                }
            }
        } else {
            assert(state_ == kDisconnected);
        }
    }

    void Connector::handleError() {
        fprintf(stderr, "Connector::handleError");
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        fprintf(stderr, "in handleError code : %d\n", err);
        retry(sockfd);
    }

    void Connector::retry(int sockfd) {
        sockets::close(sockfd);
        setState(kDisconnected);
        if (connect_) {
            timerId_ = loop_->runAfter(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, this));
            int a = kMaxRetDelayMs;
            retryDelayMs_ = std::min(kMaxRetDelayMs, retryDelayMs_ * 2);
        } else {
            fprintf(stderr, "do not connect\n");
        }
    }

    int Connector::removeAndResetChannel() {
        channel_->disableAll();
        loop_->removeChannel(channel_.get());
        int sockfd = channel_->fd();
        loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
        return sockfd;
    }

    void Connector::resetChannel() {
        channel_.reset();
    }
}
