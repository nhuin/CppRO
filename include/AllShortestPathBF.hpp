#ifndef ALLSHORTESTPATHBF_HPP
#define ALLSHORTESTPATHBF_HPP

#include <algorithm>

#include "Graph.hpp"
#include "Matrix.hpp"
#include "utility.hpp"
#include "gsl/gsl"

template <typename G>
class AllShortestPathBellmanFord {
  public:
    explicit AllShortestPathBellmanFord(G& _graph)
        : m_graph(_graph)
        , m_distance([&]() {
            Matrix<double> distance(m_graph.getOrder(), m_graph.getOrder(),
                std::numeric_limits<double>::max());
            for (Graph::Node u = 0; u < m_graph.getOrder(); ++u) {
                distance(u, u) = 0.0;
            }
            return distance;
        }())
        , m_parent([&]() {
            Matrix<Graph::Node> parent(m_graph.getOrder(), m_graph.getOrder(),
                -1);
            for (Graph::Node u = 0; u < m_graph.getOrder(); ++u) {
                parent(u, u) = u;
            }
            return parent;
        }()) {}

    AllShortestPathBellmanFord(const AllShortestPathBellmanFord&) = default;
    AllShortestPathBellmanFord&
    operator=(const AllShortestPathBellmanFord&) = default;
    AllShortestPathBellmanFord(AllShortestPathBellmanFord&&) noexcept = default;
    AllShortestPathBellmanFord&
    operator=(AllShortestPathBellmanFord&&) noexcept = default;
    ~AllShortestPathBellmanFord() = default;

    void getAllShortestPaths() {
        const auto edges = m_graph.getEdges();
        for (Graph::Node s = 0; s < m_graph.getOrder(); ++s) {
            for (int i = 0; i < m_graph.getOrder(); ++i) {
                for (const auto& edge : edges) {
                    const double dist =
                        m_distance(s, edge.first) + m_graph.getEdgeWeight(edge);
                    if (dist < m_distance(s, edge.second)) {
                        m_distance(s, edge.second) = dist;
                        m_parent(s, edge.second) = edge.first;
                    }
                }
            }
        }
    }

    const Matrix<double>& getDistance() const { return m_distance; }

  private:
    G& m_graph;
    Matrix<double> m_distance;
    Matrix<Graph::Node> m_parent;
};

#endif