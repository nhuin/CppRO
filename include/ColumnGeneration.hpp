#ifndef COLUMN_GENERATION_HPP
#define COLUMN_GENERATION_HPP

#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "TypeTraits.hpp"
#include "cplex_utility.hpp"
#include "utility.hpp"

#include <omp.h>

#include <atomic>

#include <boost/range/adaptors.hpp>

struct ColumnGenerationParameters {
    int nbThreads;
    bool verbose;
};

struct StabilizationParameters {
    int nbThreads;
    bool verbose;
    double alpha;
    double precision;
};

template <typename RMP>
void addColumns(RMP& _rmp, std::vector<typename RMP::Column>& _columns) {
    if constexpr (has_get_column<RMP>::value) {
        IloNumColumnArray columns(_rmp.getEnv());
        columns.setSize(_columns.size());
        std::transform(_columns.begin(), _columns.end(), begin(columns),
            [&](auto&& _col) { return _rmp.getColumn(_col); });
        _rmp.addColumns(columns);
        columns.end();
    } else {
        _rmp.addColumns(_columns);
    }
}

template <typename Column>
std::string getName(const Column& _sPath) {
    std::stringstream stream;
    stream << _sPath;
    return stream.str();
}

template <typename Column>
Column reduceColumn(Column& _col1, const Column& _col2) {
    return _col1 += _col2;
}

template <typename... Ts>
using StaticPolymorhicStorage = typename std::tuple<std::vector<Ts>...>;

template <typename Tuple>
struct TupleOfPricings;

template <typename... Tuple>
struct TupleOfPricings<std::tuple<Tuple...>> {
    using ColumnStoringType =
        std::vector<typename PricingProblemTraits<Tuple>::Column...>;
    using ColumnType =
        std::tuple<typename PricingProblemTraits<Tuple>::Column...>;
};

template <typename PricingsTuple>
struct ThreadingBase;

template <typename... PricingProblems>
struct ThreadingBase<std::tuple<PricingProblems...>> {
    using LocalPricingStorage = std::vector<std::tuple<PricingProblems...>>;
    using LocalColumnStorage = std::vector<std::tuple<std::vector<
        typename PricingProblemTraits<PricingProblems>::Column>...>>;

    constexpr explicit ThreadingBase(
        int _nbThreads, const std::tuple<PricingProblems...> _pps)
        : pricingProblems(_nbThreads, std::move(_pps)) {}

    LocalPricingStorage pricingProblems;
    LocalColumnStorage columns;
};

template <typename PricingProblem, typename RMP, typename = void>
struct is_pricing_problem : std::false_type {};

template <typename PricingProblem, typename RMP>
struct is_pricing_problem<PricingProblem, RMP,
    std::enable_if_t<
        std::is_same_v<typename PricingProblem::Column, typename RMP::Column>>>
    : std::true_type {};

template <typename RMP, typename PricingTuple>
bool solve(RMP& _rmp, PricingTuple _pricingProblems,
    const ColumnGenerationParameters& _params) {
    // Create a threading base that contains the pricing problems
    ThreadingBase<PricingTuple> base(
        _params.nbThreads, std::forward<PricingTuple>(_pricingProblems));

    return solve_impl(_rmp, base, _params);
}

/*
template <typename RMP, typename PricingTuple>
bool solveStabilized(RMP& _rmp, PricingTuple&& _pricingProblems,
    const StabilizationParameters& _params, bool convert = true) {
    ThreadingBase<PricingTuple> base(
        _params.nbThreads, std::forward<PricingTuple>(_pricingProblems));
    typename RMP::DualValues bestDuals = _rmp.getDualValues();
    if (!solveStabilized_impl(
            _rmp, base, bestDuals, _params.alpha, _params.verbose)) {
        return false;
    }
    if (_rmp.hasActiveDummy()) {
        return false;
    }
    if (convert) {
        if (_params.verbose) {
            std::cout << "Converting to integer...";
        }
        _rmp.convertToInteger();
        if (!_rmp.solve()) {
            return false;
        }
        if (_params.verbose) {
            std::cout << "Found a solution with obj. value of "
                      << _rmp.getObjValue() << '\n';
        }
        return true;
    }
    return true;
}
*/

template <typename RMP, typename ColumnValidator, typename PricingProblem,
    typename ThreadingBase>
int generateColumns_impl(RMP& _rmp, const typename RMP::DualValues& _duals,
    ThreadingBase& _threadingBase, const ColumnValidator& _columnValidator) {

    using Column = typename PricingProblemTraits<PricingProblem>::Column;
    //_threadingBase.duals = tbb::enumerable_thread_specific<typename
    // RMP::DualValues>(_rmp.getDualValues());
    for (auto& cols : _threadingBase.columns) {
        std::get<std::vector<Column>>(cols).clear();
    }
    int nbAddedColumns = 0;
    if constexpr (PricingProblemTraits<PricingProblem>::IsUnique::value) {
#pragma omp parallel num_threads(_threadingBase.pricingProblems.size())
        for (int i = 0; i < _rmp.template getNbPricing<Column>(); ++i) {
#pragma omp task
            {
                auto& pp = std::get<PricingProblem>(
                    _threadingBase.pricingProblems[omp_get_thread_num()]);
                pp.setPricingID(i);
                if (!pp.solve(_duals)) {
                    throw std::runtime_error("Pricing could not be solved");
                }
                if (_columnValidator(pp)) {
                    auto& v = std::get<std::vector<Column>>(
                        _threadingBase.columns[omp_get_thread_num()]);
                    v.emplace_back(pp.getColumn());
                }
            }
            for (auto& cols : _threadingBase.columns) {
                nbAddedColumns += std::get<std::vector<Column>>(cols).size();
                for (auto& col : std::get<std::vector<Column>>(cols)) {
                    _rmp.addColumn(std::move(col));
                }
            }
        }
    } else {
        std::vector<std::optional<Column>> columns(
            _rmp.template getNbPricing<Column>());

#pragma omp parallel num_threads(_threadingBase.pricingProblems.size())
        for (int i = 0; i < _rmp.template getNbPricing<Column>(); ++i) {
#pragma omp task
            {
                auto& pp = std::get<PricingProblem>(
                    _threadingBase.pricingProblems[omp_get_thread_num()]);
                pp.setPricingID(i);
                pp.updateDual(_duals);
                if (!pp.solve()) {
                    throw std::runtime_error(
                        "Pricing could not be solved: " + std::to_string(i));
                }
                if (_columnValidator(pp)) {
                    columns[i] = std::move(pp.getColumn());
                }
            }
        }
        for (auto& col : columns) {
            if (col) {
                nbAddedColumns++;
                _rmp.addColumn(std::move(*col));
            }
        }
    }
    return nbAddedColumns;
}

template <typename RMP, typename ColumnValidator, typename... PricingProblems>
int generateColumns(RMP& _rmp, const typename RMP::DualValues& _duals,
    ThreadingBase<std::tuple<PricingProblems...>>& _threadingBase,
    ColumnValidator _columnValidator) {
    return (...
            + generateColumns_impl<RMP, ColumnValidator, PricingProblems>(
                _rmp, _duals, _threadingBase, _columnValidator));
}

template <typename RMP, typename PricingTuple>
bool solve_impl(RMP& _rmp, ThreadingBase<PricingTuple>& _threadingBase,
    ColumnGenerationParameters _params) {
    int nbIte = 0;
    int nbAddedColumns = 0;
    do {
        ++nbIte;
        if (_params.verbose) {
            std::cout << "#ite: " << nbIte << "...Solving RMP..." << std::flush;
        }
        if (_rmp.solve()) {
            assert([&]() {
                const auto dualObj = _rmp.getDualValues().getDualSumRHS();
                const auto currentObjValue = _rmp.getObjValue();
                if (epsilon_equal<double>()(dualObj, currentObjValue)) {
                    return true;
                }
                std::cout << "currentObjValue: " << currentObjValue << " vs."
                          << "getDualSumRHS: "
                          << _rmp.getDualValues().getDualSumRHS() << '\n';
                return false;
            }());
            if (_params.verbose) {
                std::cout << "RMP Objective Value = " << std::fixed
                          << std::setprecision(6) << _rmp.getObjValue()
                          << ", #cols:" << _rmp.getNbColumns() << '\n';
                std::cout << "Searching for new columns..." << std::flush;
            }
            const auto validator = [&](const auto& pp) {
                assert([&]() {
                    if (epsilon_equal<double>()(pp.getObjValue(),
                            _rmp.getReducedCost(pp.getColumn()))) {
                        return true;
                    }
                    std::cerr << pp.getObjValue() << " vs "
                              << _rmp.getReducedCost(pp.getColumn()) << '\n';
                    return false;
                }());
                return epsilon_less<double>()(pp.getObjValue(), 0.0);
            };
            nbAddedColumns = generateColumns(
                _rmp, _rmp.getDualValues(), _threadingBase, validator);
            if (_params.verbose) {
                std::cout << "added " << nbAddedColumns
                          << " new columns...\n\n";
            }
        } else {
            if (_params.verbose) {
                std::cout << "No solution found for RMP\n";
            }
            return false;
        }
    } while (nbAddedColumns > 0);
    return true;
}

template <typename RMP, typename PricingTuple>
bool solveStabilized_impl(RMP& _rmp,
    ThreadingBase<PricingTuple>& _threadingBase,
    typename RMP::DualValues& _bestDuals, const double _alpha = 0.1,
    bool _verbose = true) {
    double bestLowerBound = -std::numeric_limits<double>::max();

    double currentObjValue = std::numeric_limits<double>::max();
    auto stabDuals = _rmp.getDualValues();
    int nbIte = 0;
    bool improvedBound;
    int nbAddedColumns;
    do {
        improvedBound = false;
        ++nbIte;
        if (_verbose) {
            std::cout << "#ite: " << nbIte << "...Solving RMP..." << std::flush;
        }
        if (_rmp.solve()) {
            currentObjValue = _rmp.getObjValue();
            if (_verbose) {
                std::cout << "RMP Objective Value = " << std::fixed
                          << std::setprecision(6) << currentObjValue
                          << ", #cols:" << _rmp.getNbColumns();
            }

            // Stop if at lower bound
            if (epsilon_equal<double>()(bestLowerBound, currentObjValue)) {
                if (_verbose) {
                    std::cout << " -> at LB\n";
                }
                break;
            }
            if (_verbose) {
                std::cout << '\n';
            }

            // Check dual objective value
            const auto& normalDuals = _rmp.getDualValues();
            assert([&]() {
                const auto dualObj = normalDuals.getDualSumRHS();
                if (epsilon_equal<double>()(dualObj, currentObjValue)) {
                    return true;
                }
                std::cerr << std::fixed << std::setprecision(9)
                          << "currentObjValue: " << currentObjValue << " vs. "
                          << "getDualSumRHS: " << normalDuals.getDualSumRHS()
                          << '\n';
                std::cerr << _rmp.getModel() << '\n';
                return false;
            }());

            // Update best and stab duals
            barycenter(normalDuals, _bestDuals, stabDuals, _alpha);
            double sumStabRC{0.0};
            const auto validator = [&](const auto& pp) {
#pragma omp critical
                sumStabRC += pp.getObjValue();
                assert([&]() {
                    if (epsilon_equal<double>()(pp.getObjValue(),
                            _rmp.getReducedCost(pp.getColumn(), stabDuals))) {
                        return true;
                    }
                    std::cerr << pp.getObjValue() << " vs "
                              << _rmp.getReducedCost(pp.getColumn(), stabDuals)
                              << '\n';
                    return false;
                }());
                if (epsilon_less<double>()(pp.getObjValue(), 0.0)) {
                    const auto column = pp.getColumn();
                    if (epsilon_less<double>()(
                            _rmp.getReducedCost(column), 0.0)) {
                        return true;
                    }
                }
                return false;
            };
            if (_verbose) {
                std::cout << "Searching for new columns..." << std::flush;
            }
            nbAddedColumns =
                generateColumns(_rmp, stabDuals, _threadingBase, validator);
            if (_verbose) {
                std::cout << "added " << nbAddedColumns
                          << " new columns...done!\n";
            }
            const double stabLowerBound = sumStabRC + stabDuals.getDualSumRHS();
            if (_verbose) {
                std::cout << "Stab. LB: " << stabLowerBound << ", ";
            }
            if (bestLowerBound < stabLowerBound) {
                bestLowerBound = stabLowerBound;
                std::swap(_bestDuals, stabDuals);
                improvedBound = true;
            }
            if (_verbose) {
                std::cout << "Current best LB: ";
                if (bestLowerBound > -std::numeric_limits<double>::max()) {
                    std::cout << bestLowerBound;
                } else {
                    std::cout << "-inf";
                }
                std::cout << "...\n\n";
            }
        } else {
            if (_verbose) {
                std::cout << "No solution found for RMP\n";
            }
            return false;
        }
    } while (nbAddedColumns > 0 || improvedBound);
    assert([&]() {
        if (epsilon_equal<double>()(bestLowerBound, currentObjValue)) {
            return true;
        }
        std::cerr << std::fixed << std::setprecision(10) << bestLowerBound
                  << " vs " << currentObjValue << '\n';
        return false;
    }());
    return true;
}

template <typename T>
struct ColumnPair {
    ColumnPair(IloNumVar _var, T _obj)
        : var(_var)
        , obj(std::move(_obj)) {}

    IloNumVar var;
    T obj;
};

/**
 * A LazyConstraint is a constraint that is not added to model immediately
 */
class LazyConstraint {
  public:
    LazyConstraint(IloModel _model, IloNum _lb, IloExpr _expr, IloNum _ub)
        : m_lb(_lb)
        , m_expr(move(std::move(_expr)))
        , m_ub(_ub)
        , m_model(_model) {}

    LazyConstraint(IloModel _model, IloNum _lb, IloNum _ub)
        : m_lb(_lb)
        , m_ub(_ub)
        , m_model(_model) {}

    LazyConstraint() = default;
    ~LazyConstraint() = default;
    LazyConstraint(const LazyConstraint& _rhs) = default;
    LazyConstraint& operator=(const LazyConstraint& _rhs) = default;

    LazyConstraint(LazyConstraint&& _rhs) noexcept
        : m_lb(_rhs.m_lb)
        , m_expr(move(std::move(_rhs.m_expr)))
        , m_ub(_rhs.m_ub)
        , m_name(std::move(_rhs.m_name))
        , m_rng(move(std::move(_rhs.m_rng)))
        , m_model(_rhs.m_model)
        , m_isInModel(_rhs.m_isInModel) {}

    LazyConstraint& operator=(LazyConstraint&& _rhs) noexcept {
        if (this != &_rhs) {
            m_lb = _rhs.m_lb;
            m_expr = IloExpr(_rhs.m_expr.getImpl());
            m_ub = _rhs.m_ub;
            m_rng = IloRange(_rhs.m_rng.getImpl());
            m_model = _rhs.m_model;
            _rhs.m_rng = IloRange();
            _rhs.m_expr = IloExpr();
            m_name = std::move(_rhs.m_name);
            m_isInModel = _rhs.m_isInModel;
        }
        return *this;
    }

    IloEnv getEnv() const { return m_model.getEnv(); }

    IloAddValueToRange operator()(IloNum _coeff) {
        if (!m_isInModel) {
            m_rng = IloRange(m_model.getEnv(), m_lb, m_expr, m_ub);
            m_rng.setName(m_name.c_str());
            m_model.add(m_rng);
            m_isInModel = true;
        }
        return m_rng(_coeff);
    }

    void setName(const std::string_view& _str) { m_name = std::string(_str); }

    double getDual(IloCplex& _solver) const {
        if (!m_isInModel) {
            return 0.0;
        }
        return _solver.getDual(m_rng);
    }

    bool isActive() const noexcept { return m_isInModel; }

    void setConstraint(const IloRange& _rng) {
        m_rng = _rng;
        m_rng.setName(m_name.c_str());
        m_model.add(m_rng);
        m_isInModel = true;
    }

    const IloRange& getConstraint() const { return m_rng; }

  private:
    IloNum m_lb = -IloInfinity;
    IloExpr m_expr = IloExpr();
    IloNum m_ub = IloInfinity;
    std::string m_name = std::string();
    IloRange m_rng = IloRange();
    IloModel m_model = IloModel();
    bool m_isInModel = false;
};

template <typename IloConstraint>
double getDual(IloCplex& _solver, const IloConstraint& _cons) {
    if constexpr (std::is_same<IloConstraint, LazyConstraint>::value) {
        return _cons.getDual(_solver);
    } else {
        return _solver.getDual(_cons);
    }
}

template <typename Constraint>
IloNumColumn getColumn(Constraint& _const, double _value) {
    if (epsilon_equal<double>()(_value, 0)) {
        return IloNumColumn(_const.getEnv());
    }
    return _const(_value);
}

#endif
