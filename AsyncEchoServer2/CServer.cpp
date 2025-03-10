#include "CServer.h"
#include "Session.h"
#include <iostream>

CServer::CServer(asio::io_context& ioc, unsigned short port)
    : _ioc(ioc), _acceptor(ioc, net::endpoint(asio::ip::address_v4::any(), port)) {
    start_accept();
}

void CServer::ClearSession(std::string uuid) {
    // session从map移除以后, 引用计数变成0则session自动释放
    _session.erase(uuid);
}

void CServer::start_accept() {
    std::shared_ptr<Session> new_session = std::make_shared<Session>(_ioc, this);
    _acceptor.async_accept(
        new_session->Socket(),
        std::bind(&CServer::handle_accept,
             this, new_session, std::placeholders::_1)
    );
}

void CServer::handle_accept(std::shared_ptr<Session> new_session, const boost::system::error_code& ec) {
    if (!ec) {
        new_session->Start();
        _session.insert(std::make_pair(new_session->GetUuid(), new_session));//保证session在完成逻辑处理之前不会被释放
    } else {
        std::cout << "Failed to handle_accept, error is: " << ec.message() << std::endl;
    }

    start_accept();
}