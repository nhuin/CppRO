#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <utility>

#include <string>
#include <vector>
#include "gsl/gsl"

/**
 * Interface for graph classes
 */
namespace Graph {
    using Node = int;
    using Path = std::vector<Graph::Node>;
    using Edge = std::pair<Graph::Node, Graph::Node>;

/**
* Save the graph as a dot file for graphviz
*/
template<typename G>
void createDotFile(const G& _g, const std::string& _name) {
    std::string dotFilename = _name + ".dot";
    std::ofstream ofs(dotFilename, std::ofstream::out);
    ofs << "graph G {" << std::endl;
    for (const auto& edge : _g.getEdges()) {
        ofs << edge.first << "--" << edge.second << ";" << std::endl;
    }
    ofs << "}";
}

template <typename G>
struct BaseDFSVisitor {
    std::vector<Graph::Node> search{};
    BaseDFSVisitor(const G& _g) {
        search.reserve(_g.getOrder());
    }

    bool onGet(const Graph::Node /*unused*/) const {
        return true;
    }

    bool onInsert(const Graph::Node _u) {
        search.push_back(_u);
        return true;
    }

    bool alreadyPushed(const Graph::Node /*unused*/) const {
        return true;
    }
};

template<typename G>
class DFSContainer
{
    DFSContainer(const G& _g) 
    : inStack(_g.getOrder())
    , stack()
    {
        stack.reserve(_g.getOrder());
    }

    void insert(const Graph::Node _u) {
        if(!inStack[_u]) {
            stack.push_back(_u);
            inStack[_u] = 0;
        }
    }

    Graph::Node get() {
        Graph::Node retval = stack.back();
        stack.pop_back();
        return retval;
    }

    bool empty() {
        return stack.empty();
    }

private:
    std::vector<uint8_t> inStack;
    std::vector<Graph::Node> stack;
};

template<typename G>
struct DefaultNeighborView
{
    DefaultNeighborView(G& _graph, const Graph::Node _u)
    : neighbors(_graph.getNeighbors(_u))
    {}

    auto begin() {
        return neighbors.begin();
    }

    auto end() {
        return neighbors.end();
    }

    auto cbegin() {
        return neighbors.cbegin();
    }

    auto cend() {
        return neighbors.cend();
    }

    typename G::NeighborList neighbors;
};

template <typename G, typename Container, typename Visitor, typename NeighborView = DefaultNeighborView<G>>
void graphTraversal(const G& _graph,
    const Graph::Node _startNode,
    Visitor& _visitor,
    Container& _container) {

    _container.insert(_startNode);
    if (!_visitor.onInsert(_startNode)) {
        return;
    }
    while (!_container.empty()) {
        // Get node
        const auto v = _container.get();
        if (!_visitor.onGet(v)) {
            return;
        }

        // Add neighbors of v
        for (const auto& w : NeighborView(_graph, v)) {
            _container.insert(w);
            if (!_visitor.onInsert(_startNode)) {
                return;
            }
        }
    }
}
} // namespace Graph


#endif
