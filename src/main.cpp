#include <iostream>
#include "ShitNet.hpp"
using namespace std;


int main(int argc, char const *argv[])
{

    ShitNet::Inst()->Start();
    //启动main服务
    ShitNet::Inst()->NewService(make_shared<string>("main"));
    ShitNet::Inst()->Wait(); //阻塞
    return 0;
}
