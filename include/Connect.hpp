#pragma once
#include <cstdint>
//对Socket 连接套接字的在封装
//目的为封装的Connet添加自定义属性

class Connect
{
private:
    /* data */
public:
    enum TYPE
    {
        LISTEN = 1,
        CLIENT,
    };
    uint8_t type; //套接字类型
    int fd;       //套接字描述符
    uint32_t serviceId;

public:
    Connect(/* args */) = default;
    ~Connect() = default;
};