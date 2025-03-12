#include <string>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
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

        std::thread send_thread([&sock]() {
            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                const char* msg = "hello world";
                std::size_t msg_length = std::strlen(msg);
                char send_msg[msg_length + HEAD_LENGTH];
                memset(send_msg, '\0', msg_length + HEAD_LENGTH);
                memcpy(send_msg, &msg_length, HEAD_LENGTH);
                memcpy(send_msg + HEAD_LENGTH, msg, msg_length);
                boost::asio::write(sock, buffer(send_msg, msg_length + HEAD_LENGTH));
            }
        });

        std::thread recv_thread([&sock]() {
            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                std::cout << "begin to receive data..." << std::endl;
                char reply_head[HEAD_LENGTH];
                memset(reply_head, '\0', HEAD_LENGTH);
                boost::asio::read(sock, buffer(reply_head, HEAD_LENGTH));
                std::size_t reply_length = 0;
                memcpy(&reply_length, reply_head, HEAD_LENGTH);
                char reply_body[MAX_LENGTH];
                memset(reply_body, '\0', MAX_LENGTH);
                std::size_t msg_length = boost::asio::read(sock, buffer(reply_body, reply_length));
                std::cout << "Reply is: ";
                std::cout.write(reply_body, msg_length) << std::endl;
                std::cout << "Reply length is: " << reply_length;
                std::cout << std::endl;
            }
        });

        recv_thread.join();
        send_thread.join();
        
        // std::cout << "Enter message: ";
        // char request[MAX_LENGTH];
        // std::cin.getline(request, MAX_LENGTH);
        // std::size_t request_length = strlen(request);
        // char send_data[MAX_LENGTH] = { 0 };
        // memcpy(send_data, &request_length, HEAD_LENGTH);
        // memcpy(send_data + HEAD_LENGTH, request, request_length);
        // sock.send(boost::asio::buffer(send_data, request_length + 2));

        // char reply_head[HEAD_LENGTH];
        // std::size_t reply_length = sock.receive(
        //     boost::asio::buffer(reply_head, HEAD_LENGTH));
        // int replylen = 0;
        // memcpy(&replylen, reply_head, HEAD_LENGTH);
        // char reply_msg[MAX_LENGTH] = { 0 };
        // std::size_t reply_msg_length = sock.receive(
        //     boost::asio::buffer(reply_msg, replylen));
        // std::cout << "Reply is: ";
        // std::cout.write(reply_msg, replylen);
        // std::cout << "\nReply length is: " << replylen;
        // std::cout << "\n";
    }
    catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}