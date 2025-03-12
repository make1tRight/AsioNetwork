# grpc通信例程GrpcServer
- 功能: 实现了简单的echo应答
- service是用户自己定义的服务端方法
- channel用于管理网络连接, 可实现负载均衡
- 客户端通过与stub的交互, 调用服务端方法, 就像调用本地方法一样
- stub是需要通过channel来构造的

# 使用方法
```bash
# 1. 编译
cd GrpcServer
mkdir build && cd build
cmake ..
make

# 2. 运行
# -- server
./GrpcServer
# -- client
./GrpcClient
```
