#pragma once
#include "BaseMsg.hpp"
class SocketAcceptMsg : public BaseMsg
{
private:
    /* data */
public:
    int listenFd;
    int clientFd;

public:
    SocketAcceptMsg(/* args */) = default;
    ~SocketAcceptMsg() = default;
};
