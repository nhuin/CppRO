#ifndef SHORTESTPATHBF_HPP
#define SHORTESTPATHBF_HPP

#include "Graph.hpp"
#include "utility.hpp"
#include <iterator>
#include <type_traits>
#include <deque>

template <typename G>
class ShortestPathBellmanFord {
  public:
    struct NegativeCycleException {
        std::string message;
    };

    explicit ShortestPathBellmanFord(const G& _graph)
        : m_graph(&_graph)
        , m_distance(m_graph->getOrder(), std::numeric_limits<int>::max())
        , m_parent(m_graph->getOrder(), -1) 
        , m_inQueue(m_graph->getOrder(), false) 
        , m_count(m_graph->getOrder(), 0) 

        {}

    ShortestPathBellmanFord(const ShortestPathBellmanFord& _other) = default;
    ShortestPathBellmanFord(ShortestPathBellmanFord&& _other) noexcept = default;
    ShortestPathBellmanFord& operator=(const ShortestPathBellmanFord& _other) = default;
    ShortestPathBellmanFord& operator=(ShortestPathBellmanFord&& _other) noexcept = default;
    ~ShortestPathBellmanFord() = default;

    void clear() {
        std::fill(m_parent.begin(), m_parent.end(), -1);
        std::fill(m_distance.begin(), m_distance.end(), std::numeric_limits<double>::max());
        std::fill(m_inQueue.begin(), m_inQueue.end(), false);
        std::fill(m_count.begin(), m_count.end(), 0);
    }

    inline Graph::Path getShortestPath(const int _s, const int _t) {
        return getShortestPath_v2(_s, _t);
    }

    double getDistance(const Graph::Node _u) const {
        return m_distance[_u];
    }

    Graph::Path getShortestPath_v1(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        const auto edges = m_graph->getEdges();
        for (int i = 0; i < m_graph->getOrder() - 1; ++i) {
            for (const auto& edge : edges) {
                const double dist = m_distance[edge.first] + m_graph->getEdgeWeight(edge);
                if (dist < m_distance[edge.second]) {
                    m_distance[edge.second] = dist;
                    m_parent[edge.second] = edge.first;
                }
            }
        }

        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    Graph::Path getShortestPath_v2(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        const auto edges = m_graph->getEdges();
        bool anyChanges = true;
        for (int i = 0; i < m_graph->getOrder() - 1 && anyChanges == true; ++i) {
            anyChanges = false;
            for (const auto& edge : edges) {
                const double dist = m_distance[edge.first] + m_graph->getEdgeWeight(edge);
                if (dist < m_distance[edge.second]) {
                    anyChanges = true;
                    m_distance[edge.second] = dist;
                    m_parent[edge.second] = edge.first;
                }
            }
        }

        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    Graph::Path getShortestPath_v3(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        const auto edges = m_graph->getEdges();

        std::deque<Graph::Node> queue {_s};

        m_inQueue[_s] = true;

        bool overRelaxed = false;
        while(!queue.empty() && !overRelaxed) {
            const auto u = queue.front();
            queue.pop_front();
            m_inQueue[u] = false;
            // Don't try to relax if parent is in queue
            if(m_parent[u] == -1 || !m_inQueue[m_parent[u]]) {
                for (const auto v : m_graph->getNeighbors(u)) {
                    const double dist = m_distance[u] + m_graph->getEdgeWeight(u, v);
                    if (dist < m_distance[v]) {
                        if(++m_count[v] == m_graph->getOrder()) {
                            overRelaxed = true;
                        }
                        m_distance[v] = dist;
                        m_parent[v] = u;
                        if(!m_inQueue[v]) {
                            queue.push_back(v);
                            m_inQueue[v] = true;
                        }
                    }
                }
            }
        }

        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    Graph::Path getShortestPath_YenFirst(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        const auto edges = m_graph->getEdges();
        bool anyChanges = true;
        std::vector<int> nbOutEdgeChange(m_graph->getOrder(), 0.0);
        std::vector<char> edgeToRelax(m_graph->size(), true);

        for (int i = 0; i < m_graph->getOrder() - 1 && anyChanges == true; ++i) {
            anyChanges = false;
            int e = 0;
            for (const auto& edge : edges) {
                if (edgeToRelax[e]) {
                    const double dist = m_distance[edge.first] + m_graph->getEdgeWeight(edge);
                    if (dist < m_distance[edge.second]) {
                        anyChanges = true;
                        m_distance[edge.second] = dist;
                        m_parent[edge.second] = edge.first;
                        nbOutEdgeChange[edge.second] = 0;
                    } else {
                        nbOutEdgeChange[edge.second]++;
                    }
                }
                e++;
            }

            e = 0;
            for (const auto& edge : edges) {
                edgeToRelax[e] = nbOutEdgeChange[edge.first] < m_graph->getOutDegree(edge.first);
                ++e;
            }
        }
        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    Graph::Path getShortestPath_YenFirst_doublevector(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        auto edges = m_graph->getEdges();
        const auto allEdges = m_graph->getEdges();
        bool anyChanges = true;
        std::vector<int> nbOutEdgeChange(m_graph->getOrder(), 0.0);

        for (int i = 0; i < m_graph->getOrder() - 1 && anyChanges == true; ++i) {
            anyChanges = false;
            // std::fill(nbOutEdgeChange.begin(), nbOutEdgeChange.end(), 0.0);
            for (const auto& edge : edges) {
                const double dist = m_distance[edge.first] + m_graph->getEdgeWeight(edge);
                if (dist < m_distance[edge.second]) {
                    anyChanges = true;
                    m_distance[edge.second] = dist;
                    m_parent[edge.second] = edge.first;
                    nbOutEdgeChange[edge.second] = 0;
                } else {
                    nbOutEdgeChange[edge.second]++;
                }
            }

            edges.clear();
            std::copy_if(allEdges.begin(), allEdges.end(), std::back_inserter(edges),
                [&](const Graph::Edge& __edge) {
                    return nbOutEdgeChange[__edge.first] < m_graph->getOutDegree(__edge.first);
                });
        }
        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    Graph::Path getShortestPath_YenFirst_partition(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        auto edges = m_graph->getEdges();
        bool anyChanges = true;
        std::vector<int> nbOutEdgeChange(m_graph->getOrder(), 0.0);
        auto iteEnd = edges.end();

        for (int i = 0; i < m_graph->getOrder() - 1 && anyChanges == true; ++i) {
            anyChanges = false;
            for (auto ite = edges.begin(); ite != iteEnd; ++ite) {
                const double dist = m_distance[(*ite).first] + m_graph->getEdgeWeight(*ite);
                nbOutEdgeChange[(*ite).second]++;
                if (dist < m_distance[(*ite).second]) {
                    anyChanges = true;
                    m_distance[(*ite).second] = dist;
                    m_parent[(*ite).second] = (*ite).first;
                    nbOutEdgeChange[(*ite).second] = 0;
                }
            }
            iteEnd = std::partition(edges.begin(), edges.end(),
                [&](const Graph::Edge& __edge) {
                    return nbOutEdgeChange[__edge.first] < m_graph->getOutDegree(__edge.first);
                });
        }
        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    Graph::Path getShortestPath_YenSecond_partition(const int _s, const int _t) {
        clear();
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        auto edges = m_graph->getEdges();
        const auto iteSecondBegin = std::partition(edges.begin(), edges.end(), 
            [&](const Graph::Edge& _edge) {
                return _edge.first < _edge.second;
        });
        auto iteFirstEnd = iteSecondBegin;
        auto iteSecondEnd = edges.end();

        std::vector<int> nbOutEdgeChange(m_graph->getOrder(), 0.0);
        bool anyChanges = true;
        for (int i = 0; i < m_graph->getOrder() - 1 && anyChanges == true; ++i) {
            anyChanges = false;
            for (auto ite = edges.begin(); ite != iteFirstEnd; ++ite) {
                const double dist = m_distance[(*ite).first] + m_graph->getEdgeWeight(*ite);
                nbOutEdgeChange[(*ite).second]++;
                if (dist < m_distance[(*ite).second]) {
                    anyChanges = true;
                    m_distance[(*ite).second] = dist;
                    m_parent[(*ite).second] = (*ite).first;
                    nbOutEdgeChange[(*ite).second] = 0;
                }
            }
            for(auto ite = iteSecondBegin; ite != iteSecondEnd; ++ite) {
                const double dist = m_distance[(*ite).first] + m_graph->getEdgeWeight(*ite);
                nbOutEdgeChange[(*ite).second]++;
                if (dist < m_distance[(*ite).second]) {
                    anyChanges = true;
                    m_distance[(*ite).second] = dist;
                    m_parent[(*ite).second] = (*ite).first;
                    nbOutEdgeChange[(*ite).second] = 0;
                }
            }
            iteFirstEnd = std::partition(edges.begin(), iteSecondBegin,
                [&](const Graph::Edge& __edge) {
                    return nbOutEdgeChange[__edge.first] < m_graph->getOutDegree(__edge.first);
                }
            );
            iteSecondEnd = std::partition(iteSecondBegin, edges.end(),
                [&](const Graph::Edge& __edge) {
                    return nbOutEdgeChange[__edge.first] < m_graph->getOutDegree(__edge.first);
                }
            );
        }

        for (const auto& edge : m_graph->getEdges()) {
            if (m_distance[edge.first] + m_graph->getEdgeWeight(edge) < m_distance[edge.second]) {
                throw NegativeCycleException{toString(edge)};
            }
        }

        Graph::Path path;
        if (m_parent[_t] != -1) {
            Graph::Node node = _t;
            while (node != _s) {
                path.push_back(node);
                node = m_parent[node];
            }
            path.push_back(node);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

  private:
    G const* m_graph;
    std::vector<double> m_distance;
    std::vector<int> m_parent;
    std::vector<char> m_inQueue;
    std::vector<int> m_count;
};

#endif