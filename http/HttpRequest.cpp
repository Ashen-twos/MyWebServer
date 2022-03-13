#include "HttpRequest.h"
#include <string.h>

char HttpRequest:: root[50] = "";

HttpRequest::HttpRequest():
index_read(0),index_checked(0), start_line(0), method(HttpEnum::GET)
{
    init();
}

HttpRequest::~HttpRequest()
{
    
}

void HttpRequest::init()
{
    memset(buff,0,sizeof buff);
    memset(file_addr,0,sizeof file_addr);
    strcpy(file_addr,root);
    index_checked = index_read = 0;
    method = HttpEnum::GET;
    Fsm_state = HttpEnum::CHECK_REQUESTLINE;
    start_line = 0;
    state_conn = false;
    islogin = false;
    content = nullptr;
}

HttpEnum::LINE_STATE HttpRequest::parse_line(char *buff)
{
    for(; index_checked<=index_read; ++(index_checked))
    {
        if(buff[index_checked] == '\r')
        {
            if(index_checked+1 == index_read)
                return HttpEnum::LINE_LOSS;
            else if(buff[index_checked+1] == '\n')
            {
                buff[index_checked++] = '\0';
                buff[index_checked++] = '\0';
                return HttpEnum::LINE_OK;
            }
            else 
                return HttpEnum::LINE_BAD;
            
        }
        else if(buff[index_checked] == '\n')
        {
            if(index_checked==0)
                return HttpEnum::LINE_BAD;
            else
            {
                if(buff[index_checked-1] == '\r')
                {
                    buff[index_checked++] = '\0';
                    buff[index_checked++] = '\0';
                    return HttpEnum::LINE_OK;
                }
                else
                    return HttpEnum::LINE_BAD;
            }
        }
    }
    return HttpEnum::LINE_LOSS;
}

HttpEnum::HTTP_CODE HttpRequest::parse_request(char *buff)
{
    char* p_start = buff;                   //起始指针
    char* p_end = strpbrk(buff, " \t");     //结束指针
    if(!p_end)
        return HttpEnum::BAD_REQUEST;
    *p_end++ = '\0';
    //判断Methed
    if(strcasecmp(p_start,"GET") == 0)
        method = HttpEnum::GET;
    else if(strcasecmp(p_start,"POST") == 0)
        method = HttpEnum::POST;
    else
        return HttpEnum::BAD_REQUEST;
    
    //判断URL
    p_start = p_end + strspn(p_end, " \t");
    p_end = strpbrk(p_start, " \t");
    if(!p_end)
        return HttpEnum::BAD_REQUEST;
    *p_end++ = '\0';
    strcat(file_addr,p_start);

    //只有/
    if(strcasecmp(p_start,"/")==0)
    {
        if(method == HttpEnum::GET)
            strcat(file_addr,"index.html");
        else
            strcat(file_addr,"404.html");
    }
    //判断登录还是注册
    if(method == HttpEnum::GET)
    {
        int len = strlen(file_addr);
        if(strcasecmp(p_start,"/1")==0)
        {
            file_addr[len-1] = '\0';
            strcat(file_addr,"register.html");
        }
        else if(strcasecmp(p_start,"/2")==0)
        {
            file_addr[len-1] = '\0';
            strcat(file_addr,"login.html");
        }
    }
    //判断http版本
    p_start = p_end + strspn(p_end, " \t");
    if(strcasecmp(p_start,"HTTP/1.1") && strcasecmp(p_start,"HTTP/1.0"))
        return HttpEnum::BAD_REQUEST;
    //开始解析header
    Fsm_state = HttpEnum::CHECK_HEADER;
    return HttpEnum::NO_REQUEST;
}

HttpEnum::HTTP_CODE HttpRequest::parse_header(char *buff)
{
    //header结束
    if(*buff == '\0')
    {
        if(method == HttpEnum::POST)
            Fsm_state = HttpEnum::CHECK_CONTENT;
        return HttpEnum::GET_REQUEST;
    }
    //判断连接状态
    else if(strncasecmp(buff, "Connection:", 11) == 0)
    {
        buff += 11;
        buff += strspn(buff, " \t");
        if(strncasecmp(buff,"Keep-Alive",10) == 0)
            state_conn = true;
        else
            state_conn = false;
    }
    //获取content长度
    else if(strncasecmp(buff,"Content-Length:",15) == 0)
    {
        buff += 15;
        buff += strspn(buff, " \t");
        content_length = atol(buff);
    }
    return HttpEnum::NO_REQUEST;
}

HttpEnum::HTTP_CODE HttpRequest::parse_content(char *buff)
{
    if (index_read >= (content_length + index_checked))
    {
        buff[content_length] = '\0';
        content = buff;
        return HttpEnum::GET_REQUEST;
    }
    return HttpEnum::NO_REQUEST;
}

HttpEnum::HTTP_CODE HttpRequest::start_read()
{
    //是否完成content的解析
    bool content_check = false;
    HttpEnum::LINE_STATE linestate = HttpEnum::LINE_OK;
    HttpEnum::HTTP_CODE httpcode = HttpEnum::NO_REQUEST;
    //！！！！！！！！！！！！！！！！！POST的content没有\r\n，需要额外判断
    while ((Fsm_state == HttpEnum::CHECK_CONTENT && linestate == HttpEnum::LINE_OK) || (linestate = parse_line(buff)) == HttpEnum::LINE_OK)
    {
        char *temp = buff + start_line;
        start_line = index_checked;
        switch(Fsm_state)
        {
            //解析请求行
            case HttpEnum::CHECK_REQUESTLINE:
            {
                httpcode = parse_request(temp);
                if(httpcode == HttpEnum::BAD_REQUEST)
                    return HttpEnum::BAD_REQUEST;
                break;
            }
            //解析头部
            case HttpEnum::CHECK_HEADER:
            {
                httpcode = parse_header(temp);
                if(httpcode == HttpEnum::BAD_REQUEST)
                    return HttpEnum::BAD_REQUEST;
                break;
            }
            //解析content
            case HttpEnum::CHECK_CONTENT:
            {
                httpcode = parse_content(temp);
                if(httpcode == HttpEnum::BAD_REQUEST)
                    return HttpEnum::BAD_REQUEST;
                content_check = true;
                break;
            }
            default:
                return HttpEnum::INTERNAL_ERROR;
        }
        if(content_check)
            break;
    }
    if(method == HttpEnum::GET)
        return HttpEnum::FILE_REQUEST;
    else if(method == HttpEnum::POST)
    {
        //解析账号密码
        if(parse_post())
        {
            int len = strlen(file_addr);
            file_addr[len-1] = '\0';
            strcat(file_addr,"welcome.html");
        }
        else
        {
            int len = strlen(file_addr);
            file_addr[len-1] = '\0';
            strcat(file_addr,"error.html");
        }
        return HttpEnum::FILE_REQUEST;
    }
}

bool HttpRequest::parse_post()
{
    if(content == nullptr)
    {
        LOG_ERROR("No Content");
        return false;
    }
    //处理content中的密码
    //account=******&password=******
    char account[40], password[40];
    int i=8,j;
    for(j=0; *(content+i) != '&'; j++,i++)
        account[j] = *(content+i);
    account[j] = '\0';
    i += 10;
    for(j=0; *(content+i) != '\0'; j++,i++)
        password[j] = *(content+i);
    password[j] = '\0';
    if(strlen(account)==0 || strlen(password)==0)
    {
        LOG_DEBUG("Blank Account Or Password");
        return false;
    }
    LOG_INFO("Account: %s  Password: %s",account,password);
    //定位到最后
    char* p = strrchr(file_addr,'/');
    //创建SQL变量
    MYSQL* con;
    MYSQL_FIELD* fields = nullptr;
    MYSQL_RES* res = nullptr;
    MYSQL_ROW row;
    char order[256] = "";
    //获取连接
    SQLRAII(&con,MySQLPool::GetInstance());
    //查询
    snprintf(order,256,"SELECT user_name, user_password FROM user WHERE user_name = '%s' LIMIT 1;",account);
    LOG_INFO("Query: %s",order);
    if(mysql_query(con,order))
    {
        LOG_ERROR("SQL Query Error");
        mysql_free_result(res);
        return false;
    }
    //获得返回
    res = mysql_store_result(con);
    fields = mysql_fetch_fields(res);
    //数据库中找到该账户
    while(row = mysql_fetch_row(res))
    {
        //1 代表注册
        if(*(p+1) == '1')
        {
            LOG_DEBUG("Account used");
            return false;
        }
        //2 代表登录
        else if(*(p+1) == '2')
        {
            if(strcmp(row[1],password) == 0)
            {
                LOG_INFO("Login Success");
                return true;
            }
            else
            {
                LOG_DEBUG("password error");
                return false;
            }
        }
        else 
            return false;
    }
    //!!!!!!!!!!!!!!!!!!FREE!!!!!!!!!!!!!!!!!!!!
    mysql_free_result(res);
    //数据库中没有该账户
    //1 代表注册
    if(*(p+1) == '1')
    {
        memset(order,0,sizeof order);
        snprintf(order, 256,"INSERT INTO user(user_name, user_password) VALUES('%s','%s');", account, password);
        if(mysql_query(con,order))
        {
            mysql_free_result(res);
            LOG_ERROR("Register Fail");
            return false;
        }
        LOG_INFO("Register Success");
        return true;
    }
    //2 代表登录
    else if(*(p+1) == '2')
    {
        LOG_DEBUG("Account Error");
        return false;
    }
    return false;
}