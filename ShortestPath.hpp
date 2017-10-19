#ifndef SHORTESTPATH_HPP
#define SHORTESTPATH_HPP

#include <cassert>
#include <functional>

#include "BinaryHeap.hpp"
#include "Graph.hpp"
#include "utility.hpp"

template <typename G>
class ShortestPath {
  public:
    explicit ShortestPath(const G& _graph)
        : m_graph(&_graph)
        , m_distance(_graph.getOrder(), std::numeric_limits<double>::max())
        , m_parent(_graph.getOrder(), -1)
        , m_color(_graph.getOrder(), 0)
        , m_handles(_graph.getOrder())
        , m_heap(_graph.getOrder(),
              [=](const Graph::Node& __u, const Graph::Node& __v) {
                  return m_distance[__u] < m_distance[__v];
              }) {}
    // Copy
    ShortestPath(const ShortestPath& _other)
        : ShortestPath(*_other.m_graph) {}

    bool compareDistance(const Graph::Node& __u, const Graph::Node& __v) {
        return m_distance[__u] < m_distance[__v];
    };

    ShortestPath& operator=(const ShortestPath& _other) {
        if (this != &_other) {
            m_graph = _other.m_graph;
            m_distance = std::vector<double>(m_graph->getOrder(),
                std::numeric_limits<double>::max());
            m_parent = std::vector<int>(m_graph->getOrder(), -1);
            m_color = std::vector<char>(m_graph->getOrder(), 0);
            m_handles = std::vector<typename BinaryHeap<
                int, std::function<bool(const Graph::Node&, const Graph::Node&)>>::
                    Handle*>(m_graph->getOrder());
            m_heap = BinaryHeap<
                int, std::function<bool(const Graph::Node&, const Graph::Node&)>>(
                m_graph->getOrder(),
                [=](const Graph::Node& __u, const Graph::Node& __v) {
                    return m_distance[__u] < m_distance[__v];
                });
        }
        return *this;
    }

    // Move
    ShortestPath(ShortestPath&& _other) noexcept
        : m_graph(std::move(_other.m_graph))
        , m_distance(std::move(_other.m_distance))
        , m_parent(std::move(_other.m_parent))
        , m_color(std::move(_other.m_color))
        , m_handles(m_graph->getOrder())
        , m_heap(m_graph->getOrder(),
              [=](const Graph::Node& __u, const Graph::Node& __v) {
                  return m_distance[__u] < m_distance[__v];
              }) {}

    ShortestPath& operator=(ShortestPath&& _other) noexcept {
        if (this != &_other) {
            m_graph = std::move(_other.m_graph);
            m_distance = std::move(_other.m_distance);
            m_parent = std::move(_other.m_parent);
            m_color = std::move(_other.m_color);
            m_handles = std::vector<typename BinaryHeap<
                int, std::function<bool(const Graph::Node&, const Graph::Node&)>>::
                    Handle*>(m_graph->getOrder());
            m_heap = BinaryHeap<
                int, std::function<bool(const Graph::Node&, const Graph::Node&)>>(
                m_graph->getOrder(),
                [=](const Graph::Node& __u, const Graph::Node& __v) {
                    return m_distance[__u] < m_distance[__v];
                });
        }
        return *this;
    }

    ~ShortestPath() = default;

    double getDistance(const Graph::Node& _u) const {
        return m_distance[_u];
    }

    void clear() {
        std::fill(m_parent.begin(), m_parent.end(), -1);
        std::fill(m_distance.begin(), m_distance.end(),
            std::numeric_limits<double>::max());
        std::fill(m_color.begin(), m_color.end(), 0);
        m_heap.clear();
    }

    Graph::Path getShortestPath(const int _s, const int _t) {
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        m_handles[_s] = m_heap.push(_s);
        m_color[_s] = 1;

        Graph::Path path;
        while (_t != m_heap.top()) {
            if (m_heap.empty()) {
                return Graph::Path();
            }
            const auto u = m_heap.top();
            m_heap.pop();
            m_color[u] = 2;

            for (const auto& v : m_graph->getNeighbors(u)) {
                if (m_color[v] != 2) {
                    auto distT = m_distance[u] + m_graph->getEdgeWeight(u, v);
                    if (distT < m_distance[v]) {
                        m_distance[v] = distT;
                        if (m_color[v] == 1) {
                            m_heap.decrease(m_handles[v]);
                        } else {
                            m_handles[v] = m_heap.push(v);
                            m_color[v] = 1;
                        }
                        m_parent[v] = u;
                    }
                }
            }
        }

        /* Path is found, need to build it */
        Graph::Node node = _t;
        while (node != _s) {
            path.push_back(node);
            node = m_parent[node];
        }
        path.push_back(node);
        std::reverse(path.begin(), path.end());
        return path;
    }

    Graph::Path getShortestPathNbArcs(const Graph::Node& _s, const Graph::Node& _t) {
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        m_handles[_s] = m_heap.push(_s);
        m_color[_s] = 1;

        Graph::Path path;
        while (_t != m_heap.top()) {
            if (m_heap.empty()) {
                return path;
            }
            const auto u = m_heap.top();
            m_heap.pop();
            m_color[u] = 2;

            for (const auto& v : m_graph->getNeighbors(u)) {
                if (m_color[v] != 2) {
                    auto distT = m_distance[u] + 1;
                    if (distT < m_distance[v]) {
                        m_distance[v] = distT;
                        if (m_color[v] == 1) {
                            m_heap.decrease(m_handles[v]);
                        } else {
                            m_handles[v] = m_heap.push(v);
                            m_color[v] = 1;
                        }
                        m_parent[v] = u;
                    }
                }
            }
        }

        /* Path is found, need to build it */
        Graph::Node node = _t;
        while (node != _s) {
            path.push_back(node);
            node = m_parent[node];
        }
        path.push_back(node);
        std::reverse(path.begin(), path.end());
        return path;
    }

    Graph::Path
    getShortestPath(const Graph::Node& _s, const Graph::Node& _t,
        const std::function<bool(const Graph::Node&, const Graph::Node&)>& _isNeighbor,
        const std::function<double(const Graph::Node&, const Graph::Node&)>& _getWeight) {
        //   //std::cout<< "getShortestPath" << Graph::Edge(_s, _t) << '\n';
        m_parent[_s] = _s;
        m_distance[_s] = 0;
        m_handles[_s] = m_heap.push(_s);
        m_color[_s] = 1;

        Graph::Path path;
        while (_t != m_heap.top()) {
            if (m_heap.empty()) {
                return Graph::Path();
            }
            const auto u = m_heap.top();
            m_heap.pop();
            m_color[u] = 2;

            for (auto& v : m_graph->getNeighbors(u)) {
                if (m_color[v] != 2 && _isNeighbor(u, v)) {
                    auto distT = m_distance[u] + _getWeight(u, v);
                    if (distT < m_distance[v]) {
                        m_distance[v] = distT;
                        if (m_color[v] == 1) {
                            m_heap.decrease(m_handles[v]);
                        } else {
                            m_handles[v] = m_heap.push(v);
                            m_color[v] = 1;
                        }
                        m_parent[v] = u;
                    }
                }
            }
        }
        /* Path is found, need to build it */
        Graph::Node node = _t;
        while (node != _s) {
            path.push_back(node);
            node = m_parent[node];
        }
        path.push_back(node);
        std::reverse(path.begin(), path.end());
        return path;
    }

    Graph::Path
    getShortestPathNbArcs(const Graph::Node& _s, const Graph::Node& _t,
        const std::function<bool(const Graph::Node&, const Graph::Node&)>& _isNeighbor) {
        m_heap.clear();

        m_parent[_s] = _s;
        m_distance[_s] = 0;
        m_handles[_s] = m_heap.push(_s);
        m_color[_s] = 1;

        Graph::Path path;
        while (_t != m_heap.top()) {
            if (m_heap.empty()) {
                return path;
            }
            const auto u = m_heap.top();
            m_heap.pop();
            m_color[u] = 2;
            for (auto& v : m_graph->getNeighbors(u)) {
                if (m_color[v] != 2 && _isNeighbor(u, v)) {
                    auto distT = m_distance[u] + 1;
                    if (distT < m_distance[v]) {
                        m_distance[v] = distT;
                        if (m_color[v] == 1) {
                            m_heap.decrease(m_handles[v]);
                        } else {
                            m_handles[v] = m_heap.push(v);
                            m_color[v] = 1;
                        }
                        m_parent[v] = u;
                    }
                }
            }
        }

        /* Path is found, need to build it */
        Graph::Node node = _t;
        while (node != _s) {
            path.push_back(node);
            node = m_parent[node];
        }
        path.push_back(node);
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<Graph::Path> getKShortestPath(const int _u, const int _v,
        const int _k) {
        std::vector<Graph::Path> paths;
        if (_k == 0) {
            return paths;
        }

        // Determine the shortest path from the source to the sink.
        paths.push_back(this->getShortestPathNbArcs(_u, _v));
        // Initialize the heap to store the potential kth shortest path.
        std::list<Graph::Path> stack;
        for (int k = 1; k < _k; ++k) {
            auto& lastPath = paths[k - 1];
            auto ite = lastPath.begin();
            for (int i = 0; i < static_cast<int>(lastPath.size() - 1); ++i, ++ite) {
                // Spur node is retrieved from the previous k-shortest path, k − 1.
                int spurNode = *ite;
                // The sequence of nodes from the source to the spur node of the
                // previous k-shortest path.
                Graph::Path rootPath{lastPath.begin(), std::next(ite)};
                assert(rootPath.back() == spurNode);

                G graphCopy = m_graph;

                for (int j = 0; j < k; ++j) {
                    auto p = paths[j];
                    bool same = true;
                    auto ite2 = p.begin();
                    for (auto ite1 = rootPath.begin(); ite1 != rootPath.end();
                         ++ite1, ++ite2) {
                        if (*ite1 != *ite2) {
                            same = false;
                            break;
                        }
                    }
                    // std::cout<< p << " <=> " << rootPath << ": " << same << '\n';
                    if (same) {
// Remove the links that are part of the previous shortest paths which share the
// same root path.
                        graphCopy.removeEdge(*std::prev(ite2), *ite2);
                    }
                }

                for (const auto& u : rootPath) {
                    if (u != spurNode) {
                        graphCopy.isolate(u);
                        // std::cout << "removeNode(" << u << ")\n";
                        // for(const auto& v : graphCopy.getNeighbors(u) ) {
                        // 	std::cout<< "removeEdge" << Graph::Edge(u, v) << '\n';
                        // 	std::cout<< "removeEdge" << Graph::Edge(v, u) << '\n';

                        // 	graphCopy.removeEdge(u, v);
                        // 	graphCopy.removeEdge(v, u);
                        // }
                    }
                }
                // Calculate the spur path from the spur node to the sink.
                Graph::Path spurPath =
                    ShortestPath<G>(graphCopy).getShortestPathNbArcs(spurNode, _v);
                if (!spurPath.empty()) {
                    assert(spurPath.front() == spurNode);
                    rootPath.insert(rootPath.end(), std::next(spurPath.begin()),
                        spurPath.end());

                    // Search first longer path
                    auto iteStack = stack.begin();
                    for (; iteStack != stack.end(); ++iteStack) {
                        if (iteStack->size() >= rootPath.size()) {
                            break;
                        }
                    }
                    stack.insert(iteStack, rootPath);
                }
            }
            if (stack.empty()) {
                return paths;
            }

            paths.push_back(stack.front());
            stack.pop_front();
        }
        return paths;
    }

  private:
    G const* m_graph;
    std::vector<double> m_distance;
    std::vector<int> m_parent;
    std::vector<char> m_color;
    std::vector<typename BinaryHeap<
        int,
        std::function<bool(const Graph::Node&, const Graph::Node&)>>::Handle*>
        m_handles;
    BinaryHeap<int, std::function<bool(const Graph::Node&, const Graph::Node&)>>
        m_heap;
};

#endif