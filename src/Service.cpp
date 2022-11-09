#include "Service.hpp"

Service::Service(/* args */)
{
    // PTHREAD_PROCESS_PRIVATE 进程不共享
    pthread_spin_init(&queueLock, PTHREAD_PROCESS_PRIVATE);  //初始化锁
    pthread_spin_init(&globalLock, PTHREAD_PROCESS_PRIVATE); //初始化锁
}

Service::~Service()
{
    pthread_spin_destroy(&queueLock);  // 销毁锁
    pthread_spin_destroy(&globalLock); // 销毁锁
}
// 多线程  临界区越小效率越高(保证线程安全的前提下)
void Service::pushMsg(shared_ptr<BaseMsg> msg)
{
    pthread_spin_lock(&queueLock);
    {
        //临界区
        msgQueue.push(msg);
    }
    pthread_spin_unlock(&queueLock);
}

shared_ptr<BaseMsg> Service::popMsg()
{
    shared_ptr<BaseMsg> msg = nullptr;
    pthread_spin_lock(&queueLock);
    {
        //临界区
        msg = msgQueue.front();
        msgQueue.pop();
    }
    pthread_spin_unlock(&queueLock);
    return msg;
}

void Service::onExit()
{
    cout << "[" << id << "] onExit" << endl;
}
void Service::onInit()
{
    cout << "[" << id << "] onInit" << endl;
}

void Service::onMsg(shared_ptr<BaseMsg> msg)
{
    cout << "[" << id << "] onMsg" << endl;
}

bool Service::processMsg()
{
    shared_ptr<BaseMsg> msg = popMsg();
    if (msg)
    {
        onMsg(msg);
    }
    return !!msg;
}

void Service::processMsgs(int msg)
{
    for (int i = 0; i < msg; i++)
    {
        bool succ = processMsg();
        if (!succ)
        {
            break;
        }
    }
}

void Service::SetInGlobalQueue(bool isIn)
{
    pthread_spin_lock(&globalLock);
    {
        inGlobal = isIn;
    }
    pthread_spin_unlock(&globalLock);
}
