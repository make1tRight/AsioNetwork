#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>
#include <iostream>

template<typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

    static std::shared_ptr<T> _instance;
public:
    ~Singleton() {
        std::cout << "this is ~Singleton() destruct..." << std::endl;
    }

    static std::shared_ptr<T> GetInstance() {
        static std::once_flag static_flag; //
        std::call_once(static_flag, [&]() {
            // _instance = std::make_shared<T>();
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }

    void PrintAddress() {
        std::cout << "raw pointer: " << _instance.get() << std::endl;
    }
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
#endif // SINGLETON_H