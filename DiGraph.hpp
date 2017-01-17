#ifndef DIGRAPH_H
#define DIGRAPH_H

#include <vector>
#include <algorithm>
#include <utility>
#include <functional>
#include <sstream>

#include <Graph.hpp>
#include <Matrix.hpp>

class DiGraph : public Graph {
	public:
	
        DiGraph(int _order):
            Graph(_order),
            m_matrix(_order, _order, std::make_pair(false, 0)),
            m_neighbors(m_order)
        {
            assert(_order > 0);
        }

        DiGraph& operator=(const DiGraph&) = default; 
        DiGraph(const DiGraph&) = default;
        DiGraph& operator=(DiGraph&&) = default; 
        DiGraph(DiGraph&&) = default;
        virtual ~DiGraph() {};

    	void addEdge(const int u, const int v, const double w = 0.0) { 
            assert(u >= 0 && u < m_matrix.size1());
            assert(v >= 0 && v < m_matrix.size2());
            if (!m_matrix(u, v).first) {
                ++m_size;
                m_neighbors[u].push_back(v);
            }
            m_matrix(u, v) = std::make_pair(true, w);
        }

        inline bool hasEdge(const int _u, const int _v) const {
            return m_matrix(_u, _v).first;
        }

        inline bool hasEdge(const Graph::Edge& _edge) const {
            return hasEdge(_edge.first, _edge.second);
        }

        void removeEdge(const int u, const int v) {
            if(m_matrix(u, v).first) {
                m_matrix(u, v).first = false;
                --m_size;
                m_neighbors[u].erase(std::remove(m_neighbors[u].begin(), m_neighbors[u].end(), v), m_neighbors[u].end());
            }
        }

        // void removeVertex(int _u) {
        //     for(int i = 0; i < m_matrix.size1(); ++i) {
        //         m_matrix(i, _u) = std::make_pair(false, 0);
        //     }
        //     for(int j = 0; j < m_matrix.size2(); ++j) {
        //         m_matrix(_u, j) = std::make_pair(false, 0);
        //     }       
        // }

        void removeEdge(const Edge& _p) {
            removeEdge(_p.first, _p.second);
        }
    
        virtual double getEdgeWeight(const int u, const int v) const { 
            return m_matrix(u,v).second; 
        }

        double getEdgeWeight(const Edge& _p) const { 
            return this->getEdgeWeight(_p.first, _p.second); 
        };
    
    	inline void setEdgeWeight(const int _u, const int _v, const double _w) { 
            m_matrix(_u, _v).second = _w; 
        }

        inline void setEdgeWeight(const Graph::Edge& _e, const double _w) { 
            setEdgeWeight(_e.first, _e.second, _w); 
        }

        inline void addEdgeWeight(const int _u, const int _v, const double _w) { 
            m_matrix(_u, _v).second += _w; 
        }

        inline void addEdgeWeight(const Graph::Edge& _e, const double _w) { 
            addEdgeWeight(_e.first, _e.second, _w); 
        }

    	std::vector<int> getTopologicalOrder() const {
            std::vector<int> topo;
            
            std::vector<int> inDegree(m_order), inOrder(m_order, false);
            for(int u = 0; u < m_order; ++u) {
                inDegree[u] = getInDegree(u);
            }

            bool changed;
            while((int) topo.size() < m_order) {
                changed = false;
                for(int u = 0; u < (int) inDegree.size(); ++u) {
                    if(inDegree[u] == 0 && !inOrder[u]) {
                        changed = true;
                        topo.push_back(u);
                        inOrder[u] = true;
                        for(auto v : getNeighbors(u)) {
                            --inDegree[v];
                        }
                    }
                }
                if(!changed) {
                    return std::vector<int>();
                }
            }
            return topo;
        }

    	std::vector<int> getNeighborsIf(const int _u, const std::function<bool(int)>& take_if) const {
            std::vector<int> neighbors;
            for(auto& n : m_neighbors[_u]) {
                if(take_if(n)) {
                    neighbors.push_back(n);
                }
            }
            return neighbors;
        }

    	virtual const std::vector<int>& getNeighbors(const int _u) const {
            return m_neighbors[_u];
        }

        void isolate(const Graph::Node _u) {
            for (Graph::Node v = 0; v < m_order; ++v) {
                if(m_matrix(_u, v).first) {
                    m_matrix(_u, v).first = false;
                    --m_size;
                }
            }
            m_neighbors[_u].clear();
        }

        virtual std::vector<Edge> getEdges() const {
            std::vector< Edge > edges;
            
            for (Graph::Node u = 0; u < m_order; ++u) {
                for (Graph::Node v = 0; v < m_order; ++v) {
                    if(m_matrix(u, v).first) {
                        edges.emplace_back(u, v);
                    }
                }
            }
            return edges;
        }

        int getOutDegree(const int _u) const {
            return m_neighbors[_u].size();
        }

        int getInDegree(const int _u) const {
            int indegree = 0;
            for(int v = 0; v < m_order; ++v) {
                if(m_matrix(v, _u).first) {
                    ++indegree;
                }
            }
            return indegree;
        }

        void createDotFile(const std::string& _name = "graph", const std::string& _type = "pdf", bool sysCall = false) const {
            const std::string dotFilename = _name + ".dot";
            std::ofstream ofs (dotFilename, std::ofstream::out);
            ofs << "digraph G {\n";
            ofs << "concentrate=true\n";
            for( auto edge : getEdges() ) {
                ofs << edge.first << "->" << edge.second << "[label=\""<< getEdgeWeight(edge)<<"\"];\n";
            }
            ofs << "}";

            if(sysCall){
                std::string sysString = "dot " + dotFilename + " -T"+_type+"  -o " + _name + "."+_type;
                std::cout << (_name + "."+_type) << '\n';
                system(sysString.c_str());
            }
        }

	   static std::tuple<DiGraph, int, int> loadFromFile(const std::string& _filename) {
            std::ifstream ifs(_filename, std::ifstream::in);
            if(!ifs) {
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
                #if !defined(NDEGUG)
                std::cout << u << '\t' << v << '\t' << w << '\n';
                #endif

                graph.addEdge(u, v, w);
                graph.addEdge(v, u, w);
                ++i;
                }
            return std::tuple<DiGraph, int, int>(graph, nbServers, nbSwitches);
        }
	private:

	Matrix< std::pair<bool, double> > m_matrix;
	std::vector<std::vector<int>> m_neighbors;
};

inline bool operator==(const DiGraph& _g1, const DiGraph& _g2) {
    if(_g1.getOrder() == _g2.getOrder() && _g1.size() == _g2.size()) {
        for(auto& edge : _g1.getEdges()) {
            if(!_g2.hasEdge(edge)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

#endif