#ifndef ILOUTILITY_HPP
#define ILOUTILITY_HPP

#include <ilcplex/ilocplex.h>
#include <numeric>

template <typename Ilo>
inline void setIloName(const Ilo& _ilo, const std::string& _str) {
    _ilo.setName(_str.c_str());
}

inline void displayConstraint(const IloCplex& _solver, const IloRange& _range) {
    std::cout << _range << '\n';
    for (auto ite = _range.getLinearIterator(); ite.ok() != 0; ++ite) {
        std::cout << ite.getVar() << " -> " << _solver.getValue(ite.getVar())
                  << '\n';
    }
}

inline void displayValue(const IloCplex& _solver, const IloNumVar& _var) {
    std::cout << _var << " = " << _solver.getValue(_var);
}

template <typename... Tr>
std::size_t someProduct(std::tuple<int, Tr...>&& _pos, std::tuple<int, Tr...>&& _dim) {
    return someProduct(std::make_tuple(std::get<Tr...>(_pos)),
               std::make_tuple(std::get<Tr...>(_dim)))
               * std::get<0>(_dim)
           + std::get<0>(_pos);
}

template <>
std::size_t someProduct(std::tuple<int>&& _pos, std::tuple<int>&& /*_v2*/) {
    return std::get<0>(_pos);
}

template <typename IloType>
class IloArrayWrapper {
};

#endif