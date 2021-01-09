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
    std::vector<ColumnType> m_columns;

  public:
    template <typename... Args>
    explicit ParallelPricer(std::size_t _nbPricing,
        std::pair<PricingInputIterator, PricingInputIterator> _inputIters,
        Args&&... _args);

    template <typename DualValues>
    void generateColumnns(const DualValues& _values);

    std::vector<ColumnType>& getColumns() { return m_columns; }
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
    , m_columns(std::distance(m_inputIters.first, m_inputIters.second))

{}

template <typename PricingProblemType, typename PricingInputIterator>
template <typename DualValues>
void ParallelPricer<PricingProblemType, PricingInputIterator>::generateColumnns(
    const DualValues& _values) {
#pragma omp parallel num_threads(m_pricings.size())
    for (auto ites = std::make_pair(m_inputIters.first, m_columns.begin());
         ites.first != m_inputIters.second; ++ites.first, ++ites.second) {
#pragma omp task
        {
            const std::size_t thread_id = omp_get_thread_num();
            *ites.second = m_pricings[thread_id](*ites.first, _values);
        }
    }
}

template <typename RMP, typename DualValues, typename PricingProblemType,
    typename PricingInputIterator>
std::size_t moveColumns(RMP& _rmp,
    ParallelPricer<PricingProblemType, PricingInputIterator>& _pricer,
    const DualValues& _values) {
    std::size_t nbAddedColumns = 0;
    for (auto& col : _pricer.getColumns()) {
        if (_rmp.getReducedCost(col, _values)) {
            _rmp.addColumn(std::move(col));
            ++nbAddedColumns;
        }
    }
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
    do {
        ++nbIte;
        if (_verbose) {
            std::cout << "#ite: " << nbIte << "...Solving RMP..." << std::flush;
        }
        if (auto dualValues = _solverFunc(_rmp); dualValues) {
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
