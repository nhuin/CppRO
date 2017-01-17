#ifndef SINGLETON_HPP
#define SINGLEON_HPP

template<typename T>
struct Singleton {
    static T& getInstance() {
        static T instance;
        return instance;
    }
    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=() = delete;
};

#endif