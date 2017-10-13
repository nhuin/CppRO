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

// #include "UnionFind.hpp"

/**
 * Interface for graph classes
 */
class Graph {
public:
  using Node = int;
  using Path = std::vector<int>;
  using Edge = std::pair<int, int>;

  // Constructor
  explicit Graph(const int _order) : m_order(_order) {}

  Graph &operator=(const Graph &) = default;
  Graph(const Graph &) = default;
  Graph &operator=(Graph &&) noexcept = default;
  Graph(Graph &&) noexcept = default;
  virtual ~Graph() = default;

  int getOrder() const { return m_order; }

  int size() const { return m_size; }

  /*
   * Return the set of neighbors for the node u
   */
  virtual const std::vector<int> &getNeighbors(Graph::Node _u) const = 0;

  virtual std::vector<Edge> getEdges() const = 0;
  virtual bool hasEdge(Graph::Node _u, Graph::Node _v) const = 0;
  bool hasEdge(const Graph::Edge &_edge) const {
    return hasEdge(_edge.first, _edge.second);
  }

  /**
   * Save the graph as a dot file for graphviz
   */
  void createDotFile(const std::string &_name) const {
    std::string dotFilename = _name + ".dot";
    std::ofstream ofs(dotFilename, std::ofstream::out);
    ofs << "graph G {" << std::endl;
    for (const auto &edge : getEdges()) {
      ofs << edge.first << "--" << edge.second << ";" << std::endl;
    }
    ofs << "}";
  }

protected:
  int m_order;
  int m_size = {0};
};

#endif