#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <chrono>
#include <deque>
#include <iomanip>
#include <iostream>
#include <iterator>
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
std::string toStringWithPrecision(const T& _value, int _n = 6);
template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::vector<T>& _v);
template <typename T, typename K>
std::ostream& operator<<(std::ostream& _out, const std::map<T, K>& _map);
template <typename T>
std::ostream& operator<<(std::ostream& _out, const std::deque<T>& _d);
template <typename T, int SIZE>
std::ostream& operator<<(std::ostream& _out, const std::array<T, SIZE>& _a);

struct Unused {
    Unused(const Unused&) = default;
    Unused(Unused&&) = default;
    Unused& operator=(const Unused&) = default;
    Unused& operator=(Unused&&) = default;
};
std::ostream& operator<<(std::ostream& _o, const Unused& /*unused*/);

template <typename T, typename K>
std::ostream& operator<<(std::ostream& _out, const std::map<T, K>& _map) {
    if (_map.empty()) {
        return _out << "[]";
    }
    _out << '[';
    const auto end = std::prev(_map.end());
    for (auto ite = _map.begin(); ite != end; ++ite) {
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
inline std::string toStringWithPrecision(const T _value, const int _n) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(_n) << _value;
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
void printTupleImpl(std::basic_ostream<Ch, Tr>& _os, const Tuple& _t,
    std::index_sequence<Is...> /*unused*/) {
    ((_os << (Is == 0 ? "" : ", ") << std::get<Is>(_t)), ...);
}

template <class Ch, class Tr, class... Args>
decltype(auto) operator<<(
    std::basic_ostream<Ch, Tr>& _os, const std::tuple<Args...>& _t) {
    _os << '(';
    print_tuple_impl(_os, _t, std::index_sequence_for<Args...>{});
    return _os << ')';
}

template <typename T>
std::string toString(const T& _obj) {
    std::stringstream sstr;
    sstr << _obj;
    return sstr.str();
}

template <typename T>
T binomialCoeff(const T& _n, T _k) {
    T res = 1;
    // Since C(n, k) = C(n, n-k)
    if (_k > _n - _k) {
        _k = _n - _k;
    }

    for (T i = 0; i < _k; ++i) {
        res *= (_n - i);
        res /= (i + 1);
    }
    return res;
}

inline std::ostream& operator<<(std::ostream& _o, const Unused& /*unused*/) {
    return _o;
}

/**
 * for each for tuple
 */

template <typename Tuple, typename F, std::size_t... Indices>
constexpr void forEachImpl(
    Tuple&& _tuple, F&& _f, std::index_sequence<Indices...> /*unused*/) {
    (_f(std::get<Indices>(std::forward<Tuple>(_tuple))), ...);
}

template <typename Tuple, typename F>
constexpr void forEach(Tuple&& _tuple, F&& _f) {
    constexpr std::size_t N =
        std::tuple_size<std::remove_reference_t<Tuple>>::value;
    forEachImpl(std::forward<Tuple>(_tuple), std::forward<F>(_f),
        std::make_index_sequence<N>{});
}

template <typename InputIterator>
typename std::ostream_iterator<
    typename std::iterator_traits<InputIterator>::value_type>::ostream_type&
printContainer(typename std::ostream_iterator<typename std::iterator_traits<
                   InputIterator>::value_type>::ostream_type& _out,
    InputIterator _first, InputIterator _last, const char* _delimiter = ", ") {
    if (std::distance(_first, _last) == 0) {
        return _out;
    }
    using value_type = typename std::iterator_traits<InputIterator>::value_type;

    const auto last = std::prev(_last);
    std::copy(
        _first, last, std::ostream_iterator<value_type>(_out, _delimiter));
    return _out << *last;
}

template <typename Function>
struct CallOnDestroy {
    explicit CallOnDestroy(Function _f)
        : f(_f) {}
    CallOnDestroy(const CallOnDestroy&) = default;
    CallOnDestroy(CallOnDestroy&&) noexcept = default;
    CallOnDestroy& operator=(const CallOnDestroy&) = default;
    CallOnDestroy& operator=(CallOnDestroy&&) noexcept = default;
    ~CallOnDestroy() { f(); }
    Function f;
};

#endif
