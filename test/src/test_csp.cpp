#include <CppRO/ConstrainedShortestPath.hpp>
#include <boost/graph/adjacency_list.hpp>
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

    DiGraph testGraph(edgeList.begin(), edgeList.end(), NB_VERTICES);
    auto delayMap = boost::iterator_property_map(
        DELAYS.begin(), get(boost::edge_index, testGraph));

    IloEnv env;
    CppRO::CompactConstrainedShortestPathModel cspModel(env, testGraph);
    cspModel.setDelays(DELAYS);

    IloExpr objExpression(env);
    for (const auto ed : boost::make_iterator_range(edges(testGraph))) {
        objExpression +=
            1.0 * cspModel.getFlowVars()[get(boost::edge_index, testGraph, ed)];
    }
    IloObjective obj(env);
    obj.setExpr(objExpression);
    const IloModel model;
    model.add(cspModel.getModel());
    model.add(obj);
    IloCplex solver(model);
    auto res = solver.solve();
    REQUIRE(res == IloTrue);
}
