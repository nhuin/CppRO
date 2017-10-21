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

/**
 * Interface for graph classes
 */
class Graph {
  public:
    using Node = std::size_t;
    using Path = std::vector<Graph::Node>;
    using Edge = std::pair<Graph::Node, Graph::Node>;

    // Constructor
    explicit Graph(const std::size_t _order)
        : m_order(_order) {}

    Graph& operator=(const Graph&) = default;
    Graph(const Graph&) = default;
    Graph& operator=(Graph&&) noexcept = default;
    Graph(Graph&&) noexcept = default;
    virtual ~Graph() = default;

    std::size_t getOrder() const {
        return m_order;
    }

    std::size_t size() const {
        return m_size;
    }

    /*
    * Return the set of neighbors for the node u
    */
    virtual const std::vector<Graph::Node>& getNeighbors(Graph::Node _u) const = 0;

    virtual std::vector<Graph::Edge> getEdges() const = 0;
    virtual bool hasEdge(Graph::Node _u, Graph::Node _v) const = 0;
    bool hasEdge(const Graph::Edge& _edge) const {
        return hasEdge(_edge.first, _edge.second);
    }

    /**
   * Save the graph as a dot file for graphviz
   */
    void createDotFile(const std::string& _name) const {
        std::string dotFilename = _name + ".dot";
        std::ofstream ofs(dotFilename, std::ofstream::out);
        ofs << "graph G {" << std::endl;
        for (const auto& edge : getEdges()) {
            ofs << edge.first << "--" << edge.second << ";" << std::endl;
        }
        ofs << "}";
    }

  protected:
    std::size_t m_order;
    std::size_t m_size = 0;
};

#endif
