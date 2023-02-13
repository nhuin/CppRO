#include <CppRO/ConstrainedShortestPath.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <catch2/catch.hpp>
#include <numeric>

SCENARIO("ILP model returns optimal solution") {
    GIVEN("A network") {
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
            boost::property<boost::edge_index_t, std::size_t>,
            boost::no_property, boost::vecS>;
        const auto NB_VERTICES = 6;

        std::vector<std::size_t> edge_index{0, 1, 2, 3, 4, 5, 6};
        DiGraph testGraph(
            edgeList.begin(), edgeList.end(), edge_index.begin(), NB_VERTICES);

        IloEnvWrapper env;
        CppRO::CompactConstrainedShortestPathModel cspModel(
            env, testGraph, get(boost::edge_index, testGraph));
        for (const auto& v : cspModel.getFlowVars()) {
            std::cout << v << '\n';
        }
        cspModel.setDelays(DELAYS);

        IloExpr objExpression(env);
        std::vector<double> edgeWeights{10.0, 10.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        for (const auto ed : boost::make_iterator_range(edges(testGraph))) {
            const auto id = get(boost::edge_index, testGraph, ed);
            objExpression += edgeWeights[id] * cspModel.getFlowVars()[id];
        }
        IloObjective obj(env);
        obj.setExpr(objExpression);

        const IloModel model(env);
        model.add(cspModel.getModel());
        model.add(obj);
        IloCplex solver(model);

        WHEN("We search for the CSP between A and C with at most 300ms") {
            cspModel.setNodePair(A, C);
            cspModel.setMaxDelay(300);
            THEN("We find the path (A, D, E, C) with a cost of 3") {
                solver.exportModel("csp_ac_300.lp");
                auto res = solver.solve();
                REQUIRE(res == IloTrue);
                REQUIRE(solver.getValue(cspModel.getFlowVars()[2]) == IloTrue);
                REQUIRE(solver.getValue(cspModel.getFlowVars()[3]) == IloTrue);
                REQUIRE(solver.getValue(cspModel.getFlowVars()[4]) == IloTrue);
                const auto pathSize =
                    std::accumulate(cspModel.getFlowVars().begin(),
                        cspModel.getFlowVars().end(), 0.0,
                        [&](const double _acc, const IloNumVar& _var) {
                            return solver.getValue(_var) + _acc;
                        });
                REQUIRE(epsilon_equal<double>()(pathSize, 3.0));
                REQUIRE(epsilon_equal<double>()(solver.getObjValue(), 3.0));
            }
        }
        WHEN("We search for the CSP between A and C with at most 100ms") {
            cspModel.setNodePair(A, C);
            cspModel.setMaxDelay(100);
            THEN("We find the path (A, B, C) with a cost of 20") {
                solver.exportModel("csp_ac_100.lp");
                auto res = solver.solve();
                REQUIRE(res == IloTrue);
                REQUIRE(solver.getValue(cspModel.getFlowVars()[0]) == IloTrue);
                REQUIRE(solver.getValue(cspModel.getFlowVars()[1]) == IloTrue);
                const auto pathSize =
                    std::accumulate(cspModel.getFlowVars().begin(),
                        cspModel.getFlowVars().end(), 0.0,
                        [&](const double _acc, const IloNumVar& _var) {
                            return solver.getValue(_var) + _acc;
                        });
                REQUIRE(epsilon_equal<double>()(pathSize, 2.0));
                REQUIRE(epsilon_equal<double>()(solver.getObjValue(), 20.0));
            }
        }
        WHEN("We search for the CSP between A and C with at most 99ms") {
            cspModel.setNodePair(A, C);
            cspModel.setMaxDelay(99);
            solver.exportModel("csp_ac_99.lp");
            auto res = solver.solve();
            THEN("We find no path") { REQUIRE(res == IloFalse); }
        }
        WHEN("We search for the CSP between A and C below 10ms") {
            cspModel.setNodePair(A, C);
            cspModel.setMaxDelay(99);
            solver.exportModel("csp_ac_99.lp");
            auto res = solver.solve();
            THEN("We find no path") { REQUIRE(res == IloFalse); }
        }
    }
}
