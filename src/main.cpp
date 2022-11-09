#include <iostream>
#include "ShitNet.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    ShitNet::Inst()->Start();
    ShitNet::Inst()->Wait();//阻塞
    return 0;
}
