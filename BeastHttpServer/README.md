# BeastHttpServer-summary
- 功能: 实现了get, post方法的简单响应
```bash
# 异步监听连接-acceptor要绑定目标监听端口
void http_server(tcp::acceptor& acceptor, tcp::socket& socket);

# 启动服务器, 读取端口数据并解析请求
void read_request();

# 状态机处理请求
void process_request();

# 构造响应报文
void create_response();
void create_post_response();

# 异步发送响应报文
void write_response();

# 超时关闭socket
void check_deadline();
```


# 使用方法
```bash
# 1. 编译
cd BeastHttpServer
sh build.sh

# 2. 运行
# -- server
./BeastHttpServer
```
