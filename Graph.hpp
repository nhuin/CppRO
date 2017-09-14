#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <utility>

#include <string>

#include "UnionFind.hpp"

class Graph {
  public:
    using Node = int;
    using Path = std::vector<int>;
    using Edge = std::pair<int, int>;

    friend std::ostream& operator<<(std::ostream& _out, const Edge& _edge);
    ;

    // Constructor
    explicit Graph(int _order)
        : m_order(_order)
        , m_size(0) {}

    Graph& operator=(const Graph&) = default;
    Graph(const Graph&) = default;
    Graph& operator=(Graph&&) = default;
    Graph(Graph&&) = default;
    virtual ~Graph() = default;

    virtual const std::vector<int>& getNeighbors(int u) const = 0;
    virtual double getEdgeWeight(int u, int v) const = 0;
    virtual std::vector<Edge> getEdges() const = 0;
    virtual bool hasEdge(int, int) const = 0;

    int getOrder() const { return m_order; }
    int size() const { return m_size; }

    std::vector<int> getNodes() const {
        std::vector<int> nodes(m_order);
        std::iota(nodes.begin(), nodes.end(), 0);
        return nodes;
    }

    std::vector<int> getVertices() const {
        std::vector<int> vertices(m_order);
        for (int i = 0; i < m_order; ++i) {
            vertices[i] = i;
        }
        return vertices;
    }

    /**
        Save the graph as a dot file for graphviz
        */
    void createDotFile(const std::string& _name) const {
        std::string dotFilename = _name + ".dot";
        std::ofstream ofs(dotFilename, std::ofstream::out);
        ofs << "graph G {" << std::endl;
        for (auto edge : getEdges()) {
            ofs << edge.first << "--" << edge.second << ";" << std::endl;
        }
        ofs << "}";
        // ofs.close();
        // std::string sysString = "dot " + dotFilename + " -T"+_type+"  -o " + _name + "."+_type;
        // system(sysString.c_str());
    }

  protected:
    int m_order;
    int m_size;
};

#endif