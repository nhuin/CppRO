#ifndef MYRANDOM_HPP
#define MYRANDOM_HPP

#include <algorithm>
#include <chrono>
#include <random>

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
    inline double getRealUniform(double _a, double _b);

    inline std::vector<int> getShuffled(int _a, int _b);
    inline std::vector<int> getKShuffled(int _k, int _a, int _b);

    inline void setEngine(std::mt19937&& _mt19937) { m_mt19937 = _mt19937; }
    inline std::mt19937& getEngine() { return m_mt19937; };

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

std::vector<int> MyRandom::getShuffled(const int _a, const int _b) {
    std::vector<int> result(_b - _a + 1);
    std::iota(result.begin(), result.end(), _a);
    std::shuffle(result.begin(), result.end(), m_mt19937);
    return result;
}

std::vector<int> MyRandom::getKShuffled(const int _k, const int _a,
    const int _b) {
    std::vector<int> result = getShuffled(_a, _b);
    result.erase(result.begin() + _k, result.end());
    return result;
}

double MyRandom::getRealUniform(const double _a, const double _b) {
    return std::uniform_real_distribution<double>(_a, _b)(m_mt19937);
}

#endif