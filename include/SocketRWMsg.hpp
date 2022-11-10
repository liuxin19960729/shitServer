#pragma once
#include "BaseMsg.hpp"
class SocketRWMsg : public BaseMsg
{
private:
    /* data */
public:
    int fd;
    bool isRread = false;
    bool isWrite = false;

public:
    SocketRWMsg(/* args */) = default;
    ~SocketRWMsg() = default;
};
