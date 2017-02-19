/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <zconf.h>
#include <arpa/inet.h>
#include <strings.h>
#include <cerrno>
#include "SocketOps.h"

namespace {
    typedef struct sockaddr SA;

    // const sockaddr_in*转换为const sockaddr*
    const SA *sockaddr_cast(const struct sockaddr_in *addr) {
        return static_cast<const SA *>((void *)addr);
    }

    // sockaddr_in*转换为sockaddr*
    SA *sockaddr_cast(struct sockaddr_in *addr) {
        return static_cast<SA *>((void *)addr);
    }

    // 设置sockfd为非阻塞和close-on-exec标志
    void setNonBlockAndCloseOnExec(int sockfd) {
        // 设置非阻塞
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        int ret = ::fcntl(sockfd, F_SETFL, flags);
        // 设置close-on-exec
        flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= FD_CLOEXEC;
        ret = ::fcntl(sockfd, F_SETFL, flags);
    }
}

int ::Netlib::sockets::createNonblockingOrDie() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        fprintf(stderr, "socket create filed\n");
        abort();
    }
    setNonBlockAndCloseOnExec(sockfd);

    return sockfd;
}

void ::Netlib::sockets::bindOrDie(int sockfd, const struct sockaddr_in &addr) {
    int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
    if (ret < 0) {
        perror("bind filed");
        abort();
    }
}

void ::Netlib::sockets::listenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        perror("listen filed");
        abort();
    }
}

int ::Netlib::sockets::accept(int sockfd, struct sockaddr_in *addr) {
    socklen_t addrLen = sizeof(*addr);
    int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrLen);

    if (connfd < 0) {
        perror("accept error");
    } else{
        setNonBlockAndCloseOnExec(sockfd);
    }

    return connfd;
}

void ::Netlib::sockets::close(int sockfd) {
    if (::close(sockfd) < 0) {
        perror("close error");
    }
}

void ::Netlib::sockets::toHostPort(char *buf, size_t size, const struct sockaddr_in &addr) {
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
    uint16_t port = sockets::networkToHost16(addr.sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}

void ::Netlib::sockets::fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    ::inet_pton(AF_INET, ip, &addr->sin_addr);
}

struct sockaddr_in Netlib::sockets::getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    ::bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = sizeof(localaddr);
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
        fprintf(stderr, "get socket name error");
    }

    return localaddr;
}

int ::Netlib::sockets::getSocketError(int sockfd) {
    int optval;
    socklen_t optlen = sizeof(optval);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}
