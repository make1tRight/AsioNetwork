#include "AsioIOContextPool.h"


AsioIOContextPool::~AsioIOContextPool() {}

IOContext& AsioIOContextPool::GetIOContext() {
    auto& ioc = _io_contexts[_next_io_context++];
    if (_next_io_context == _io_contexts.size()) {
        _next_io_context = 0;
    }
    return ioc;
}
void AsioIOContextPool::Stop() {
    for (auto& work : _works) {
        work.reset();
    }

    for (auto& t : _threads) {
        t.join();
    }
}

AsioIOContextPool::AsioIOContextPool(std::size_t size)
     : _io_contexts(size), _works(size), _next_io_context(0) {
    for (int i = 0; i < size; ++i) {
        _works[i] = std::unique_ptr<Work>(new Work(_io_contexts[i]));
    }
    
    for (int i = 0; i < _io_contexts.size(); ++i) {
        _threads.emplace_back([this, i]() {
            _io_contexts[i].run();
        });
    }
}