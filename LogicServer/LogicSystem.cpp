#include "LogicSystem.h"
#include "const.h"
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include "MsgNode.h"

LogicSystem::LogicSystem() : _b_stop(false) {
    RegisterCallBacks();
    _worker = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::RegisterCallBacks() {
    _func_callbacks[MSG_HELLO_WORLD] = std::bind(&LogicSystem::HelloWorldCallback,
         this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::HelloWorldCallback(std::shared_ptr<CSession> session,
    short msg_id, std::string msg_data) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(msg_data, root);
    std::cout << "msg id: " << root["id"].asInt() 
        << " msg data: " << root["data"].asString() << std::endl;
    root["data"] = "server has receive data: " + root["data"].asString();
    std::string return_str = root.toStyledString();
    session->Send(return_str, root["id"].asInt());
}

void LogicSystem::DealMsg() {
    for (;;) {
        std::unique_lock<std::mutex> unique_lk(_mutex);
        while (_msg_que.empty() && !_b_stop) {
            _consume.wait(unique_lk);
        }
        if (_b_stop) {
            while (!_msg_que.empty()) {
                auto& msgnode = _msg_que.front();
                std::cout << "msg id: " << msgnode->_recvnode->_msg_id << std::endl;
                auto func_iter = _func_callbacks.find(msgnode->_recvnode->_msg_id);
                if (func_iter == _func_callbacks.end()) {
                    _msg_que.pop();
                    continue;
                }
                func_iter->second(msgnode->_session,
                     msgnode->_recvnode->_msg_id,
                     std::string(msgnode->_recvnode->_data, msgnode->_recvnode->_cur_len));
                _msg_que.pop();
            }
            break;
        }
        auto& msgnode = _msg_que.front();
        std::cout << "msg id: " << msgnode->_recvnode->_msg_id << std::endl;
        auto func_iter = _func_callbacks.find(msgnode->_recvnode->_msg_id);
        if (func_iter == _func_callbacks.end()) {
            _msg_que.pop();
            continue;
        }
        func_iter->second(msgnode->_session,
            msgnode->_recvnode->_msg_id,
            std::string(msgnode->_recvnode->_data, msgnode->_recvnode->_cur_len));
        _msg_que.pop();
    }
}

LogicSystem::~LogicSystem() {
    _b_stop = true;
    _consume.notify_one();
    _worker.join();
}

void LogicSystem::PostMsg2Que(std::shared_ptr<LogicNode> msg) {
    std::unique_lock<std::mutex> unique_lk(_mutex);
    _msg_que.push(msg);
    if (_msg_que.size() == 1) {
        unique_lk.unlock();
        _consume.notify_one();
    }
}

