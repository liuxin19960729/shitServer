#include "ShitNet.hpp"

ShitNet *ShitNet::Inst()
{
    if (!!!_ins)
    {
        _ins = new ShitNet();
    }

    return _ins;
}

ShitNet *ShitNet::_ins = nullptr;

void ShitNet::Wait()
{
    if (vecThreads[0])
    {
        vecThreads[0]->join(); //当前现状阻塞到  vecThreads[0] 退出
    }
}

void ShitNet::Start()
{
    cout << "Hello Shitnet" << endl;
    pthread_rwlock_init(&servicesMapLock, nullptr);
    pthread_spin_init(&globalQueueLock, PTHREAD_PROCESS_PRIVATE);
    pthread_mutex_init(&sleepMtx, nullptr);
    pthread_cond_init(&sleepCond, nullptr);
    StartWorker();
}

void ShitNet::StartWorker()
{
    for (size_t i = 0; i < Worker_NUM; i++)
    {
        Worker *wk = new Worker();
        wk->id = i;
        wk->eachNum = 1 << i; // 1 2 4
        thread *t = new thread(*wk);
        vecThreads.push_back(t);
        vecWorkers.push_back(wk);
    }
}
uint32_t ShitNet::NewService(shared_ptr<string> type)
{
    auto srv = make_shared<Service>();
    srv->type = type;
    pthread_rwlock_wrlock(&servicesMapLock); //写锁
    {
        srv->id = maxId++;
        servicesMap.emplace(srv->id, srv);
    }
    pthread_rwlock_unlock(&servicesMapLock); //释放写锁
    srv->OnInit();
    return srv->id;
}

shared_ptr<Service> ShitNet::GetService(uint32_t id)
{
    shared_ptr<Service> srv = nullptr;
    pthread_rwlock_rdlock(&servicesMapLock); //读锁
    {
        auto iter = servicesMap.find(id);
        if (iter != servicesMap.end())
        {
            srv = iter->second;
        }
    }
    pthread_rwlock_unlock(&servicesMapLock);
    return srv;
}

void ShitNet::Killservice(uint32_t id)
{
    shared_ptr<Service> srv = GetService(id);
    if (srv)
    {
        srv->OnExit();
        srv->isExiting = true; //用于告诉service 和 ShitNet 切断联系的指示 ,(可能从ShitNet切断但是正在消息发送)
        pthread_rwlock_wrlock(&servicesMapLock);
        {
            servicesMap.erase(id);
        }
        pthread_rwlock_unlock(&servicesMapLock);
    }
}

shared_ptr<Service> ShitNet::PopGlobalQueue()
{
    shared_ptr<Service> srv = nullptr;
    pthread_spin_lock(&globalQueueLock);
    {
        if (!globalQueue.empty())
        {
            srv = globalQueue.front();
            globalQueue.pop();
            globalLen--;
        }
    }
    pthread_spin_unlock(&globalQueueLock);
    return srv;
}

void ShitNet::PushGlobalQueue(shared_ptr<Service> srv)
{
    pthread_spin_lock(&globalQueueLock);
    {
        globalQueue.push(srv);
        globalLen++;
    }
    pthread_spin_unlock(&globalQueueLock);
}
void ShitNet::Send(uint32_t toId, shared_ptr<BaseMsg> msg)
{
    shared_ptr<Service> toSrv = GetService(toId);
    if (toSrv)
    {
        bool hasPush = false;
        // 将消息加入对应的服务
        toSrv->PushMsg(msg);
        pthread_spin_lock(&toSrv->globalLock);
        {
            //没有在全局对垒加入对列
            if (!toSrv->inGlobal)
            {
                PushGlobalQueue(toSrv);
                toSrv->inGlobal = true;
                hasPush = true;
            }
        }
        pthread_spin_unlock(&toSrv->globalLock);

        if (hasPush)
            CheckAndWeakUp();
    }
}
void ShitNet::WorkerWait()
{
    pthread_mutex_lock(&sleepMtx);
    sleepCount++;
    pthread_cond_wait(&sleepCond, &sleepMtx); //休眠 解锁 ,
    sleepCount--;                             //唤醒后 抢到锁开始执行
    pthread_mutex_unlock(&sleepMtx);
}
//唤醒任意一个线程 pthread_cond_signal 有一定开销
void ShitNet::CheckAndWeakUp()
{
    // 1 检查是否所有线程都在工作 无需 唤醒
    if (sleepCount == 0)
        return;
    // 2 检查当前服务是否应付当前任务 来判断是否唤醒
    if (Worker_NUM - sleepCount <= globalLen) //不能应付当前任务
    {
        cout << "wake up thread" << endl;
        pthread_cond_signal(&sleepCond);
    }
}
// ShitNet::ShitNet(/* args */)
// {
// }

// ShitNet::~ShitNet()
// {
// }