/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_SOCKETOPS_H
#define NETLIB_SOCKETOPS_H

#include <cstdint>
#include <endian.h>
#include <netinet/in.h>

namespace Netlib {
    namespace sockets {
        // 转换64位的数字为网络字节序
        inline uint64_t hostToNetwork64(uint64_t host64) {
            // 主机字节序到大端字节序转换
            return htobe64(host64);
        }
        // 转换32位的数字为网络字节序
        inline uint32_t hostToNetwork32(uint32_t host32) {
            return htonl(host32);
        }
        // 转换16位的数字为网络字节序
        inline uint16_t hostToNetwork16(uint16_t host16) {
            return htons(host16);
        }
        // 转换64位的网络字节序数字到主机字节序
        inline uint64_t networkToHost64(uint64_t net64) {
            return be64toh(net64);
        }
        // 转换32位的网络字节序数字到主机字节序
        inline uint32_t networkToHost32(uint32_t net32) {
            return be32toh(net32);
        }
        // 转换16位的网络字节序数字到主机字节
        inline uint16_t networkToHost16(uint16_t net16) {
            return be16toh(net16);
        }

        int createNonblockingOrDie();
        void bindOrDie(int sockfd, const struct sockaddr_in &addr);
        void listenOrDie(int sockfd);
        int accept(int sockfd, struct sockaddr_in *addr);
        void close(int sockfd);
        void toHostPort(char *buf, size_t size, const struct sockaddr_in &addr);
        void fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr);
        struct sockaddr_in getLocalAddr(int sockfd);
    }
}

#endif //NETLIB_SOCKETOPS_H
