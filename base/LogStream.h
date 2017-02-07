/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Created Time: 17-2-7
* >Description: 
**********************************************/

#ifndef NETLIB_LOGSTREAM_H
#define NETLIB_LOGSTREAM_H

#include <cstddef>
#include <cstring>
#include <string>
#include "StringPiece.h"

template <int SIZE>

class FixedBuffer {
public:

    // 构造函数
    FixedBuffer() : cur(data_) {
        setCookie(cookiesStart);
    }

    // 析构函数
    ~FixedBuffer() {
        setCookie(cookiesEnd);
    }

    // buffer中添加数据
    void append(const char *buf, size_t len) {
        // 剩余空间足够
        if (avail() > len) {
            ::memcpy(cur, buf, len);
            cur += len;
        }
    }

    // 返回数据的实际位置
    const char *data() const {
        return data_;
    }

    // 获取数据长度
    int length() const {
        return static_cast<int>(cur - data_);
    }

    // 返回当前位置
    char *current() {
        return cur;
    }

    // 获得buffer当前剩余空间
    int avail() const {
        return static_cast<int> (end() - cur);
    }

    // 增加当前位置
    void add(size_t len) {
        cur += len;
    }

    // 重置
    void reset() {
        cur = data_;
    }

    // 清零
    void bzero() {
        ::bzero(data_, sizeof(data_));
    }

    //
    const char *debugString();

    void setCookie(void (*cookie)()) {
        this->cookie = cookie;
    }

    // toString
    std::string toString() const {
        return std::string(data_, length());
    }

    // toStringPiece
    StringPiece toStringPiece() const {
        return StringPiece(data_, length());
    }

private:

    // 返回缓存区的末尾
    const char *end() const {
        return data_ + sizeof(data_);
    }

    // cookies的开始
    static void cookiesStart();

    // cookie的末尾
    static void cookiesEnd();

private:
    static void (*cookie)();
    char data_[SIZE];
    char *cur;
};



class LogStream {

};


#endif //NETLIB_LOGSTREAM_H
