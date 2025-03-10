#include <memory>
#include <queue>
#include <boost/asio.hpp>
using net = boost::asio::ip::tcp;
const int max_length = 1024;

class MsgNode {
public:
    MsgNode(const char* msg, int total_len);
    MsgNode(int total_len);
    ~MsgNode();
public:
    char* _msg;
    int _total_len;
    int _cur_len;
};


class CServer;
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::io_context& ioc, CServer* server);
    net::socket& Socket();

    void Start();
    std::string GetUuid();
    void Send(char* msg, int length);
private:
    void HandleWrite(const boost::system::error_code& error,
         std::shared_ptr<Session> self_shared);
    void HandleRead(const boost::system::error_code& error,
         size_t bytes_transferred, std::shared_ptr<Session> self_shared);
    net::socket _socket;
    char _data[max_length];
    std::string _uuid;

    CServer* _server;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    std::mutex _send_lock;
};