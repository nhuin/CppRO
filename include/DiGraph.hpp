#ifndef DIGRAPH_H
#define DIGRAPH_H

#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <utility>
#include <vector>

#include "Graph.hpp"
#include "utility.hpp"
#include "Matrix.hpp"

template<typename DG>
std::vector<Graph::Node> getTopologicalOrder(const DG& _g);
template<typename DG>
std::vector<int> getInDegrees(const DG& _g);

template<typename W = double>
class DiGraph {
  public:
    explicit DiGraph(const int _order)
        : m_order(_order)
        , m_matrix(_order, _order, {false, 0})
        , m_neighbors(m_order) {
    }

    DiGraph& operator=(const DiGraph&) = default;
    DiGraph(const DiGraph&) = default;
    DiGraph& operator=(DiGraph&&) = default;
    DiGraph(DiGraph&&) = default;
    ~DiGraph() = default;

    void addEdge(const Graph::Node _u, const Graph::Node _v, const W& _w = W()) {
        assert(_u < m_matrix.size1());
        assert(_v < m_matrix.size2());        
        if (!m_matrix(_u, _v).first) {
            ++m_size;
            m_neighbors[_u].push_back(_v);
            m_edgeChanges = true;
        }
        m_matrix(_u, _v).first = true;
        m_matrix(_u, _v).second = _w;
    }

    bool hasEdge(Graph::Edge _edge) const {
        return hasEdge(_edge.first, _edge.second);
    }
    
    bool hasEdge(const Graph::Node _u, const Graph::Node _v) const {
        return m_matrix(_u, _v).first;
    }

    void removeEdge(const Graph::Node _u, const Graph::Node _v) {
        if (m_matrix(_u, _v).first) {
            m_edgeChanges = true;
            m_matrix(_u, _v).first = false;
            --m_size;
            m_neighbors[_u].erase(
                std::remove(m_neighbors[_u].begin(), m_neighbors[_u].end(), _v),
                m_neighbors[_u].end());
        }
    }

    void removeEdge(const Graph::Edge& _p) {
        removeEdge(_p.first, _p.second);
    }

    const W& getEdgeWeight(const Graph::Node _u, const Graph::Node _v) const {
        assert(_u < m_order);
        assert(_v < m_order);
        assert(m_matrix(_u, _v).first);
        return m_matrix(_u, _v).second;
    }

    const W& getEdgeWeight(const Graph::Edge& _edge) const {
        return getEdgeWeight(_edge.first, _edge.second);
    }

    inline void setEdgeWeight(const Graph::Node _u, const Graph::Node _v, const W& _w) {
        m_matrix(_u, _v).second = _w;
    }

    inline void setEdgeWeight(const Graph::Edge& _e, const W& _w) {
        setEdgeWeight(_e.first, _e.second, _w);
    }

    /**
    * \brief Add _w to the weight of the edge (_u, _v)
    */
    inline void addEdgeWeight(const Graph::Node _u, const Graph::Node _v, const W& _w) {
        m_matrix(_u, _v).second += _w;
    }

    /**
    * \brief Add _w to the weight of the edge _e
    */
    inline void addEdgeWeight(const Graph::Edge& _e, const W& _w) {
        addEdgeWeight(_e.first, _e.second, _w);
    }

    [[deprecated]] std::vector<Graph::Node> getTopologicalOrder() const {
        return ::getTopologicalOrder(*this);
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

    std::vector<Graph::Node>
    getNeighborsIf(const Graph::Node _u, const std::function<bool(const Graph::Node)>& take_if) const {
        std::vector<Graph::Node> neighbors;
        for (const auto& n : m_neighbors[_u]) {
            if (take_if(n)) {
                neighbors.push_back(n);
            }
        }
        return neighbors;
    }

    const std::vector<Graph::Node>& getNeighbors(const Graph::Node _u) const {
        return m_neighbors[_u];
    }

    /**
    * \brief Returns the set of nodes v such that G has the edge (v, _u)
    */
    std::vector<Graph::Node> getInNeighbors(const Graph::Node _u) const {
        std::vector<Graph::Node> inNeighbors;
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
        // Remove incoming arc 
        for (Graph::Node v = 0; v < m_order; ++v) {
            if (m_matrix(_u, v).first) {
                m_matrix(_u, v).first = false;
                --m_size;
            }
        }
    }	

    int getOrder() const {
    	return m_order;
    }

    int size() const {
    	return m_size;
    }

    const std::vector<Graph::Edge>& getEdges() const {
        if(m_edgeChanges) {
            m_edgeChanges = false;
            m_edges.clear();
            for (Graph::Node u = 0; u < m_order; ++u) {
                for (Graph::Node v = 0; v < m_order; ++v) {
                    if (m_matrix(u, v).first) {
                        m_edges.emplace_back(u, v);
                    }
                }
            }
        }
        return m_edges;
    }

    int getOutDegree(const Graph::Node _u) const {
        return m_neighbors[_u].size();
    }

    std::vector<int> getOutDegrees() const {
        std::vector<int> outDegrees(m_order);
        for (Graph::Node u = 0; u < m_order; ++u) {
            outDegrees[u] = m_neighbors[u].size();
        }
        return outDegrees;
    }

    int getInDegree(const Graph::Node _u) const {
        int indegree = 0;
        for (int v = 0; v < m_order; ++v) {
            if (m_matrix(v, _u).first) {
                ++indegree;
            }
        }
        return indegree;
    }

    [[deprecated]] std::vector<int> getInDegrees() const {
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

    void createDotFile(const std::string& _name = "graph") const {
        const std::string dotFilename = _name + ".dot";
        std::ofstream ofs(dotFilename, std::ofstream::out);
        ofs << "digraph DG {\n";
        ofs << "concentrate=true\n";
        for (const auto& edge : getEdges()) {
            ofs << edge.first << "->" << edge.second << "[label=\""
                << getEdgeWeight(edge) << "\"];\n";
        }
        ofs << "}";
    }

    static std::tuple<DiGraph, int, int>
    loadFromFile(const std::string& _filename) {
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

        Graph::Node u, v;
        double w;
        while (ifs.good()) {
            std::getline(ifs, line, '\n');

            std::stringstream(line) >> u >> v >> w;
            graph.addEdge(u, v, w);
            graph.addEdge(v, u, w);
        }
        return std::make_tuple(graph, nbServers, nbSwitches);
    }

    /**
   * Return the set of SCCs using Tarjan's algorithm
   * \todo{Transform to iterative}
   */
    [[deprecated]] std::vector<std::vector<Graph::Node>> getStronglyConnectedComponent() const {
        std::vector<std::vector<Graph::Node>> SCCs;
        int index = 0;
        std::vector<Graph::Node> S;
        std::vector<int> indexes(m_order, -1), lowLink(m_order);
        std::vector<bool> onStack(m_order, false);

        const std::function<void(Graph::Node)> strongConnect = [&](const Graph::Node v) -> void {
            // Set the depth index for v to the smallest unused index
            indexes[v] = lowLink[v] = index;
            ++index;
            S.push_back(v);
            onStack[v] = true;
            // Consider successors of v
            for (const auto& w : getNeighbors(v)) {
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
                Graph::Node w;
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

    [[deprecated]] std::vector<DiGraph> getStronglyConnectedSubGraph() const {
        const auto SCCs = getStronglyConnectedComponent();
        std::vector<DiGraph> subGraphs;
        subGraphs.reserve(SCCs.size());

        for (const auto& SCC : SCCs) {
            subGraphs.emplace_back(SCC.size());
            int u2 = 0;
            for (const auto& u : SCC) {
                int v2 = 0;
                for (const auto& v : SCC) {
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
    int m_order;
    int m_size = 0;
    Matrix<std::pair<bool, W>> m_matrix;
    std::vector<std::vector<Graph::Node>> m_neighbors;
    mutable bool m_edgeChanges;
    mutable std::vector<Graph::Edge> m_edges;
};

template<typename W>
std::ostream& operator<<(std::ostream&, const DiGraph<W>& _g);


/**
* Compare two graphs and return true if the two graphs have the same order
* and the have the same set of nodes and edges.
* \param _g1 The first graph to compare
* \param _g2 The second graph to compare
* \retval true If the graphs are equal
* \retval false If the the graps are different
*/
template<typename W1, typename W2>
inline bool operator==(const DiGraph<W1>& _g1, const DiGraph<W2>& _g2) {
    // First, check size and order of both digraph
    if (_g1.getOrder() == _g2.getOrder() && _g1.size() == _g2.size()) {
        // Second, check that all edge in _g1 belong to _g2
        // Since _g1.size() == _g2.size(),
        const auto edges = _g1.getEdges();
        return std::all_of(std::begin(edges), std::end(edges), [&](auto&& __edge) {
            return _g2.hasEdge(__edge) && _g1.getEdgeWeight(__edge) == _g2.getEdgeWeight(__edge);
        });
    }
    return false;
}

/**
* \brief Return true is _g is a directed acyclic graph
* Compute a topological order and return true if the order is not empty
* \param _g The graph to be tested 
* \retval true _g is a DAG
* \retval false _g is a DAG
* \todo Move away from topological order to test the graph. Should be a simple DFS.
*/
template<typename DG>
inline bool isDAG(const DG& _g) {
    return !getTopologicalOrder(_g).empty();
}

/**
* \brief Return the topological order of the digraph _g if _g is a DAG. Otherwise, return an empty vector.
* \param _g The graph to be tested
*/
template<typename DG>
inline std::vector<Graph::Node> getTopologicalOrder(const DG& _g) {
    std::vector<Graph::Node> topo;
    topo.reserve(_g.getOrder());

    std::vector<int> inDegrees = ::getInDegrees(_g);

    std::vector<bool> inOrder(_g.getOrder(), false);
    bool foundNullInDegree = false;
    while (topo.size() < _g.getOrder()) {
        foundNullInDegree = false;
        for (int u = 0; u < _g.getOrder(); ++u) {
            if (inDegrees[u] == 0 
                && !inOrder[u]) {
                foundNullInDegree = true;
                topo.push_back(u);
                inOrder[u] = true;
                for (const auto& v : _g.getNeighbors(u)) {
                    --inDegrees[v];
                }
            }
        }
        if (!foundNullInDegree) {
            return {};
        }
    }
    return topo;
}

template<typename DG, typename W>
inline std::vector<DiGraph<W>> getStronglyConnectedSubGraph(const DG& _g) {
	std::vector<DiGraph<W>> subGraphs;
    const auto SCCs = getStronglyConnectedComponent(_g);
    subGraphs.reserve(SCCs.size());

    for (const auto& SCC : SCCs) {
        subGraphs.emplace_back(SCC.size());
        int u2 = 0;
        for (const auto& u : SCC) {
            int v2 = 0;
            for (const auto& v : SCC) {
                if (u != v && _g.hasEdge(u, v)) {
                    subGraphs.back().addEdge(u2, v2);
                }
                ++v2;
            }
            ++u2;
        }
    }
    return subGraphs;
}

template<typename DG>
inline std::vector<int> getInDegrees(const DG& _g) {
    std::vector<int> inDegrees(_g.getOrder(), 0);
    for(const auto& edge : _g.getEdges()) {
        ++inDegrees[edge.second];
    }
    return inDegrees;
}
/**
* Return the set of SCCs using Tarjan's algorithm
* \todo{Transform to iterative}
*/
template<typename DG>
inline std::vector<std::vector<Graph::Node>> getStronglyConnectedComponent(const DG& _g) {
    std::vector<std::vector<Graph::Node>> SCCs;
    int index = 0;
    std::vector<Graph::Node> S;
    std::vector<int> indexes(_g.getOrder(), -1);
    std::vector<int> lowLink(_g.getOrder(), -1);
    std::vector<bool> onStack(_g.getOrder(), false);

    const std::function<void(Graph::Node)> strongConnect = [&](const Graph::Node v) {
        // Set the depth index for v to the smallest unused index
        indexes[v] = lowLink[v] = index;
        ++index;
        S.push_back(v);
        onStack[v] = true;
        // Consider successors of v
        for (const auto& w : _g.getNeighbors(v)) {
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
            Graph::Node w;
            do {
                w = S.back();
                SCCs.back().push_back(w);
                onStack[w] = false;
                S.pop_back();
            } while (w != v);
        }
    };

    for (int v = 0; v < _g.getOrder(); ++v) {
        if (indexes[v] == -1) {
            strongConnect(v);
        }
    }
    return SCCs;
}

template<typename W>
inline std::ostream& operator<<(std::ostream& _out, const DiGraph<W>& _g) {
    return _out << _g.getOrder() << " nodes, " << _g.size() << " edges: " << _g.getEdges() << '\n';
}

#endif
