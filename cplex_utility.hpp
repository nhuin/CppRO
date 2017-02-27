#ifndef ILOUTILITY_HPP
#define ILOUTILITY_HPP

#include <ilcplex/ilocplex.h>

template<typename Ilo>
void setIloName(const Ilo& _ilo, const std::string& _str) {
	_ilo.setName(_str.c_str());
}

void displayConstraint(const IloCplex& _solver, const IloRange& _range) {
    std::cout << _range << '\n';
    for(auto ite = _range.getLinearIterator(); ite.ok(); ++ite) {
        std::cout << ite.getVar() << " -> " << _solver.getValue(ite.getVar()) << '\n';
    }
}

#endif