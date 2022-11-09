#include <iostream>
#include "ShitNet.hpp"

using namespace std;

int test()
{
    auto pingType = make_shared<string>("ping");
    uint32_t p1 = ShitNet::Inst()->NewService(pingType);
    uint32_t p2 = ShitNet::Inst()->NewService(pingType);
    uint32_t p3 = ShitNet::Inst()->NewService(pingType);

    ShitNet::Inst()->Killservice(p1);
    ShitNet::Inst()->Killservice(p2);
    ShitNet::Inst()->Killservice(p3);
}

int main(int argc, char const *argv[])
{
    ShitNet::Inst()->Start();
    test();
    ShitNet::Inst()->Wait(); //阻塞
    return 0;
}
