#include "SocketWorker.hpp"
#include "SocketAcceptMsg.hpp"
#include "SocketRWMsg.hpp"
#include "ShitNet.hpp"
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
        const int EVENT_SIZE = 64;
        epoll_event events[EVENT_SIZE];
        int eventCount = epoll_wait(epollFd, events, EVENT_SIZE, -1); //-1 代表会一直等待到有事件发送
        for (size_t i = 0; i < eventCount; i++)
        {
            OnEvent(events[i]);
        }
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

void SocketWorker::OnEvent(epoll_event ev)
{
    cout << "OnEvent" << endl;
    int fd = ev.data.fd;
    auto conn = ShitNet::Inst()->GetConnct(fd);
    if (!conn)
    {
        cout << "OnEvent error,conn=null" << endl;
        return;
    }

    bool isR = ev.events & EPOLLIN;
    bool isW = ev.events & EPOLLOUT;
    bool isErr = ev.events & EPOLLERR;
    if (conn->type == Connect::TYPE::LISTEN)
    {
        if (isR)
            OnAccept(conn);
    }
    else
    {
        if (isR || isW)
            OnRW(conn, isR, isW);
        if (isErr)
        {
            cout << "onError fd: " << conn->fd << endl;
        }
    }
}
void SocketWorker::OnAccept(shared_ptr<Connect> connect)
{
    cout << "OnAccept" << connect->fd << endl;
    // accept
    int clientfd = accept(connect->fd, nullptr, nullptr);
    if (clientfd < 0)
    {
        cout << "OnAccept: accept error" << endl;
    }
    fcntl(clientfd, F_SETFL, O_NONBLOCK); //飞阻塞
    //添加连接对象
    ShitNet::Inst()->AddConnect(clientfd, connect->serviceId, Connect::TYPE::CLIENT);
    epoll_event ev;
    ev.events = EPOLLET | EPOLLIN;
    ev.data.fd = clientfd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientfd, &ev) == -1)
    {
        cout << "OnAccept:epoll_ctl Fail:" << strerror(errno) << endl;
    }
    //通知服务
    auto msg = make_shared<SocketAcceptMsg>();
    msg->type = BaseMsg::Type::SOCKET_ACCEPT;
    msg->listenFd = connect->fd;
    msg->clientFd = clientfd;
    ShitNet::Inst()->Send(connect->serviceId, msg);
}
void SocketWorker::OnRW(shared_ptr<Connect> connect, bool r, bool w)
{
    cout << "OnRW fd:" << connect->fd << endl;
    auto msg = make_shared<SocketRWMsg>();
    msg->type = BaseMsg::Type::SOCKET_RW;
    msg->fd = connect->fd;
    msg->isRread = r;
    msg->isWrite = w;
    ShitNet::Inst()->Send(connect->serviceId, msg);
}