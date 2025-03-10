#include "Session.h"
#include "CServer.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

MsgNode::MsgNode(const char* msg, int total_len)
     : _total_len(total_len + HEAD_LENGTH), _cur_len(0) {
    // ()申请内存的同时还可以把值初始化为0
    _msg = new char[_total_len + 1]();
    // msg里面只有数据, total_len是本次数据包的长度
    // _msg保存数据是[total_len|msg]
    memcpy(_msg, &total_len, HEAD_LENGTH); //这里第二个参数是*src, 所以要取地址
    memcpy(_msg + HEAD_LENGTH, msg, total_len);
    _msg[_total_len] = '\0';
}

MsgNode::MsgNode(int total_len) : _total_len(total_len), _cur_len(0) {
    _msg = new char[total_len + 1]();
}

MsgNode::~MsgNode() {
    delete[] _msg;
}

void MsgNode::Clear() {
    memset(_msg, '\0', _total_len);
    _cur_len = 0;
}

Session::Session(boost::asio::io_context& ioc, CServer* server)
     : _socket(ioc), _server(server) {
    boost::uuids::uuid session_id = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(session_id);

    _recv_head_node = std::make_shared<MsgNode>(HEAD_LENGTH);
}

Session::~Session() {
    std::cout << "this is ~Session() destruct:" << this << std::endl;
}

net::socket& Session::Socket() {
    return _socket;
}

void Session::Start() {
    memset(_data, '\0', MAX_LENGTH);
    _socket.async_read_some(
        boost::asio::buffer(_data, MAX_LENGTH),
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
        int copy_len = 0;
        while (bytes_transferred > 0) {
            if (!_b_head_parse) {
                if (_recv_head_node->_cur_len + bytes_transferred < HEAD_LENGTH) {
                    memcpy(_recv_head_node->_msg + _recv_head_node->_cur_len, 
                        _data + copy_len, bytes_transferred);
                    _recv_head_node->_cur_len += bytes_transferred;
                    memset(_data, '\0', MAX_LENGTH);
                    _socket.async_read_some(
                        asio::buffer(_data, MAX_LENGTH),
                        std::bind(&Session::HandleRead,
                             this, std::placeholders::_1, std::placeholders::_2, self_shared)
                    );
                    return;
                }
                int head_remain = HEAD_LENGTH - _recv_head_node->_cur_len;
                memcpy(_recv_head_node->_msg + _recv_head_node->_cur_len,
                    _data + copy_len, head_remain);
                copy_len += head_remain;
                bytes_transferred -= head_remain;
                int data_len = 0;
                memcpy(&data_len, _recv_head_node->_msg, HEAD_LENGTH);
                if (data_len > MAX_LENGTH) {
                    std::cerr << "Invalid data_len=" << data_len << std::endl;
                    _server->ClearSession(_uuid);
                    return;
                }
                std::cout << "data_len=" << data_len << std::endl;
                _recv_msg_node = std::make_shared<MsgNode>(data_len);
                
                if (bytes_transferred < data_len) {
                    memcpy(_recv_msg_node->_msg + _recv_msg_node->_cur_len,
                        _data + copy_len, bytes_transferred);
                    _recv_msg_node->_cur_len += bytes_transferred;
                    memset(_data, '\0', MAX_LENGTH);
                    _socket.async_read_some(
                        asio::buffer(_data, MAX_LENGTH),
                        std::bind(&Session::HandleRead,
                             this, std::placeholders::_1, std::placeholders::_2, self_shared)
                    );
                    _b_head_parse = true;
                    return;
                }
                memcpy(_recv_msg_node->_msg + _recv_msg_node->_cur_len,
                    _data + copy_len, data_len);
                _recv_msg_node->_cur_len += data_len;
                copy_len += data_len;
                bytes_transferred -= data_len;
                _recv_msg_node->_msg[_recv_msg_node->_total_len] = '\0'; //
                std::cout << "receive data: " << _recv_msg_node->_msg << std::endl;
                Send(_recv_msg_node->_msg, _recv_msg_node->_total_len);
                _b_head_parse = false;
                _recv_head_node->Clear(); //
                if (bytes_transferred <= 0) {
                    memset(_data, '\0', MAX_LENGTH);
                    _socket.async_read_some(
                        asio::buffer(_data, MAX_LENGTH),
                        std::bind(&Session::HandleRead,
                             this, std::placeholders::_1, std::placeholders::_2, self_shared)
                    );
                    return;
                }
                continue;
            }
            int msg_remain = _recv_msg_node->_total_len - _recv_msg_node->_cur_len;
            if (bytes_transferred < msg_remain) {
                memcpy(_recv_msg_node->_msg + _recv_msg_node->_cur_len,
                    _data + copy_len, bytes_transferred);
                _recv_msg_node->_cur_len += bytes_transferred;
                memset(_data, '\0', MAX_LENGTH);
                _socket.async_read_some(
                    asio::buffer(_data, MAX_LENGTH),
                    std::bind(&Session::HandleRead,
                         this, std::placeholders::_1, std::placeholders::_2, self_shared)
                );
                return;
            }
            memcpy(_recv_msg_node->_msg + _recv_msg_node->_cur_len,
                _data + copy_len, msg_remain);
            _recv_msg_node->_cur_len += msg_remain;
            copy_len += msg_remain;
            bytes_transferred -= msg_remain;
            _recv_msg_node->_msg[_recv_msg_node->_total_len] = '\0'; //
            std::cout << "receive data: " << _recv_msg_node->_msg << std::endl;
            Send(_recv_msg_node->_msg, _recv_msg_node->_total_len);
            _b_head_parse = false;
            _recv_head_node->Clear(); //
            if (bytes_transferred <= 0) {
                memset(_data, '\0', MAX_LENGTH);
                _socket.async_read_some(
                    asio::buffer(_data, MAX_LENGTH),
                    std::bind(&Session::HandleRead,
                         this, std::placeholders::_1, std::placeholders::_2, self_shared)
                );
                return;
            }
        }
    } else {
        std::cout 
            << "Failed to HandleRead, error is "
            << error.message() << std::endl;
        Close();
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

void Session::Close() {
    _socket.close();
    _b_close = true;
}