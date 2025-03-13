#ifndef CSESSION_H
#define CSESSION_H

#include <memory>
#include <queue>
#include <boost/asio.hpp>
using net = boost::asio::ip::tcp;
const int MAX_LENGTH = 1024*2;
const int HEAD_LENGTH = 2;

class MsgNode;
class CServer;
class RecvNode;
class LogicSystem;

class CSession : public std::enable_shared_from_this<CSession> {
public:
    CSession(boost::asio::io_context& ioc, CServer* server);
    net::socket& Socket();

    void Start();
    std::string GetUuid();
    void Send(const char* msg, short max_length, short msg_id);
    void Send(std::string msg, short msg_id);
    // void Send(std::string msg);
    void Close();
    void PrintRecvData(char* data, int length);
    
    std::shared_ptr<CSession> SharedSelf();
    ~CSession();
private:
    // void HandleReadHead(const boost::system::error_code& ec,
    //      std::size_t bytes_transferred, std::shared_ptr<CSession> self_shared);
    // void HandleReadMsg(const boost::system::error_code& ec,
    //     std::size_t bytes_transferred, std::shared_ptr<CSession> self_shared);
    void HandleWrite(const boost::system::error_code& error,
         std::shared_ptr<CSession> self_shared);
    void HandleRead(const boost::system::error_code& error,
         size_t bytes_transferred, std::shared_ptr<CSession> self_shared);
    net::socket _socket;
    char _data[MAX_LENGTH];
    std::string _uuid;

    CServer* _server;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    std::mutex _send_lock;

    bool _b_head_parse;
    std::shared_ptr<RecvNode> _recv_msg_node;
    std::shared_ptr<MsgNode> _recv_head_node;
    bool _b_close;
};

class LogicNode {
    friend class LogicSystem;
public:
    LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);
private:
    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecvNode> _recvnode;
};

#endif //CSESSION_H