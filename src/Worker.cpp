#include <thread>
#include <iostream>
#include <unistd.h>

#include "Worker.hpp"
#include "ShitNet.hpp"
using namespace std;
// Worker::Worker(/* args */)
// {
// }

// Worker::~Worker()
// {
// }

void Worker::operator()()
{
    while (true)
    {
        shared_ptr<Service> srv = ShitNet::Inst()->PopGlobalQueue();
        if (srv)
        {
            srv->ProcessMsgs(eachNum); // 1 2 4 8 16;
            CheckAndPutGlobal(srv);
        }
        else
        {
            usleep(1e2);
        }
    }
}

void Worker::CheckAndPutGlobal(shared_ptr<Service> srv)
{
    // 调用KillServce
    if (srv->isExiting)
        return;

    //重新放回队列
    pthread_spin_lock(&srv->queueLock);
    {
        if (!srv->msgQueue.empty())
        {
            ShitNet::Inst()->PushGlobalQueue(srv);
        }
        else
        {
            srv->SetInGlobalQueue(false);
        }
    }
    pthread_spin_unlock(&srv->queueLock);
}