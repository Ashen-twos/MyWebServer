#include "Log.h"


Log::Log() : alive(false)
{

}

Log::~Log()
{

}

void Log::setalive(bool isalive)
{
    alive = isalive;
}

Log* Log::GetInstance()
{
    static Log ret;
    return &ret;
}

void Log::init(const char* path, const char* extension)
{
    if(!alive)
        return;
    log_path = path;
    log_extension = extension;
    memset(log_buff, '\0', sizeof(log_buff));
    //创建异步线程
    pthread_t pth;
    pthread_create(&pth,NULL,work,NULL);
    //获取时间
    time_t rawtime = time(nullptr);
    tm *sysTime = localtime(&rawtime);
    struct tm* local_time = localtime(&rawtime);
    struct tm tim = *local_time;
    m_today = tim.tm_mday;
    //创建目录
    char file_name[MAX_LOG_NAME] = "";
    snprintf(file_name, MAX_LOG_NAME-1, "%s/%04d_%02d_%02d%s",
                path, tim.tm_year+1900, tim.tm_mon+1, tim.tm_mday, extension);
    openfile(file_name);
    //目录不存在
    if(p_file == nullptr)
    {
        mkdir(log_path,0777);
        p_file = fopen(file_name,"a");
    }
}

void* Log::work(void*args)
{
    Log::GetInstance()->thread_write();
    return NULL;
}

void* Log::thread_write()
{
    std::string temp;
    while (que.pop(temp))
    {
        //printf("start thread write\n");
        mutex.lock();
        fputs(temp.c_str(),p_file);
        fflush(p_file);
        mutex.unlock();
        //printf("finish thread write\n");
    }
    return NULL;
}

void Log::flush()
{
    mutex.lock();
    fflush(p_file);
    mutex.unlock();
}

void Log::openfile(const char* path)
{
    if(p_file)
    {
        fflush(p_file);
        fclose(p_file);
    }
    p_file = fopen(path,"a");
}

void Log::AddTitle(int level, char* str)
{
    //printf("start add title\n");
    switch(level)
    {
        case 0:
            strcpy(str,"[debug]:");
            break;
        case 1:
            strcpy(str,"[info]:");
            break;
        case 2:
            strcpy(str,"[warn]:");
            break;
        case 3:
            strcpy(str,"[errno]:");
            break;
        default:
            strcpy(str,"[info]:");
            break;
    }
}

void Log::write(int level, const char* format, ...)
{
    if(!alive)
        return;
    //printf("start write log\n");
    //获取时间
    struct timeval now ={0, 0};
    gettimeofday(&now,NULL);
    time_t sec = now.tv_sec;
    struct tm *local_time = localtime(&sec);
    struct tm tim = *local_time;
    //处理参数
    va_list valist;
    va_start(valist,format);    
    //打开或创建文件
    mutex.lock();
    line++;
    //如果日期发生变化，或行数达到上限
    if(m_today != tim.tm_mday || line % MAX_LOG_LINE == 0)
    {
        char new_path[MAX_LOG_PATH] = "";
        char new_name[36] = "";
        snprintf(new_name, 36, "%04d_%02d_%02d",tim.tm_year+1900, tim.tm_mon+1, tim.tm_mday);
        if(m_today != tim.tm_mday)
        {
            snprintf(new_path,MAX_LOG_PATH-72, "%s/%s%s",log_path,new_name,log_extension);
            m_today = tim.tm_mday;      //更新日期
            line = 0;                   //新一天，行数清零
        }
        else
            snprintf(new_path,MAX_LOG_PATH-72, "%s/%s-%d%s",log_path,new_name,line/MAX_LOG_LINE,log_extension);
        openfile(new_path);
    }
    mutex.unlock();

    //添加title
    char title[20];
    AddTitle(level,title);

    std::string newlog;
    //开始写入
    mutex.lock();
    //写入时间
    int n = sprintf(log_buff, "%d-%02d-%02d %02d:%02d:%02d %s ",
                    tim.tm_year+1900, tim.tm_mon+1, tim.tm_mday,
                    tim.tm_hour, tim.tm_min, tim.tm_sec, title);
    int m = vsnprintf(log_buff+n, MAX_LOG_BUFF-1,format,valist);
    log_buff[n+m] = '\n';
    log_buff[n+m+1] = '\0';
    newlog = log_buff;
    mutex.unlock();

    if(!que.full())
    {
        que.push(newlog);
        //printf("push finish\n");
    }
    va_end(valist);
    //刷新缓冲区
    flush();
}