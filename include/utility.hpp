#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <chrono>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
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
template <typename T, typename K>
std::ostream& operator<<(std::ostream& _out, const std::map<T, K>& _v);
template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::deque<T>& _d);
template <typename T, int SIZE>
std::ostream& operator<<(std::ostream& _out, const std::array<T, SIZE>& _a);

struct unused {};
std::ostream& operator<<(std::ostream& _o, const unused& /*unused*/);

template <typename T, typename K>
std::ostream& operator<<(std::ostream& _out, const std::map<T, K>& _map) {
    if (_map.empty()) {
        return _out << "[]";
    }
    _out << '[';
    const auto end = std::prev(_map.end());
    for (auto ite = _map.begin(); ite != ite; ++ite) {
        _out << '(' << ite->first << ", " << ite->second << ')';
    }
    return _out << '(' << end->first << ", " << end->second << ')';
}

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::list<T>& _list) {
    if (_list.empty()) {
        return _out << "[]";
    }
    _out << '[';
    for (auto ite = _list.begin(); ite != std::prev(_list.end()); ++ite) {
        _out << *ite << ", ";
    }
    return _out << _list.back() << ']';
}

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::set<T>& _set) {
    if (_set.empty()) {
        return _out << "[]";
    }
    _out << '[';
    for (auto ite = _set.begin(); ite != std::prev(_set.end()); ++ite) {
        _out << *ite << ", ";
    }
    return _out << *(_set.rbegin()) << ']';
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
    _out << '[';
    for (typename std::vector<T>::size_type i = 0; i < _v.size() - 1; ++i) {
        _out << _v[i] << ", ";
    }
    return _out << _v.back() << ']';
}

template <typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::deque<T>& _d) {
    if (_d.empty()) {
        return _out << "[]";
    }
    _out << '[';
    for (typename std::deque<T>::size_type i = 0; i < _d.size() - 1; ++i) {
        _out << _d[i] << ", ";
    }
    return _out << _d.back() << ']';
}

template <typename T, int SIZE>
std::ostream& operator<<(std::ostream& _out, const std::array<T, SIZE>& _a) {
    _out << '[';
    for (int i = 0; i < SIZE; ++i) {
        _out << _a[i] << ", ";
    }
    return _out << _a.back() << ']';
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
int getMaxSetByte(T _v) {
    int ret = 0;
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

/**
* for each for tuple
*/

template <typename Tuple, typename F, std::size_t ...Indices>
constexpr void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    (f(std::get<Indices>(std::forward<Tuple>(tuple))), ...);
}

template <typename Tuple, typename F>
constexpr void for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}

// template <typename RandomIterator, typename T>
// std::pair<RandomIterator, RandomIterator>
// find_consecutive_common_value(RandomIterator _first1, RandomIterator _last1,
//     RandomIterator _first2,
//     const T& _value, const int _size) {

//     int counter = 0;
//     for(RandomIterator window1 = _first1 + _size, window2 = _first2 + _size;
//         ; window1 != _last1;
//         window1 += _size, window2 += _size) {
//         if(*window1 == _value && *window2 == _value) {
//             for(
//                 ; _first1 != _window1 && _first2 != _last1;
//                 ++_first1, ++_first2) {

//             }
//         }
//     }
//     return {_first1, _first2};
// }

template <typename T, typename... Args>
auto product(T _val, Args... _args) {
    return _val * product(_args...);
}
#endif
