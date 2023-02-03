#include <CppRO/ConstrainedShortestPath.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <catch2/catch.hpp>

TEST_CASE("ILP model returns optimal solution") {
    const std::vector<std::size_t> DELAYS{50, 50, 100, 100, 60, 50, 100};
    constexpr auto A = 0;
    constexpr auto B = 1;
    constexpr auto C = 2;
    constexpr auto D = 3;
    constexpr auto E = 4;
    constexpr auto F = 5;

    std::vector<std::pair<std::size_t, std::size_t>> edgeList{
        {A, B}, {B, C}, {A, D}, {D, E}, {E, C}, {E, F}, {F, C}};

    using DiGraph = boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS, boost::no_property,
        boost::property<boost::edge_index_t, std::size_t>, boost::no_property,
        boost::vecS>;
    const auto NB_VERTICES = 6;

    std::vector<std::size_t> edge_index{0, 1, 2, 3, 4, 5, 6};
    DiGraph testGraph(
        edgeList.begin(), edgeList.end(), edge_index.begin(), NB_VERTICES);

    IloEnvWrapper env;
    CppRO::CompactConstrainedShortestPathModel cspModel(env, testGraph);
    REQUIRE(cspModel.getFlowVars().size() == edgeList.size());
    for (const auto& v : cspModel.getFlowVars()) {
        std::cout << v << '\n';
    }
    cspModel.setDelays(DELAYS);

    IloExpr objExpression(env);
    for (const auto ed : boost::make_iterator_range(edges(testGraph))) {
        objExpression +=
            1.0 * cspModel.getFlowVars()[get(boost::edge_index, testGraph, ed)];
    }
    cspModel.setNodePair(A, C);
    cspModel.setMaxDelay(10000);
    IloObjective obj(env);
    obj.setExpr(objExpression);
    const IloModel model(env);
    model.add(cspModel.getModel());
    model.add(obj);
    IloCplex solver(model);
    solver.exportModel("csp.lp");
    auto res = solver.solve();
    REQUIRE(res == IloTrue);
    cspModel.setMaxDelay(10000);
    res = solver.solve();
    REQUIRE(res == IloTrue);
}
