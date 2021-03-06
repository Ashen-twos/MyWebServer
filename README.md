轻量级高性能Web服务器，目前支持GET、POST请求解析，拥有用户登录、注册功能。


**核心技术**

* 主从**Reactor**并发模型，底层选用 非阻塞IO + epoll ET模式 实现IO多路复用
* 使用有限**状态机**完成HTTP请求解析，支持长连接
* 利用**线程池**复用线程资源，通用std::forward实现任务的完美转发
* 基于小根堆的**定时器**处理超时请求
* 采用单例模式与阻塞队列实现了异步 **日志系统** ，记录服务器运行状态
* 使用RAII机制实现 **数据库连接池** ，完成用户数据的CGI校验

![Alt](https://github.com/Ashen-twos/pic/blob/0314de412cf4f95827a98fd6cdb46def0d7f106f/%E4%BB%A3%E7%A0%81%E7%BB%9F%E8%AE%A1.png)
