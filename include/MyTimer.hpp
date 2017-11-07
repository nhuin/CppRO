#ifndef MYTIMER_H
#define MYTIMER_H

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <utility>

class Time {
  private:
    std::clock_t m_cStart{};
    std::chrono::high_resolution_clock::time_point m_tStart{};

  public:
    Time() = default;
    Time(const Time&) = default;
    Time& operator=(const Time&) = default;
    Time(Time&&) noexcept = default;
    Time& operator=(Time&&) noexcept = default;
    ~Time() = default;

    void start() {
        m_cStart = std::clock();
        m_tStart = std::chrono::high_resolution_clock::now();
    }

    std::pair<double, double> show() const {
        std::clock_t c_end = std::clock();
        auto t_end = std::chrono::high_resolution_clock::now();
        double timeDifference = c_end - m_cStart;
        std::cout
            << std::fixed << std::setprecision(2)
            << "CPU time used: " << 1000.0 * timeDifference / CLOCKS_PER_SEC
            << " ms\nWall clock time passed: "
            << std::chrono::duration<double, std::milli>(t_end - m_tStart).count()
            << " ms\n";
        return {
            1000.0 * (timeDifference) / CLOCKS_PER_SEC,
            std::chrono::duration<double, std::milli>(t_end - m_tStart).count()};
    }
};

#endif
