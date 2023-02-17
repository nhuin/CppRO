#ifndef ROW_GENERATION_HPP
#define ROW_GENERATION_HPP

#include <bits/c++config.h>
#include <ilcplex/ilocplexi.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <ranges>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
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

    void clear() {
        for (auto& row : m_rows) {
            row.clear();
        }
    }
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
#pragma omp parallel num_threads(m_separators.size())                          \
    shared(m_separators, _state)
    {
#pragma omp single nowait
        for (auto first = begin(m_inputRange), last = end(m_inputRange);
             first != last; ++first) {
#pragma omp task default(none) firstprivate(first) shared(m_separators, _state)
            {
                const auto threadId =
                    static_cast<std::size_t>(omp_get_thread_num());
                if (auto row = m_separators[threadId](*first, _state); row) {
                    m_rows[threadId].emplace_back(*row);
                }
            }
        }
    }
#pragma omp taskwait
}

struct Visitor {
    struct RMPSolveFailed {};
    struct IterationStarted {};
    struct RMPSolvedSuccess {};
    struct RowGenerated {};

    std::size_t nbIterations{0};
    void operator()(IterationStarted /*_unused*/) {
        spdlog::info("#ite: {}... Solving RMP...", nbIterations++);
    }

    void operator()(RMPSolvedSuccess /*_unused*/) {
        spdlog::info("RMP solved");
    }

    void operator()(RMPSolveFailed /*_unused*/) {
        spdlog::info("No solution found for RMP");
    }

    void operator()(RowGenerated /*_unused*/) {
        spdlog::info("Row generation done");
    }
};

/**
 * Run the column generation algorithm on a reduced master problem
 * _solveFunction is the function used to solve the rmp and return an
 * std::optional with the dual values.
 */
template <typename RowRange, typename SolveFunctionType,
    typename GenerationFunctionType, typename VisitorType>
bool solve(RowRange&& _initRows, SolveFunctionType&& _solveFunction,
    GenerationFunctionType&& _genFunction, VisitorType _vis) {
    _vis(Visitor::IterationStarted{});
    auto rmpState = _solveFunction(_initRows);
    if (!rmpState.has_value()) {
        _vis(Visitor::RMPSolveFailed{});
        return false;
    }
    _vis(Visitor::RMPSolvedSuccess{});
    for (auto newRows = _genFunction(*rmpState); newRows;
         newRows = _genFunction(*rmpState)) {
        _vis(Visitor::RowGenerated{});
        rmpState = _solveFunction(*newRows);
        if (!rmpState.has_value()) {
            _vis(Visitor::RMPSolveFailed{});
            return false;
        }
    }
    return true;
}

} // namespace CppRO::RowGeneration
#endif
