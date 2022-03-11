#include "Server.h"


Server::Server() : 
ep(), sock(), pool(), addr(), server_state(true), client_addr(), client_sock(-1)
{
    root = getenv("PWD");
    strcpy(HttpRequest::root,root);
    strcat(HttpRequest::root,"/www");
    conn = new HttpData[MAX_CONN];
    initlog(true);
    timermanager.setalive(true);
    sql = MySQLPool::GetInstance();
}

Server::Server(const char* ip, int port, bool log, bool timer) :
ep(), sock(), pool(), addr(ip,port), server_state(true), client_addr(), client_sock(-1)
{
    //获取运行目录
    root = getenv("PWD");
    strcpy(HttpRequest::root,root);
    strcat(HttpRequest::root,"/www");
    conn = new HttpData[MAX_CONN];
    //初始化日志
    initlog(log);
    //设置定时器
    timermanager.setalive(timer);
    //获取sql连接池
    sql = MySQLPool::GetInstance();
}

Server::~Server()
{
    delete [] conn;
}

//LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!
void Server::init()
{
    //设置优雅关闭
    //sock.setlinger();
    //监听
    sock.setnonblocking();
    sock.bind(&addr);
    sock.listen();
    ep.addFd(sock.getFd(), EPOLLIN | EPOLLRDHUP);      //LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!
    server_state = true;
    //更新静态变量
    HttpData::epfd = ep.getepfd();
    sql->init("localhost",3306,"webserver","root","admin",30);
}
//LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!LT模式!!!!!!

void Server::initlog(bool log)
{
    Log::GetInstance()->setalive(log);
    Log::GetInstance()->init();
    LOG_INFO("========== Server init ==========");
    LOG_INFO("srcDir: %s", root);
}

bool Server::acceptclient(int sockfd)
{
    client_addr.init();
    client_sock = sock.accept(&client_addr);
    if(client_sock<=0)
        return false;
    //初始化连接
    conn[client_sock].init();
    conn[client_sock].sockfd = client_sock;
    //设置非阻塞
    Socket::setnonblocking2(client_sock);
    //添加定时器
    timermanager.pushtimer(conn+client_sock,3000,std::bind(&Server::CloseConn,this,conn+client_sock));
    //上树
    ep.addFd(client_sock, EPOLLIN | EPOLLET| EPOLLONESHOT | EPOLLRDHUP);
    LOG_INFO("New Client:%d IP:%s",client_sock,inet_ntoa(client_addr.addr.sin_addr));
    return true;
}

void Server::evenloop()
{
    printf("Server Start\n");
    LOG_INFO("------------Server Start-------------");
    std::vector<epoll_event> events;
    while(server_state)
    {
        //定时器处理超时连接
        timermanager.update();
        events = ep.poll();
        for (int i = 0; i < events.size(); i++)
        {
            //监听到新连接
            if (events[i].data.fd ==  sock.getFd())
            {
                acceptclient(events[i].data.fd);
            }
            //发生错误
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                CloseConn(conn+events[i].data.fd);
            }
            //读事件
            else if(events[i].events & EPOLLIN)
            {
                pool.AddTask(std::bind(&Server::OnRead,this,conn+events[i].data.fd));
            }
            //写事件
            else if(events[i].events & EPOLLOUT)
            {
                pool.AddTask(std::bind(&Server::OnWrite,this,conn+events[i].data.fd));
            }
            else 
                LOG_ERROR("Other event");
        }
    }
}

void Server::run()
{
    init();
    evenloop();
}

void Server::OnRead(HttpData* user)
{
    int err = 0;
    if(user->read(&err)<=0 && err!= EAGAIN)
    {
        LOG_ERROR("Error In Read %d",user->sockfd);
        CloseConn(user);
        return;
    }
    if(user->parse_and_make())
    {
        //解析成功，设置读事件
        user->timer->update(300);
        Epoll::modfd(user->sockfd,user->epfd, EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP | EPOLLET);
    }
    else
    {
        //请求不完整，继续读
        Epoll::modfd(user->sockfd,user->epfd, EPOLLIN | EPOLLONESHOT | EPOLLRDHUP | EPOLLET);
    }
}

void Server::OnWrite(HttpData* user)
{
    int err = 0;
    int ret = user->write(&err);
    //发送完毕
    if(user->message->bytes_to_send == 0)
    {
        LOG_INFO("FD:%d Send Success",user->sockfd);
        //如果是长连接
        if(user->message->state_conn)
        {
            user->timer->update(1000);
            if(user->message->file_stat.st_size)
            {
                munmap(user->message->maddr,user->message->file_stat.st_size);
                user->message->file_stat.st_size = 0;
            }
            Epoll::modfd(user->sockfd,user->epfd,EPOLLIN | EPOLLONESHOT | EPOLLRDHUP | EPOLLET);
            user->init();
        }
        else
            CloseConn(user);
        return;
    }
    else if(ret <= 0)
    {
        if(err == EAGAIN)
        {
            //继续发
            user->timer->update(200);
            Epoll::modfd(user->sockfd,user->epfd,EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP | EPOLLET);
            return;
        }
        LOG_ERROR("Error In Send %d",user->sockfd);
    }
    CloseConn(user);
}


void Server::CloseConn(HttpData* user)
{
    if(user->alive)
    {
        Epoll::delfd(user->sockfd,user->epfd);
        user->disconnect();
        user->alive = false;
        LOG_INFO("FD:%d Disconnected",user->sockfd);
    }
}