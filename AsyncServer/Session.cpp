#include "Session.h"

MsgNode::MsgNode(const char* msg, int total_len) : _total_len(total_len), _cur_len(0) {
    _msg = new char[total_len];
    memcpy(_msg, msg, total_len);
}

MsgNode::MsgNode(int total_len) : _total_len(total_len), _cur_len(0) {
    _msg = new char[total_len];
}

MsgNode::~MsgNode() {
    delete[] _msg;
}

void Session::Connect(net::endpoint ep) {
    _socket->connect(ep);
}

void Session::WriteToSocket(const std::string& buf) {
    _send_queue.emplace(new MsgNode(buf.c_str(), buf.size()));
    if (_send_pending) {
        return;
    }
    this->_socket->async_write_some(
        boost::asio::buffer(buf),
        std::bind(&Session::WriteCallBack,
            std::placeholders::_1, std::placeholders::_2, _send_queue)
    );
    _send_pending = true;
}

void Session::WriteCallBack(
    const boost::system::error_code& ec,
    std::size_t bytes_tranferred,
    std::shared_ptr<MsgNode> msg_node) {
    if (bytes_tranferred + msg_node->_cur_len < msg_node->_total_len) {
        _send_node->_cur_len += bytes_tranferred;
        this->_socket->async_write_some(
            boost::asio::buffer(_send_node->_msg + _send_node->_cur_len,
                 _send_node->_total_len - _send_node->_cur_len),
            std::bind(&Session::WriteCallBackErr,
                 this, std::placeholders::_1, std::placeholders::_2, _send_node)
        );
    }
}
