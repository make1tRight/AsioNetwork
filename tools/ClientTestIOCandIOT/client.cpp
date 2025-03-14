#include <string>
#include <thread>
#include <iostream>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
const int MAX_LENGTH = 1024*2;
const int HEAD_DATA_LENGTH = 2;
const int HEAD_ID_LENGTH = 2;
const int HEAD_TOTAL_LENGTH = 4;
using net = boost::asio::ip::tcp;
using namespace boost::asio;

std::vector<std::thread> vec_threads;
int main() {
    try {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            vec_threads.emplace_back([]() {
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
                    for (int j = 0; j < 5000; ++j) {
                        Json::Value root;
                        root["id"] = 1001;
                        root["data"] = "hello world!";
                        std::string request = root.toStyledString();
                        std::size_t request_length = request.size();
                        int msgid = 1001;
                        msgid = boost::asio::detail::socket_ops::host_to_network_short(msgid);
                        char send_data[MAX_LENGTH] = { 0 };
                        unsigned short request_network_length = 
                            boost::asio::detail::socket_ops::host_to_network_short(request_length);
                        memcpy(send_data, &msgid, HEAD_ID_LENGTH);
                        memcpy(send_data + HEAD_ID_LENGTH, &request_network_length, HEAD_DATA_LENGTH);
                        memcpy(send_data + HEAD_TOTAL_LENGTH, request.c_str(), request_length);
                        sock.send(boost::asio::buffer(send_data, request_length + HEAD_TOTAL_LENGTH));

                        std::cout << "begin to receive..." << std::endl;
                        char reply_head[HEAD_TOTAL_LENGTH];
                        std::size_t reply_length = sock.receive(
                            boost::asio::buffer(reply_head, HEAD_TOTAL_LENGTH));
                        msgid = 0;
                        memcpy(&msgid, reply_head, HEAD_ID_LENGTH);
                        msgid = boost::asio::detail::socket_ops::network_to_host_short(msgid);
                        short replylen = 0;
                        memcpy(&replylen, reply_head + HEAD_ID_LENGTH, HEAD_DATA_LENGTH);
                        replylen = boost::asio::detail::socket_ops::network_to_host_short(replylen);
                        char reply_msg[MAX_LENGTH] = { 0 };
                        std::size_t reply_msg_length = sock.receive(
                            boost::asio::buffer(reply_msg, replylen));
                        // 将接收到的消息反序列化
                        Json::Reader reader;
                        reader.parse(std::string(reply_msg, reply_msg_length), root);
                        std::cout << "msg id is: " << root["id"] 
                            << ", msg is: " << root["data"] << std::endl;
                        std::cout << "j=" << j << std::endl;
                    }
                } catch(std::exception& e) {
                    std::cerr << "Exception: " << e.what() << std::endl;
                }
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        for (auto& t : vec_threads) {
            t.join();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "duration: " << duration.count() << std::endl;
        getchar();
    }
    catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
