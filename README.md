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
## Msg
```
基类
BaseMsg:消息通用属性

子类
ServiceMsg:服务间的消息传递(Actor模型)


```
## lock
```
服务消息队列锁选择的是 spin_lock(自旋锁)

自旋锁:
当另一个线程要使用该对象的是够 入过该对象使用了锁 则 会一直自旋转检查所释放被释放 该线程一直自旋转检查 很浪费性能

互斥锁:
线程A 上锁
线程B 看见上锁会休息 

线程 A 释放 所 会 唤醒 所有休息等待该线程锁的线程 让他们重亲抢

唤醒线程的过程很浪费性能

Actor 模型:
    <<百万在线:大型服务端开发>> 认为自旋锁 更加合适(在Actor 中)

```
![shitNet_arch](./img/shitNet_arch.png)
```
globalQueue:有Msg的服务
Worker:当一个任务执行完继续跑到globalQueue领取任务
```
## Worker eachNum 设计
![worker_eachNum_design](./img/worker_eachNum_design.png)
```
灰色:全局队列取出放入用到的时间(锁非常消耗性能)

worker[0]:一条消息一个损耗
eachNum越大效率越高 
worker[0] 第6条，worker[2] 第8条消息开始执行


为什么 需要 eachNum 小的数呢?
如果 为了有的消息能够及时处理,


eachNum 有大有小为了达到及时和非及时的平衡

```
## 后台程序
```
#include <iostream>
demond();

左信号处理方法
```
## 套接字
![Linux所有资源都是文件](./img/Linux%E4%B8%80%E5%88%87%E9%83%BD%E6%98%AF%E6%96%87%E4%BB%B6.png)
```
Socket c语言结构体
       协议信息  本地地址  远程地址...


Linux 将资源抽象成文件(设备 文件  套件子...)
fd:文件描述符,可以使用read() write() close() 同一套API操作这些资源
```

##  SocketAcceptMsg
```cpp
class SocketAcceptMsg : public BaseMsg
{
private:
    /* data */
public:
    int listenFd;//监听套接字描述符
    int clientFd;//accept() 建立连接成功 用于通信的套接字描述符

public:
    SocketAcceptMsg(/* args */);
    ~SocketAcceptMsg();
};

```
## 线程设计
```
workers group+网路线程

```
## ShitNet初始化
![ShitNet初始化](./img/ShitNet%20%E5%88%9D%E5%A7%8B%E5%8C%96.png)

## ShitNet系统结构
![ShitNet系统结构](./img/ShitNet%E7%B3%BB%E7%BB%9F%E7%BB%93%E6%9E%84%E5%9B%BE.png)

## 网络
### 代码设计
```
client-->[网路底层]-->自定义网络服务代码

client-->连接-->OnConnet();
client-->发送数据-->onData();
.....
```
### 网络
```
事件模型:
依赖操作系统提供的多路复用功能

多路复用:一个程序能够同时处理多个客户端的链接技术
```
### 事件模型
```
Epoll(网络线程)+网络服务+管理事件和网络

```
### IO多路复用
#### 轮询(Select)
```
array_tasks

while(true){
    for(i){
    if( array_tasks[i].isOk){
            //处理
    }
    }
}


适用场合:
大部分客户端都在高平率发送消息的场合


游戏场景：
    轮询的效率很低
    
    例如:
        100 个客户端
        只有 10 个客户端在在发送消息
        那么循环一转只能执行10个  (1/10)

        循环到 第 99 个的时后 前面的已经准备好那么只有等到下一次执行到它的是够执行
```
```
epoll 和 select 都是IO多路复用
当没有消息或连接的时候会把IO线程阻塞
```
## Epoll
```

epoll 和  select 
wait() 阻塞等待方法


时序图
os  |  user
      wait()
....
....
....
有消息 wait() 返回
      执行接下来的逻辑 



epoll_create()
创建成功 会提供一个监听列表
当监听列表中的元素有一个事件发生操作系统都会通知进程


```
### epoll_ctrl 示意图
![epoll_ctrl 示意图](./img/epoll_ctr_%E5%9B%BE.png)
```
events:
  触发模式
     EPOLLET 对应的文件描述符有事件发生
  监听事件
    EPOLLIN 读
    EPOLLOUT 写
    EPOLLERR 文件描述符发生错误
   

    EPOLLPRI 对应的文件描述符有紧急数据可读
    EPOLLHUP 对应的文件描述符被挂断


读
EPOLLET|EPOLLET

写
EPOLLET|EPOLLOUT

错误
EPOLLET|EPOLLIN|EPOLLOUT|EPOLLERR

可读可写
EPOLLET|EPOLLIN|EPOLLOUT
```
### epoll_wait
#### epoll_wait 有消息时
![epoll_wait 有消息时](./img/epoll_wait%E6%9C%89%E6%B6%88%E6%81%AF%E7%A4%BA%E6%84%8F%E5%9B%BE.png)
#### epoll_wait 无消息是
![epoll_wait 无消息是](./img/epoll_wait_%E6%97%A0%E6%B6%88%E6%81%AF%E7%A4%BA%E6%84%8F%E5%9B%BE.png)
#### epoll_waait 有消息唤醒IO 用户线程
![epoll_waait 有消息唤醒IO 用户线程](./img/epoll_wait%E5%94%A4%E9%86%92%E7%BA%BF%E7%A8%8B%E7%A4%BA%E6%84%8F%E5%9B%BE.png)