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

ShitNet::ShitNet(/* args */)
{
}

ShitNet::~ShitNet()
{
}