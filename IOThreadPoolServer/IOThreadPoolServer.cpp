#include <iostream>
#include "CServer.h"
#include "AsioIOThreadPool.h"

int main() {
    try {
        auto pool = AsioIOThreadPool::GetInstance();
        asio::io_context ioc;
        asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool](const boost::system::error_code&, int) {
            ioc.stop();
            pool->Stop();
        });
        CServer server(ioc, 3333);
        ioc.run();
    }
    catch(std::exception& e) {
        std::cerr << "Exception is: " << e.what() << std::endl;
    }
}
