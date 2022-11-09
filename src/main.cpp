#include <iostream>
#include "ShitNet.hpp"
using namespace std;

void test()
{
    auto pingType = make_shared<string>("ping");
    uint32_t p1 = ShitNet::Inst()->NewService(pingType);
    uint32_t p2 = ShitNet::Inst()->NewService(pingType);
    uint32_t pong1 = ShitNet::Inst()->NewService(pingType);

    auto m1 = ShitNet::Inst()->testMakeMsg(p1, new char[3]{'h', '1', '\0'}, 3);
    auto m2 = ShitNet::Inst()->testMakeMsg(p2, new char[3]{'h', '2', '\0'}, 3);
    ShitNet::Inst()->Send(pong1, m1);
    ShitNet::Inst()->Send(pong1, m2);
}

int main(int argc, char const *argv[])
{
    ShitNet::Inst()->Start();
    test();
    ShitNet::Inst()->Wait(); //阻塞
    return 0;
}
