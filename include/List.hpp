#ifndef LIST_HPP
#define LIST_HPP

0

template <typename T>
class List {
  public:
    List()
        : m_begin(nullptr)
        , m_end(nullptr)
        , m_size() {}

    T& front() { return m_begin->value; }
    const T& front() const { return m_begin->value; }

    T& back() { return m_end->value; }
    const T& back() const { return m_end->value; }

    void push_front(const T& _value) {
        m_begin->prev = new Node{_value, nullptr, nullptr};
        m_end = m_end->next;
        ++m_size;
    }

    void push_back(const T& _value) {
        m_end->next = new Node{_value, m_end, nullptr};
        m_end = m_end->next;
        ++m_size;
    }

  private:
    struct Node {
        Node(const T& _value, Node* _prev, Node* _next)
            : value(_value)
            , prev(_prev)
            , next(_next) {}
        T value;
        Node* prev;
        Node* next;
    };

    Node* m_begin;
    Node* m_end;
    int m_size;
};

#endif