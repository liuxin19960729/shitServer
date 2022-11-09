#include "Worker.hpp"

Worker::Worker(/* args */)
{
}

Worker::~Worker()
{
}

void Worker::operator()()
{
    while (true)
    {
        cout << "working id:" << id << endl;
        usleep(0.1e6);
    }
}