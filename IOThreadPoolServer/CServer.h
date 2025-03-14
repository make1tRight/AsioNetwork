#ifndef CSERVER_H
#define CSERVER_H

#include <map>
#include <memory>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using net = boost::asio::ip::tcp;
class CSession;
class CServer {
public:
    CServer(asio::io_context& ioc, unsigned short port);
    void ClearSession(std::string );
private:
    void start_accept();
    void handle_accept(std::shared_ptr<CSession> new_session, const boost::system::error_code& ec);
    asio::io_context& _ioc;
    net::acceptor _acceptor;
    unsigned short _port;
    std::map<std::string, std::shared_ptr<CSession>> _session; //保护Session的生命周期
};
#endif // CSERVER_H