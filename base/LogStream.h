/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
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
public:
    // <<运算符的一些类型
    LogStream &operator << (bool v);

    LogStream &operator << (short v);
    LogStream &operator << (unsigned short v);
    LogStream &operator << (int v);
    LogStream &operator << (unsigned int v);
    LogStream &operator << (long v);
    LogStream &operator << (unsigned long v);
    LogStream &operator << (long long v);
    LogStream &operator << (unsigned long long v);

    LogStream &operator << (const void * v);
    LogStream &operator << (const char * v);
    LogStream &operator << (const unsigned char * v);

    LogStream &operator << (float v);
    LogStream &operator << (double v);

    LogStream &operator << (char v);

    LogStream &operator << (const std::string &v);
    LogStream &operator << (const StringPiece &v);

    template <int SIZE>
    LogStream &operator << (const FixedBuffer<SIZE> &v);

    void append(const char *data, int len);
    template <int SIZE>
    const FixedBuffer<SIZE> &buffer() const ;
    void resetBuffer();

private:
    template <typename T>
    void formatInteger(T);

private:
    static const int kSmallBuffer  = 4000;
    static const int kLargeBuffer  = 4000 * 1000;
    static const int kMaxNumericSize = 32;
    FixedBuffer<kSmallBuffer> buffer_;

};


#endif //NETLIB_LOGSTREAM_H
