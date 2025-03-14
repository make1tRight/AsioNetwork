#ifndef ASIOIOCONTEXTPOOL_H
#define ASIOIOCONTEXTPOOL_H
#include "Singleton.h"
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

using IOContext = boost::asio::io_context;
using Work = boost::asio::io_context::work;
using WorkPtr = std::unique_ptr<Work>;

class AsioIOContextPool : public Singleton<AsioIOContextPool> {
    friend class Singleton<AsioIOContextPool>;
public:
    ~AsioIOContextPool();
    AsioIOContextPool(const AsioIOContextPool&) = delete;
    AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
    IOContext& GetIOContext();
    void Stop();
private:
    AsioIOContextPool(std::size_t size = std::thread::hardware_concurrency());
private:
    std::vector<IOContext> _io_contexts;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _next_io_context;
};

#endif // ASIOIOCONTEXTPOOL_H