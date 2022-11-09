#pragma once
#include <vector>
#include <unordered_map>
#include "Worker.hpp"
#include "Service.hpp"
#include "ServiceMsg.hpp"
using namespace std;
class ShitNet
{
private:
    static ShitNet *_ins;
    int Worker_NUM = 3;
    vector<thread *> vecThreads;
    vector<Worker *> vecWorkers;
    //全局队列
    queue<shared_ptr<Service>> globalQueue;
    int globalLen = 0;
    pthread_spinlock_t globalQueueLock; // 全局队列锁

private:
    //开启工作线程
    void
    StartWorker();

public:
    static ShitNet *Inst();
    unordered_map<uint32_t, shared_ptr<Service>> servicesMap;
    uint32_t maxId = 0; //最大ID
    pthread_rwlock_t servicesMapLock;

public:
    //发送消息
    void Send(uint32_t toId, shared_ptr<BaseMsg> msg);
    //全局队列操作
    shared_ptr<Service> PopGlobalQueue();
    void PushGlobalQueue(shared_ptr<Service> srv);
    uint32_t NewService(shared_ptr<string> type); //增加服务
    void Killservice(uint32_t id);                //仅服务自己调用 kill 服务
    shared_ptr<Service> GetService(uint32_t id);  // 查找服务
    void Start();                                 //初始化并开始
    void Wait();                                  //阻塞
    ShitNet(/* args */) = default;
    ~ShitNet() = default;
    shared_ptr<BaseMsg> testMakeMsg(uint32_t source, char *buff, int len)
    {
        auto msg = make_shared<ServiceMsg>();
        msg->type = BaseMsg::Type::SERVICE;
        msg->sources = source;
        msg->buf = shared_ptr<char>(buff);
        msg->size = len;
        return msg;
    }
};
