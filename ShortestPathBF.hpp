#ifndef SHORTESTPATHBF_HPP
#define SHORTESTPATHBF_HPP

#include "Graph.hpp"
#include "utility.hpp"

template<typename G>
class ShortestPathBellmanFord {
	public:
		ShortestPathBellmanFord(const G& _graph) :
			m_graph(_graph),
			m_distance(m_graph.getOrder(), std::numeric_limits<int>::max()),
		    m_parent(m_graph.getOrder(), -1)
		{}

		ShortestPathBellmanFord(const ShortestPathBellmanFord& _other) : 
			ShortestPathBellmanFord(_other.m_graph)
		{}

		ShortestPathBellmanFord(ShortestPathBellmanFord&& _other) : 
			ShortestPathBellmanFord(_other.m_graph)
		{}

		ShortestPathBellmanFord& operator=(const ShortestPathBellmanFord& _other) {
			if(this != &_other) {
				m_graph = _other.m_graph;
				m_distance = std::vector<double>(m_graph.getOrder(), std::numeric_limits<int>::max());
			    m_parent = std::vector<int>(m_graph.getOrder(), -1);
			}
			return *this;
		}

		ShortestPathBellmanFord& operator=(ShortestPathBellmanFord&& _other) {
			if(this != &_other) {
				m_graph = _other.m_graph;
				m_distance = std::vector<double>(m_graph.getOrder(), std::numeric_limits<int>::max());
			    m_parent = std::vector<int>(m_graph.getOrder(), -1);
			}
			return *this;
		}

		void clear() {
		    std::fill(m_parent.begin(), m_parent.end(), -1);
		    std::fill(m_distance.begin(), m_distance.end(), std::numeric_limits<double>::max());
		}

		Graph::Path getShortestPath(const int _s, const int _t) {
			#if defined(LOG_LEVEL) && LOG_LEVEL<=0
			std::cout << "getShortestPath("<<_s<<", "<<_t<<") -> ";
			#endif
		    m_parent[_s] = _s;
		    m_distance[_s] = 0;
		    const auto edges = m_graph.getEdges();
		    for(int i = 0; i < m_graph.getOrder(); ++i) {
		    	for(const auto& edge : edges) {
		    		const double dist = m_distance[edge.first] + m_graph.getEdgeWeight(edge);
		    		if(dist < m_distance[edge.second]) {
		    			m_distance[edge.second] = dist;
		    			m_parent[edge.second] = edge.first;
		    		}
		    	}
		    }
		    Graph::Path path;
		    if(m_parent[_t] != -1) {
		    	Graph::Node node = _t;
			    while(node != _s) {
			    	path.push_back(node);
			    	node = m_parent[node];
			    }
			    path.push_back(node);
			    std::reverse(path.begin(), path.end());
		    }
		    #if defined(LOG_LEVEL) && LOG_LEVEL<=0
		    std::cout << path << '\n';
		    #endif
		    return path;
		}

		double getDistance(const Graph::Node _u) const {
			return m_distance[_u];
		}

		// Graph::Path getShortestPathNbArcs(const int _s, const int _t) {
		// 	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 		std::cout << "\ngetShortestPathNbArcs(" << _s << ", " << _t << ")\n";
	 //        #endif
		//     m_parent[_s] = _s;
		//     m_distance[_s] = 0;
		//     m_handles[_s] = m_heap.push(_s);
		//     m_color[_s] = 1;
		    
		//     Graph::Path path; 
		//     while (_t != m_heap.top()) {
		//         if(m_heap.empty()) {
		//             return path;
		//         }
		//         const auto u = m_heap.top(); m_heap.pop();
		//         m_color[u] = 2;
		//         #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		//         std::cout << "Visiting " << u << '\n';
		//         #endif

		//         for (const auto& v : m_graph.getNeighbors(u)) {
		//         	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 	        std::cout << "Considering " << v << " m_color: " << m_color[v] << '\t';
		// 	        #endif
		//             if(m_color[v] != 2) {
		//             	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 		        std::cout << " -> " << (m_distance[u] + 1);
		// 		        #endif
		//                 auto distT = m_distance[u] + 1;
		//                 if ( distT < m_distance[v] ) {
		//                     m_distance[v] = distT;
		//                     if(m_color[v] == 1) {
		//                         m_heap.decrease(m_handles[v]);
		//                     } else {
		//                         m_handles[v] = m_heap.push(v);
		//                         m_color[v] = 1;
		//                     }
		//                     m_parent[v] = u;
		//                 }
		//             }
		//             #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 	        std::cout << "\n";
		// 	        #endif
		//         }
		//     }
		//     #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		//     std::cout << m_parent << '\n';
		//     #endif

		//     /* Path is found, need to build it */
		//     Graph::Node node = _t;
		//     while(node != _s) {
		//     	path.push_back(node);
		//     	node = m_parent[node];
		//     }
		//     path.push_back(node);
		//     std::reverse(path.begin(), path.end());
		//     return path;
		// }

		// Graph::Path getShortestPath(const int _s, const int _t, const std::function<bool(int, int)>& _isNeighbor, const std::function<double(int, int)>& _getWeight) {
		// 	//   //std::cout<< "getShortestPath" << Graph::Edge(_s, _t) << '\n';
		//     m_parent[_s] = _s;
		//     m_distance[_s] = 0;
		//     m_handles[_s] = m_heap.push(_s);
		//     m_color[_s] = 1;
		    
		//     Graph::Path path; 
		//     while (_t != m_heap.top()) {
		//         if(m_heap.empty()) {
		//             return Graph::Path();
		//         }
		//         const auto u = m_heap.top(); m_heap.pop();
		//         m_color[u] = 2;
		        
		//         for (auto& v : m_graph.getNeighbors(u)) {
		//             if(m_color[v] != 2 && _isNeighbor(u, v)) {
		//                 auto distT = m_distance[u] + _getWeight(u, v);
		//                 if ( distT < m_distance[v] ) {
		//                     m_distance[v] = distT;
		//                     if(m_color[v] == 1) {
		//                         m_heap.decrease(m_handles[v]);
		//                     } else {
		//                         m_handles[v] = m_heap.push(v);
		//                         m_color[v] = 1;
		//                     }
		//                     m_parent[v] = u;
		//                 }
		//             }
		//         }
		//     }
		//     /* Path is found, need to build it */
		//     Graph::Node node = _t;
		//     while(node != _s) {
		//     	path.push_back(node);
		//     	node = m_parent[node];
		//     }
		//     path.push_back(node);
		//     std::reverse(path.begin(), path.end());
		//     return path;
		// }

		// Graph::Path getShortestPathNbArcs(const int _s, const int _t, const std::function<bool(int, int)>& _isNeighbor) {
	 //        #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 		std::cout << "\ngetShortestPathNbArcs(" << _s << ", " << _t << ")\n";
	 //        #endif
		//     m_heap.clear();

		//     m_parent[_s] = _s;
		//     m_distance[_s] = 0;
		//     m_handles[_s] = m_heap.push(_s);
		//     m_color[_s] = 1;
		    
		//     Graph::Path path; 
		//     while (_t != m_heap.top()) {
		//         if(m_heap.empty()) {
		//             return path;
		//         }
		//         const auto u = m_heap.top(); m_heap.pop();
		//         m_color[u] = 2;
		//         #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		//         std::cout << "Visiting " << u << '\n';
		//         #endif
		//         for (auto& v : m_graph.getNeighbors(u)) {
		//         	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 	        std::cout << "Considering " << v << " m_color: " << m_color[v] << ", _isNeighbor: " << _isNeighbor(u, v) << '\t';
		// 	        #endif
		//             if(m_color[v] != 2 && _isNeighbor(u, v)) {
		//             	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 		        std::cout << " -> " << (m_distance[u] + 1);
		// 		        #endif
		//                 auto distT = m_distance[u] + 1;
		//                 if ( distT < m_distance[v] ) {
		//                     m_distance[v] = distT;
		//                     if(m_color[v] == 1) {
		//                         m_heap.decrease(m_handles[v]);
		//                     } else {
		//                         m_handles[v] = m_heap.push(v);
		//                         m_color[v] = 1;
		//                     }
		//                     m_parent[v] = u;
		//                 }
		//             }
		//             #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		// 	        std::cout << "\n";
		// 	        #endif
		//         }
		//     }

		//     /* Path is found, need to build it */
		//     Graph::Node node = _t;
		//     while(node != _s) {
		//     	path.push_back(node);
		//     	node = m_parent[node];
		//     }
		//     path.push_back(node);
		//     std::reverse(path.begin(), path.end());
		//     return path;
		// }

	private:
	    const G& m_graph;
	    std::vector<double> m_distance;
	    std::vector<int> m_parent;
};

#endif