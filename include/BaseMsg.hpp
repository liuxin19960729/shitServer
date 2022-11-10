#pragma once
#include <cstdint>
using namespace std;

class BaseMsg
{
public:
    enum Type
    {
        SERVICE = 1,   //服务消息
        SOCKET_ACCEPT, //套接字 accept
        SOCKET_RW      //套接字读写
    };

private:
public:
    uint8_t type;
    //测试
    char load[999999]{};
    BaseMsg() = default;
    //虚析构 查询 虚表 调用析构 到基类虚构
    // note:多态使用非析构更加当前指定类型调用析构函数 可能造成内存泄露
    virtual ~BaseMsg() = default;
};
