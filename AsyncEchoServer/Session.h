#include <memory>
#include <queue>
#include <boost/asio.hpp>
using net = boost::asio::ip::tcp;

class Session {
public:
    Session(boost::asio::io_context& ioc);
    net::socket& Socket();

    void Start();
private:
    void handle_read(const boost::system::error_code& ec, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& ec, std::size_t bytes_transferred);
    net::socket _socket;
    enum {max_length = 1024};
    char _data[max_length];
};