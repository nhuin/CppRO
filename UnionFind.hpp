#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>

class UnionFind {
  public:
    explicit UnionFind(const int _size)
        : m_rank(_size, 0)
        , m_parent(_size) {
        for (int i = 0; i < _size; ++i) {
            m_parent[i] = i;
        }
    }

    void merge(const int _x, const int _y) {
        int xRoot = this->find(_x),
            yRoot = this->find(_y);

        if (m_rank[xRoot] < m_rank[yRoot]) {
            m_parent[xRoot] = yRoot;
        } else if (m_rank[yRoot] < m_rank[xRoot]) {
            m_parent[yRoot] = xRoot;
        } else {
            m_parent[yRoot] = xRoot;
            ++m_rank[xRoot];
        }
    }
    int find(int _x) {
        if (m_parent[_x] != _x) {
            m_parent[_x] = find(m_parent[_x]);
        }
        return m_parent[_x];
    }

  private:
    std::vector<int> m_rank;
    std::vector<int> m_parent;
};

#endif