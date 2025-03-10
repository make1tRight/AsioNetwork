# AsyncEchoSever2-summary

## Session
- 异步处理读写
- HandleRead和HandleWrite分别是async_read_some和async_send的封装, 用于处理读写
- HandleRead调用Send以后持续触发HandleRead回调, 保证事件到来时都能进行读取
- Send内部调用HandleWrite
## CServer
- 监听端口, 异步接收连接, 分配给session进行处理
- 实现map+ClearSession管理session的生命周期
- 避免在读写过程中session过早析构导致崩溃
