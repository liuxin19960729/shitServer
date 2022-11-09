#pragma once
#include <memory>
#include "BaseMsg.hpp"
using namespace std;

class ServiceMsg : public BaseMsg
{
private:
    /* data */
public:
    //BaseMsg type 1  char xxx 
    uint32_t sources;    //消息发送方 4 byte
    shared_ptr<char> buf; //发送的内容 16 byte
    size_t size;          //消息大小 8byte
    ServiceMsg(/* args */);
    ~ServiceMsg();
};
