#ifndef ROW_GENERATION_HPP
#define ROW_GENERATION_HPP

#include <bits/c++config.h>
#include <ilcplex/ilocplexi.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

#include <omp.h>

namespace CppRO::RowGeneration {

/**
 * ParallelSeparator manages multi-threaded separator resolutions
 * It stores multiple copies of a separator problem class (one for each thread)
 * and stores the generated rows by these separators.
 */
template <typename SeparatorType, typename RowType,
    typename SeparatorInputRangeType>
class ParallelSeparator {
    std::vector<SeparatorType> m_separators;
    SeparatorInputRangeType m_inputRange;
    std::vector<std::vector<RowType>> m_rows;

  public:
    template <typename... ArgTypes>
    explicit ParallelSeparator(std::size_t _nbSeparators,
        SeparatorInputRangeType _inputRange, ArgTypes&&... _args);

    template <typename RMPState>
    void generateRows(const RMPState& _state);

    const std::vector<std::vector<RowType>>& getRows() const { return m_rows; }
};

template <typename SeparatorType, typename RowType,
    typename SeparatorInputRangeType>
template <typename... ArgTypes>
ParallelSeparator<SeparatorType, RowType,
    SeparatorInputRangeType>::ParallelSeparator(std::size_t _nbSeparators,
    SeparatorInputRangeType _inputRange, ArgTypes&&... _args)
    : m_separators([&] {
        std::vector<SeparatorType> retval;
        retval.reserve(_nbSeparators);
        for (std::size_t i = 0; i < _nbSeparators; ++i) {
            retval.emplace_back(std::forward<ArgTypes>(_args)...);
        }
        return retval;
    }())
    , m_inputRange(std::move(_inputRange))
    , m_rows(_nbSeparators)

{}

template <typename SeparatorType, typename RowType,
    typename SeparatorInputRangeType>
template <typename RMPState>
void ParallelSeparator<SeparatorType, RowType,
    SeparatorInputRangeType>::generateRows(const RMPState& _state) {
#pragma omp parallel num_threads(m_separators.size()) default(none)            \
    shared(m_separators, _state)
    {
#pragma omp single nowait
        for (const auto& separator : m_inputRange) {
#pragma omp task default(none) firstprivate(separator)                         \
    shared(m_separators, _state)
            {
                const auto threadId =
                    static_cast<std::size_t>(omp_get_thread_num());
                m_rows[threadId].emplace_back(
                    m_separators[threadId](separator, _state));
            }
        }
    }
#pragma omp taskwait
}

template <typename RMP, typename DualValues, typename SeparatorType,
    typename RowType, typename SeparatorInputRangeType>
std::size_t moveRows(RMP& _rmp,
    ParallelSeparator<SeparatorType, RowType, SeparatorInputRangeType>& _pricer,
    const DualValues& _values) {
    std::size_t nbAddedRows = 0;
    for (const auto& rows : _pricer.getRows()) {
        for (const auto& row : rows) {
            if (_rmp.isImprovingRow(row, _values)) {
                _rmp.addRow(row);
                ++nbAddedRows;
            }
        }
    }
    return nbAddedRows;
}

/**
 * Run the column generation algorithm on a reduced master problem
 * _solveFunction is the function used to solve the rmp and return an
 * std::optional with the dual values.
 */
template <typename RMP, typename SolveFunctionType, typename RowSearchFunction>
bool solve(RMP& _rmp, SolveFunctionType&& _solveFunction,
    RowSearchFunction&& _rowSearchFunc, bool _verbose) {
    std::size_t nbIte = 0;
    std::size_t nbAddedRows = 0;
    do {
        ++nbIte;
        if (_verbose) {
            std::cout << "#ite: " << nbIte << "...Solving RMP..." << std::flush;
        }
        if (auto rmpState = _solveFunction(_rmp); rmpState) {
            if (_verbose) {
                std::cout << "#cols:" << _rmp.getNbRows() << '\n';
                std::cout << "Searching for new rows..." << std::flush;
            }
            nbAddedRows = _rowSearchFunc(_rmp, *rmpState);
            if (_verbose) {
                std::cout << "added " << nbAddedRows << " new rows...\n\n";
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
