#include "MultiDimArray.hpp"
#include "benchmark/benchmark.h"
#include <algorithm>
#include <boost/multi_array.hpp>

static void MINE_init_list_access(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    MultiDimArray<int, 3> m({{x, y, z}});
    while (state.KeepRunning()) {
        m({x, y, z}) = 1;
  }
}

static void MINE_variadic_access(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    MultiDimArray<int, 3> m({{x, y, z}});
    while (state.KeepRunning()) {
        m(x, y, z) = 1;
    }
}

static void BOOST(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    boost::multi_array<int, 3> m(boost::extents[x][y][z]);
    while (state.KeepRunning()) {
        m[x][y][z] = 1;
    }
}

static void BOOST_ref(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    std::vector<int> container(x * y * z);
    boost::multi_array_ref<int, 3> m(container.data(), boost::extents[x][y][z]);
    while (state.KeepRunning()) {
        m[x][y][z] = 1;
    }
}

static void BOOST_fill(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    while (state.KeepRunning()) {
        boost::multi_array<int, 3> m(boost::extents[x][y][z]);
        std::fill_n(m.data(), m.num_elements(), 5);
    }
}

static void BOOST_ref_fill(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    while (state.KeepRunning()) {
        std::vector<int> container(x * y * z, 5);
        boost::multi_array_ref<int, 3> m(container.data(), boost::extents[x][y][z]);
    }
}

static void MINE_fill(benchmark::State& state) {
    const std::size_t x = state.range(0),
                      y = state.range(1),
                      z = state.range(2);
    while (state.KeepRunning()) {
        MultiDimArray<int, 3> m({{x, y, z}}, 5);
    }
}

BENCHMARK(MINE_init_list_access)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});
BENCHMARK(MINE_variadic_access)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});
BENCHMARK(BOOST)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});
BENCHMARK(BOOST_ref)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});
BENCHMARK(BOOST_fill)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});
BENCHMARK(BOOST_ref_fill)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});
BENCHMARK(MINE_fill)->Args({3, 3, 3})->Args({10, 10, 10})->Args({30, 30, 30});

BENCHMARK_MAIN();
