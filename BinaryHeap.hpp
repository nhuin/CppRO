#ifndef BINARYHEAP_HPP
#define BINARYHEAP_HPP

#include <algorithm>

template <typename T, typename Comparator = std::less<T>>
class BinaryHeap {

  public:
    class Handle {
        explicit Handle(const int i)
            : index(i){};
        int index;
        friend BinaryHeap;
    };

  private:
    struct Node {
        Node(const T& _object, Handle* _index)
            : object(_object)
            , handle(_index){};
        T object;
        Handle* handle;

        void changeIndex(int index) { handle->index = index; }
    };

  public:
    explicit BinaryHeap(int _maxSize, Comparator _comparator = Comparator())
        : m_maxSize(_maxSize)
        , m_array(static_cast<Node*>(malloc(sizeof(Node) * m_maxSize)))
        , m_handle(static_cast<Handle*>(malloc(sizeof(Handle) * m_maxSize)))
        , m_nbElements(0)
        , m_comparator(std::move(_comparator)) {}

    ~BinaryHeap() {
        for (int i = 0; i < m_nbElements; ++i) {
            m_array[i].~Node();
        }
        for (int i = 0; i < m_maxSize; ++i) {
            m_handle[i].~Handle();
        }
        free(m_array);
        free(m_handle);
    }

    // Copy
    BinaryHeap(const BinaryHeap& _o)
        : m_maxSize(_o.m_maxSize)
        , m_array(static_cast<Node*>(malloc(sizeof(Node) * m_maxSize)))
        , m_handle(static_cast<Handle*>(malloc(sizeof(Handle) * m_maxSize)))
        , m_nbElements(_o.m_nbElements)
        , m_comparator(_o.m_comparator) {
        for (int i = 0; i < m_maxSize; ++i) {
            new (m_array + i) Node(_o.m_array[i]);
            new (m_handle + i) Handle(_o.m_handle[i]);
        }
    }

    BinaryHeap& operator=(const BinaryHeap& _o) {
        m_maxSize = _o.m_maxSize;
        m_array = static_cast<Node*>(realloc(m_array, sizeof(Node) * m_maxSize));
        m_handle =
            static_cast<Handle*>(realloc(m_handle, sizeof(Handle) * m_maxSize));
        m_nbElements = _o.m_nbElements;
        m_comparator = _o.m_comparator;
        for (int i = 0; i < m_maxSize; ++i) {
            new (m_array + i) Node(_o.m_array[i]);
            new (m_handle + i) Handle(_o.m_handle[i]);
        }
        return *this;
    }

    // Move
    BinaryHeap(BinaryHeap&& _bh) noexcept
        : m_maxSize(std::move(_bh.m_maxSize))
        , m_array(std::move(_bh.m_array))
        , m_handle(std::move(_bh.m_handle))
        , m_nbElements(std::move(_bh.m_nbElements))
        , m_comparator(std::move(_bh.m_comparator)) {
        _bh.m_array = nullptr;
        _bh.m_handle = nullptr;
    }

    BinaryHeap& operator=(BinaryHeap&& _bh) noexcept {
        std::swap(m_maxSize, _bh.m_maxSize);
        std::swap(m_handle, _bh.m_handle);
        std::swap(m_array, _bh.m_array);
        std::swap(m_nbElements, _bh.m_nbElements);
        m_comparator = std::move(_bh.m_comparator);
        return *this;
    }

    const T& top() { return m_array[0].object; }

    bool empty() const { return m_nbElements != 0; }

    Handle* push(const T& object) {
        Handle* h = m_handle + m_nbElements;
        new (h) Handle(m_nbElements);

        Node* n = m_array + m_nbElements;
        new (n) Node(object, m_handle + m_nbElements);

        if (m_nbElements > 0) {
            int obj = m_nbElements;
            int parent = (obj - 1) / 2;
            while (obj != 0 && m_comparator(m_array[obj].object, m_array[parent].object)) {
                std::swap(m_array[obj], m_array[parent]);
                m_array[obj].handle->index = obj;
                m_array[parent].handle->index = parent;
                obj = parent;
                parent = (obj - 1) / 2;
            }
        }
        ++m_nbElements;
        return h;
    }

    void clear() { m_nbElements = 0; }

    void pop() {
        if (m_nbElements > 0) {
            std::swap(m_array[0], m_array[--m_nbElements]);
            m_array[0].handle->index = 0;

            int obj = 0;
            int firstChild = 2 * obj + 1;
            int secondChild = firstChild + 1;
            while (firstChild < m_nbElements) {
                int child = secondChild < m_nbElements
                                ? (m_comparator(m_array[firstChild].object,
                                       m_array[secondChild].object)
                                          ? firstChild
                                          : secondChild)
                                : firstChild;
                if (m_comparator(m_array[child].object, m_array[obj].object)) {
                    std::swap(m_array[child], m_array[obj]);
                    m_array[obj].handle->index = obj;
                    m_array[child].handle->index = child;
                    obj = child;
                    firstChild = 2 * obj + 1;
                    secondChild = firstChild + 1;
                } else {
                    break;
                }
            }
            m_array[m_nbElements].~Node();
        }
    }

    void decrease(Handle* h) {
        int obj = h->index;
        int parent = (obj - 1) / 2;
        while (obj != 0 && m_comparator(m_array[obj].object, m_array[parent].object)) {
            std::swap(m_array[obj], m_array[parent]);
            m_array[obj].handle->index = obj;
            m_array[parent].handle->index = parent;
            obj = parent;
            parent = (obj - 1) / 2;
        }
    }

    void decrease(Handle* h, const T& obj) {
        m_array[h->index].object = obj;
        decrease(h);
    }

    void increase(Handle* h) {
        int obj = h->index;
        int firstChild = 2 * obj + 1;
        int secondChild = firstChild + 1;
        while (firstChild < m_nbElements) {
            // Swap with biggest child
            int child = secondChild < m_nbElements
                            ? (m_comparator(m_array[firstChild].object,
                                   m_array[secondChild].object)
                                      ? firstChild
                                      : secondChild)
                            : firstChild;
            if (m_comparator(m_array[child].object, m_array[obj].object)) {
                std::swap(m_array[child], m_array[obj]);
                m_array[obj].handle->index = obj;
                m_array[child].handle->index = child;
                obj = child;
                firstChild = 2 * obj + 1;
                secondChild = firstChild + 1;
            } else {
                break;
            }
        }
    }

    void increase(Handle* h, const T& obj) {
        m_array[h->index].object = obj;
        increase(h);
    }

  private:
    int m_maxSize;
    Node* m_array;
    Handle* m_handle;
    int m_nbElements;
    Comparator m_comparator;
};
#endif