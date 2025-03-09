#include <boost/asio.hpp>
#include "Session.h"
namespace asio = boost::asio;
using net = boost::asio::ip::tcp;

class CServer {
public:
    CServer(asio::io_context& ioc, unsigned short port);
private:
    void start_accept();
    void handle_accept(Session* new_session, const boost::system::error_code& ec);
    asio::io_context& _ioc;
    net::acceptor _acceptor;
};