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
std::ostream &operator<<(std::ostream &_out, const std::list<T> &_list);
template <typename A, typename B>
std::ostream &operator<<(std::ostream &_out, const std::pair<A, B> &_p);
template <typename T>
std::ostream &operator<<(std::ostream &_out, const std::set<T> &_set);
template <typename T>
std::string to_string_with_precision(const T &a_value, int n = 6);
template <typename T>
std::ostream &operator<<(std::ostream &_out, const std::vector<T> &_v);
template <typename T>
std::ostream &operator<<(std::ostream &_out, const std::deque<T> &_d);
template <typename T, int SIZE>
std::ostream &operator<<(std::ostream &_out, const std::array<T, SIZE> &_a);

template <typename T>
inline std::ostream &operator<<(std::ostream &_out, const std::list<T> &_list) {
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
inline std::ostream &operator<<(std::ostream &_out, const std::set<T> &_set) {
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
inline std::ostream &operator<<(std::ostream &_out, const std::vector<T> &_v) {
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
inline std::ostream &operator<<(std::ostream &_out, const std::deque<T> &_d) {
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
std::ostream &operator<<(std::ostream &_out, const std::array<T, SIZE> &_a) {
  _out << "[";
  for (int i = 0; i < SIZE; ++i) {
    _out << _a[i] << ", ";
  }
  return _out << _a.back() << "]";
}

template <typename A, typename B>
inline std::ostream &operator<<(std::ostream &_out, const std::pair<A, B> &_p) {
  return _out << "(" << _p.first << ", " << _p.second << ")";
}

template <typename T> int getMaxSetBit(T _v) {
  int ret = 0;
  while (_v >>= 1) {
    ++ret;
  }
  return ret;
}

template <typename T> int getMaxSetByte(T _v) {
  int ret = 0;
  while (_v >>= 8) {
    ++ret;
  }
  return ret;
}

namespace aux {
template <std::size_t...> struct seq {};

template <std::size_t N, std::size_t... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <std::size_t... Is> struct gen_seq<0, Is...> : seq<Is...> {};

template <class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple(std::basic_ostream<Ch, Tr> &os, Tuple const &t,
                 seq<Is...> /*unused*/) {
  using swallow = int[];
  (void)swallow{0,
                (void(os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), 0)...};
}
} // namespace aux

template <class Ch, class Tr, class... Args>
inline auto operator<<(std::basic_ostream<Ch, Tr> &os,
                       std::tuple<Args...> const &t)
    -> std::basic_ostream<Ch, Tr> & {
  os << "(";
  aux::print_tuple(os, t, aux::gen_seq<sizeof...(Args)>());
  return os << ")";
}

template <class... Args>
inline std::string to_string(std::tuple<Args...> const &t) {
  std::stringstream sstr;
  aux::print_tuple(sstr, t, aux::gen_seq<sizeof...(Args)>());
  return sstr.str();
}

template <typename T> inline std::string toString(const T &_obj) {
  std::stringstream sstr;
  sstr << _obj;
  return sstr.str();
}

int binomialCoeff(const int n, int k) {
  int res = 1;
  // Since C(n, k) = C(n, n-k)
  if (k > n - k) {
    k = n - k;
  }

  for (int i = 0; i < k; ++i) {
    res *= (n - i);
    res /= (i + 1);
  }
  return res;
}

struct unused {};

std::ostream &operator<<(std::ostream &_o, const unused & /*unused*/) {
  return _o;
}

  // class CombinationIterator
  // {
  // public:
  //     CombinationIterator(const int _k, const int _n) :
  //         m_n(_n),
  //         m_k(_k),
  //         m_combination(m_n),
  //         m_v([&](){
  //             std::vector<bool> v(m_n);
  //             std::fill(v.end() - m_k, v.end(), true);
  //             return v;
  //         }())
  //     {
  //     }

  //     ~CombinationIterator() {}

  //     const std::vector<int>& operator++() {
  //         std::next_permutation(m_v.begin(), m_v.end());
  //         for(int i = 0, j = 0; i < m_n; ++i) {
  //             if(m_v[i]) {
  //                 m_combination[j] = i;
  //                 ++j;
  //             }
  //         }
  //         return m_combination;
  //     }

  //     std::vector<int> operator++(int /*unused*/) {
  //         auto copy = m_combination;
  //         std::next_permutation(m_v.begin(), m_v.end());
  //         for(int i = 0, j = 0; i < m_n; ++i) {
  //             if(m_v[i]) {
  //                 m_combination[j] = i;
  //                 ++j;
  //             }
  //         }
  //         return copy;
  //     }

  //     const std::vector<int>&  operator*(){
  //         return m_combination;
  //     }

  // private:
  //     int m_n;
  //     int m_k;
  //     std::vector<int> m_combination;
  //     bool end = false;
  //     std::vector<bool> m_v;
  // };

  // std::vector<int> combination(const int k, const int n) {

  //     do {
  //         for (int i = 0; i < n; ++i) {
  //             if (v[i]) {
  //                 std::cout << (i + 1) << " ";
  //             }
  //         }
  //         std::cout << "\n";
  //     } while (std::next_permutation(v.begin(), v.end()));
  // }

#endif
