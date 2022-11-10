#include "Service.hpp"
#include "ShitNet.hpp"
Service::Service(/* args */)
{
    // PTHREAD_PROCESS_PRIVATE 进程不共享
    pthread_spin_init(&queueLock, PTHREAD_PROCESS_PRIVATE);  //初始化锁
    pthread_spin_init(&globalLock, PTHREAD_PROCESS_PRIVATE); //初始化锁
}

Service::~Service()
{
    pthread_spin_destroy(&queueLock);  // 销毁锁
    pthread_spin_destroy(&globalLock); // 销毁锁
}
// 多线程  临界区越小效率越高(保证线程安全的前提下)
void Service::PushMsg(shared_ptr<BaseMsg> msg)
{
    pthread_spin_lock(&queueLock);
    {
        //临界区
        msgQueue.push(msg);
    }
    pthread_spin_unlock(&queueLock);
}

shared_ptr<BaseMsg> Service::PopMsg()
{
    shared_ptr<BaseMsg> msg = nullptr;
    pthread_spin_lock(&queueLock);
    {
        //临界区
        if (!msgQueue.empty())
        {
            msg = msgQueue.front();
            msgQueue.pop();
        }
    }
    pthread_spin_unlock(&queueLock);
    return msg;
}

void Service::OnExit()
{
    cout << "[" << id << "] onExit" << endl;
    //关闭lua虚拟机
    lua_close(luaState);
}
void Service::OnInit()
{
    cout << "[" << id << "] onInit" << endl;
    luaState = luaL_newstate(); //创建虚拟机
    luaL_openlibs(luaState);    //开启标准库的使用
    string filename = "../service/" + *type + "/init.lua";
    // 0 成功  1 失败
    if (luaL_dofile(luaState, filename.c_str()) == 1)
    {
        cout << "run lua fail:" << lua_tostring(luaState, -1) << endl;
    }
}

void Service::OnMsg(shared_ptr<BaseMsg> msg)
{
    if (msg->type == BaseMsg::Type::SERVICE)
    {
        auto m = dynamic_pointer_cast<ServiceMsg>(msg);
        OnServiceMsg(m);
    }
    else if (msg->type == BaseMsg::Type::SOCKET_ACCEPT)
    {
        auto m = dynamic_pointer_cast<SocketAcceptMsg>(msg);
        OnAcceptMsg(m);
    }
    else if (msg->type == BaseMsg::Type::SOCKET_RW)
    {
        auto m = dynamic_pointer_cast<SocketRWMsg>(msg);
        OnRWMsg(m);
    }
}
void Service::OnAcceptMsg(shared_ptr<SocketAcceptMsg> msg)
{
    cout << "OnAcceptMsg new conn fd:" << msg->clientFd << endl;
    auto w = make_shared<ConnectWriter>();
    w->fd = msg->clientFd;
    writers.emplace(msg->clientFd, w);
}
/**
 * BUFFIZE
 *
 * 1.
 *  len=read()  len< BUFFIZE  write
 *
 * 2.
 *   len=read() len==BUFFIZE
 *   两种情况
 *      1.数据刚好  BUFFIZE  再次 len=read()  len=-1
 *      2.数据大于  BUFFIZE  再次 len=read()  len< BUFFIZE  退出循环  or len= BUFFIZE  再次循环
 *
 * 3.
 * 当 fd close() ,执行read() 则error 则会赋值  会调用   OnSocketClose
 *
 * note:
 * linux 坑
 * socket1 close() socket 接收到RST信号
 * socket1 write() OS 判断是否RST信号 入有 向进程发送SIGPIPE信号  终止进程
 */
void Service::OnRWMsg(shared_ptr<SocketRWMsg> msg)
{
    cout << "OnRWMsg" << endl;
    int fd = msg->fd;
    if (msg->isRread)
    {
        const int BUFF_SIZE = 512;
        char buff[BUFF_SIZE];
        int len = 0;
        do
        {
            len = read(fd, &buff, BUFF_SIZE);
            if (len > 0)
                OnSocketData(fd, buff, len); //可能用户在执行该方法内部是 close()

        } while (len == BUFF_SIZE);

        if (len <= 0 && errno != EAGAIN) // len<0 && errno 被设置
        {
            //保证只执行一次(放在服务器主动close,在继续处理接收消息后再调用)
            if (ShitNet::Inst()->GetConnct(fd))
            {
                OnSocketClose(fd);
                ShitNet::Inst()->CloseConnect(fd);
            }
        }
    }
    // 可读可写事件发生
    // 可写
    if (msg->isWrite)
    {
        if (ShitNet::Inst()->GetConnct(fd)) //保证在 read or 之前的一些操作 没有关闭 close;
        {
            OnSocketWritable(fd);
        }
    }
}
void Service::OnServiceMsg(shared_ptr<ServiceMsg> msg)
{
    cout << "OnServiceMsg" << endl;
}
void Service::OnSocketData(int fd, const char *buf, int len)
{
    cout << "OnSocketData fd: " << fd << buf << endl;
    char *wbuff1 = new char[1000000]{'b', 'e', 'g', 'i', 'n'};
    wbuff1[1000000 - 2] = 'l';
    wbuff1[1000000 - 2] = 'x';
    int r = write(fd, wbuff1, 1000000);
    cout << "write r: " << r << endl;
    auto w = writers[fd];
}

void Service::OnSocketWritable(int fd)
{
    cout << "OnSocketWritable fd: " << fd << endl;
    auto w = writers[fd];
 
}
void Service::OnSocketClose(int fd)
{
    cout << "OnSocketClose fd: " << fd << endl;
    writers.erase(fd);
}
bool Service::ProcessMsg()
{
    shared_ptr<BaseMsg> msg = PopMsg();
    if (msg)
    {
        OnMsg(msg);
    }
    return !!msg;
}

void Service::ProcessMsgs(int msg)
{
    for (int i = 0; i < msg; i++)
    {
        bool succ = ProcessMsg();
        if (!succ)
        {
            break;
        }
    }
}

void Service::SetInGlobalQueue(bool isIn)
{
    pthread_spin_lock(&globalLock);
    {
        inGlobal = isIn;
    }
    pthread_spin_unlock(&globalLock);
}
