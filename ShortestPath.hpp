#ifndef SHORTESTPATH_HPP
#define SHORTESTPATH_HPP

#include "Graph.h"
#include "BinaryHeap.hpp"

template<typename G>
class ShortestPath {
	public:
		ShortestPath(G& _graph) :
			m_graph(_graph),
			m_distance(m_graph.getOrder()),
	    	m_comp(m_distance),
		    m_parent(m_graph.getOrder()),	
		    m_color(m_graph.getOrder()),
	    	m_handles(m_graph.getOrder()),
	    	m_heap(m_graph.getOrder(), m_comp)
		{}

		ShortestPath(ShortestPath&& _sp) = default;
		ShortestPath& operator=(ShortestPath&& _sp) {
			m_graph = std::move(_sp.m_graph);
		    m_distance = std::move(_sp.m_distance);
		    m_comp = std::move(_sp.m_comp);
		    m_parent = std::move(_sp.m_parent);
		    m_color = std::move(_sp.m_color);
		    m_handles = std::move(_sp.m_handles);
		    m_heap = std::move(_sp.m_heap);
		    return *this;
		}

		ShortestPath(const ShortestPath& _sp) = default;
		ShortestPath& operator=(const ShortestPath& _sp) {
			m_graph = _sp.m_graph;
		    m_distance = _sp.m_distance;
		    m_comp = _sp.m_comp;
		    m_parent = _sp.m_parent;
		    m_color = _sp.m_color;
		    m_handles = _sp.m_handles;
		    m_heap = _sp.m_heap;
		    return *this;
		}	

		void clear() {
		    std::fill(m_parent.begin(), m_parent.end(), -1);
		    std::fill(m_distance.begin(), m_distance.end(), std::numeric_limits<int>::max());
		    std::fill(m_color.begin(), m_color.end(), 0);
		    m_heap.clear();
		}

		Graph::Path getShortestPath(int u, int v) {
		    m_parent[u] = u;
		    m_distance[u] = 0;
		    m_handles[u] = m_heap.push(u);
		    m_color[u] = 1;
		    
		    while (m_color[v] != 2) {
		        if(m_heap.empty()) {
		            return Graph::Path();
		        }
		        auto s = m_heap.top();
		        m_heap.pop();
		        m_color[s] = 2;
		        
		        for (auto& t : m_graph.getNeighbors(s)) {
		            if(m_color[t] != 2) {
		                
		                auto distT = m_distance[s] + m_graph.getEdgeWeight(s, t);
		                if ( distT < m_distance[t] ) {
		                    m_distance[t] = distT;
		                    if(m_color[t] == 1) {
		                        m_heap.decrease(m_handles[t]);
		                    } else {
		                        m_handles[t] = m_heap.push(t);
		                        m_color[t] = 1;
		                    }
		                    m_parent[t] = s;
		                }
		            }
		        }
		    }

		    // Path found, build it
		    Graph::Path path;
		    int node = v;
		    while ( node != u ) {
		        path.push_front(node);
		        node = m_parent[node];
		    }
		    path.push_front(u);
		    return path;
		}

		Graph::Path getShortestPathNbArcs(int u, int v) {
		    m_parent[u] = u;
		    m_distance[u] = 0;
		    m_handles[u] = m_heap.push(u);
		    m_color[u] = 1;
		    
		    while (m_color[v] != 2) {
		        if(m_heap.empty()) {
		            return Graph::Path();
		        }
		        auto s = m_heap.top();
		        m_heap.pop();
		        m_color[s] = 2;
		        
		        for (auto& t : m_graph.getNeighbors(s)) {
		            if(m_color[t] != 2) {
		                
		                auto distT = m_distance[s] + 1;
		                if ( distT < m_distance[t] ) {
		                    m_distance[t] = distT;
		                    if(m_color[t] == 1) {
		                        m_heap.decrease(m_handles[t]);
		                    } else {
		                        m_handles[t] = m_heap.push(t);
		                        m_color[t] = 1;
		                    }
		                    m_parent[t] = s;
		                }
		            }
		        }
		    }
		    // Path found, build it
		    Graph::Path path;
		    int node = v;
		    while ( node != u ) {
		        path.push_front(node);
		        node = m_parent[node];
		    }
		    path.push_front(u);
		    return path;
		}

		Graph::Path getShortestPath(int u, int v, const std::function<bool(int, int)>& isNeighbor, const std::function<double(int, int)>& _getWeight) {
		    m_parent[u] = u;
		    m_distance[u] = 0;
		    m_handles[u] = m_heap.push(u);
		    m_color[u] = 1;
		    
		    while (m_color[v] != 2) {
		        if(m_heap.empty()) {
		            return Graph::Path();
		        }
		        auto s = m_heap.top(); m_heap.pop();
		        m_color[s] = 2;
		        
		        for (auto& t : m_graph.getNeighbors(s)) {
		            if(m_color[t] != 2 && isNeighbor(s, t)) {
		                
		                auto distT = m_distance[s] + _getWeight(s, t);
		                if ( distT < m_distance[t] ) {
		                    m_distance[t] = distT;
		                    if(m_color[t] == 1) {
		                        m_heap.decrease(m_handles[t]);
		                    } else {
		                        m_handles[t] = m_heap.push(t);
		                        m_color[t] = 1;
		                    }
		                    m_parent[t] = s;
		                }
		            }
		        }

		    }
		    // Path found, build it
		    Graph::Path path;
		    int node = v;
		    while ( node != u ) {
		        path.push_front(node);
		        node = m_parent[node];
		    }
		    path.push_front(u);
		    return path;
		}

		Graph::Path getShortestPathNbArcs(int u, int v, const std::function<bool(int, int)>& isNeighbor) {
		    m_heap.clear();

		    m_parent[u] = u;
		    m_distance[u] = 0;
		    m_handles[u] = m_heap.push(u);
		    m_color[u] = 1;
		    
		    while (m_color[v] != 2) {
		        if(m_heap.empty()) {
		            return Graph::Path();
		        }
		        auto s = m_heap.top();
		        m_heap.pop();
		        m_color[s] = 2;
		        
		        for (auto& t : m_graph.getNeighbors(s)) {
		            if(m_color[t] != 2 && isNeighbor(s, t)) {
		                
		                auto distT = m_distance[s] + 1;
		                if ( distT < m_distance[t] ) {
		                    m_distance[t] = distT;
		                    if(m_color[t] == 1) {
		                        m_heap.decrease(m_handles[t]);
		                    } else {
		                        m_handles[t] = m_heap.push(t);
		                        m_color[t] = 1;
		                    }
		                    m_parent[t] = s;
		                }
		            }
		        }
		    }
		    // Path found, build it
		    Graph::Path path;
		    int node = v;
		    while ( node != u ) {
		        path.push_front(node);
		        node = m_parent[node];
		    }
		    path.push_front(u);
		    return path;
		}

	private:
		struct Comparator {
            std::vector<double>& distance;

            Comparator(std::vector<double>& _distance) : distance(_distance) {}


            bool operator()(int u, int v) const {
                return distance[u] < distance[v];
            }

            // Copy
            Comparator(const Comparator& _other) : distance(_other.distance) {};
            Comparator& operator=(const Comparator& _other) {
                distance = _other.distance;
                return *this;
            };
            // Move
            Comparator(Comparator&& _other) : distance(_other.distance) {};
            Comparator& operator=(Comparator&& _other) {
                distance = _other.distance;
                return *this;
            };
        };

	    G& m_graph;
	    std::vector<double> m_distance;
	    Comparator m_comp;
	    std::vector<int> m_parent;
	    std::vector<short> m_color;	
	    std::vector< typename BinaryHeap<int, Comparator>::Handle* > m_handles;
	    BinaryHeap<int, Comparator> m_heap;
};

#endif