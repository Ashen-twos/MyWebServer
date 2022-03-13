#include "HttpMsg.h"
#include <string.h>

HttpMsg::HttpMsg()
{
    init();
}

HttpMsg::~HttpMsg()
{
    
}

void HttpMsg::init()
{
    memset(Msg,0,sizeof Msg);
    code = HttpEnum::NO_REQUEST;
    index_write = bytes_to_send = bytes_had_send = head_had_send = 0;
    file_stat.st_size = 0;
    maddr = nullptr;
    state_conn = false;
    file_addr = nullptr;
}

bool HttpMsg::writebuf(const char* format,...)
{
    if(index_write >= HttpMsg::MAX_MSG_SIZE)
        return false;

    //参数列表
    va_list args;
    //初始化
    va_start(args,format);
    int len = vsnprintf(Msg + index_write, HttpMsg::MAX_MSG_SIZE-1-index_write, format, args);
    //写入越界
    if(len >= HttpMsg::MAX_MSG_SIZE-1-index_write)
    {
        va_end(args);
        return false;
    }
    index_write += len;
    va_end(args);
    return true;
}

//添加状态行
bool HttpMsg::add_state(int state, const char* title)
{
    return writebuf("%s %d %s\r\n", "HTTP/1.1", state, title);
}

//添加Content-Type
bool HttpMsg::add_content_type(const char* type)
{
    return writebuf("Content-Type:%s\r\n",type);
}

//添加Content-Length
bool HttpMsg::add_content_length(int len)
{
    return writebuf("Content-Length:%d\r\n",len);
}

//添加Connection
bool HttpMsg::add_connection()
{
    return writebuf("Connection:%s\r\n",state_conn?"Keep-Alive":"Close");
}

//添加空行
bool HttpMsg::add_blank_line()
{
    return writebuf("\r\n");
}

//添加响应头
bool HttpMsg::add_header(const char* type,int len)
{
    add_content_type(type);
    add_content_length(len);
    add_connection();
    add_blank_line();
    return true;
}

//添加content正文
bool HttpMsg::add_content(const char* content)
{
    return writebuf( "%s\r\n", content);
}

//开始响应
bool HttpMsg::make_response()
{
    //printf("start make res\n");
    switch(code)
    {
        //500
        case HttpEnum::INTERNAL_ERROR:
        {
            //添加状态行
            if(!add_state(500,state_500))
                return false;
            //添加消息头
            if(!add_header("text",strlen(content_500)))
                return false;
            //添加消息正文
            if(!add_content(content_500))
                return false;
            break;
        }
        //404
        case HttpEnum::BAD_REQUEST:
        {
            //添加状态行
            if(!add_state(404,state_404))
                return false;
            //添加消息头
            if(!add_header("text",strlen(content_404)))
                return false;
            //添加消息正文
            if(!add_content(content_404))
                return false;
            break;
        }
        //403
        case HttpEnum::FORBIDDEN_REQUEST:
        {
            //添加状态行
            if(!add_state(403,state_403))
                return false;
            //添加消息头
            if(!add_header("text",strlen(content_403)))
                return false;
            //添加消息正文
            if(!add_content(content_403))
                return false;
            break;
        }
        //200
        case HttpEnum::FILE_REQUEST:
        {
            //添加状态行
            if(!add_state(200,state_200))
                return false;
            //获取文件属性
            stat(file_addr, &file_stat);
            //文件不存在
            if(!file_stat.st_size)
            {
                file_addr = page404;
                stat(file_addr, &file_stat);
            }
            //映射
            int fds = open(file_addr,O_RDONLY);
            maddr = (char *)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fds, 0);
            close(fds);
            //添加响应头
            if(!add_header("html",file_stat.st_size))
                return false;
            //设置文件的返回指针
            iv.iov_base = maddr;
            iv.iov_len = file_stat.st_size;
            bytes_to_send += file_stat.st_size;
            break;
        }
        default:
            return -1;
    }
    bytes_to_send +=  index_write;
    return true;
}