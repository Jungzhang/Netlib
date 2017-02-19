/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <netinet/in.h>
#include <strings.h>
#include "Socket.h"
#include "SocketOps.h"
#include "InetAddress.h"

Netlib::Socket::Socket(int sockfd) :sockfd_(sockfd) { }

Netlib::Socket::~Socket() {
    sockets::close(sockfd_);
}

int Netlib::Socket::fd() const {
    return sockfd_;
}

void Netlib::Socket::bindAddress(const Netlib::InetAddress &localAddr) {
    sockets::bindOrDie(sockfd_, localAddr.getSockAddrInet());
}

void Netlib::Socket::listen() {
    sockets::listenOrDie(sockfd_);
}

int Netlib::Socket::accept(Netlib::InetAddress *peeraddr) {
    struct sockaddr_in addr;
    ::bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(sockfd_, &addr);

    if (connfd >= 0) {
        peeraddr->setSockAddrInet(addr);
    }

    return connfd;
}

void Netlib::Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Netlib::Socket::shutdownWrite() {
    sockets::shutdownWrite(sockfd_);
}
