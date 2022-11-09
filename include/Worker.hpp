#pragma once
#include <thread>
#include <iostream>
#include <unistd.h>
using namespace std;
//前置声明
class ShitNet;

class Worker
{
private:
    /* data */
public:
    int id;      // Worker 编号
    int eachNum; //每次处理多次条数据
    Worker(/* args */);
    ~Worker();
    void operator()(); //函数符号重载
};
