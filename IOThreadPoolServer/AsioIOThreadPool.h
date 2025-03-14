#ifndef ASIOIOTHREADPOOL_H
#define ASIOIOTHREADPOOL_H
#include "Singleton.h"
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

using Work = boost::asio::io_context::work;

class AsioIOThreadPool : public Singleton<AsioIOThreadPool> {
    friend class Singleton<AsioIOThreadPool>;
public:
    ~AsioIOThreadPool();
    AsioIOThreadPool(const AsioIOThreadPool&) = delete;
    AsioIOThreadPool& operator=(const AsioIOThreadPool&) = delete;
    boost::asio::io_context& GetIOContext();
    void Stop();
private:
    AsioIOThreadPool(std::size_t size = std::thread::hardware_concurrency());
private:
    boost::asio::io_context _io_context;
    std::vector<std::thread> _threads;
    std::unique_ptr<Work> _work;
    // std::mutex _mutex;
    // std::condition_variable _consume;
};
#endif // ASIOIOTHREADPOOL_H