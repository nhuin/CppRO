#ifndef MYTIMER_H
#define MYTIMER_H

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <utility>

class Time {
    using wall_clock = std::chrono::steady_clock;

  private:
    std::clock_t m_cStart{};
    wall_clock::time_point m_tStart{};

  public:
    Time() = default;
    Time(const Time&) = default;
    Time& operator=(const Time&) = default;
    Time(Time&&) noexcept = default;
    Time& operator=(Time&&) noexcept = default;
    ~Time() = default;

    void start() {
        m_cStart = std::clock();
        m_tStart = wall_clock::now();
    }

    [[deprecated]] std::pair<double, double> show() const {
        std::clock_t c_end = std::clock();
        const auto t_end = wall_clock::now();
        const auto timeDifference = double(c_end - m_cStart);
        std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
                  << 1000.0 * timeDifference / CLOCKS_PER_SEC
                  << " ms\nWall clock time passed: "
                  << std::chrono::duration<double, std::milli>(t_end - m_tStart)
                         .count()
                  << " ms\n";
        return {1000.0 * (timeDifference) / CLOCKS_PER_SEC,
            std::chrono::duration<double, std::milli>(t_end - m_tStart)
                .count()};
    }

    std::pair<double, double> get(bool verbose = true) const {
        std::clock_t c_end = std::clock();
        const auto t_end = wall_clock::now();
        const auto timeDifference = double(c_end - m_cStart);
        if (verbose) {
            std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
                      << 1000.0 * timeDifference / CLOCKS_PER_SEC
                      << " ms\nWall clock time passed: "
                      << std::chrono::duration<double, std::milli>(
                             t_end - m_tStart)
                             .count()
                      << " ms\n";
        }
        return {1000.0 * (timeDifference) / CLOCKS_PER_SEC,
            std::chrono::duration<double, std::milli>(t_end - m_tStart)
                .count()};
    }
};

#endif
