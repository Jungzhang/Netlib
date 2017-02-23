/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "TcpClient.h"
#include "Connector.h"
#include "TcpConnection.h"
#include "SocketOps.h"


namespace Netlib{
    namespace NetlibTemp{
        void removeConnection(EventLoop *loop, const TcpConnectionPtr &conn) {
            loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
        }

        void removeConnector(const ConnectorPtr &connector) {

        }
    }
}

void Netlib::TcpClient::connect() {
    connect_ = true;
    connector_->start();
}

void Netlib::TcpClient::disconnect() {
    connect_ = false;
    {
        std::lock_guard<std::mutex> locker(mutex_);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void Netlib::TcpClient::stop() {
    connect_ = false;
    connector_->stop();
}

Netlib::TcpConnectionPtr Netlib::TcpClient::connection() const {
    return connection_;
}

bool Netlib::TcpClient::retry() const {
    return retry_;
}

void Netlib::TcpClient::enableRetry() {
    retry_ = true;
}

void Netlib::TcpClient::setConnectionCallback(const Netlib::ConnectionCallback &cb) {
    connectionCallback_ = cb;
}

void Netlib::TcpClient::setMessageCallback(const Netlib::MessageCallback &cb) {
    messageCallback_ = cb;
}

void Netlib::TcpClient::setWriteCompleteCallback(const Netlib::WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
}

void Netlib::TcpClient::newConnection(int sockfd) {
    loop_->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = buf;
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    {
        std::lock_guard<std::mutex> locker(mutex_);
        connection_ = conn;
    }

    conn->connectEstablished();
}

void Netlib::TcpClient::removeConnection(const Netlib::TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());
    {
        std::lock_guard<std::mutex> locker(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));

    if (retry_ && connect_) {
        connector_->restart();
    }
}

Netlib::TcpClient::TcpClient(Netlib::EventLoop *loop, const Netlib::InetAddress &serverAddr)
        : loop_(loop),
          connector_(new Connector(loop, serverAddr)),
          retry_(false),
          connect_(true),
          nextConnId_(1) {
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

Netlib::TcpClient::~TcpClient() {
    TcpConnectionPtr conn;
    {
        std::lock_guard<std::mutex> locaker(mutex_);
        conn = connection_;
    }

    if (conn) {
        CloseCallback cb = std::bind(&NetlibTemp::removeConnection, loop_, std::placeholders::_1);
        loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
    } else {
        connector_->stop();
        loop_->runAfter(1, std::bind(&NetlibTemp::removeConnector, connector_));
    }
}
