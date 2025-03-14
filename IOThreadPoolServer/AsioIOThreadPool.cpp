#include "AsioIOThreadPool.h"

AsioIOThreadPool::~AsioIOThreadPool() {
    Stop(); 
}

boost::asio::io_context& AsioIOThreadPool::GetIOContext() {
    return _io_context;
}

void AsioIOThreadPool::Stop() {
    _work.reset();
    for (auto& t : _threads) {
        t.join();
    }
}

AsioIOThreadPool::AsioIOThreadPool(std::size_t size) : _threads(size) {
    for (int i = 0; i < size; ++i) {
        _threads.emplace_back([this]() {
            _io_context.run();
        });
    }
}