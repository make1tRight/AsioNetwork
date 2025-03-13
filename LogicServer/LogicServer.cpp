#include <iostream>
#include "CServer.h"

int main() {
    try {
        asio::io_context ioc;
        asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code&, int) {
            ioc.stop();
        });
        CServer server(ioc, 3333);
        ioc.run();
    }
    catch(std::exception& e) {
        std::cerr << "Exception is: " << e.what() << std::endl;
    }
}
