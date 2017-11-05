#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <chrono>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

#if defined(NDEBUG) || defined(PROFILE)
#define DBOUT(x)
#else
#define DBOUT(x) x
#endif

template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::list<T>& _list);
template <typename A, typename B>
std::ostream& operator<<(std::ostream& _out, const std::pair<A, B>& _p);
template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::set<T>& _set);
template <typename T>
std::string to_string_with_precision(const T& a_value, int n = 6);
template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::vector<T>& _v);
template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::deque<T>& _d);
template <typename T, int SIZE>
std::ostream& operator<<(std::ostream& _out, const std::array<T, SIZE>& _a);

struct unused {};
std::ostream& operator<<(std::ostream& _o, const unused& /*unused*/);

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::list<T>& _list) {
    if (_list.empty()) {
        return _out << "[]";
    }
    _out << "[";
    for (auto ite = _list.begin(); ite != std::prev(_list.end()); ++ite) {
        _out << *ite << ", ";
    }
    return _out << _list.back() << "]";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::set<T>& _set) {
    if (_set.empty()) {
        return _out << "[]";
    }
    _out << "[";
    for (auto ite = _set.begin(); ite != std::prev(_set.end()); ++ite) {
        _out << *ite << ", ";
    }
    return _out << *(_set.cbegin()) << "]";
}

template <typename T>
inline std::string to_string_with_precision(const T a_value, const int n) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::vector<T>& _v) {
    if (_v.empty()) {
        return _out << "[]";
    }
    _out << "[";
    for (int i = 0; i < static_cast<int>(_v.size() - 1); ++i) {
        _out << _v[i] << ", ";
    }
    return _out << _v.back() << "]";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::deque<T>& _d) {
    if (_d.empty()) {
        return _out << "[]";
    }
    _out << "[";
    for (int i = 0; i < static_cast<int>(_d.size() - 1); ++i) {
        _out << _d[i] << ", ";
    }
    return _out << _d.back() << "]";
}

template <typename T, int SIZE>
std::ostream& operator<<(std::ostream& _out, const std::array<T, SIZE>& _a) {
    _out << "[";
    for (int i = 0; i < SIZE; ++i) {
        _out << _a[i] << ", ";
    }
    return _out << _a.back() << "]";
}

template <typename A, typename B>
inline std::ostream& operator<<(std::ostream& _out, const std::pair<A, B>& _p) {
    return _out << "(" << _p.first << ", " << _p.second << ")";
}

template <typename T>
int getMaxSetBit(T _v) {
    int ret = 0;
    while (_v >>= 1) {
        ++ret;
    }
    return ret;
}

template <typename T>
std::size_t getMaxSetByte(T _v) {
    std::size_t ret = 0;
    while (_v >>= 8) {
        ++ret;
    }
    return ret;
}

template <class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch, Tr>& os,
    const Tuple& t,
    std::index_sequence<Is...> /*unused*/) {
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template <class Ch, class Tr, class... Args>
decltype(auto) operator<<(std::basic_ostream<Ch, Tr>& os,
    const std::tuple<Args...>& t) {
    os << '(';
    print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
    return os << ')';
}

template <typename T>
inline std::string toString(const T& _obj) {
    std::stringstream sstr;
    sstr << _obj;
    return sstr.str();
}

template <typename T>
T binomialCoeff(const T& n, T k) {
    T res = 1;
    // Since C(n, k) = C(n, n-k)
    if (k > n - k) {
        k = n - k;
    }

    for (T i = 0; i < k; ++i) {
        res *= (n - i);
        res /= (i + 1);
    }
    return res;
}

inline std::ostream& operator<<(std::ostream& _o, const unused& /*unused*/) {
    return _o;
}

template <typename Ite, typename T>
std::pair<Ite, Ite> find_consecutive_common_value(Ite _first1, const Ite& _last1,
    Ite _first2, const Ite& _last2,
    T _value, std::size_t _size) {
    std::size_t counter = 0;
    for (; _first1 != _last1 && _first2 != _last2;
         ++_first1, ++_first2) {
        if (*_first1 == _value && *_first2 == _value) {
            if (++counter == _size) {
                break;
            }
        } else {
            counter = 0;
        }
    }
    return {_first1, _first2};
}

template <typename T, typename... Args>
auto product(T _val, Args... _args) {
    return _val * product(_args...);
}
#endif
