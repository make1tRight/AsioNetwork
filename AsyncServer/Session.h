#include <memory>
#include <queue>
#include <boost/asio.hpp>
using net = boost::asio::ip::tcp;
const int RECVSIZE = 1024;

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

class Session {
public:
    Session(std::shared_ptr<net::socket> socket);
    void Connect(net::endpoint ep);
    void WriteToSocket(const std::string& buf);
    void WriteCallBack(
        const boost::system::error_code& ec,
        std::size_t bytes_tranferred,
        std::shared_ptr<MsgNode> msg_node);
private:
    std::shared_ptr<net::socket> _socket;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    bool _send_pending;
};