# AsioNetwork

## 实现回射服务器EchoServer
### session
- 用于处理echo任务
### server
1. 初始化、监听端口并accept对应连接
2. 创建线程完成session任务

## 实现异步读写的回射服务器AsyncEchoServer
1. ioc内部调用epoll_wait(linux), 事件驱动完成读写
2. 如果是windows, ioc内部调用iocp

## 异步读写回射服务器改良AsyncEchoServer2
1. 伪闭包机制管理session生命周期
2. 锁+队列保证数据异步读写顺序

## 使用jsoncpp进行序列化的JsonServer
- async_read -> 读取消息头与读取消息体的逻辑解耦

## grpc通信例程GrpcServer
- 实现了简单的echo应答

## 实现带逻辑系统的服务器LogicServer
- 实现了根据不同消息类型(消息id)进行消息分发

## 实现多线程模型的服务器
### IOContextPoolServer
1. IOContextPool构造CPU核心数的iocontext与thread
2. 各thread运行iocontext的run函数
3. 更好利用cpu多核性能优势
### IOThreadPoolServer
1. IOThreadtPool构造CPU核心数的thread和1个iocontext
2. 各thread运行iocontext的run函数, 多线程共享1个iocontext

## beast网络库实现简易http服务器
- 实现了get, post方法的简单响应示例