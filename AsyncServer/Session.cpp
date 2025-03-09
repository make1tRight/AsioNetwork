#include "Session.h"
#include <iostream>

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
            std::placeholders::_1, std::placeholders::_2)
    );
    _send_pending = true;
}

void Session::WriteAll2Socket(const std::string& buf) {
    _send_queue.emplace(new MsgNode(buf.c_str(), buf.size()));
    if (_send_pending) {
        return;
    }
    this->_socket->async_send(
        boost::asio::buffer(buf),
        std::bind(&Session::WriteAllCallBack,
             std::placeholders::_1, std::placeholders::_1)
    );
    _send_pending = true;
}

void Session::WriteCallBack(
    const boost::system::error_code& ec,
    std::size_t bytes_tranferred) {
    if (ec.value() != 0) {
        std::cout << "write err, code is: " << ec.value()
         << ". Message is: " << ec.message() << std::endl;
        return;
    }
    auto& send_node = _send_queue.front();
    send_node->_cur_len += bytes_tranferred;
    if (send_node->_cur_len < send_node->_total_len) {
        // 这里启动完以后, 会直接返回, 但是写操作不一定完成了
        this->_socket->async_write_some(
            boost::asio::buffer(send_node->_msg + send_node->_cur_len,
                send_node->_total_len - send_node->_cur_len),
            std::bind(&Session::WriteCallBack,
                 this, std::placeholders::_1, std::placeholders::_2)
        );
        // 加上return防止写操作还没完成就把节点pop出去
        return;
    }
    _send_queue.pop();
    if (_send_queue.empty()) {
        _send_pending = false;
    }
    if (!_send_queue.empty()) {
        auto& send_node = _send_queue.front();
        this->_socket->async_write_some(
            boost::asio::buffer(send_node->_msg + send_node->_cur_len,
                send_node->_total_len - send_node->_cur_len),
            std::bind(&Session::WriteCallBack,
                this, std::placeholders::_1, std::placeholders::_2)
        );
    }
}

void Session::WriteAllCallBack(
    const boost::system::error_code& ec,
    std::size_t bytes_tranferred) {
    if (ec.value() != 0) {
        std::cout << "write err, code is: " << ec.value()
            << ". Message is: " << ec.message() << std::endl;
        return;
    }
    _send_queue.pop();
    if (_send_queue.empty()) {
        _send_pending = false;
    }
    if (!_send_queue.empty()) {
        auto& send_node = _send_queue.front();
        this->_socket->async_send(
            boost::asio::buffer(
                send_node->_msg + send_node->_cur_len,
                send_node->_total_len - send_node->_cur_len),
            std::bind(&Session::WriteCallBack,
                 this, std::placeholders::_1, std::placeholders::_2)
        );
    }
}

void Session::ReadFromSocket() {
    if (_recv_pending) {
        return;
    }
    _recv_node = std::make_shared<MsgNode>(RECVSIZE);
    this->_socket->async_read_some(
        boost::asio::buffer(_recv_node->_msg, _recv_node->_total_len),
        std::bind(&Session::ReadCallBack,
             this, std::placeholders::_1, std::placeholders::_2)
    );
    _recv_pending = true;
}

void Session::ReadAllFromSocket() {
    if (_recv_pending) {
        return;
    }
    _recv_node = std::make_shared<MsgNode>(RECVSIZE);
    this->_socket->async_receive(
        boost::asio::buffer(_recv_node->_msg, _recv_node->_total_len),
        std::bind(&Session::ReadAllCallBack,
             this, std::placeholders::_1, std::placeholders::_2)
    );
    _recv_pending = true;
}

void Session::ReadCallBack(boost::system::error_code& ec,
    std::size_t bytes_tranferred) {
    _recv_node->_cur_len += bytes_tranferred;
    if (_recv_node->_cur_len < _recv_node->_total_len) {
        this->_socket->async_read_some(
            boost::asio::buffer(_recv_node->_msg + _recv_node->_cur_len,
                _recv_node->_total_len - _recv_node->_cur_len),
            std::bind(&Session::ReadCallBack,
                 this, std::placeholders::_1, std::placeholders::_2)
        );
        return;
    }
    /* 读取到的数据投递到逻辑线程, 让逻辑线程进行处理*/
    _recv_pending = false;
    _recv_node = nullptr;
}


void Session::ReadAllCallBack(
    boost::system::error_code& ec,
    std::size_t bytes_tranferred) {
    _recv_node->_cur_len += bytes_tranferred;
    /* 读取到的数据投递到逻辑线程, 让逻辑线程进行处理*/
    _recv_pending = false;
    _recv_node = nullptr;
}
