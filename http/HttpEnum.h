#pragma once

class HttpEnum{
    public:
        //http请求方法
        enum METHOD {
            GET = 0,
            POST,
            HEAD,
            PUT,
            DELETE,
            TRACE,
            OPTIONS,
            CONNECT,
            PATH
        };
        //主状态
        enum CHECK_STATE {               
            CHECK_REQUESTLINE=0,        //分析请求行    
            CHECK_HEADER,               //分析头部
            CHECK_CONTENT               //分析入口
        };
        //从状态
        enum LINE_STATE {           
            LINE_OK=0,              //行完整
            LINE_BAD,               //行错误
            LINE_LOSS               //行缺失
        };
        //http结果
        enum HTTP_CODE {
            GET_REQUEST=0,            //获得请求
            NO_REQUEST,             //无请求
            BAD_REQUEST,            //请求错误
            FORBIDDEN_REQUEST,      //权限不足
            INTERNAL_ERROR,         //服务器内部错误
            CLOSE_CONNECTION,        //客户端已关闭
            FILE_REQUEST            //文件请求
        };
};