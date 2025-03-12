# AsioNetwork

## 实现回射服务器EchoServer
- session
1. 用于处理echo任务
- server
1. 初始化、监听端口并accept对应连接
2. 创建线程完成session任务
- main
1. 启动服务器

## 实现异步读写的回射服务器AsyncEchoServer
- ioc内部调用epoll_wait, 事件驱动完成读写

## 异步读写回射服务器改良AsyncEchoServer2
- 伪闭包机制管理session生命周期
- 锁+队列保证数据异步读写顺序

## 使用jsoncpp进行序列化的JsonServer
- async_read -> 读取消息头与读取消息体的逻辑解耦

## grpc通信例程GrpcServer
- 实现了简单的echo应答
