#ifndef FIBONACCI_HEAP_HPP
#define FIBONACCI_HEAP_HPP

#include "FixedSizeAllocator.hpp"
#include <algorithm>
#include <cassert>
#include <list>
#include <vector>

template <typename T, typename Comparator = std::less<T>>
class FibonacciHeap {
  public:
    struct Node {
        T value;

      private:
        explicit Node(const T& _value)
            : value(_value) {}
        Node* next = nullptr;
        Node* prev = nullptr;
        Node* parent = nullptr;
        Node* firstChild = nullptr;
        Node* lastChild = nullptr;
        int rank = 0;
        bool marked = false;
        friend class FibonacciHeap;
    };

    explicit FibonacciHeap(int _maxSize, const Comparator& _comp = Comparator())
        : m_heap(nullptr)
        , m_min(nullptr)
        , m_comparator(_comp)
        , m_allocator(_maxSize)
        , m_nodesAtRank(_maxSize) {}

    ~FibonacciHeap() {
        m_allocator.deallocateAll();
    }

    FibonacciHeap(const FibonacciHeap&) = default;
    FibonacciHeap& operator=(const FibonacciHeap&) = default;

    FibonacciHeap(FibonacciHeap&&) = delete;
    FibonacciHeap& operator=(FibonacciHeap&&) = delete;

    bool empty() const {
        return !m_heap;
    }

    Node* push(const T& value) {
        Node* n = m_allocator.allocate();
        new (n) Node(value);
        addToRootList(n);
        assert((n == m_heap && n->next == n) || n->next != n);
        return n;
    }

    void pop() {
        assert(m_min);
        Node* toPop = m_min;
        assert(!toPop->parent);

        if (toPop->firstChild) { // Has children
            replaceWithChildrenInRootList(toPop);
        } else {
            if (toPop->next == toPop) { // Only root
                m_heap = m_min = nullptr;
                m_nodesAtRank[0] -= 1;
            } else { // Multiple roots
                removeFromRoot(toPop);
            }
        }
        toPop->~Node();
        m_allocator.deallocate(toPop);

        if (m_heap) {
            consolidate();
            m_min = findMin();
        }
    }

    const T& top() {
        return m_min->value;
    }

    void decrease(Node* n) {
        Node* parent = n->parent;
        if (parent && m_comparator(n->value, parent->value)) {
            cutToRootList(n);
        }
        if (m_comparator(n->value, m_min->value)) {
            m_min = n;
        }
    }

    void decrease(Node* n, const T& value) {
        assert(n->value > value);
        n->value = value;
        decrease(n);
    }

    void clear() {
        if (!empty()) {
            m_allocator.deallocateAll();
        }
        m_heap = m_min = nullptr;
        std::fill(m_nodesAtRank.begin(), m_nodesAtRank.end(), 0);
    }

  private:
    Node* m_heap;
    Node* m_min;
    // Node* m_lastNode;
    Comparator m_comparator;
    FixedSizeAllocator<Node> m_allocator;
    std::vector<int> m_nodesAtRank;

    void cutToRootList(Node* n) {
        Node* parent = n->parent;
        assert(parent);
        removeFromChildren(n);
        addToRootList(n);
        updateRank(parent);
        if (parent->marked) {
            cutToRootList(parent);
        } else if (parent->parent) {
            parent->marked = true;
        }
        assert(!n->parent);
    }

    void updateRank(Node* n) {
        Node* ptr = n->firstChild;
        int rank = 0;
        if (ptr) {
            do {
                if (rank < ptr->rank) {
                    rank = ptr->rank;
                }
                ptr = ptr->next;
            } while (ptr != n->firstChild);
        }
        if (rank + 1 > n->rank) {
            if (!n->parent) {
                m_nodesAtRank[n->rank] -= 1;
                m_nodesAtRank[rank + 1] += 1;
            }
            n->rank = rank + 1;
        }
    }

    Node* findMin() const {
        assert(m_heap);
        Node* min = m_heap;
        if (m_heap != m_heap->next) { // Multiple roots
            Node* ptr = m_heap->next;
            do {
                if (m_comparator(ptr->value, min->value)) {
                    min = ptr;
                }
                ptr = ptr->next;
            } while (ptr != m_heap);
        }
        return min;
    }

    void removeFromChildren(Node* n) {
        assert(n->parent);
        // Connect prev and next
        n->prev->next = n->next;
        n->next->prev = n->prev;

        // Update first and last child
        if (n->parent->firstChild == n->parent->lastChild) {
            n->parent->firstChild = n->parent->lastChild = 0;
        } else if (n == n->parent->firstChild) {
            n->parent->firstChild = n->next;
            n->parent->lastChild = n->parent->firstChild->prev;
        } else if (n == n->parent->lastChild) {
            n->parent->lastChild = n->prev;
            n->parent->firstChild = n->parent->lastChild->next;
        }
    }

    void removeFromRoot(Node* n) {
        assert(!n->parent);
        m_nodesAtRank[n->rank] -= 1;

        // Connect prev and next
        n->prev->next = n->next;
        n->next->prev = n->prev;

        // Update first and last child
        if (n == m_heap) {
            m_heap = n->next;
        }
    }

    void addToRootList(Node* n) {
        n->parent = nullptr;
        n->marked = false;
        m_nodesAtRank[n->rank] += 1;

        if (m_heap) {
            n->prev = m_heap->prev;
            n->next = m_heap;

            n->prev->next = n;
            n->next->prev = n;
            assert(m_heap->prev == n && m_heap->prev->prev == n->prev);
            if (m_comparator(n->value, m_heap->value)) {
                m_heap = n;
            }
        } else {
            m_heap = m_min = n;
            m_heap->next = m_heap->prev = m_heap;
        }
        assert(!n->parent);
    }

    void replaceWithChildrenInRootList(Node* father) {
        assert(m_heap && !father->parent && father->firstChild);

        if (father->lastChild != father->firstChild) { // Multiple children
            Node* ptr = father->firstChild;
            do {
                ptr->parent = nullptr;
                ptr->marked = false;
                m_nodesAtRank[ptr->rank] += 1;
                ptr = ptr->next;
            } while (ptr != father->firstChild);

            if (m_heap->next == m_heap) { // Father is the only root
                m_nodesAtRank[father->rank] -= 1;
                m_heap = father->firstChild;
                m_heap = findMin();
            } else if (m_heap->next != m_heap) { // If multiple roots
                removeFromRoot(father);
                // Connect both end
                father->lastChild->next = father->next;
                father->prev->next = father->firstChild;
                father->firstChild->prev = father->prev;
                father->next->prev = father->lastChild;
            }

        } else {                          // Only one child
            if (m_heap->next == m_heap) { // Only one root
                m_heap = nullptr;
                m_nodesAtRank[father->rank] -= 1;
            } else { // If multiple roots
                removeFromRoot(father);
            }
            addToRootList(father->firstChild);
        }
    }

    void addChild(Node* f, Node* n) {
        assert(!n->parent);
        n->parent = f;        // Change parent
        if (!f->firstChild) { // If f is a node
            f->firstChild = f->lastChild = n->next = n->prev = n;
        } else { // If f is a tree
            n->next = f->firstChild;
            n->prev = f->lastChild;
            f->lastChild->next = n;
            f->firstChild->prev = n;
            f->lastChild = n;
        }
        if (n->rank >= f->rank) {
            m_nodesAtRank[f->rank] -= 1;
            f->rank = n->rank + 1;
            m_nodesAtRank[f->rank] += 1;
        }
    }

    void merge(Node* n1, Node* n2) {
        assert(n1 != n2);
        assert(!n1->parent && !n2->parent);
        if (m_comparator(n2->value, n1->value)) {
            std::swap(n1, n2); // Make sure n1 is lowest value
        }
        removeFromRoot(n2);
        addChild(n1, n2);
        // assert(m_lastNode->next == m_firstNode);
    }

    void consolidate() {
        for (int i = 0; i < static_cast<int>(m_nodesAtRank.size()); ++i) {
            while (m_nodesAtRank[i] > 1) {
                // Search first root at rank i
                Node* r1 = m_heap;
                while (r1->rank != i) {
                    r1 = r1->next;
                }
                // Search second root at rank i
                Node* r2 = r1->next;
                while (r2->rank != i) {
                    r2 = r2->next;
                }
                merge(r1, r2);
            }
        }
    }
};

#endif