#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>
#include <numeric>

class UnionFind {
  public:
    explicit UnionFind(const std::vector<int>::size_type _size)
        : m_rank(_size, 0)
        , m_parent(_size) {
        std::iota(m_parent.begin(), m_parent.end(), 0);
    }

    void merge(const unsigned long _x, const unsigned long _y) {
        const auto xRoot = find(_x),
            yRoot = find(_y);

        if (m_rank[xRoot] < m_rank[yRoot]) {
            m_parent[xRoot] = yRoot;
        } else if (m_rank[yRoot] < m_rank[xRoot]) {
            m_parent[yRoot] = xRoot;
        } else {
            m_parent[yRoot] = xRoot;
            ++m_rank[xRoot];
        }
    }
    unsigned long find(const unsigned long _x) {
        if (m_parent[_x] != _x) {
            m_parent[_x] = find(m_parent[_x]);
        }
        return m_parent[_x];
    }

  private:
    std::vector<int> m_rank;
    std::vector<unsigned long> m_parent;
};

#endif
