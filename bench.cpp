#include <vector>
#include <algorithm>

#include "benchmark/benchmark.h"
#include "RadixSort.hpp"
#include "MyRandom.hpp"

using Integer = unsigned int;

static void BM_stdsort(benchmark::State& state) {
	while(state.KeepRunning()) {
		state.PauseTiming();
		std::vector<Integer> tab(state.range(0));
	    for (auto& v : tab) {
	        v = MyRandom::getInstance().getIntUniform<Integer>(std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::min());
	    }
		state.ResumeTiming();
		std::sort(tab.begin(), tab.end());
	}
}

static void BM_right_radix_sort(benchmark::State& state) {
	while(state.KeepRunning()) {
		state.PauseTiming();
		std::vector<Integer> tab(state.range(0));
	    for (auto& v : tab) {
	        v = MyRandom::getInstance().getIntUniform<Integer>(std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::min());
	    }
		state.ResumeTiming();
		RightRadixSort<Integer>::sort(tab.begin(), tab.end());
	}
}

static void BM_counting_right_radix_sort(benchmark::State& state) {
	while(state.KeepRunning()) {
		state.PauseTiming();
		std::vector<Integer> tab(state.range(0));
	    for (auto& v : tab) {
	        v = MyRandom::getInstance().getIntUniform<Integer>(std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::min());
	    }
		state.ResumeTiming();
		CountingRightRadixSort<Integer>::sort(tab.begin(), tab.end());
	}
}

static void BM_counting_right_radix_sort_no_ite(benchmark::State& state) {
	while(state.KeepRunning()) {
		state.PauseTiming();
		std::vector<Integer> tab(state.range(0));
	    for (auto& v : tab) {
	        v = MyRandom::getInstance().getIntUniform<Integer>(std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::min());
	    }
		state.ResumeTiming();
		CountingRightRadixSort<Integer>::sort(tab);
	}
}

BENCHMARK(BM_stdsort)->RangeMultiplier(10)->Range(100000, 100000000)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_right_radix_sort)->RangeMultiplier(10)->Range(100000, 100000000)->Unit(benchmark::kMillisecond);;
BENCHMARK(BM_counting_right_radix_sort)->RangeMultiplier(10)->Range(100000, 100000000)->Unit(benchmark::kMillisecond);;
BENCHMARK(BM_counting_right_radix_sort_no_ite)->RangeMultiplier(10)->Range(100000, 100000000)->Unit(benchmark::kMillisecond);;

BENCHMARK_MAIN();