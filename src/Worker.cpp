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
            srv->processMsgs(eachNum);//1 2 4 8 16;
            checkAndPutGlobal(srv);
        }
        else
        {
            usleep(1e2);
        }
    }
}

void Worker::checkAndPutGlobal(shared_ptr<Service> srv)
{
}