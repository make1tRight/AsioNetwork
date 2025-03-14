#include "CSession.h"
#include "CServer.h"
#include "MsgNode.h"
#include <thread>
#include <iomanip>
#include <iostream>
#include "LogicSystem.h"
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

const int MAX_SENDQUE_SIZE = 1024;

CSession::CSession(boost::asio::io_context& ioc, CServer* server)
     : _socket(ioc), _server(server) {
    boost::uuids::uuid session_id = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(session_id);

    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession() {
    std::cout << "this is ~CSession() destruct:" << this << std::endl;
}

net::socket& CSession::Socket() {
    return _socket;
}

void CSession::Start() {
    memset(_data, '\0', MAX_LENGTH);
    _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
        std::bind(&CSession::HandleRead,
             this, std::placeholders::_1, std::placeholders::_2, SharedSelf())
    );
}

std::string CSession::GetUuid() {
    return _uuid;
}

void CSession::HandleWrite(const boost::system::error_code& error,
     std::shared_ptr<CSession> self_shared) {
    if (!error) {
        std::lock_guard<std::mutex> lock(_send_lock);
        _send_queue.pop();
        if (!_send_queue.empty()) {
            auto& send_node = _send_queue.front();
            _socket.async_send(
                asio::buffer(
                    send_node->_data, send_node->_total_len),
                std::bind(&CSession::HandleWrite,
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

void CSession::HandleRead(const boost::system::error_code& error,
    size_t bytes_transferred, std::shared_ptr<CSession> self_shared) {
    if (!error) {
        int copy_len = 0;
        while (bytes_transferred > 0) {
            if (!_b_head_parse) {
                if (_recv_head_node->_cur_len + bytes_transferred < HEAD_TOTAL_LEN) {
                    memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, 
                        _data + copy_len, bytes_transferred);
                    _recv_head_node->_cur_len += bytes_transferred;
                    memset(_data, '\0', MAX_LENGTH);
                    _socket.async_read_some(
                        asio::buffer(_data, MAX_LENGTH),
                        std::bind(&CSession::HandleRead,
                             this, std::placeholders::_1, std::placeholders::_2, self_shared)
                    );
                    return;
                }
                int head_remain = HEAD_TOTAL_LEN - _recv_head_node->_cur_len;
                memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
                    _data + copy_len, head_remain);
                copy_len += head_remain;
                bytes_transferred -= head_remain;
                // 解析消息id
                short msg_id = 0;
                memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
                msg_id = asio::detail::socket_ops::network_to_host_short(msg_id);
                if (msg_id > MAX_LENGTH) {
                    std::cout << "invalid msg_id: " << msg_id << std::endl;
                    _server->ClearSession(_uuid);
                    return;
                }
                std::cout << "msg_id: " << msg_id << std::endl;
                // 解析包长
                short msg_len = 0;
                memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
                msg_len = asio::detail::socket_ops::network_to_host_short(msg_len);
                if (msg_len > MAX_LENGTH) {
                    std::cerr << "Invalid msg_len=" << msg_len << std::endl;
                    _server->ClearSession(_uuid);
                    return;
                }
                std::cout << "msg_len: " << msg_len << std::endl;
                _recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);    
                if (bytes_transferred < msg_len) {
                    memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                        _data + copy_len, bytes_transferred);
                    _recv_msg_node->_cur_len += bytes_transferred;
                    memset(_data, '\0', MAX_LENGTH);
                    _socket.async_read_some(
                        asio::buffer(_data, MAX_LENGTH),
                        std::bind(&CSession::HandleRead,
                             this, std::placeholders::_1, std::placeholders::_2, self_shared)
                    );
                    _b_head_parse = true;
                    return;
                }
                memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                    _data + copy_len, msg_len);
                _recv_msg_node->_cur_len += msg_len;
                copy_len += msg_len;
                bytes_transferred -= msg_len;
                _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0'; //
                LogicSystem::GetInstance()->PostMsg2Que(
                    std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));

                _b_head_parse = false;
                _recv_head_node->Clear(); //
                if (bytes_transferred <= 0) {
                    memset(_data, '\0', MAX_LENGTH);
                    _socket.async_read_some(
                        asio::buffer(_data, MAX_LENGTH),
                        std::bind(&CSession::HandleRead,
                             this, std::placeholders::_1, std::placeholders::_2, self_shared)
                    );
                    return;
                }
                continue; /**3.12到这里 */
            }
            int msg_remain = _recv_msg_node->_total_len - _recv_msg_node->_cur_len;
            if (bytes_transferred < msg_remain) {
                memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                    _data + copy_len, bytes_transferred);
                _recv_msg_node->_cur_len += bytes_transferred;
                memset(_data, '\0', MAX_LENGTH);
                _socket.async_read_some(
                    asio::buffer(_data, MAX_LENGTH),
                    std::bind(&CSession::HandleRead,
                         this, std::placeholders::_1, std::placeholders::_2, self_shared)
                );
                return;
            }
            memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                _data + copy_len, msg_remain);
            _recv_msg_node->_cur_len += msg_remain;
            copy_len += msg_remain;
            bytes_transferred -= msg_remain;
            _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0'; //
            LogicSystem::GetInstance()->PostMsg2Que(
                std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
            _b_head_parse = false;
            _recv_head_node->Clear(); //
            if (bytes_transferred <= 0) {
                memset(_data, '\0', MAX_LENGTH);
                _socket.async_read_some(
                    asio::buffer(_data, MAX_LENGTH),
                    std::bind(&CSession::HandleRead,
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

void CSession::Send(const char* msg, short max_length, short msg_id) {
    std::lock_guard<std::mutex> lock(_send_lock);
    int send_queue_size = _send_queue.size();
    // 限制队列长度, 避免无限增长
    if (send_queue_size > MAX_SENDQUE_SIZE) {
        std::cout << "CSession: " << _uuid 
            << " send_queue is full, MAX_SENDQUE_SIZE=" << MAX_SENDQUE_SIZE << std::endl;
        return;
    }

    _send_queue.push(std::make_shared<SendNode>(msg, max_length, msg_id));
    if (send_queue_size > 0) {
        return;
    }
    auto& msgnode = _send_queue.front();
    _socket.async_send(
        asio::buffer(msgnode->_data, msgnode->_total_len),
        std::bind(&CSession::HandleWrite,
            this, std::placeholders::_1, SharedSelf())
    );
}

void CSession::Send(std::string msg, short msg_id) {
    std::lock_guard<std::mutex> lock(_send_lock);
    int send_queue_size = _send_queue.size();
    // 限制队列长度, 避免无限增长
    if (send_queue_size > MAX_SENDQUE_SIZE) {
        std::cout << "CSession: " << _uuid 
            << " send_queue is full, MAX_SENDQUE_SIZE=" << MAX_SENDQUE_SIZE << std::endl;
        return;
    }

    _send_queue.push(std::make_shared<SendNode>
        (msg.c_str(), msg.size(), msg_id));
    if (send_queue_size > 0) {
        return;
    }
    auto& msgnode = _send_queue.front();
    _socket.async_send(
        asio::buffer(msgnode->_data, msgnode->_total_len),
        std::bind(&CSession::HandleWrite,
            this, std::placeholders::_1, SharedSelf())
    );
}

void CSession::Close() {
    _socket.close();
    _b_close = true;
}

std::shared_ptr<CSession> CSession::SharedSelf() {
    return shared_from_this();
}

void CSession::PrintRecvData(char* data, int length) {
    std::stringstream ss;
    std::string result = "0x";
    for (int i = 0; i < length; ++i) {
        std::string hexstr;
        ss << std::hex << std::setw(2) << std::setfill('0') << data[i] << std::endl;
        ss >> hexstr;
        result += hexstr;
    std::cout << "receive raw data is: " << result << std::endl;
    }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, 
    std::shared_ptr<RecvNode> recvnode)
     : _session(session), _recvnode(recvnode) {

}