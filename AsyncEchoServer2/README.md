# AsyncEchoSever-summary

## Session
- 异步处理读写
- 读完调用返回`async_send`发回给client, 写完继续调用异步读的回调
- 保证可以长时间监听端口
## CServer
- 监听端口, 异步接收连接, 分配给session进行处理
- 异步读写的执行依赖于io_context所运行的事件循环, 所以主函数要有ioc.run()
- ioc就是封装的IO多路复用技术, 可以理解成epoll去监听事件

