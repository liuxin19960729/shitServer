#pragma once
#include <vector>
#include "Worker.hpp"
using namespace std;
class ShitNet
{
private:
    static ShitNet *_ins;
    int Worker_NUM = 3;
    vector<thread *> vecThreads;
    vector<Worker *> vecWorkers;
    //开启工作线程
    void StartWorker();

public:
    static ShitNet *Inst();
    //初始化并开始
    void Start();
    //阻塞
    void Wait();
    ShitNet(/* args */);
    ~ShitNet();
};
