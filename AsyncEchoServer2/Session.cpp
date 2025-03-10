#include "Session.h"
#include "CServer.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
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

Session::Session(boost::asio::io_context& ioc, CServer* server)
     : _socket(ioc), _server(server) {
    boost::uuids::uuid session_id = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(session_id);
}

net::socket& Session::Socket() {
    return _socket;
}

void Session::Start() {
    memset(_data, '\0', max_length);
    _socket.async_read_some(
        boost::asio::buffer(_data, max_length),
        std::bind(&Session::HandleRead,
             this, std::placeholders::_1, std::placeholders::_2, shared_from_this())
    );
}

std::string Session::GetUuid() {
    return _uuid;
}

void Session::HandleWrite(const boost::system::error_code& error,
     std::shared_ptr<Session> self_shared) {
    if (!error) {
        std::lock_guard<std::mutex> lock(_send_lock);
        _send_queue.pop();
        if (!_send_queue.empty()) {
            auto& send_node = _send_queue.front();
            _socket.async_send(
                asio::buffer(
                    send_node->_msg, send_node->_total_len),
                std::bind(&Session::HandleWrite,
                     this, std::placeholders::_1, self_shared)
            );
        }
    } else {
        std::cout 
            << "Failed to HandleWrite, error is "
            << error.message() << std::endl;
        _server->ClearSession(_uuid);
    }
}

void Session::HandleRead(const boost::system::error_code& error,
    size_t bytes_transferred, std::shared_ptr<Session> self_shared) {
    if (!error) {
        std::cout << "receive data: " << _data << std::endl;
        Send(_data, bytes_transferred);
        memset(_data, '\0', max_length);
        _socket.async_read_some(
            asio::buffer(_data, max_length),
            std::bind(&Session::HandleRead,
                this, std::placeholders::_1, std::placeholders::_2, self_shared)
        );
    } else {
        std::cout 
            << "Failed to HandleRead, error is "
            << error.message() << std::endl;
        _server->ClearSession(_uuid);
    }
}

void Session::Send(char* msg, int length) {
    bool pending = false;
    std::lock_guard<std::mutex> lock(_send_lock);
    if (_send_queue.size() > 0) {
        pending = true;
    }
    _send_queue.push(std::make_shared<MsgNode>(msg, length));
    if (pending == true) {
        return;
    }
    auto& send_node = _send_queue.front();
    _socket.async_send(
        asio::buffer(send_node->_msg, send_node->_total_len),
        std::bind(&Session::HandleWrite,
            this, std::placeholders::_1, shared_from_this())
    );
}