# shitServer
## 工程目录
```
build:构建工程可执行文件
src:源文件
include:存放头文件
CMakeLists.txt:cmake指导文件
```
## 规范
```
1.x.hpp 
使用 #progra once 保证只编译一次

2.例(最小原则) 减少编译器 模版编译次数 或 头文件 复制次数
  a.hpp a.cpp

  a.hpp 和 a.cpp 都使用到 x.hpp , a.hpp 引用
  若 只有 a.cpp 用到 则 a.cpp 引用 x.hpp


3.防止循环引用  A.hpp ->B.hpp    B.hpp->A.hplp
（前置申明来解决)
```