#pragma once
#include <memory>
#include "Service.hpp"
//前置声明
class ShitNet;

class Worker
{
private:
    /* data */
public:
    int id;      // Worker 编号
    int eachNum; //每次处理多次条数据
    Worker(/* args */) = default;
    ~Worker() = default;
    void operator()(); //函数符号重载
    void checkAndPutGlobal(shared_ptr<Service> srv);
};
