#include "Service.hpp"
#include "ServiceMsg.hpp"
#include "ShitNet.hpp"
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
void Service::PushMsg(shared_ptr<BaseMsg> msg)
{
    pthread_spin_lock(&queueLock);
    {
        //临界区
        msgQueue.push(msg);
    }
    pthread_spin_unlock(&queueLock);
}

shared_ptr<BaseMsg> Service::PopMsg()
{
    shared_ptr<BaseMsg> msg = nullptr;
    pthread_spin_lock(&queueLock);
    {
        //临界区
        if (!msgQueue.empty())
        {
            msg = msgQueue.front();
            msgQueue.pop();
        }
    }
    pthread_spin_unlock(&queueLock);
    return msg;
}

void Service::OnExit()
{
    cout << "[" << id << "] onExit" << endl;
}
void Service::OnInit()
{
    cout << "[" << id << "] onInit" << endl;
}

void Service::OnMsg(shared_ptr<BaseMsg> msg)
{
    if (msg->type == BaseMsg::Type::SERVICE)
    {
        auto m = dynamic_pointer_cast<ServiceMsg>(msg);
        cout << "[" << id << "] onMsg:" << m->buf << endl;
        auto msgRet = ShitNet::Inst()->testMakeMsg(id, new char[9999999]{'p', 'i', 'n', 'g', '\0'}, 9999999);
        ShitNet::Inst()->Send(m->sources, msgRet);
    }
    else
    {
        cout << "[" << id << "] onMsg" << endl;
    }
}

bool Service::ProcessMsg()
{
    shared_ptr<BaseMsg> msg = PopMsg();
    if (msg)
    {
        OnMsg(msg);
    }
    return !!msg;
}

void Service::ProcessMsgs(int msg)
{
    for (int i = 0; i < msg; i++)
    {
        bool succ = ProcessMsg();
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
