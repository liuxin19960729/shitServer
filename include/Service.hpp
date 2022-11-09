#pragma once
#include <cstdlib>
#include <queue>
#include <thread>
#include <iostream>
#include "BaseMsg.hpp"

using namespace std;

class Service
{
private:
    shared_ptr<BaseMsg> popMsg(); //取出一条消息
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
    void onInit();
    void onMsg(shared_ptr<BaseMsg>);
    void onExit();                         //不保证线程安全 自己服务Kill自己
    void pushMsg(shared_ptr<BaseMsg> msg); //插入消息
    bool processMsg();                     //执行消息
    void processMsgs(int max);
};