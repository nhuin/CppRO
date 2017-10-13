#ifndef FIXEDSIZEALLOCATOR_HPP
#define FIXEDSIZEALLOCATOR_HPP

#include <cassert>
#include <vector>
#include <numeric>

template <typename T>
class FixedSizeAllocator {
  private:
    int m_size;
    std::vector<T> m_objects;

    std::vector<T*> m_allocation;
    typename std::vector<T*>::reverse_iterator m_ptrAlloc;
    

  public:
    explicit FixedSizeAllocator(int _size)
        : m_size(_size)
        , m_objects(m_size)
        , m_allocation(m_size)
        , m_ptrAlloc(m_allocation.rbegin()) {
        std::iota(m_allocation.begin(), m_allocation.end(), &m_objects);
        m_ptrAlloc = m_size - 1;
    }

    ~FixedSizeAllocator() = default;

    FixedSizeAllocator(const FixedSizeAllocator& _other) = delete;
    FixedSizeAllocator& operator=(const FixedSizeAllocator&) = delete;
    FixedSizeAllocator(FixedSizeAllocator&&) = delete;
    FixedSizeAllocator& operator=(FixedSizeAllocator&&) = delete;

    bool canAllocate() const {
        return m_ptrAlloc != m_allocation.rend();
    }

    T* allocate() {
        assert(canAllocate());
        return *(m_ptrAlloc--);
    }

    void deallocate(T* _obj) {
        m_allocation[++m_ptrAlloc] = _obj;
    }

    void deallocateAll() {
        for (int i = 0; i < m_size; ++i) {
            m_allocation[i] = i;
        }
        m_ptrAlloc = m_size - 1;
    }
};

#endif