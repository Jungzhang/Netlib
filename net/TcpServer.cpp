/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <cassert>
#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketOps.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

Netlib::TcpServer::TcpServer(Netlib::EventLoop *loop, const Netlib::InetAddress &listenAddr)
        : loop_(loop),
          name_(listenAddr.toHostPort()),
          acceptor_(new Acceptor(loop, listenAddr)),
          started_(false),
          threadPool_(new EventLoopThreadPool(loop)),
          nextConnId_(1) {
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,
                                                  std::placeholders::_1, std::placeholders::_2));
}

Netlib::TcpServer::~TcpServer() { }

void Netlib::TcpServer::start() {
    if (!started_) {
        started_ = true;
        threadPool_->setTimeoutMs(loop_->getTimeoutMs());
        threadPool_->start();
    }

    if (!acceptor_->listenning()) {
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void Netlib::TcpServer::removeConnection(const Netlib::TcpConnectionPtr &conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void Netlib::TcpServer::setConnectionCallback(const Netlib::ConnectionCallback &cb) {
    connectionCallback_ = cb;
}

void Netlib::TcpServer::setMessageCallback(const Netlib::MessageCallback &cb) {
    messageCallback_ = cb;
}

void Netlib::TcpServer::newConnection(int sockfd, const Netlib::InetAddress &peerAdrr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + std::string(buf);
    fprintf(stdout, "new connection IP and Port : %s\n", peerAdrr.toHostPort().c_str());
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    EventLoop *ioLoop = threadPool_->getNextLoop();
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAdrr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    // 建立连接,其修改状态并调用ConnectionCallback
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void Netlib::TcpServer::removeConnectionInLoop(const Netlib::TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    // 在IO事件循环中执行相应的销毁连接操作,而不是在主线程循环中执行销毁操作
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void Netlib::TcpServer::setThreadNum(int numThread) {
    assert(numThread >= 0);
    threadPool_->setThreadNum(numThread);
}
