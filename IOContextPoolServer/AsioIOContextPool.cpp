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
        // io_context有可能还在监听读写事件, 只是把work给析构不能保证iocontext一定退出
        // 这里需要手动让io_context停止
        work->get_io_context().stop();
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