#include "Session.h"
#include <iostream>

Session::Session(boost::asio::io_context& ioc) : _socket(ioc) {

}

net::socket& Session::Socket() {
    return _socket;
}

void Session::Start() {
    memset(_data, '\0', max_length);
    _socket.async_read_some(
        boost::asio::buffer(_data, max_length),
        std::bind(&Session::handle_read,
             this, std::placeholders::_1, std::placeholders::_2)
    );
}

void Session::handle_read(
    const boost::system::error_code& ec, size_t bytes_transferred) {
    if (!ec) {
        std::cout << "receive data: " << _data << std::endl;
        _socket.async_send(
            boost::asio::buffer(
                _data, bytes_transferred),
            std::bind(&Session::handle_write,
                this, std::placeholders::_1, std::placeholders::_2)
        );
    } else {
        delete this;
    }
}

void Session::handle_write(
    const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (!ec) {
        memset(_data, '\0', max_length);
        _socket.async_read_some(
            boost::asio::buffer(_data, max_length),
            std::bind(&Session::handle_read,
                this, std::placeholders::_1, std::placeholders::_2)
        );
    } else {
        delete this;
    }
}