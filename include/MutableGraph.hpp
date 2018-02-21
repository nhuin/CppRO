#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Matrix.hpp"

class MutableGraph {
  public:
    MutableGraph(int _order)
        : m_matrix(_order, _order)
        , m_available_ids() {}
    MutableGraph(const MutableGraph&) = default;
    MutableGraph& operator=(const MutableGraph&) = default;
    MutableGraph(MutableGraph&&) = default;
    MutableGraph& operator=(MutableGraph&&) = default;
    ~MutableGraph() = default;

    int addVertex() {
        int id = -1;
        // if (m_available_ids.empty()) {
        //     m_matrix.addRowAndColumn();
        //     id = m.size1() - 1;
        // } else {
        //     id = m_available_ids.back();
        //     m_available_ids.pop_back();
        // }
        // assert(id != -1);
        return id;
    }

    void removeVertex(int _id) {
        assert(_id < m_matrix.size1());
        for (int i = 0; i < m_matrix.size1(); ++i) {
            m_matrix(_id, i) = false;
            m_matrix(i, _id) = false;
        }
        m_available_ids.push_back(_id);
    }

    void addEdge(const int _u, const int _v) { m_matrix(_u, _v) = true; }

  private:
    Matrix<char> m_matrix;
    std::vector<int> m_available_ids;
    std::vector<int> m_vertices;
};

#endif