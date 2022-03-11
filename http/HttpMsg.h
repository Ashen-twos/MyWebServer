#pragma once
#include "HttpEnum.h"
#include <sys/uio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


class HttpMsg{
    public:
        char page404[15] = "./www/404.html";
        static const int MAX_MSG_SIZE = 2048;

    public:
        HttpMsg();
        ~HttpMsg();
        void init();

        HttpEnum::HTTP_CODE code;       //报文类型
        char Msg[MAX_MSG_SIZE];         //应答头缓冲区
        int index_write;                //写下标
        int bytes_to_send;              //需要发送的文件字节数
        int head_had_send;              //头已发送
        int bytes_had_send;             //已发送
        char* maddr;                    //映射地址
        iovec iv;                       //文件缓冲区
        struct stat file_stat;          //文件属性
        bool state_conn;                //连接状态
        char* file_addr;                //指向request的file_addr

    private:
        //状态码
        const char* state_200 = "OK";
        const char* state_404 = "Not Found";
        const char* state_403 = "Forbidden";
        const char* state_500 = "Internal Server Error";
        //content正文
        const char* content_404 = "Page Not Found";
        const char* content_403 = "Forbidden";
        const char* content_500 = "Internal Server Error";
        //通用写缓存函数
        bool writebuf(const char* format, ...);
        //添加状态行
        bool add_state(int state, const char* title);
        //添加Content-Type
        bool add_content_type(const char* type);
        //添加Content-Length
        bool add_content_length(int len);
        //添加Connection
        bool add_connection();
        //添加空行
        bool add_blank_line();
        //添加响应头
        bool add_header(const char* type, int len);
        //添加content正文
        bool add_content(const char* content);

    public:
        //开始制造响应
        bool make_response();

};