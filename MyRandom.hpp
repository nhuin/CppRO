#ifndef MYRANDOM_H
#define MYRANDOM_H

#include <random>
#include <algorithm>
#include <chrono>

class MyRandom
{
    public:
        static MyRandom& getInstance()
        {
            static MyRandom instance;
            return instance;
        }
		
		MyRandom(const MyRandom&) = delete;
        void operator=(const MyRandom&) = delete;
		
		template<typename T>
		T getIntUniform(T a, T b);
		double getRealUniform(double a, double b);
		
		std::vector<int> getShuffled(int a, int b);
		std::vector<int> getKShuffled(int _k, int _a, int _b);
		
		inline void setEngine(std::mt19937&& _mt19937) {m_mt19937 = _mt19937;}
    private:
        MyRandom() = default;
        ~MyRandom() = default;
		std::mt19937 m_mt19937 {std::mt19937(std::chrono::system_clock::now().time_since_epoch().count())};
 };


template<typename T>
T MyRandom::MyRandom::getIntUniform(const T a, const T b) {
	static_assert(std::is_integral<T>::value, "T must be an integral type");
	return std::uniform_int_distribution<T>(a, b)(m_mt19937);
}

std::vector<int> MyRandom::getShuffled(const int a, const int b) {
	std::vector<int> result(b-a+1);
	std::iota(result.begin(), result.end(), a);
	std::shuffle(result.begin(), result.end(), m_mt19937);
	return result;
}

std::vector<int> MyRandom::getKShuffled(const int _k, const int _a, const int _b) {
	std::vector<int> result = getShuffled(_a, _b);
	result.erase(result.begin()+_k, result.end());
	return result;
}

double MyRandom::getRealUniform(const double _a, const double _b) {
	return std::uniform_real_distribution<double>(_a, _b)(m_mt19937);
}

#endif