#ifndef ROW_GENERATION_HPP
#define ROW_GENERATION_HPP

#include <bits/c++config.h>
#include <ilcplex/ilocplexi.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

#include <omp.h>

namespace CppRO::RowGeneration {

/**
 * ParallelSeparater manages multi-threaded pricing resolutions
 * It stores multiple copies of a pricing problem class (one for each thread)
 * and stores the generated columns by these pricings.
 */
template <typename PricingProblemType, typename PricingInputIterator>
class ParallelSeparater {
    using RowType = typename PricingProblemType::RowType;
    std::vector<PricingProblemType> m_pricings;
    std::pair<PricingInputIterator, PricingInputIterator> m_inputIters;
    std::vector<std::vector<RowType>> m_columns;

  public:
    template <typename... Args>
    explicit ParallelSeparater(std::size_t _nbPricing,
        std::pair<PricingInputIterator, PricingInputIterator> _inputIters,
        Args&&... _args);

    template <typename DualValues>
    void generateRows(const DualValues& _values);

    const std::vector<std::vector<RowType>>& getRows() const {
        return m_columns;
    }
};

template <typename PricingProblemType, typename PricingInputIterator>
template <typename... Args>
ParallelSeparater<PricingProblemType, PricingInputIterator>::ParallelSeparater(
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
template <typename DualValues>
void ParallelSeparater<PricingProblemType, PricingInputIterator>::generateRows(
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
std::size_t moveRows(RMP& _rmp,
    ParallelSeparater<PricingProblemType, PricingInputIterator>& _pricer,
    const DualValues& _values) {
    std::size_t nbAddedRows = 0;
    for (const auto& cols : _pricer.getRows()) {
        for (const auto& col : cols) {
            if (_rmp.isImprovingRow(col, _values)) {
                _rmp.addRow(col);
                ++nbAddedRows;
            }
        }
    }
    return nbAddedRows;
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
template <typename RMP, typename SolverFunc, typename GenerateRowFunc>
bool solve(RMP& _rmp, SolverFunc&& _solverFunc, GenerateRowFunc&& _genRowFunc,
    bool _verbose) {
    std::size_t nbIte = 0;
    std::size_t nbAddedRows = 0;
    do {
        ++nbIte;
        if (_verbose) {
            std::cout << "#ite: " << nbIte << "...Solving RMP..." << std::flush;
        }
        if (auto dualValues = _solverFunc(_rmp); dualValues) {
            if (_verbose) {
                std::cout << "#cols:" << _rmp.getNbRows() << '\n';
                std::cout << "Searching for new columns..." << std::flush;
            }
            nbAddedRows = _genRowFunc(_rmp, *dualValues);
            if (_verbose) {
                std::cout << "added " << nbAddedRows << " new columns...\n\n";
            }
        } else {
            if (_verbose) {
                std::cout << "No solution found for RMP\n";
            }
            return false;
        }
    } while (nbAddedRows > 0);
    return true;
}

} // namespace CppRO::RowGeneration
#endif
