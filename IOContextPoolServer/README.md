# IOContextPoolServer-summary
## 关于IOContextPool
- worker保证iocontext不会提前退出
    1. iocontext进行的是异步操作, 触发对应的操作后会立即返回
    2. 如果返回以后iocontext没有其他事件到来, 就会直接退出
    3. iocontext提前退出会导致所有正在进行的异步操作都被取消
    4. 因此, 需要一个work来保证iocontext不提前退出
    5. `WorkPtr`调用reset方法, 让unique_ptr置空并释放, 调用work的析构函数, 此时iocontext可安全退出
- 为什么要根据CPU核数来封装池中实例的个数?
    1. `CPU核数>IOContext数`, 可能会有CPU核心处于空闲状态, 线程处理任务效率不高可能导致IO任务排队的情况
    2. `CPU核数<IOContext数`, 会出现多线程竞争一个CPU核心的情况, 涉及线程切换, 导致上下文切换, 增加开销
- 关于`_works[i] = std::unique_ptr<Work>(new Work(_io_contexts[i]))`
    1. unique_ptr的初始化要么放在构造函数的初始化列表中初始化
    2. 要么通过临时的std::unique_ptr右值初始化, 这行代码采取的是本方案 

## LogicServer
- 配置server
- 优雅退出
- ioc底层调用epoll, 事件驱动服务器

## CServer
- 监听连接

## CSession
- 异步读写TLV格式的消息, 实现将消息封装为逻辑节点, 插入逻辑队列
- 实现线程安全的逻辑队列
- 逻辑节点需要CSession和RecvNode的信息

## Singleton
- 为什么要将逻辑层设置成单例
    - 业务逻辑可被多组件共享, 涉及多线程场景, 单例模式确保了线程安全
- 关于once_flag
    - 因为他决定了是否要构造一个新的实例, 所以要确保生命周期与函数相同
    - 所以once_flag要static

## LogicSystem
- 从逻辑队列中取出消息, 根据已注册回调对消息进行业务逻辑处理
- 逻辑系统的核心是用户注册的回调函数
- id+回调的形式, 实现了将消息根据不同的id进行处理(分发)

# 使用方法
```bash
# 1. 编译
cd IOContextPoolServer
mkdir build && cd build
cmake ..
make

# 2. 运行
# -- server
./IOContextPoolServer
# -- client 见../tools/ClientTlvFormat
```

