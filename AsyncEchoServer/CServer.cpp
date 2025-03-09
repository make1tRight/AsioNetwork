#include "CServer.h"

CServer::CServer(asio::io_context& ioc, unsigned short port)
    : _ioc(ioc), _acceptor(ioc, net::endpoint(asio::ip::address_v4::any(), port)) {
    start_accept();
}

void CServer::start_accept() {
    Session* new_session = new Session(_ioc);
    _acceptor.async_accept(
        new_session->Socket(),
        std::bind(&CServer::handle_accept,
             this, new_session, std::placeholders::_1)
    );
}

void CServer::handle_accept(Session* new_session, const boost::system::error_code& ec) {
    if (!ec) {
        new_session->Start();
    } else {
        delete new_session;
    }

    start_accept();
}