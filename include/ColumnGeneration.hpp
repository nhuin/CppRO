#ifndef COLUMN_GENERATION_HPP
#define COLUMN_GENERATION_HPP

#include <ilcplex/ilocplexi.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

#include <omp.h>

/**
 * ParallelPricer manages multi-threaded pricing resolutions
 * It stores multiple copies of a pricing problem class (one for each thread)
 * and stores the generated columns by these pricings.
 */
template <typename PricingProblemType, typename PricingInputIterator>
class ParallelPricer {
    using ColumnType = typename PricingProblemType::ColumnType;
    std::vector<PricingProblemType> m_pricings;
    std::pair<PricingInputIterator, PricingInputIterator> m_inputIters;
    std::vector<std::vector<ColumnType>> m_columns;

  public:
    template <typename... Args>
    explicit ParallelPricer(std::size_t _nbPricing,
        std::pair<PricingInputIterator, PricingInputIterator> _inputIters,
        Args&&... _args);

    template <typename DualValues, typename RMP>
    void generateColumnnsUntilFound(
        const DualValues& _dualValues, const RMP& _rmp);

    template <typename DualValues>
    void generateColumnns(const DualValues& _dualValues);

    const std::vector<std::vector<ColumnType>>& getColumns() const {
        return m_columns;
    }

    void clearColumns() {
        for (auto& cols : m_columns) {
            cols.clear();
        }
    }
};

template <typename PricingProblemType, typename PricingInputIterator>
template <typename... Args>
ParallelPricer<PricingProblemType, PricingInputIterator>::ParallelPricer(
    std::size_t _nbPricing,
    std::pair<PricingInputIterator, PricingInputIterator> _inputIters,
    Args&&... _args)
    : m_pricings([&] {
        std::vector<PricingProblemType> retval;
        retval.reserve(_nbPricing);
        for (std::size_t i = 0; i < _nbPricing; ++i) {
            retval.emplace_back(std::forward<Args>(_args)...);
        }
        return retval;
    }())
    , m_inputIters(std::move(_inputIters))
    , m_columns(_nbPricing)

{}

template <typename PricingProblemType, typename PricingInputIterator>
template <typename DualValues, typename RMP>
void ParallelPricer<PricingProblemType, PricingInputIterator>::
    generateColumnnsUntilFound(const DualValues& _dualValues, const RMP& _rmp) {
#pragma omp parallel num_threads(m_pricings.size()) default(none)              \
    shared(m_pricings, _rmp, _dualValues)
    {
        bool foundColumn = false;
#pragma omp single nowait
        for (auto inputIte = m_inputIters.first;
             inputIte != m_inputIters.second; ++inputIte) {
#pragma omp task default(none) firstprivate(inputIte)                          \
    shared(m_pricings, _rmp, _dualValues, foundColumn)
            {
                if (!foundColumn) {
                    const auto threadId =
                        static_cast<std::size_t>(omp_get_thread_num());
                    auto col = m_pricings[threadId](*inputIte, _dualValues);
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

template <typename PricingProblemType, typename PricingInputIterator>
template <typename DualValues>
void ParallelPricer<PricingProblemType, PricingInputIterator>::generateColumnns(
    const DualValues& _dualValues) {
#pragma omp parallel num_threads(m_pricings.size()) default(none)              \
    shared(m_pricings, _dualValues)
    {
#pragma omp single nowait
        for (auto inputIte = m_inputIters.first;
             inputIte != m_inputIters.second; ++inputIte) {
#pragma omp task default(none) firstprivate(inputIte)                          \
    shared(m_pricings, _dualValues)
            {
                const auto threadId =
                    static_cast<std::size_t>(omp_get_thread_num());
                m_columns[threadId].emplace_back(
                    m_pricings[threadId](*inputIte, _dualValues));
            }
        }
    }
#pragma omp taskwait
}

template <typename RMP, typename DualValues, typename PricingProblemType,
    typename PricingInputIterator>
std::size_t moveColumns(RMP& _rmp,
    ParallelPricer<PricingProblemType, PricingInputIterator>& _pricer,
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

#endif
