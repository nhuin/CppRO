#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "Graph.hpp"
#include "DiGraph.hpp"
#include "catch.hpp"
#include <algorithm>
#include <array>

TEST_CASE("Test DFS", "[]") {
    DiGraph g(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    BaseDFSVisitor<DiGraph> visitor(g);
    graphTraversal(g, 0, visitor);
    std::array<Graph::Node, 4> realDFS{{0, 1, 2, 3}};
    REQUIRE(std::mismatch(visitor.search.begin(), visitor.search.end(),
                realDFS.begin())
                .first
            == visitor.search.end());
}
