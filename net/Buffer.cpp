/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <sys/uio.h>
#include "Buffer.h"
namespace Netlib {

    const char Buffer::kCRLF[] = "\r\n";

    ssize_t Buffer::readFd(int fd, int *saveErrno) {
        char extrabuf[65536];
        struct iovec vec[2];
        const size_t writeable = writeableBytes();
        vec[0].iov_base = begin() + writerIndex;
        vec[0].iov_len = writeable;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof(extrabuf);

        // FIXME:此处应更改为Sockets::readv
        const ssize_t n = readv(fd, vec, 2);

        if (n < 0) {
            *saveErrno = errno;
        } else if (n <= writeable) {
            writerIndex += n;
        } else {
            writerIndex = buffer.size();
            append(extrabuf, n - writeable);
        }

        return n;
    }
}