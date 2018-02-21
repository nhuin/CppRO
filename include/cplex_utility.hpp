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
		std::cout << ite.getVar() << " -> " << _solver.getValue(ite.getVar()) << '\n';
	}
}

inline void displayValue(const IloCplex& _solver, const IloNumVar& _var) {
	std::cout << _var << " = " << _solver.getValue(_var);
}

template<typename IloArray>
class IloArrayIterator {
	friend IloArrayIterator<IloArray> begin(IloArray& _arr);
	friend IloArrayIterator<IloArray> end(IloArray& _arr);
public:
	using reference = decltype(IloArray().operator[](0));
	using size_type = decltype(IloArray().getSize());	
	using value_type = typename std::remove_reference<reference>::type;
	using difference_type = IloInt;
    using iterator_category = std::bidirectional_iterator_tag; 
    using pointer = value_type*;
	
	IloArrayIterator() = default;
	// Copy
	IloArrayIterator(const IloArrayIterator&) = default;
	IloArrayIterator& operator=(const IloArrayIterator&) = default;
	// Move
	IloArrayIterator(IloArrayIterator&&) = default;
	IloArrayIterator& operator=(IloArrayIterator&&) = default;	
	~IloArrayIterator() = default;
	
	reference operator*() {
		return m_array[m_position];
	}

	// Pre-incrementable
	IloArrayIterator& operator++() {
		++m_position;
		return *this;		
	}	
	
	IloArrayIterator operator++(int /*unused*/) {
		IloArrayIterator now = *this;
		++m_position;
		return now;
	}

	// Pre-decrementable
	IloArrayIterator& operator--() {
		--m_position;
		return *this;		
	}	
	
	IloArrayIterator operator--(int /*unused*/) {
		IloArrayIterator now = *this;
		--m_position;
		return now;
	}

	bool operator==(const IloArrayIterator& _rhs) const {
		return &m_array == &_rhs.m_array 
			&& m_position == _rhs.m_position;
	}

	bool operator!=(const IloArrayIterator& _rhs) const {
		return !(*this == _rhs);
	}

	IloArrayIterator(IloArray& _arr, size_type _pos) 
	: m_array(_arr)
	, m_position(_pos)
	{}
	
	// Compound addition assignment
	IloArrayIterator& operator+=(int _offset) {
		m_position += _offset;
		return *this;
	}

	// Compound subtraction assignment
	IloArrayIterator& operator-=(int _offset) {
		m_position -= _offset;
		return *this;
	}

	// Addition
	IloArrayIterator operator+(int _offset) {
		IloArrayIterator newIte = *this;
		this->m_position += _offset;
		return newIte;
	}

	// Subtraction
	IloArrayIterator operator-(int _offset) {
		IloArrayIterator newIte = *this;
		this->m_position -= _offset;
		return newIte;
	}

	//Relational less-than
	bool operator<(const IloArrayIterator& _ite) {
		return m_position < _ite.m_position;
	}

	// Relational greather-than 
	bool operator>(const IloArrayIterator& _ite) {
		return m_position > _ite.m_position;	
	}

	// Relational less-than-or-equal
	bool operator<=(const IloArrayIterator& _ite) {
		return m_position <= _ite.m_position;
	}
	// Relational greater-than-or-equal
	bool operator>=(const IloArrayIterator& _ite) {
	return m_position >= _ite.m_position;
	}

	// Subscripting
	value_type operator[](int _idx) {
		return m_array[_idx];		
	}
private:
	IloArray& m_array;
	size_type m_position;
};

template<typename IloArray>
IloArrayIterator<IloArray> begin(IloArray& _arr) {
	return IloArrayIterator<IloArray>(_arr, 0);
}

template<typename IloArray>
IloArrayIterator<IloArray> end(IloArray& _arr) {
	return IloArrayIterator<IloArray>(_arr, _arr.getSize());
}

#endif