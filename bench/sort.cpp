#include <algorithm>
#include <array>
#include <string>

#include "DiGraph.hpp"
#include "MyRandom.hpp"
#include "RadixSort.hpp"
#include "ShortestPathBF.hpp"

#include "benchmark/benchmark.h"

// using Integer = unsigned long;

// static void BM_std_sort(benchmark::State& state) {
//     while (state.KeepRunning()) {
//         state.PauseTiming();
//         std::vector<Integer> tab(state.range(0));
//         std::generate(tab.begin(), tab.end(), [] {
//             return MyRandom::getInstance().getIntUniform<Integer>(
//                 std::numeric_limits<Integer>::min(),
//                 std::numeric_limits<Integer>::max());
//         });
//         state.ResumeTiming();
//         std::sort(tab.begin(), tab.end());
//     }
// }

// static void BM_LSD_radix_sort(benchmark::State& state) {
//     while (state.KeepRunning()) {
//         state.PauseTiming();
//         std::vector<Integer> tab(state.range(0));
//         std::generate(tab.begin(), tab.end(), [] {
//             return MyRandom::getInstance().getIntUniform<Integer>(
//                 std::numeric_limits<Integer>::min(),
//                 std::numeric_limits<Integer>::max());
//         });
//         state.ResumeTiming();
//         LSDRadixSort<Integer>::sort(tab.begin(), tab.end());
//     }
// }

// static void BM_MSD_radix_sort(benchmark::State& state) {
//     while (state.KeepRunning()) {
//         state.PauseTiming();
//         std::vector<Integer> tab(state.range(0));
//         std::generate(tab.begin(), tab.end(), [] {
//             return MyRandom::getInstance().getIntUniform<Integer>(
//                 std::numeric_limits<Integer>::min(),
//                 std::numeric_limits<Integer>::max());
//         });
//         state.ResumeTiming();
//         MSDRadixSort<Integer>::sort(tab.begin(), tab.end());
//     }
// }

// static void BM_counting_LSD_radix_sort(benchmark::State& state) {
//     while (state.KeepRunning()) {
//         state.PauseTiming();
//         std::vector<Integer> tab(state.range(0));
//         std::generate(tab.begin(), tab.end(), [] {
//             return MyRandom::getInstance().getIntUniform<Integer>(
//                 std::numeric_limits<Integer>::min(),
//                 std::numeric_limits<Integer>::max());
//         });
//         state.ResumeTiming();
//         CountingLSDRadixSort<Integer>::sort(tab.begin(), tab.end());
//     }
// }

// static void BM_counting_MSD_radix_sort(benchmark::State& state) {
//     while (state.KeepRunning()) {
//         state.PauseTiming();
//         std::vector<Integer> tab(state.range(0));
//         std::generate(tab.begin(), tab.end(), [] {
//             return MyRandom::getInstance().getIntUniform<Integer>(
//                 std::numeric_limits<Integer>::min(),
//                 std::numeric_limits<Integer>::max());
//         });
//         state.ResumeTiming();
//         CountingMSDRadixSort<Integer>::sort(tab.begin(), tab.end());
//     }
// }

// static void BM_counting_LSD_radix_sort_no_ite(benchmark::State& state) {
//     while (state.KeepRunning()) {
//         state.PauseTiming();
//         std::vector<Integer> tab(state.range(0));
//         std::generate(tab.begin(), tab.end(), [] {
//             return MyRandom::getInstance().getIntUniform<Integer>(
//                 std::numeric_limits<Integer>::min(),
//                 std::numeric_limits<Integer>::max());
//         });
//         state.ResumeTiming();
//         CountingLSDRadixSort<Integer>::sort(tab);
//     }
// }

// static void BM_bellmanford_v1(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_v1(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }

// static void BM_bellmanford_v2(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_v2(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }

// static void BM_bellmanford_YenFirst(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_YenFirst(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }

// static void BM_bellmanford_YenFirst_doublevector(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_YenFirst_doublevector(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }

// static void BM_bellmanford_YenFirst_partition(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_YenFirst_partition(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }

// static void BM_bellmanford_YenSecond_partition(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_YenSecond_partition(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }

// static void BM_bellmanford_v3(benchmark::State& state) {
//     const std::array<std::string, 3> networks{{"germany50", "zib54", "ta2"}};
//     auto[graph, nbServers, nbSwitches] = DiGraph::loadFromFile("./instances/" + networks[state.range(0)] + "_topo.txt");
//     ShortestPathBellmanFord<DiGraph> sh_path(graph);
//     while (state.KeepRunning()) {
//         Graph::Path path = sh_path.getShortestPath_v3(
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1),
//             MyRandom::getInstance().getIntUniform<Integer>(
//                 0, graph.getOrder() - 1));
//     }
// }
// BENCHMARK(BM_std_sort)->RangeMultiplier(10)->Range(1000000, 100000000)->Unit(benchmark::kMillisecond);

// BENCHMARK(BM_LSD_radix_sort)->RangeMultiplier(10)->Range(1000000, 100000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_counting_LSD_radix_sort)->RangeMultiplier(10)->Range(1000000, 100000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_counting_LSD_radix_sort_no_ite)->RangeMultiplier(10)->Range(1000000, 100000000)->Unit(benchmark::kMillisecond);

// BENCHMARK(BM_MSD_radix_sort)->RangeMultiplier(10)->Range(1000000, 100000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_counting_MSD_radix_sort)->RangeMultiplier(10)->Range(1000000, 100000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_bellmanford_v1)->Range(0, 2);
// BENCHMARK(BM_bellmanford_v2)->Range(0, 2);
// BENCHMARK(BM_bellmanford_v3)->Range(0, 2);
// BENCHMARK(BM_bellmanford_YenFirst)->Range(0, 2);
// BENCHMARK(BM_bellmanford_YenFirst_doublevector)->Range(0, 2);
// BENCHMARK(BM_bellmanford_YenFirst_partition)->Range(0, 2);
// BENCHMARK(BM_bellmanford_YenSecond_partition)->Range(0, 2);
// BENCHMARK(BM_bellmanford_YenSecond_partition)->Range(0, 2);

BENCHMARK_MAIN();
