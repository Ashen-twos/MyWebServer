#pragma once
#include "HttpEnum.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../MySQL/SQLRAII.h"

class HttpRequest{
    public:
        
        static const int MAX_BUF_SIZE = 4096;
        static const int MAX_ADDR_SIZE = 60;
        static char root[50];                              //工作目录

    public:
        HttpRequest();
        ~HttpRequest();
        void init();

        char buff[MAX_BUF_SIZE];        //读入缓冲区
        char file_addr[MAX_ADDR_SIZE];  //文件路径
        char* content;                  //POST缓存
        int content_length;             //content长度
        int index_read;                 //读入下标
        int index_checked;              //已检查
        HttpEnum::METHOD method;                  //http请求方法
        HttpEnum::CHECK_STATE Fsm_state;          //状态机状态
        int start_line;                 //开始行数  
        bool state_conn;                //连接状态
        bool islogin;                   //是否为登录请求

    private:
        //分析请求行
        HttpEnum::HTTP_CODE parse_request(char *buff);
        //分析头部
        HttpEnum::HTTP_CODE parse_header(char *buff);
        //分析是否被完整读入
        HttpEnum::HTTP_CODE parse_content(char *buff);
        //分析一行内容
        HttpEnum::LINE_STATE parse_line(char *buff);
        //分析POST
        bool parse_post();
    public:
        //开始分析
        HttpEnum::HTTP_CODE start_read();
        

};