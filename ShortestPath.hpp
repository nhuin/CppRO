#ifndef SHORTESTPATH_HPP
#define SHORTESTPATH_HPP

#include "Graph.hpp"
#include "BinaryHeap.hpp"
#include "utility.hpp"

template<typename G>
class ShortestPath {
	public:
		ShortestPath(const G& _graph) :
			m_graph(_graph),
			m_distance(m_graph.getOrder(), std::numeric_limits<int>::max()),
			m_numberHops(m_graph.getOrder(), 0),
	    	m_comp(m_distance),
		    m_parent(m_graph.getOrder(), -1),	
		    m_color(m_graph.getOrder(), 0),
	    	m_handles(m_graph.getOrder()),
	    	m_heap(m_graph.getOrder(), m_comp)
		{}

		ShortestPath(const ShortestPath& _other) : 
			ShortestPath(_other.m_graph)
		{}

		ShortestPath(ShortestPath&& _other) : 
			ShortestPath(_other.m_graph)
		{}

		ShortestPath& operator=(const ShortestPath& _other) {
			if(this != &_other) {
				m_graph = _other.m_graph;
				m_distance = std::vector<double>(m_graph.getOrder(), std::numeric_limits<int>::max());
				m_numberHops =  std::vector<int>(m_graph.getOrder(), 0);
		    	m_comp = Comparator(m_distance);
			    m_parent = std::vector<int>(m_graph.getOrder(), -1);	
			    m_color = std::vector<short>(m_graph.getOrder(), 0);
		    	m_handles = std::vector< typename BinaryHeap<int, Comparator>::Handle* >(m_graph.getOrder());
		    	m_heap = BinaryHeap<int, Comparator>(m_graph.getOrder(), m_comp);
			}
			return *this;
		}

		ShortestPath& operator=(ShortestPath&& _other) {
			if(this != &_other) {
				m_graph = _other.m_graph;
				m_distance = std::vector<double>(m_graph.getOrder(), std::numeric_limits<int>::max());
				m_numberHops =  std::vector<int>(m_graph.getOrder(), 0);
		    	m_comp = Comparator(m_distance);
			    m_parent = std::vector<int>(m_graph.getOrder(), -1);	
			    m_color = std::vector<short>(m_graph.getOrder(), 0);
		    	m_handles = std::vector< typename BinaryHeap<int, Comparator>::Handle* >(m_graph.getOrder());
		    	m_heap = BinaryHeap<int, Comparator>(m_graph.getOrder(), m_comp);
			}
			return *this;
		}

		
			

		void clear() {
		    std::fill(m_parent.begin(), m_parent.end(), -1);
		    std::fill(m_distance.begin(), m_distance.end(), std::numeric_limits<double>::max());
		    std::fill(m_numberHops.begin(), m_numberHops.end(), 0);
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
		        if(m_heap.empty()) {
		            return Graph::Path();
		        }
		        const auto u = m_heap.top(); m_heap.pop();
		        m_color[u] = 2;
		        
		        for (const auto& v : m_graph.getNeighbors(u)) {
		            if(m_color[v] != 2) {
		                auto distT = m_distance[u] + m_graph.getEdgeWeight(u, v);
		                if ( distT < m_distance[v] ) {
		                    m_distance[v] = distT;
		                    m_numberHops[v] = m_numberHops[u] + 1;
		                    if(m_color[v] == 1) {
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
		    while(node != _s) {
		    	path.push_back(node);
		    	node = m_parent[node];
		    }
		    path.push_back(node);
		    std::reverse(path.begin(), path.end());
		    return path;
		}

		Graph::Path getShortestPathNbArcs(const int _s, const int _t) {
			#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				std::cout << "\ngetShortestPathNbArcs(" << _s << ", " << _t << ")\n";
	        #endif
		    m_parent[_s] = _s;
		    m_distance[_s] = 0;
		    m_handles[_s] = m_heap.push(_s);
		    m_color[_s] = 1;
		    
		    Graph::Path path; 
		    while (_t != m_heap.top()) {
		        if(m_heap.empty()) {
		            return path;
		        }
		        const auto u = m_heap.top(); m_heap.pop();
		        m_color[u] = 2;
		        #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		        std::cout << "Visiting " << u << '\n';
		        #endif

		        for (const auto& v : m_graph.getNeighbors(u)) {
		        	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
			        std::cout << "Considering " << v << " m_color: " << m_color[v] << '\t';
			        #endif
		            if(m_color[v] != 2) {
		            	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				        std::cout << " -> " << (m_distance[u] + 1);
				        #endif
		                auto distT = m_distance[u] + 1;
		                if ( distT < m_distance[v] ) {
		                    m_distance[v] = distT;
		                    m_numberHops[v] = m_numberHops[u] + 1;
		                    if(m_color[v] == 1) {
		                        m_heap.decrease(m_handles[v]);
		                    } else {
		                        m_handles[v] = m_heap.push(v);
		                        m_color[v] = 1;
		                    }
		                    m_parent[v] = u;
		                }
		            }
		            #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
			        std::cout << "\n";
			        #endif
		        }
		    }
		    #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		    std::cout << m_parent << '\n';
		    std::cout << m_numberHops << '\n';
		    #endif

		    /* Path is found, need to build it */
		    Graph::Node node = _t;
		    while(node != _s) {
		    	path.push_back(node);
		    	node = m_parent[node];
		    }
		    path.push_back(node);
		    std::reverse(path.begin(), path.end());
		    return path;
		}

		Graph::Path getShortestPath(const int _s, const int _t, const std::function<bool(int, int)>& _isNeighbor, const std::function<double(int, int)>& _getWeight) {
			//   //std::cout<< "getShortestPath" << Graph::Edge(_s, _t) << '\n';
		    m_parent[_s] = _s;
		    m_distance[_s] = 0;
		    m_handles[_s] = m_heap.push(_s);
		    m_color[_s] = 1;
		    
		    Graph::Path path; 
		    while (_t != m_heap.top()) {
		        if(m_heap.empty()) {
		            return Graph::Path();
		        }
		        const auto u = m_heap.top(); m_heap.pop();
		        m_color[u] = 2;
		        
		        for (auto& v : m_graph.getNeighbors(u)) {
		            if(m_color[v] != 2 && _isNeighbor(u, v)) {
		                auto distT = m_distance[u] + _getWeight(u, v);
		                m_numberHops[v] = m_numberHops[u] + 1;
		                if ( distT < m_distance[v] ) {
		                    m_distance[v] = distT;
		                    if(m_color[v] == 1) {
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
		    while(node != _s) {
		    	path.push_back(node);
		    	node = m_parent[node];
		    }
		    path.push_back(node);
		    std::reverse(path.begin(), path.end());
		    return path;
		}

		Graph::Path getShortestPathNbArcs(const int _s, const int _t, const std::function<bool(int, int)>& _isNeighbor) {
	        #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				std::cout << "\ngetShortestPathNbArcs(" << _s << ", " << _t << ")\n";
	        #endif
		    m_heap.clear();

		    m_parent[_s] = _s;
		    m_distance[_s] = 0;
		    m_handles[_s] = m_heap.push(_s);
		    m_color[_s] = 1;
		    
		    Graph::Path path; 
		    while (_t != m_heap.top()) {
		        if(m_heap.empty()) {
		            return path;
		        }
		        const auto u = m_heap.top(); m_heap.pop();
		        m_color[u] = 2;
		        #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
		        std::cout << "Visiting " << u << '\n';
		        #endif
		        for (auto& v : m_graph.getNeighbors(u)) {
		        	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
			        std::cout << "Considering " << v << " m_color: " << m_color[v] << ", _isNeighbor: " << _isNeighbor(u, v) << '\t';
			        #endif
		            if(m_color[v] != 2 && _isNeighbor(u, v)) {
		            	#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				        std::cout << " -> " << (m_distance[u] + 1);
				        #endif
		                auto distT = m_distance[u] + 1;
		                m_numberHops[v] = m_numberHops[u] + 1;
		                if ( distT < m_distance[v] ) {
		                    m_distance[v] = distT;
		                    if(m_color[v] == 1) {
		                        m_heap.decrease(m_handles[v]);
		                    } else {
		                        m_handles[v] = m_heap.push(v);
		                        m_color[v] = 1;
		                    }
		                    m_parent[v] = u;
		                }
		            }
		            #if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
			        std::cout << "\n";
			        #endif
		        }
		    }

		    /* Path is found, need to build it */
		    Graph::Node node = _t;
		    while(node != _s) {
		    	path.push_back(node);
		    	node = m_parent[node];
		    }
		    path.push_back(node);
		    std::reverse(path.begin(), path.end());
		    return path;
		}

		std::vector<Graph::Path> getKShortestPath(const int _u, const int _v, const int _k) {
			std::vector<Graph::Path> paths;
			if(_k == 0) {
				return paths;
			}

			// Determine the shortest path from the source to the sink.
			paths.push_back(this->getShortestPathNbArcs(_u, _v));
   			// Initialize the heap to store the potential kth shortest path.
			std::list<Graph::Path> stack;
			for (int k = 1; k < _k; ++k) {
				#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				std::cout<< "Looking for " << (k+1) << "th path" << '\n';				
				#endif
				auto& lastPath = paths[k-1];
				auto ite = lastPath.begin();
				#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				std::cout<< "lastPath: " << lastPath << '\n';
				#endif
				for (int i = 0; i < static_cast<int>(lastPath.size()-1); ++i, ++ite) {
					// Spur node is retrieved from the previous k-shortest path, k − 1.
					int spurNode = *ite;
					#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
					std::cout << "spurNode: " << spurNode << '\n';
					#endif
					// The sequence of nodes from the source to the spur node of the previous k-shortest path.
					Graph::Path rootPath { lastPath.begin(), std::next(ite) };
					#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
					std::cout << "rootPath: " << rootPath << '\n';
					#endif
					assert(rootPath.back() == spurNode);

					G graphCopy = m_graph;
					#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
					std::cout << graphCopy.getEdges() << '\n';
					std::cout << m_graph.getEdges() << '\n';
					#endif

					for (int j = 0; j < k; ++j) {
						auto p = paths[j];
						#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
						std::cout << "paths["<<j<<"]: " << p << '\n';
						#endif

						bool same = true;
						auto ite2 = p.begin();
						for(auto ite1 = rootPath.begin(); ite1 != rootPath.end(); ++ite1, ++ite2) {
							if(*ite1 != *ite2) {
								same = false;
								break;
							}
						}
						//std::cout<< p << " <=> " << rootPath << ": " << same << '\n';
						if(same) {
							// Remove the links that are part of the previous shortest paths which share the same root path.
							#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
							std::cout<< "removeEdge" << Graph::Edge(*std::prev(ite2), *ite2) << '\n';
							#endif
							graphCopy.removeEdge(*std::prev(ite2), *ite2);
						}
					}

					for(const auto& u : rootPath) {
						if( u != spurNode ) {
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
					Graph::Path spurPath = ShortestPath<G>(graphCopy).getShortestPathNbArcs(spurNode, _v);
					if(!spurPath.empty()) {
						assert(spurPath.front() == spurNode);
						#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
						std::cout<< "spurPath: " << spurPath << '\n';
						#endif
						rootPath.insert(rootPath.end(), std::next(spurPath.begin()), spurPath.end());

						// Search first longer path
						auto ite = stack.begin();
						for(; ite != stack.end(); ++ite) {
							if(ite->size() >= rootPath.size()) {
								break; 
							}
						}
						#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
						std::cout << "Inserting " << rootPath << '\n';
						#endif
						stack.insert(ite, rootPath);
					}
					#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
					std::cout << '\n';
					#endif
				}
				#if !defined(NDEBUG) && defined(LOG_LEVEL) && LOG_LEVEL <= 0
				std::cout << "Paths in stack => " << stack << '\n';
				#endif
				if(stack.empty()) {
					return paths;		
				}

				paths.push_back(stack.front());
				stack.pop_front();
			}
			return paths;
		}

	private:
		struct Comparator {
            std::vector<double>& distance;

            Comparator(std::vector<double>& _distance) : distance(_distance) {}


            bool operator()(int u, int v) const {
                return distance[u] < distance[v];
            }

            Comparator& operator=(const Comparator&) = default; 
            Comparator(const Comparator&) = default;
            Comparator& operator=(Comparator&& _other) {
            	if(this != &_other) {
            		distance = _other.distance;
            	}
            	return *this;
            } 
            Comparator(Comparator&& _other) = default;
        };

	    const G& m_graph;
	    std::vector<double> m_distance;
	    std::vector<int> m_numberHops;
	    Comparator m_comp;
	    std::vector<int> m_parent;
	    std::vector<short> m_color;	
	    std::vector< typename BinaryHeap<int, Comparator>::Handle* > m_handles;
	    BinaryHeap<int, Comparator> m_heap;
};

#endif