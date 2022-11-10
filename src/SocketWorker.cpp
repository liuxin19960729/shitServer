#include "SocketWorker.hpp"
void SocketWorker::Init()
{
    cout << "SocketWorker Init" << endl;
    epollFd = epoll_create(1024); //-1 创建失败
    assert(epollFd > 0);
}

void SocketWorker::operator()()
{
    while (true)
    {
        cout << "SocketWorker working" << endl;
        usleep(1e3);
    }
}

// 注意跨线程调用
// 将 fd 添加到 epoll 的事件列表中
void SocketWorker::AddEvent(int fd)
{
    cout << "AddEvent fd: " << fd << endl;
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET; // 可读 边缘
    ev.data.fd = fd;
    //  EPOLL_CTL_ADD 新增   EPOLL_CTL_MOD 修改    EPOLL_CTL_DEL 删除
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        cout << "AddEvent fd: " << fd << "epoll_ctl fai:l" << strerror(errno) << endl;
    }
}

// 注意跨线程调用
// 将 fd 从 epoll 事件列表中删除
void SocketWorker::RemoveEvent(int fd)
{
    cout << "RemoveEvent fd: " << fd << endl;
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
}

void SocketWorker::ModifyEvent(int fd, bool epollOut)
{
    cout << "ModifyEvent fd: " << fd << endl;
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLET | EPOLLIN;
    if (epollOut)
    {
        ev.events |= EPOLLOUT;
    }
    epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
}