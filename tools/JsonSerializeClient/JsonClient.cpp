#include <string>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
const int MAX_LENGTH = 1024*2;
const int HEAD_LENGTH = 2;
using net = boost::asio::ip::tcp;
using namespace boost::asio;


int main() {
    try {
        std::string raw_ip_address = "127.0.0.1";
        unsigned short port_num = 3333;
        boost::asio::ip::address ip_address = 
            boost::asio::ip::address::from_string(raw_ip_address);
        boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc);
    
        boost::system::error_code ec;
        sock.connect(ep, ec);
        if (ec) {
            std::cout 
                << "Failed to connect, error code = " 
                << ec.value() << ". Message: " << ec.message()
                << std::endl;
            return 0;
        }
        // // 序列化后发送消息
        // MsgData msgdata;
        // msgdata.set_id(1001);
        // msgdata.set_data("hello world!");
        // std::string request;
        // msgdata.SerializeToString(&request);

        Json::Value root;
        root["id"] = 1001;
        root["data"] = "hello world!";
        std::string request = root.toStyledString();


        std::size_t request_length = request.size();
        char send_data[MAX_LENGTH] = { 0 };
        unsigned short request_network_length = 
            boost::asio::detail::socket_ops::host_to_network_short(request_length);
        memcpy(send_data, &request_network_length, HEAD_LENGTH);
        memcpy(send_data + HEAD_LENGTH, request.c_str(), request_length);
        sock.send(boost::asio::buffer(send_data, request_length + 2));

        std::cout << "begin to receive..." << std::endl;
        char reply_head[HEAD_LENGTH];
        std::size_t reply_length = sock.receive(
            boost::asio::buffer(reply_head, HEAD_LENGTH));
        short replylen = 0;
        memcpy(&replylen, reply_head, HEAD_LENGTH);
        short host_replylen = boost::asio::detail::socket_ops::network_to_host_short(replylen);
        char reply_msg[MAX_LENGTH] = { 0 };
        std::size_t reply_msg_length = sock.receive(
            boost::asio::buffer(reply_msg, host_replylen));
        // 将接收到的消息反序列化
        Json::Reader reader;
        reader.parse(std::string(reply_msg, reply_msg_length), root);
        std::cout << "msg id is: " << root["id"] 
            << ", msg is: " << root["data"] << std::endl;
        getchar();
    }
    catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
