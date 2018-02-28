#ifndef ILOUTILITY_HPP
#define ILOUTILITY_HPP

#include <ilcplex/ilocplex.h>
#include <numeric>
#include <utility>
#include <type_traits>

template <typename IloObject>
inline void setIloName(const IloObject& _ilo, const std::string& _str) {
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

template <typename Array>
class IloArrayIterator {
    friend IloArrayIterator<Array> begin(Array& _arr);
    friend IloArrayIterator<Array> end(Array& _arr);

  public:
    using reference = decltype(Array().operator[](0));
    using size_type = decltype(Array().getSize());
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

    IloArrayIterator(Array& _arr, size_type _pos)
        : m_array(_arr)
        , m_position(_pos) {}

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
    Array& m_array;
    size_type m_position;
}; 

template <typename Array>
IloArrayIterator<Array> begin(Array& _arr) {
    return IloArrayIterator<Array>(_arr, 0);
}

template <typename Array>
IloArrayIterator<Array> end(Array& _arr) {
    return IloArrayIterator<Array>(_arr, _arr.getSize());
}

template <typename Array>
Array copy(const Array& _array) {
    Array retval(_array.getEnv(), _array.getSize());
    std::copy(begin(_array), end(_array),
        begin(retval));
    return retval;
}

template <typename Array>
Array move(Array&& _array) {
    return Array(_array.getImpl());
}

static double epsilon_value = 1e-6;

template <typename T>
struct epsilon_less {
    constexpr bool operator()(const T& _lhs, const T& _rhs) {
        return _lhs + epsilon_value < _rhs;
    }
    //static T epsilon_value;
};

template <typename T>
struct epsilon_equal {
    constexpr bool operator()(const T& _lhs, const T& _rhs) {
        return std::fabs(_lhs - _rhs) < epsilon_value;
    }
    //static T epsilon_value;
};

template <typename Array, bool OWNING = false>
class IloWrapper {
  public:
    // using const_array = const Array;
  	using reference = decltype(std::declval<Array>().operator[](0));
    // using const_reference = typename std::result_of<decltype(&const_array::operator[])(Array, IloInt)>::type;
    using const_reference = decltype(std::declval<const Array>().operator[](0));
    using size_type = decltype(Array().getSize());
    using value_type = typename std::remove_reference<reference>::type;
    using difference_type = IloInt;
    using iterator_category = std::bidirectional_iterator_tag;
    using pointer = value_type*;

    template <typename FirstArg, 
    	typename... Args, 
    	typename = typename std::enable_if_t<
		                std::conjunction_v<
		                    typename std::is_constructible<Array, FirstArg, Args...>,
		                    std::conjunction<
		                    	typename std::negation<
		                    		typename std::is_same<std::decay_t<FirstArg>, std::decay_t<Array>>
		                    	>,
		                    	typename std::negation<
		                    		typename std::is_same<std::decay_t<FirstArg>, std::decay_t<IloWrapper>>
		                    	>
		                    >
		                >
                   >
    >
    IloWrapper(FirstArg&& arg, Args&&... args)
        : m_array(std::forward<FirstArg>(arg), std::forward<Args>(args)...)
	{}

    // Copy
    IloWrapper(const IloWrapper& _other)
        : m_array(copy(_other.m_array)) {
    }

    IloWrapper& operator=(const IloWrapper& _other) {
        if (this != &_other) {
            this->~IloWrapper();
            m_array = copy(_other.m_array);
        }
        return *this;
    }

    // Move
    IloWrapper& operator=(IloWrapper&& _other) {
        if (this != &_other) {
            this->~IloWrapper();
            m_array = move(_other.m_array);
        }
        return *this;
    }

    IloWrapper(IloWrapper&& _other)
        : m_array(move(std::move(_other.m_array)))
	{}

    operator Array() {
        return Array(m_array.getImpl());
    }

    operator const Array&() const {
        return m_array;
    }

    ~IloWrapper() {
        if constexpr (OWNING) {
            m_array.endElements();
        }
        m_array.end();
    }

    reference operator[](int _idx) {
        return m_array[_idx];
    }

    const_reference operator[](int _idx) const {
        return m_array[_idx];
    }

    int size() const {
        return m_array.getSize();
    }

    IloArrayIterator<Array> begin() {
        return ::begin(m_array);
    }

    IloArrayIterator<Array> end() {
        return ::end(m_array);
    }

    IloArrayIterator<Array> cbegin() const {
        return ::begin(m_array);
    }

    IloArrayIterator<Array> cend() const {
        return ::end(m_array);
    }

  private:
    Array m_array;
};

/**
*
*/
template<typename ValArray, typename ConstraintArray>
ValArray getDuals(IloCplex& _solver, const ConstraintArray& _constArr) {
	ValArray retval(_constArr.getEnv());
	_solver.getDuals(retval, _constArr);
	return retval;
}
#endif
