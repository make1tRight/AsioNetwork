# AsyncSever-summary

## 异步写
1. 触发写操作后立即返回, 每次写操作结束都会调用回调函数
2. `async_write_some`是否写完都可能触发回调
3. `async_send`触发回调的情况只有两种
    1. 已经写完
    2. 写出错
4. 通过队列的形式来保证调用写操作的有序性
## 异步读
1. 构造一个空的字符数组用于存储数据
2. `async_write_read`是否写完都可能触发回调
3. `async_receive`触发回调的情况只有两种
    1. 已经读完
    2. 读出错
4. 通过pending来保证每次读操作都正常完成
