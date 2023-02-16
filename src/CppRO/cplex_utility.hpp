#ifndef ILOUTILITY_HPP
#define ILOUTILITY_HPP

#include <cmath>
#include <ilcplex/ilocplex.h>
#include <numeric>
#include <type_traits>
#include <utility>

constexpr double DEFAULT_EPSILON = 1e-6;

template <typename IloObject>
inline void setIloName(IloObject& _ilo, const std::string& _str) {
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

template <typename T>
struct epsilon_less {
    explicit constexpr epsilon_less(const T& _value)
        : epsilon_value(_value) {}

    constexpr epsilon_less() = default;
    ~epsilon_less() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs + epsilon_value < _rhs;
    }
    T epsilon_value{DEFAULT_EPSILON};
};

template <typename T>
struct epsilon_less_equal {
    explicit constexpr epsilon_less_equal(const T& _value)
        : epsilon_value(_value) {}

    constexpr epsilon_less_equal() = default;
    ~epsilon_less_equal() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs + epsilon_value < _rhs
               || std::fabs(_lhs - _rhs) < epsilon_value;
    }
    T epsilon_value{DEFAULT_EPSILON};
};

template <typename T>
struct epsilon_greater {
    explicit constexpr epsilon_greater(const T& _value)
        : epsilon_value(_value) {}

    constexpr epsilon_greater() = default;
    ~epsilon_greater() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs - _rhs > epsilon_value;
    }
    T epsilon_value{DEFAULT_EPSILON};
};

template <typename T>
struct epsilon_greater_equal {
    explicit constexpr epsilon_greater_equal(const T& _value)
        : epsilon_value(_value) {}

    constexpr epsilon_greater_equal() = default;
    ~epsilon_greater_equal() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return _lhs + epsilon_value > _rhs
               || std::fabs(_lhs - _rhs) < epsilon_value;
    }
    T epsilon_value{DEFAULT_EPSILON};
};

template <typename T>
struct epsilon_equal {
    explicit constexpr epsilon_equal(const T& _value)
        : epsilon_value(_value) {}

    constexpr epsilon_equal() = default;
    ~epsilon_equal() = default;

    constexpr bool operator()(const T& _lhs, const T& _rhs) const {
        return std::fabs(_lhs - _rhs) < epsilon_value;
    }
    T epsilon_value{DEFAULT_EPSILON};
};

/**
 * Wrapper around an IloEnv object.
 * Used to delete default copy constructor and copy assigment, and for RAII
 */
class IloEnvWrapper {
    IloEnv env{};

  public:
    explicit IloEnvWrapper(IloEnv _env)
        : env(_env) {}

    IloEnvWrapper() = default;
    IloEnvWrapper(const IloEnvWrapper&) = delete;
    IloEnvWrapper(IloEnvWrapper&&) = default;
    IloEnvWrapper& operator=(const IloEnvWrapper&) = delete;
    IloEnvWrapper& operator=(IloEnvWrapper&&) = default;
    ~IloEnvWrapper() { env.end(); }

    operator IloEnv() const { return env; }
};

template <typename T>
bool isInteger(T _val) {
    return epsilon_equal<T>()(_val, std::round(_val));
}

template <std::floating_point T>
bool isMostFractional(T _val1, T _val2) {
    const auto ceil1 = std::ceil(_val1);
    const auto floor1 = std::floor(_val1);
    const auto ceil2 = std::ceil(_val2);
    const auto floor2 = std::floor(_val2);
    if (isInteger(_val1)) {
        return false;
    }
    if (isInteger(_val2)) {
        return true;
    }
    return std::fabs(_val1 - (ceil1 + floor1) / 2)
           < std::fabs(_val2 - (ceil2 + floor2) / 2);
}

/**
 *
 */
template <typename ValArray, typename ConstraintArray>
ValArray getDuals(IloCplex& _solver, const ConstraintArray& _constArr) {
    ValArray retval(_constArr.getEnv());
    _solver.getDuals(retval, _constArr);
    return retval;
}

template <typename T>
struct ColumnPair {
    ColumnPair(IloNumVar _var, T _obj)
        : var(_var)
        , obj(std::move(_obj)) {}
    IloNumVar var;
    T obj;
};
#endif
