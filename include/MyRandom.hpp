#ifndef MYRANDOM_HPP
#define MYRANDOM_HPP

#include <algorithm>
#include <chrono>
#include <random>
#include "gsl/gsl"

class MyRandom {
  public:
    static MyRandom& getInstance() {
        static MyRandom instance;
        return instance;
    }

    MyRandom(const MyRandom&) = delete;
    MyRandom& operator=(const MyRandom&) = delete;
    MyRandom(MyRandom&&) = delete;
    MyRandom& operator=(MyRandom&&) = delete;

    template <typename T>
    inline T getIntUniform(T _a, T _b);

    template <typename T>
    inline T getRealUniform(T _a, T _b);

    template <typename T>
    inline std::vector<T> getShuffled(const T& _a, const T& _b);

    template <typename T>
    inline std::vector<T> getKShuffled(int _k, const T& _a, const T& _b);

    inline void setEngine(std::mt19937&& _mt19937) { m_mt19937 = _mt19937; }
    inline std::mt19937& getEngine() { return m_mt19937; }

  private:
    MyRandom() = default;
    ~MyRandom() = default;
    std::mt19937 m_mt19937{std::random_device{}()};
};

template <typename T>
T MyRandom::MyRandom::getIntUniform(const T _a, const T _b) {
    static_assert(std::is_integral<T>::value, "T must be an integral type");
    return std::uniform_int_distribution<T>(_a, _b)(m_mt19937);
}

template <typename T>
std::vector<T> MyRandom::getShuffled(const T& _a, const T& _b) {
    std::vector<T> result(_b - _a + 1);
    std::iota(result.begin(), result.end(), _a);
    std::shuffle(result.begin(), result.end(), m_mt19937);
    return result;
}

template <typename T>
std::vector<T> MyRandom::getKShuffled(const int _k, const T& _a,
    const T& _b) {
    auto result = getShuffled(_a, _b);
    result.erase(result.begin() + _k, result.end());
    return result;
}

template <typename T>
T MyRandom::getRealUniform(const T _a, const T _b) {
    return std::uniform_real_distribution<T>(_a, _b)(m_mt19937);
}

#endif
