#include "MsgNode.h"
#include <string.h>
#include <iostream>
#include <boost/asio.hpp>


MsgNode::MsgNode(int total_len) : _total_len(total_len), _cur_len(0) {
    _data = new char[total_len + 1]();
    _data[_total_len] = '\0';
}

MsgNode::~MsgNode() {
    std::cout << "this is ~MsgNode() destruct" << std::endl;
    delete[] _data;
}

void MsgNode::Clear() {
    memset(_data, '\0', _total_len);
    _cur_len = 0;
}

RecvNode::RecvNode(short max_len, short msg_id)
     : MsgNode(max_len), _msg_id(msg_id) {

}

SendNode::SendNode(const char* msg, short max_len, short msg_id)
     : MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id) {
    // 先写id
    short msg_id_network =
        boost::asio::detail::socket_ops::host_to_network_short(msg_id);
    memcpy(_data, &msg_id_network, HEAD_ID_LEN);
    // 再写包长
    short max_len_network = 
        boost::asio::detail::socket_ops::host_to_network_short(max_len);
    memcpy(_data + HEAD_ID_LEN, &max_len_network, HEAD_DATA_LEN);
    // 最后写包体
    memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
    _data[_total_len] = '\0';
}