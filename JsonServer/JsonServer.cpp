#include <iostream>
#include "CServer.h"

int main() {
    try {
        asio::io_context ioc;
        CServer server(ioc, 3333);
        ioc.run();
    }
    catch(std::exception& e) {
        std::cerr << "Exception is: " << e.what() << std::endl;
    }
}
