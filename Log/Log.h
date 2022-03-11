#pragma once
#include "BlockQue.h"
#include <string>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>

class Log{
    //静态常量
    private:
        static const int MAX_LOG_BUFF = 4096;
        static const int MAX_LOG_PATH = 256;
        static const int MAX_LOG_NAME = 256;
        static const int MAX_LOG_LINE = 20000;

    //接口
    public:
        static Log* GetInstance();
        void write(int level, const char* format, ...);     //生成日志，入队
        void init(const char* path = "./mylog", const char* extension = ".log");                   //初始化
        static void* work(void*args);                       //异步线程回调函数
        void* thread_write();                               //异步线程持续写文件
        void setalive(bool);                                //设置日志状态

        

    private:
        Log();
        ~Log();
        void openfile(const char* path);                //打开文件
        void AddTitle(int level, char* str);            //添加title
        void flush();                                   //刷新缓冲区

    private:
        char log_buff[MAX_LOG_BUFF];    //日志缓冲区
        const char *log_path;           //日志路径
        const char *log_extension;      //日志拓展名
        const char *log_name;           //日志文件名
        BlockQue<std::string> que;     //阻塞队列
        Mutex mutex;                    //互斥锁
        FILE* p_file;                   //日志文件指针
        int line;                       //行数
        int m_today;                    //当前日
        bool alive;                     //是否开启日志
};


//给其他模块用的宏
//do-while是为了解决不加大括号的if的问题的：
/*
    if(1)
        LOG_INFO(); <====此分号无do-while报错
    else
*/
#define LOG_DEBUG(format, ...) do{Log::GetInstance()->write(0,format,##__VA_ARGS__);}while(0);
#define LOG_INFO(format, ...) do{Log::GetInstance()->write(1,format,##__VA_ARGS__);}while(0);
#define LOG_WARN(format, ...) do{Log::GetInstance()->write(2,format,##__VA_ARGS__);}while(0);
#define LOG_ERROR(format, ...) do{Log::GetInstance()->write(3,format,##__VA_ARGS__);}while(0);