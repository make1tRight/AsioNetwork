#include <set>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
const int MAX_LENGTH = 1024;

int server_end_point() {
    // Step 1. Here we assume that the server application has
    // already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Create special object of asio::ip::address class
    // that specifies all IP-addresses available on the host. Note
    // that here we assume that server works over IPv6 protocol.
    boost::asio::ip::address ip_address =
        boost::asio::ip::address_v6::any();

    // Step 3.
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);

    // Step 4. The endpoint is created and can be used to 
    // specify the IP addresses and a port number on which 
    // the server application wants to listen for incoming 
    // connections.
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
            << ec.value() << ". Message is: " << ec.message();
        return ec.value();
    }
    return 0;
}

int create_acceptor_socket() {
    boost::asio::io_context ioc;

    boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v6();

    boost::asio::ip::tcp::acceptor acceptor(ioc);

    boost::system::error_code ec;
    acceptor.open(protocol, ec);
    if (ec.value() != 0) {
        std::cout
            << "Failed to open the acceptor socket... Error code = "
            << ec.value() << ". Message is: " << ec.message();
        return ec.value();
    }
    return 0;
}

int bind_acceptor_socket() {
    // Step 1. Here we assume that the server application has
    // already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Creating an endpoint.
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address_v4::any(), port_num);
    
    // Step 3. Creating and opening an acceptor socket.
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::acceptor acceptor(ioc, ep.protocol());
    
    // Step 4. Binding the acceptor socket.
    boost::system::error_code ec;
    acceptor.bind(ep, ec);
    // Handling errors if any.
    if (ec.value() != 0) {
        std::cout 
            << "Failed to bind the acceptor socket... Error code = "
            << ec.value() << ". Message is: " << ec.message();
        return ec.value();
    }
    return 0;
}

int accept_new_connection() {
    // The size of the queue containing the pending connection
    // requests.
    const int BACKLOG_SIZE = 30;
    // Step 1. Here we assume that the server application has
    // already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Creating a server endpoint.
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address_v4::any(), port_num);
    
    boost::asio::io_context ioc;
    try {
        // Step 3. Instantiating and opening an acceptor socket.
        boost::asio::ip::tcp::acceptor acceptor(ioc, ep.protocol());

        // Step 4. Binding the acceptor socket to the 
        // server endpint.
        acceptor.bind(ep);

        // Step 5. Starting to listen for incoming connection
        // requests.
        acceptor.listen(BACKLOG_SIZE);
    
        // Step 6. Creating an active socket.
        boost::asio::ip::tcp::socket sock(ioc);

        // Step 7. Processing the next connection request and 
        // connecting the active socket to the client.
        acceptor.accept(sock);
    }
    catch(boost::system::system_error& ec) {
        std::cout
            << "Failed to accept the acceptor socket... Error code = "
            << ec.code().value() << ". Message is: " << ec.what();
        return ec.code().value();
    }
    return 0;
}

void write_to_socket(boost::asio::ip::tcp::socket& sock) {
    std::string buf = "hello world";
    std::size_t total_bytes_written = 0;

    while (total_bytes_written <= buf.size()) {
        total_bytes_written += sock.write_some(
            boost::asio::buffer(buf.c_str() + total_bytes_written,
            buf.size() - total_bytes_written));
    }
}

std::set<std::shared_ptr<std::thread>> thread_set;
using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;

void session(socket_ptr sock) {
    try {
        for (;;) {
            char data[MAX_LENGTH];
            memset(data, '\0', MAX_LENGTH);
            boost::system::error_code ec;
            int length = sock->read_some(boost::asio::buffer(data, MAX_LENGTH), ec);
            if (ec == boost::asio::error::eof) {
                std::cout << "Connection closed by peer..." << std::endl;
                break;
            }
            else if (ec) {
                throw boost::system::system_error(ec);
                break;
            }
            std::cout
                << "receive from: "
                << sock->remote_endpoint().address().to_string()
                << "\nMessage is: "
                << data << std::endl;
            sock->send(boost::asio::buffer(data, MAX_LENGTH));
        }
    }
    catch(std::exception& e) {
        std::cout << "Exception is: " << e.what() << std::endl;
    }
}

void server(boost::asio::io_context& ioc, unsigned short port_num) {
    boost::asio::ip::address_v4 ip_address = boost::asio::ip::address_v4::any();
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    // acceptor的初始化写在循环外
    // 因为acceptor绑定一次端口就够, 不需要重复绑定
    boost::asio::ip::tcp::acceptor acceptor(ioc, ep);
    for (;;) {
        socket_ptr socket(new boost::asio::ip::tcp::socket(ioc));
        acceptor.accept(*socket);
        auto t = std::make_shared<std::thread>(session, socket);
        thread_set.insert(t);
    }
}

int main() {
    try {
        boost::asio::io_context ioc;
        server(ioc, 3333);
        for (auto& t : thread_set) {
            t->join();
        }
    }
    catch(std::exception& e) {
        std::cout << "Exception is: " << e.what() << std::endl;
    }
}