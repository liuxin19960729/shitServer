#pragma once
#include <sys/epoll.h>
#include <assert.h>
#include <memory>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "Connect.hpp"
using namespace std;
class SocketWorker
{
private:
    int epollFd;

private:
    void OnEvent(epoll_event ev);
    void OnAccept(shared_ptr<Connect> connect);
    void OnRW(shared_ptr<Connect> connect, bool r, bool w);

public:
    void Init();       //初始化函数
    void operator()(); //线程函数
    void AddEvent(int fd);
    void RemoveEvent(int fd);
    void ModifyEvent(int fd, bool epollOut);
    SocketWorker(/* args */) = default;
    ~SocketWorker() = default;
};
