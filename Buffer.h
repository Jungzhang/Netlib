/*************************************************************************
	> File Name: Buffer.h
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description:
 ************************************************************************/
#ifndef _NETLIB_BUFFER_H
#define _NETLIB_BUFFER_H

#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include "base/StringPiece.h"

class Buffer {

public:

    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    // 构造函数,声明为显式,避免隐式转换
    explicit Buffer(size_t initialSize = kInitialSize) :
            buffer(kCheapPrepend + initialSize), readerIndex(kCheapPrepend), writerIndex(kCheapPrepend) {
        assert(readableBytes() == 0);
        assert(writeableBytes() == initialSize);
        assert(prependBytes() == kCheapPrepend);
    }

    // 可读buffer的大小(字节数)
    size_t readableBytes() const {
        return writerIndex - readerIndex;
    }

    // 可写buffer的大小(字节数)
    size_t writeableBytes() const {
        return buffer.size() - writerIndex;
    }

    // 前向保留大小(字节数)
    size_t prependBytes() const {
        return readerIndex;
    }

    // 得到读缓存区的起始位置
    const char *peek() const {
        return begin() + readerIndex;
    }

    // 写缓存的起始地址(char *版)
    char *beginWrite() {
        return begin() + writerIndex;
    }

    // 写缓存的起始位置(const版)
    const char *beginWrite() const {
        return begin() + writerIndex;
    }

    // 交换
    void swap(Buffer &rhs) {
        buffer.swap(rhs.buffer);
        std::swap(this->readerIndex, rhs.readerIndex);
        std::swap(this->writerIndex, rhs.writerIndex);
    }

    // 寻找\r\n带参数(起始地址)版
    const char *findCRLF(const char *start) const {
        assert(peek() <= start); // 读缓存起始位置小于等于起始位置
        assert(start <= beginWrite()); // 寻找的起始位置需要比写缓存的开始位置小于或等于
        const char *crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2); // 寻找\r\n
        return crlf == beginWrite() ? nullptr : crlf;
    }

    // 寻找\r\n不带参数版
    const char *findCRLF() const {
        const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2); // 寻找\r\n
        return crlf == beginWrite() ? nullptr : crlf;
    }

    // 寻找\n带参数(起始地址)版
    const char *findEOL(const char *start) const {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const void *eol = memchr(start, '\n', beginWrite() - start); // 寻找\n
        return static_cast<const char *>(eol);
    }

    // 检索
    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            readerIndex += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveUntil(const char *end) {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveInt64() {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32() {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16() {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8() {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll() {
        readerIndex = kCheapPrepend;
        writerIndex = kCheapPrepend;
    }

    // 检索出所有的字符串
    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    // 检索长度为len的字符串
    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string str(peek(), len);
        retrieve(len);
        return str;
    }

    // 转换为StringPiece
    StringPiece toStringPiece() const {
        return StringPiece(peek(), static_cast<int>(readableBytes()));
    }

    // 追加
    void append(const StringPiece &str) {
//        append(str.data(), str.size());
    }



private:

    // 返回首地址,普通版本
    char *begin() {
        return &(*buffer.begin());
    }

    // 返回首地址,const重载版本
    const char *begin() const {
        return &(*buffer.begin());
    }

    //

private:
    std::vector<char> buffer;  // buffer保存的地方
    // 采用下标应对迭代器失效
    size_t readerIndex;
    size_t writerIndex;

    static const char kCRLF[];  // \r\n
};

#endif  // _NETLIB_BUGGER_H
