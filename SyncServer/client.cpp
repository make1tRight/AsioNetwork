#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
const int MAX_LENGTH = 1024;

int client_end_point() {
    // Step 1. Assume that the client application has already 
    // obtained the IP-address and the protocol port number.
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    boost::system::error_code ec;
    // Step 2. Using IP protocol version independent address
    // representation.
    boost::asio::ip::address ip_address = 
        boost::asio::ip::address::from_string(raw_ip_address, ec);
    
    if (ec.value() != 0) {
        std::cout 
            << "Failed to parse the IP address... Error_code = "
            << ec.value() << ". Message: " << ec.message();  
        return ec.value();
    }
    // Step 3.
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);

    // Step 4. The endpoint is ready and can be used to specify a 
    // particular server in the network the client wants to 
    // communicate with.
    return 0;
}

int create_tcp_socket() {
    // Step 1. An instance of 'io_service' class is required by
            // socket constructor. 
    boost::asio::io_context ioc;

    // Step 2. Creating an object of 'tcp' class representing
    // a TCP protocol with IPv4 as underlying protocol.
    boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();

    // Step 3. Instantiating an active TCP socket object.
    boost::asio::ip::tcp::socket sock(ioc);

    // Step 4. Opening the socket.
    boost::system::error_code ec;
    sock.open(protocol, ec);
    if (ec.value() != 0) {
        std::cout
            << "Failed to open socket... Error code = "
            << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    return 0;
}

int connect_to_end() {
    // Step 1. Assume that the client application has already 
    // obtained the IP-address and the protocol port number.
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        // Step 2. Creating an endpoint designating 
        // a target server application.
        boost::asio::ip::address ip_address = 
            boost::asio::ip::address::from_string(raw_ip_address);
    
        boost::asio::ip::tcp::endpoint ep(ip_address, port_num);

        // Step 3. Creating and opening a socket.
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
    
        // Step 4. Connecting a socket.
        sock.connect(ep); 
    }
    catch(boost::system::system_error& ec) {
        std::cout
            << "Error occured... Error code = " 
            << ec.code().value() << ". Message: " << ec.what();
        return ec.code().value();
    }

    return 0;
}

int send_data_by_send() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        boost::asio::ip::address ip_address = 
            boost::asio::ip::address::from_string(raw_ip_address);
        boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        std::string buf = "hello world";
        int send_length = sock.send(boost::asio::buffer(buf.c_str(), buf.size()));
        if (send_length <= 0) {
            std::cout << "Failed to send..." << std::endl;
            return 0;
        }
    }
    catch(boost::system::system_error& ec) {
        std::cout
            << "Error occured... Error code = " 
            << ec.code().value() << ". Message: " << ec.what();
        return ec.code().value();
    }
    return 0;
}

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
        
        std::cout << "Enter message: ";
        char request[MAX_LENGTH];
        std::cin.getline(request, MAX_LENGTH);
        std::size_t request_length = strlen(request);
        sock.send(boost::asio::buffer(request, request_length));
        char reply[MAX_LENGTH];
        std::size_t reply_length = sock.receive(
            boost::asio::buffer(reply, request_length));
        std::cout << "Reply is: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }
    catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}