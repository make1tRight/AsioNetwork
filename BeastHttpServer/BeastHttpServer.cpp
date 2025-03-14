#include <memory>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;

namespace my_program_state
{
	std::size_t
		request_count()
	{
		static std::size_t count = 0;
		return ++count;
	}

	std::time_t
		now()
	{
		return std::time(0);
	}
}


class http_connection : public std::enable_shared_from_this<http_connection> {
public:
    http_connection(tcp::socket socket) : _socket(std::move(socket)) {}
    void start() {
        read_request();
        check_deadline();
    }

private:
    tcp::socket _socket;
    beast::flat_buffer _buffer{8192};
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;
    asio::steady_timer _deadline{
        _socket.get_executor(), std::chrono::seconds(60)
    };

    void check_deadline() {
        auto self = shared_from_this();
        _deadline.async_wait([self](beast::error_code ec) {
            if (!ec) {
                self->_socket.close(ec);
            }
        });
    }

    void read_request() {
        auto self = shared_from_this();
        http::async_read(_socket, _buffer, _request, [self](
            beast::error_code ec, std::size_t byte_transferred) {
            boost::ignore_unused(byte_transferred);
            if (!ec) {
                self->process_request();
            }
        });
    }

    void process_request() {
        _response.version(_request.version());
        _response.keep_alive(false);

        switch(_request.method()) {
            case http::verb::get:
                _response.result(http::status::ok);
                _response.set(http::field::server, "Beast");
                create_response();
                break;
            case http::verb::post:
                _response.result(http::status::ok);
                _response.set(http::field::server, "Beast");
                create_post_response();
                break;
            default:
                _response.result(http::status::bad_request);
                _response.set(http::field::content_type, "text/plain");
                beast::ostream(_response.body())
                    << "Invalid request-method: "
                    << std::string(_request.method_string())
                    << ".";
                break;
        }
        write_response();
    }

    void create_response() {
        if (_request.target() == "/count") {
            _response.set(http::field::content_type, "text/html");
            beast::ostream(_response.body()) 
                << "<html>\n"
                << "<head><title>Request count</title></head>\n"
                << "<body>\n"
                << "<h1>Request count</h1>\n"
                << "<p>There have been "
                << my_program_state::request_count()
                << " requests so far.</p>\n"
                << "</body>\n"
                << "</html>\n";
        } else if (_request.target() == "/time") {
            _response.set(http::field::content_type, "text/html");
            beast::ostream(_response.body())
                << "<html>\n"
                << "<head><title>Current time</title></head>\n"
                << "<body>\n"
                << "<h1>Current time</h1>\n"
                << "<p>The current time is "
                << my_program_state::now()
                << " seconds since the epoch.</p>\n"
                << "</body>\n"
                << "</html>\n";
        } else {
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            beast::ostream(_response.body()) << "File not found\r\n";
        }
    }

    void create_post_response() {
        if (_request.target() == "/email") {
            auto& body = _request.body();
            std::string body_string = boost::beast::buffers_to_string(body.data());
            std::cout << "body string: " << body_string << std::endl;

            Json::Value root;
            Json::Reader reader;
            Json::Value root_src;
            bool parse_success = reader.parse(body_string, root_src);
            if (!parse_success) {
                std::cout << "Failed to parse JSON data" << std::endl;
                root["error"] = 1001;
                std::string jsonstr = root.toStyledString();
                beast::ostream(this->_response.body()) << jsonstr;
                return;
            }
            auto email = root_src["email"].asString();
            std::cout << "receive email: " << email << std::endl;

            root["error"] = 0;
            root["email"] = root_src["email"];
            root["msg"] = "server has receive post request";
            std::string jsonstr = root.toStyledString();
            beast::ostream(this->_response.body()) << jsonstr;
        } else {
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            beast::ostream(_response.body()) << "File not found\r\n";
        }
    }

    void write_response() {
        auto self = shared_from_this();
        _response.content_length(_response.body().size());
        http::async_write(_socket, _response, [self](
            beast::error_code ec, std::size_t byte_transferred) {
            self->_socket.shutdown(tcp::socket::shutdown_send, ec);
            self->_deadline.cancel();
        });
    }
};

void http_server(tcp::acceptor& acceptor, tcp::socket& socket) {
    acceptor.async_accept(socket, [&](const boost::system::error_code& error) {
        if (!error) {
            std::make_shared<http_connection>(std::move(socket))->start();
            http_server(acceptor, socket);
        }
    });
}

int main(int argc, char** argv) {
    try {
        asio::io_context ioc;
        const auto ip_address = asio::ip::make_address("0.0.0.0");
        unsigned short port = static_cast<unsigned short>(3333);
        tcp::acceptor acceptor(ioc, tcp::endpoint(ip_address, port));
        tcp::socket socket(ioc);
        http_server(acceptor, socket);
        ioc.run();
    } catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1; 
    }
    return 0;
}