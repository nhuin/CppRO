#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <utility>

namespace CppRO {

constexpr auto DEFAULT_PRECISION = 1e-6;

template <typename T>
struct epsilon_less {
    constexpr explicit epsilon_less(const T& _value = T(DEFAULT_PRECISION))
        : epsilon_value(_value) {}

    epsilon_less(const epsilon_less&) = default;
    epsilon_less(epsilon_less&&) noexcept = default;
    epsilon_less& operator=(const epsilon_less&) = default;
    epsilon_less& operator=(epsilon_less&&) noexcept = default;
    ~epsilon_less() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs + epsilon_value < _rhs;
    }
    T epsilon_value;
};

template <typename T>
struct epsilon_less_equal {
    constexpr explicit epsilon_less_equal(
        const T& _value = T(DEFAULT_PRECISION))
        : epsilon_value(_value) {}

    epsilon_less_equal(const epsilon_less_equal&) = default;
    epsilon_less_equal(epsilon_less_equal&&) noexcept = default;
    epsilon_less_equal& operator=(const epsilon_less_equal&) = default;
    epsilon_less_equal& operator=(epsilon_less_equal&&) noexcept = default;
    ~epsilon_less_equal() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs + epsilon_value < _rhs
               || std::fabs(_lhs - _rhs) < epsilon_value;
    }
    T epsilon_value;
};

template <typename T>
struct epsilon_greater {
    constexpr explicit epsilon_greater(const T& _value = T(DEFAULT_PRECISION))
        : epsilon_value(_value) {}

    epsilon_greater(const epsilon_greater&) = default;
    epsilon_greater(epsilon_greater&&) noexcept = default;
    epsilon_greater& operator=(const epsilon_greater&) = default;
    epsilon_greater& operator=(epsilon_greater&&) noexcept = default;
    ~epsilon_greater() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs - _rhs > epsilon_value;
    }
    T epsilon_value;
};

template <typename T>
struct epsilon_greater_equal {
    constexpr explicit epsilon_greater_equal(
        const T& _value = T(DEFAULT_PRECISION))
        : epsilon_value(_value) {}

    epsilon_greater_equal(const epsilon_greater_equal&) = default;
    epsilon_greater_equal(epsilon_greater_equal&&) noexcept = default;
    epsilon_greater_equal& operator=(const epsilon_greater_equal&) = default;
    epsilon_greater_equal& operator=(
        epsilon_greater_equal&&) noexcept = default;
    ~epsilon_greater_equal() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs + epsilon_value > _rhs
               || std::fabs(_lhs - _rhs) < epsilon_value;
    }
    T epsilon_value;
};

template <typename T>
struct epsilon_equal {
    constexpr explicit epsilon_equal(const T& _value = T(DEFAULT_PRECISION))
        : epsilon_value(_value) {}
    epsilon_equal(const epsilon_equal&) = default;
    epsilon_equal(epsilon_equal&&) noexcept = default;
    epsilon_equal& operator=(const epsilon_equal&) = default;
    epsilon_equal& operator=(epsilon_equal&&) noexcept = default;
    ~epsilon_equal() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return std::fabs(_lhs - _rhs) < epsilon_value;
    }
    T epsilon_value;
};
} // namespace CppRO
#endif
