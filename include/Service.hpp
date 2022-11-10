#pragma once
#include <cstdlib>
#include <queue>
#include <thread>
#include <iostream>
#include <unordered_map>
#include "lua.hpp"
#include "BaseMsg.hpp"
#include "ServiceMsg.hpp"
#include "SocketAcceptMsg.hpp"
#include "SocketRWMsg.hpp"
#include "ConnectWriter.hpp"
using namespace std;

class Service
{
private:
    unordered_map<int, shared_ptr<ConnectWriter>> writers;
    lua_State *luaState; // lua
private:
    shared_ptr<BaseMsg> PopMsg(); //取出一条消息
    void OnServiceMsg();
    void OnAcceptMsg(shared_ptr<SocketAcceptMsg> msg);
    void OnRWMsg(shared_ptr<SocketRWMsg> msg);
    void OnServiceMsg(shared_ptr<ServiceMsg> msg);
    void OnSocketData(int fd, const char *buf, int len);
    void OnSocketWritable(int fd);
    void OnSocketClose(int fd);

public:
    uint32_t id;             //唯一id
    shared_ptr<string> type; //类型
    bool isExiting = false;  //是否正在退出 //不保证线程安全 自己服务Kill自己
    queue<shared_ptr<BaseMsg>> msgQueue;
    pthread_spinlock_t queueLock; //队列锁
    bool inGlobal = false;
    pthread_spinlock_t globalLock;

public:
    void SetInGlobalQueue(bool isIn);

    Service(/* args */);
    ~Service();
    void OnInit();
    void OnMsg(shared_ptr<BaseMsg>);
    void OnExit();                         //不保证线程安全 自己服务Kill自己
    void PushMsg(shared_ptr<BaseMsg> msg); //插入消息
    bool ProcessMsg();                     //执行消息
    void ProcessMsgs(int max);
};
