#include "MultiDimArray.hpp"
#include "benchmark/benchmark.h"

static void BM_init_list_access(benchmark::State& state) {
	const std::size_t x = state.range(0), 
		y = state.range(0),
		z = state.range(0);
	while (state.KeepRunning()) {
		MultiDimArray<int, 3> m({{x, y, z}});
		for(std::size_t i = 0; i < x; ++i) {
			for(std::size_t j = 0; j < y; ++j) {
				for(std::size_t k = 0; k < z; ++k) {
					m({x, y, z}) = 1;
				}
			}
		}
	}
}

static void BM_variadic_access(benchmark::State& state) {
	const std::size_t x = state.range(0), 
		y = state.range(0),
		z = state.range(0);
	while (state.KeepRunning()) {
		MultiDimArray<int, 3> m({{x, y, z}});
		for(std::size_t i = 0; i < x; ++i) {
			for(std::size_t j = 0; j < y; ++j) {
				for(std::size_t k = 0; k < z; ++k) {
					m(x, y, z) = 1;
				}
			}
		}
	}
}

BENCHMARK(BM_init_list_access)->Range(1, 100);
BENCHMARK(BM_variadic_access)->Range(1, 100);

BENCHMARK_MAIN();
