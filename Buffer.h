/*************************************************************************
	> File Name: Buffer.h
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description:
 ************************************************************************/
#ifndef _BUFFER_H
#define _BUFFER_H

#include <vector>

class Buffer {

private:
    std::vector<char> buffer;  // buffer保存的地方
    // 采用下标应对迭代器失效
    size_t readerIndex;
    size_t writerIndex;
};

#endif
