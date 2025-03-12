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

## grpc通信例程GrpcServer
- service是用户自己定义的服务端方法
- channel用于管理网络连接, 可实现负载均衡
- 客户端通过与stub的交互, 调用服务端方法, 就像调用本地方法一样
- stub是需要通过channel来构造的
