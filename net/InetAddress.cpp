/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <strings.h>
#include "InetAddress.h"
#include "SocketOps.h"

namespace Netlib{
    static const in_addr_t kInaddrAny = INADDR_ANY;
}

Netlib::InetAddress::InetAddress(uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
    addr_.sin_port = sockets::hostToNetwork16(port);
}

Netlib::InetAddress::InetAddress(const std::string &ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    sockets::fromHostPort(ip.c_str(), port, &addr_);
}

Netlib::InetAddress::InetAddress(const struct sockaddr_in &addr) : addr_(addr) { }

std::string Netlib::InetAddress::toHostPort() const {
    char buf[32];
    sockets::toHostPort(buf, sizeof(buf), addr_);
    return std::string(buf);
}

const struct sockaddr_in &Netlib::InetAddress::getSockAddrInet() const {
    return addr_;
}

void Netlib::InetAddress::setSockAddrInet(const struct sockaddr_in &addr) {
    addr_ = addr;
}
