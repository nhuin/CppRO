#ifndef DIGRAPH_H
#define DIGRAPH_H

#include <algorithm>
#include <functional>
#include <sstream>
#include <utility>
#include <vector>

#include "Graph.hpp"
#include "Matrix.hpp"

class DiGraph : public Graph {
public:
  explicit DiGraph(const int _order)
      : Graph(_order), m_matrix(_order, _order, std::make_pair(false, 0)),
        m_neighbors(m_order) {
    assert(_order > 0);
  }

  DiGraph &operator=(const DiGraph &) = default;
  DiGraph(const DiGraph &) = default;
  DiGraph &operator=(DiGraph &&) = default;
  DiGraph(DiGraph &&) = default;
  ~DiGraph() override = default;

  void addEdge(const int u, const int v, const double w = 0.0) {
    assert(u >= 0);
    assert(u < m_matrix.size1());
    assert(v >= 0);
    assert(v < m_matrix.size2());

    if (!m_matrix(u, v).first) {
      ++m_size;
      m_neighbors[u].push_back(v);
    }
    m_matrix(u, v).first = true;
    m_matrix(u, v).second = w;
  }

  using Graph::hasEdge;
  bool hasEdge(const int _u, const int _v) const override {
    return m_matrix(_u, _v).first;
  }

  void removeEdge(const int u, const int v) {
    if (m_matrix(u, v).first) {
      m_matrix(u, v).first = false;
      --m_size;
      m_neighbors[u].erase(
          std::remove(m_neighbors[u].begin(), m_neighbors[u].end(), v),
          m_neighbors[u].end());
    }
  }

  void removeEdge(const Edge &_p) { removeEdge(_p.first, _p.second); }

  double getEdgeWeight(const int u, const int v) const {
    return m_matrix(u, v).second;
  }

  double getEdgeWeight(const Graph::Edge &_edge) const {
    return getEdgeWeight(_edge.first, _edge.second);
  };

  inline void setEdgeWeight(const int _u, const int _v, const double _w) {
    m_matrix(_u, _v).second = _w;
  }

  inline void setEdgeWeight(const Graph::Edge &_e, const double _w) {
    setEdgeWeight(_e.first, _e.second, _w);
  }

  inline void addEdgeWeight(const int _u, const int _v, const double _w) {
    m_matrix(_u, _v).second += _w;
  }

  inline void addEdgeWeight(const Graph::Edge &_e, const double _w) {
    addEdgeWeight(_e.first, _e.second, _w);
  }

  std::vector<int> getTopologicalOrder() const {
    std::vector<int> topo;

    std::vector<int> inDegree(m_order);
    std::vector<bool> inOrder(m_order, false);
    for (int u = 0; u < m_order; ++u) {
      inDegree[u] = getInDegree(u);
    }

    bool changed;
    while (static_cast<int>(topo.size()) < m_order) {
      changed = false;
      for (int u = 0; u < static_cast<int>(inDegree.size()); ++u) {
        if (inDegree[u] == 0 && !inOrder[u]) {
          changed = true;
          topo.push_back(u);
          inOrder[u] = true;
          for (const auto &v : getNeighbors(u)) {
            --inDegree[v];
          }
        }
      }
      if (!changed) {
        return {};
      }
    }
    return topo;
  }

  DiGraph getReversedGraph() const {
    DiGraph reverse(m_order);
    for (Graph::Node u = 0; u < m_order; ++u) {
      for (Graph::Node v = 0; v < m_order; ++v) {
        if (m_matrix(u, v).first) {
          reverse.m_matrix(v, u) = m_matrix(u, v);
          reverse.m_neighbors[v].push_back(u);
          ++reverse.m_size;
        }
      }
    }
    return reverse;
  }

  std::vector<int>
  getNeighborsIf(const int _u, const std::function<bool(int)> &take_if) const {
    std::vector<int> neighbors;
    for (auto &n : m_neighbors[_u]) {
      if (take_if(n)) {
        neighbors.push_back(n);
      }
    }
    return neighbors;
  }

  const std::vector<int> &getNeighbors(const int _u) const override {
    return m_neighbors[_u];
  }

  std::vector<int> getInNeighbors(const int _u) const {
    std::vector<int> inNeighbors;
    for (Graph::Node v = 0; v < m_order; ++v) {
      if (m_matrix(v, _u).first) {
        inNeighbors.push_back(v);
      }
    }
    return inNeighbors;
  }

  // Remove all adjacent edges to _u
  void isolate(const Graph::Node _u) {
    for (Graph::Node v = 0; v < m_order; ++v) {
      if (m_matrix(_u, v).first) {
        m_matrix(_u, v).first = false;
        --m_size;
      }
    }
    m_neighbors[_u].clear();
  }

  std::vector<Edge> getEdges() const override {
    std::vector<Edge> edges;

    for (Graph::Node u = 0; u < m_order; ++u) {
      for (Graph::Node v = 0; v < m_order; ++v) {
        if (m_matrix(u, v).first) {
          edges.emplace_back(u, v);
        }
      }
    }
    return edges;
  }

  int getOutDegree(const int _u) const { return m_neighbors[_u].size(); }

  std::vector<int> getOutDegree() const {
    std::vector<int> outDegrees(m_order);
    for (Graph::Node u = 0; u < m_order; ++u) {
      outDegrees[u] = m_neighbors[u].size();
    }
    return outDegrees;
  }

  int getInDegree(const int _u) const {
    int indegree = 0;
    for (int v = 0; v < m_order; ++v) {
      if (m_matrix(v, _u).first) {
        ++indegree;
      }
    }
    return indegree;
  }

  std::vector<int> getInDegrees() const {
    std::vector<int> inDegrees(m_order);
    for (Graph::Node u = 0; u < m_order; ++u) {
      for (int v = 0; v < m_order; ++v) {
        if (m_matrix(v, u).first) {
          ++inDegrees[u];
        }
      }
    }
    return inDegrees;
  }

  void createDotFile(const std::string &_name = "graph") const {
    const std::string dotFilename = _name + ".dot";
    std::ofstream ofs(dotFilename, std::ofstream::out);
    ofs << "digraph G {\n";
    ofs << "concentrate=true\n";
    for (const auto &edge : getEdges()) {
      ofs << edge.first << "->" << edge.second << "[label=\""
          << getEdgeWeight(edge) << "\"];\n";
    }
    ofs << "}";
  }

  static std::tuple<DiGraph, int, int>
  loadFromFile(const std::string &_filename) {
    std::ifstream ifs(_filename, std::ifstream::in);
    if (!ifs) {
      std::cerr << _filename << " does not exists!\n";
      exit(-1);
    }
    std::string line;
    std::getline(ifs, line, '\n');

    int vertexNumber = 0;
    int nbServers = 0;
    int nbSwitches = 0;
    std::stringstream(line) >> vertexNumber >> nbServers >> nbSwitches;

    DiGraph graph(vertexNumber);

    int i = 0;

    int u, v;
    double w;
    while (ifs.good()) {
      std::getline(ifs, line, '\n');

      std::stringstream(line) >> u >> v >> w;
      graph.addEdge(u, v, w);
      graph.addEdge(v, u, w);
      ++i;
    }
    return std::tuple<DiGraph, int, int>(graph, nbServers, nbSwitches);
  }

  /**
   * Return the set of SCCs using Tarjan's algorithm
   * @TODO: Transform to iterative
   */
  std::vector<std::vector<Graph::Node>> getStronglyConnectedComponent() const {
    std::vector<std::vector<Graph::Node>> SCCs;
    int index = 0;
    std::vector<Node> S;
    std::vector<int> indexes(m_order, -1), lowLink(m_order);
    std::vector<bool> onStack(m_order, false);

    const std::function<void(Node)> strongConnect = [&](const Node v) {
      // Set the depth index for v to the smallest unused index
      indexes[v] = lowLink[v] = index;
      ++index;
      S.push_back(v);
      onStack[v] = true;
      // Consider successors of v
      for (const auto &w : getNeighbors(v)) {
        if (indexes[w] == -1) {
          // Successor w has not yet been visited; recurse on it
          strongConnect(w);
          lowLink[v] = std::min(lowLink[v], lowLink[w]);
        } else if (onStack[w]) {
          // Successor w is in stack S and hence in the current SCC
          // Note: The next line may look odd - but is correct.
          // It says w.index not w.lowlink; that is deliberate and from the
          // original paper
          lowLink[v] = std::min(lowLink[v], indexes[w]);
        }
      }

      // If v is a root node, pop the stack and generate an SCC
      if (lowLink[v] == indexes[v]) {
        SCCs.emplace_back();
        Node w;
        do {
          w = S.back();
          SCCs.back().push_back(w);
          onStack[w] = false;
          S.pop_back();
        } while (w != v);
      }
    };

    for (int v = 0; v < m_order; ++v) {
      if (indexes[v] == -1) {
        strongConnect(v);
      }
    }
    return SCCs;
  }

  std::vector<DiGraph> getStronglyConnectedSubGraph() const {
    const auto SCCs = getStronglyConnectedComponent();
    std::vector<DiGraph> subGraphs;
    subGraphs.reserve(SCCs.size());

    for (const auto &SCC : SCCs) {
      subGraphs.emplace_back(SCC.size());
      int u2 = 0;
      for (const auto &u : SCC) {
        int v2 = 0;
        for (const auto &v : SCC) {
          if (u != v && hasEdge(u, v)) {
            subGraphs.back().addEdge(u2, v2);
          }
          ++v2;
        }
        ++u2;
      }
    }
    return subGraphs;
  }

private:
  Matrix<std::pair<bool, double>> m_matrix;
  std::vector<std::vector<int>> m_neighbors;
};

inline bool operator==(const DiGraph &_g1, const DiGraph &_g2) {
  if (_g1.getOrder() == _g2.getOrder() && _g1.size() == _g2.size()) {
    for (const auto &edge : _g1.getEdges()) {
      if (!_g2.hasEdge(edge)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

#endif