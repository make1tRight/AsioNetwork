# 关于IOThreadPoolServer-summary
## 关于IOThreadPool
1. 多个线程共用1个IOContext
2. 其余逻辑同IOContextPoolServer

## LogicServer
1. 配置server
2. 优雅退出
3. ioc底层调用epoll, 事件驱动服务器

## CServer
- 监听连接

## CSession
1. 异步读写TLV格式的消息, 实现将消息封装为逻辑节点, 插入逻辑队列
2. 实现线程安全的逻辑队列
3. 逻辑节点需要CSession和RecvNode的信息

## Singleton
1. 为什么要将逻辑层设置成单例
    - 业务逻辑可被多组件共享, 涉及多线程场景, 单例模式确保了线程安全
2. 关于once_flag
    - 因为他决定了是否要构造一个新的实例, 所以要确保生命周期与函数相同
    - 所以once_flag要static

## LogicSystem
1. 从逻辑队列中取出消息, 根据已注册回调对消息进行业务逻辑处理
2. 逻辑系统的核心是用户注册的回调函数
3. id+回调的形式, 实现了将消息根据不同的id进行处理(分发)

# 使用方法
```bash
# 1. 编译
cd IOThreadPoolServer
mkdir build && cd build
cmake ..
make

# 2. 运行
# -- server
./IOThreadPoolServer
# -- client 见../tools/ClientTestIOCandIOT
```

