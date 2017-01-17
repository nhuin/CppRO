#ifndef ILOUTILITY_HPP
#define ILOUTILITY_HPP

#include <ilcplex/ilocplex.h>

template<typename Ilo>
void setIloName(const Ilo& _ilo, const std::string& _str) {
	_ilo.setName(_str.c_str());
}
#endif