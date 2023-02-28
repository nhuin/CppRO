#ifndef COLUMN_GENERATION_HPP
#define COLUMN_GENERATION_HPP

#include <ilcplex/ilocplexi.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <spdlog/spdlog.h>
#include <vector>

#include <omp.h>

namespace CppRO::ColumnGeneration {

/**
 * ParallelPricer manages multi-threaded pricing resolutions
 * It stores multiple copies of a pricing problem class (one for each thread)
 * and stores the generated columns by these pricings.
 */
template <typename PricingProblemType, typename PricingInputIterator>
class SimplePricer {
    using ColumnType = typename PricingProblemType::ColumnType;
    PricingProblemType m_pricer;
    std::pair<PricingInputIterator, PricingInputIterator> m_inputIters;
    std::vector<ColumnType> m_columns;

  public:
    template <typename... Args>
    explicit SimplePricer(
        std::pair<PricingInputIterator, PricingInputIterator> _inputIters,
        Args&&... _args);

    template <typename DualValues, typename RMP>
    void generateColumnsUntilFound(
        const DualValues& _dualValues, const RMP& _rmp);

    template <typename DualValues>
    void generateColumns(const DualValues& _dualValues);

    const std::vector<ColumnType>& getColumns() const { return m_columns; }

    void clearColumns() { m_columns.clear(); }
};

template <typename PricingProblemType, typename PricingInputIterator>
template <typename... Args>
SimplePricer<PricingProblemType, PricingInputIterator>::SimplePricer(
    std::pair<PricingInputIterator, PricingInputIterator> _inputIters,
    Args&&... _args)
    : m_pricer(std::forward<Args>(_args)...)
    , m_inputIters(std::move(_inputIters))

{}

template <typename PricingProblemType, typename PricingInputIterator>
template <typename DualValues, typename RMP>
void SimplePricer<PricingProblemType, PricingInputIterator>::
    generateColumnsUntilFound(const DualValues& _dualValues, const RMP& _rmp) {
    bool foundColumn = false;
    for (auto inputIte = m_inputIters.first; inputIte != m_inputIters.second;
         ++inputIte) {
        if (!foundColumn) {
            auto col = m_pricer(*inputIte, _dualValues);
            if (_rmp.isImprovingColumn(col, _dualValues)) {
                m_columns.emplace_back(std::move(col));
                foundColumn = true;
            }
        }
    }
}

template <typename PricingProblemType, typename PricingInputIterator>
template <typename DualValues>
void SimplePricer<PricingProblemType, PricingInputIterator>::generateColumns(
    const DualValues& _dualValues) {
    for (auto inputIte = m_inputIters.first; inputIte != m_inputIters.second;
         ++inputIte) {
        m_columns.emplace_back(m_pricer(*inputIte, _dualValues));
    }
}

template <typename RMP, typename DualValues, typename PricingProblemType,
    typename PricingInputIterator>
std::size_t moveColumns(RMP& _rmp,
    SimplePricer<PricingProblemType, PricingInputIterator>& _pricer,
    const DualValues& _values) {
    std::size_t nbAddedColumns = 0;
    for (const auto& col : _pricer.getColumns()) {
        if (_rmp.isImprovingColumn(col, _values)) {
            _rmp.addColumn(col);
            ++nbAddedColumns;
        }
    }
    _pricer.clearColumns();
    return nbAddedColumns;
}

template <typename ColumnType, typename PricingInputRange,
    typename PricingProblemType>
class ParallelPricer {
    std::vector<PricingProblemType> m_pricings;
    PricingInputRange m_inputRange;
    std::vector<std::vector<ColumnType>> m_columns;

  public:
    template <typename... Args>
    explicit ParallelPricer(
        std::size_t _nbPricing, PricingInputRange _inputRange, Args&&... _args);

    template <typename DualValues, typename RMP>
    void generateColumnsUntilFound(
        const DualValues& _dualValues, const RMP& _rmp);

    template <typename DualValues>
    void generateColumns(const DualValues& _dualValues);

    const std::vector<std::vector<ColumnType>>& getColumns() const {
        return m_columns;
    }

    void clearColumns() {
        for (auto& cols : m_columns) {
            cols.clear();
        }
    }

    [[nodiscard]] std::size_t getNbPricers() const { return m_pricings.size(); }
};

template <typename ColumnType, typename PricingInputRange,
    typename PricingProblemType>
template <typename... Args>
ParallelPricer<ColumnType, PricingInputRange,
    PricingProblemType>::ParallelPricer(std::size_t _nbPricing,
    PricingInputRange _inputRange, Args&&... _args)
    : m_pricings([&] {
        std::vector<PricingProblemType> retval;
        retval.reserve(_nbPricing);
        for (std::size_t i = 0; i < _nbPricing; ++i) {
            retval.emplace_back(std::forward<Args>(_args)...);
        }
        return retval;
    }())
    , m_inputRange(std::move(_inputRange))
    , m_columns(_nbPricing)

{}

template <typename ColumnType, typename PricingInputRange,
    typename PricingProblemType>
template <typename DualValues, typename RMP>
void ParallelPricer<ColumnType, PricingInputRange, PricingProblemType>::
    generateColumnsUntilFound(const DualValues& _dualValues, const RMP& _rmp) {
#pragma omp parallel num_threads(m_pricings.size()) default(none)              \
    shared(m_pricings, _rmp, _dualValues)
    {
        bool foundColumn = false;
#pragma omp single nowait
        for (auto first = begin(m_inputRange), last = end(m_inputRange);
             first != last; ++first) {
#pragma omp task default(none) firstprivate(first)                             \
    shared(m_pricings, _rmp, _dualValues, foundColumn)
            {
                if (!foundColumn) {
                    const auto threadId =
                        static_cast<std::size_t>(omp_get_thread_num());
                    auto col = m_pricings[threadId](*first, _dualValues);
                    if (_rmp.isImprovingColumn(col, _dualValues)) {
                        m_columns[threadId].emplace_back(std::move(col));
#pragma omp critical
                        foundColumn = true;
                    }
                }
            }
        }
    }
#pragma omp taskwait
}

template <typename ColumnType, typename PricingInputRange,
    typename PricingProblemType>
template <typename DualValues>
void ParallelPricer<ColumnType, PricingInputRange,
    PricingProblemType>::generateColumns(const DualValues& _dualValues) {
#pragma omp parallel num_threads(m_pricings.size())                            \
    shared(m_pricings, _dualValues)
    {
#pragma omp single nowait
        for (auto first = begin(m_inputRange), last = end(m_inputRange);
             first != last; ++first) {
#pragma omp task default(none) firstprivate(first)                             \
    shared(m_pricings, _dualValues)
            {
                const auto threadId =
                    static_cast<std::size_t>(omp_get_thread_num());
                m_columns[threadId].emplace_back(
                    m_pricings[threadId](*first, _dualValues));
            }
        }
    }
#pragma omp taskwait
}

template <typename RMP, typename DualValues, typename ColumnType,
    typename PricingInputRange, typename PricingProblemType>
std::size_t moveColumns(RMP& _rmp,
    ParallelPricer<ColumnType, PricingInputRange, PricingProblemType>& _pricer,
    const DualValues& _values) {
    std::size_t nbAddedColumns = 0;
    for (const auto& cols : _pricer.getColumns()) {
        for (const auto& col : cols) {
            if (_rmp.isImprovingColumn(col, _values)) {
                _rmp.addColumn(col);
                ++nbAddedColumns;
            }
        }
    }
    _pricer.clearColumns();
    return nbAddedColumns;
}

template <typename RMP, typename DualValues>
std::optional<DualValues> solve(const RMP& _rmp, IloCplex& _solver) {
    if (_solver.solve()) {
        return _rmp.getDualValues(_solver);
    }
    return std::nullopt;
}

/**
 * Run the column generation algorithm on a reduced master problem
 * _solverFunc is the function used to solve the rmp and return an std::optional
 * with the dual values.
 */
template <typename RMP, typename SolverFunc, typename GenerateColumnFunc>
bool solve(RMP& _rmp, SolverFunc&& _solverFunc,
    GenerateColumnFunc&& _genColFunc, bool _verbose) {
    std::size_t nbIte = 0;
    std::size_t nbAddedColumns = 0;
    decltype(_solverFunc(_rmp)) dualValues;
    do {
        ++nbIte;
        if (_verbose) {
            std::cout << "#ite: " << nbIte << "...Solving RMP..." << std::flush;
        }
        dualValues = _solverFunc(_rmp);
        if (dualValues) {
            if (_verbose) {
                std::cout << "#cols:" << _rmp.getNbColumns() << '\n';
                std::cout << "Searching for new columns..." << std::flush;
            }
            nbAddedColumns = _genColFunc(_rmp, *dualValues);
            if (_verbose) {
                std::cout << "added " << nbAddedColumns
                          << " new columns...\n\n";
            }
        } else {
            if (_verbose) {
                std::cout << "No solution found for RMP\n";
            }
            return false;
        }
    } while (nbAddedColumns > 0);
    return true;
}

struct Visitor {
    struct RMPSolveFailed {};
    struct IterationStarted {};
    struct RMPSolvedSuccess {};
    struct ColumnsGenerated {};

    std::size_t nbIterations{0};
    void operator()(IterationStarted /*_unused*/) {
        spdlog::info("#ite: {}... Solving RMP...", nbIterations++);
    }

    void operator()(RMPSolvedSuccess /*_unused*/) const {
        spdlog::info("RMP solved");
    }

    void operator()(RMPSolveFailed /*_unused*/) const {
        spdlog::info("No solution found for RMP");
    }

    void operator()(ColumnsGenerated /*_unused*/) const {
        spdlog::info("Column generation done");
    }
};

/**
 * Run the column generation algorithm on a reduced master problem
 * _solveFunction is the function used to solve the rmp and return an
 * std::optional with the dual values.
 */
template <typename ColumnRange, typename SolveFunctionType,
    typename GenerationFunctionType, typename VisitorType>
bool solve(ColumnRange&& _initColumns, SolveFunctionType&& _solveFunction,
    GenerationFunctionType&& _genFunction, VisitorType _vis) {
    _vis(Visitor::IterationStarted{});
    auto rmpState = _solveFunction(_initColumns);
    if (!rmpState.has_value()) {
        _vis(Visitor::RMPSolveFailed{});
        return false;
    }
    _vis(Visitor::RMPSolvedSuccess{});
    for (auto newColumns = _genFunction(*rmpState); newColumns;
         newColumns = _genFunction(*rmpState)) {
        _vis(Visitor::ColumnsGenerated{});
        rmpState = _solveFunction(*newColumns);
        if (!rmpState.has_value()) {
            _vis(Visitor::RMPSolveFailed{});
            return false;
        }
    }
    return true;
}

} // namespace CppRO::ColumnGeneration
#endif
