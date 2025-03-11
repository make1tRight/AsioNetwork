#include <memory>
#include <queue>
#include <boost/asio.hpp>
using net = boost::asio::ip::tcp;
const int MAX_LENGTH = 1024*2;
const int HEAD_LENGTH = 2;

class MsgNode {
    friend class Session;
public:
    MsgNode(const char* msg, int total_len);
    MsgNode(int total_len);
    ~MsgNode();

    void Clear();
private:
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
    void Close();
    void PrintRecvData(char* data, int length);

    ~Session();
private:
    void HandleWrite(const boost::system::error_code& error,
         std::shared_ptr<Session> self_shared);
    void HandleRead(const boost::system::error_code& error,
         size_t bytes_transferred, std::shared_ptr<Session> self_shared);
    net::socket _socket;
    char _data[MAX_LENGTH];
    std::string _uuid;

    CServer* _server;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    std::mutex _send_lock;

    bool _b_head_parse;
    std::shared_ptr<MsgNode> _recv_msg_node;
    std::shared_ptr<MsgNode> _recv_head_node;
    bool _b_close;
};