/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketOps.h"
#include "TcpConnection.h"

Netlib::TcpServer::TcpServer(Netlib::EventLoop *loop, const Netlib::InetAddress &listenAddr)
        : loop_(loop),
          name_(listenAddr.toHostPort()),
          acceptor_(new Acceptor(loop, listenAddr)),
          started_(false),
          nextConnId_(1) {
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,
                                                  std::placeholders::_1, std::placeholders::_2));
}

Netlib::TcpServer::~TcpServer() { }

void Netlib::TcpServer::start() {
    if (!started_) {
        started_ = true;
    }

    if (!acceptor_->listenning()) {
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
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
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAdrr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->connectEstablished();
}
