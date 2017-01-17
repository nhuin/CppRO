#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <chrono>
#include <iomanip>

template<typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::list<T>& _set) {
	if(_set.empty()) {
		return _out << "[]";
	} else {
	    _out << "[";
	    for(auto ite = _set.begin(); ite != std::prev(_set.end()); ++ite) {
	        _out << *ite << ", ";
	    }
	    return _out << _set.back() << "]";
	}
}

template<typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::set<T>& _l) {
	if(_l.empty()) {
		return _out << "[]";
	} else {
	    _out << "[";
	    for(auto ite = _l.begin(); ite != std::prev(_l.end()); ++ite) {
	        _out << *ite << ", ";
	    }
	    return _out << *(_l.cbegin()) << "]";
	}
}

template <typename T>
inline std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}

template<typename T>
inline std::ostream& operator<<(std::ostream& _out, const std::vector<T>& _v) {
	if(_v.empty()) {
		return _out << "[]";
	} else {
	    _out << "[";
	    for(int i = 0; i < (int) _v.size()-1; ++i) {
	        _out << _v[i] << ", ";
	    }
	    return _out << _v.back() << "]";
	}
}

template<typename A, typename B>
inline std::ostream& operator<<(std::ostream& _out, const std::pair<A, B>& _p) {
    return _out << "(" << _p.first << ", " << _p.second << ")";
}

namespace aux{
	template<std::size_t...> struct seq{};

	template<std::size_t N, std::size_t... Is>
	struct gen_seq : gen_seq<N-1, N-1, Is...>{};

	template<std::size_t... Is>
	struct gen_seq<0, Is...> : seq<Is...>{};

	template<class Ch, class Tr, class Tuple, std::size_t... Is>
	void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, seq<Is...>){
	  using swallow = int[];
	  (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
	}
} // aux::

template<class Ch, class Tr, class... Args>
inline auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t)
    -> std::basic_ostream<Ch, Tr>&
{
  os << "(";
  aux::print_tuple(os, t, aux::gen_seq<sizeof...(Args)>());
  return os << ")";
}

template<class... Args>
inline std::string to_string(std::tuple<Args...> const& t) {
	std::stringstream sstr;
	aux::print_tuple(sstr, t, aux::gen_seq<sizeof...(Args)>());
	return sstr.str();
}

template<typename T> inline std::string toString(const T& _obj) {
	std::stringstream sstr;
	sstr << _obj;
	return sstr.str();
}

#endif
