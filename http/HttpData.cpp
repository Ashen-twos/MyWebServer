#include "HttpData.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int HttpData::epfd = -1;

HttpData::HttpData() :
request(new HttpRequest),message(new HttpMsg),timer(new TimerNode)
{
    sockfd = -1;
    
    alive = false;
}

HttpData::HttpData(int epf, int fd) : HttpData()
{
    sockfd = fd;
}

HttpData::~HttpData()
{

}

void HttpData::init()
{
    alive = true;
    message->init();
    request->init();
}

void HttpData::disconnect()
{
    if(message->file_stat.st_size)
    {
        //解除映射
        munmap(message->maddr,message->file_stat.st_size);
        message->file_stat.st_size = 0;
    }
    close(sockfd);
    init();
}


int HttpData::read(int *err)
{
    int ret = -1;
    while(1)
    {
        ret = recv(sockfd, request->buff+request->index_read,
                    HttpRequest::MAX_BUF_SIZE-request->index_read,0);
        if(ret <= 0)
        {
            *err = errno;
            break;
        }
        request->index_read += ret;
    }
    return ret;
}

int HttpData::write(int *err)
{
    int ret = -1;
    //发响应头
    while(message->head_had_send < message->index_write)
    {
        ret = send(sockfd, message->Msg+message->head_had_send, message->index_write-message->head_had_send, 0);
        if(ret <= 0)
        {
            *err = errno;
            return ret;
        }
        message->head_had_send += ret;
        message->bytes_to_send -= ret;
    }
    //发文件
    while(1)
    {
        ret = writev(sockfd,&message->iv,1);
        if(ret < 0)
        {
            *err = errno;
            return ret;
        }
        message->bytes_to_send -= ret;
        message->bytes_had_send += ret;
        message->iv.iov_base = message->iv.iov_base + ret;
        message->iv.iov_len -= ret;
        if(message->iv.iov_len == 0 || message->bytes_to_send==0)
            return 1;
    }
    return ret;
}

bool HttpData::parse_and_make()
{
    //传递请求状态
    message->file_addr = request->file_addr;
    message->state_conn = request->state_conn;
    //开始解析
    HttpEnum::HTTP_CODE tmp = request->start_read();
    if(tmp == HttpEnum::GET_REQUEST || tmp == HttpEnum::FILE_REQUEST)
    {
        message->code = tmp;
        //开始生成响应
        if(message->make_response())
            return true;
    }
    return false;
}