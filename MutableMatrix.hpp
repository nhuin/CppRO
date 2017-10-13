#ifndef MUTABLE_MATRIX_HPP
#define MUTABLE_MATRIX_HPP

class MutableMatrix {
public:
  MutableMatrix(int _nbRow, int _nbColumn, const T &_value = T())
      : m_nbRow(_nbRow), m_nbColumn(_nbColumn),
        m_matrix((T *)malloc(sizeof(T) * m_nbRow * m_nbColumn)) {
    for (int i = 0; i < m_nbRow; ++i) {
      for (int j = 0; j < m_nbColumn; ++j) {
        T *v = m_matrix + (m_nbColumn * j + i);
        new (v) T(_value);
      }
    }
  }

  // Copy

  MutableMatrix(const MutableMatrix &_other)
      : m_nbRow(_other.m_nbRow), m_nbColumn(_other.m_nbColumn),
        m_matrix((T *)malloc(sizeof(T) * m_nbRow * m_nbColumn)) {
    for (int i = 0; i < m_nbRow; ++i) {
      for (int j = 0; j < m_nbColumn; ++j) {
        T *v = m_matrix + (m_nbColumn * j + i);
        new (v) T(_other.m_matrix[m_nbColumn * j + i]);
      }
    }
  }

  MutableMatrix &operator=(const MutableMatrix &_other) {
    for (int i = 0; i < m_nbRow; ++i) {
      for (int j = 0; j < m_nbColumn; ++j) {
        m_matrix[m_nbColumn * j + i].~T();
      }
    }

    // Copy matrix
    m_nbRow = _other.m_nbRow;
    m_nbColumn = _other.m_nbColumn;
    m_matrix = (T *)realloc(m_matrix, sizeof(T) * m_nbRow * m_nbColumn);

    for (int i = 0; i < m_nbRow; ++i) {
      for (int j = 0; j < m_nbColumn; ++j) {
        T *v = m_matrix + (m_nbColumn * j + i);
        new (v) T(_other.m_matrix[m_nbColumn * j + i]);
      }
    }
    return *this;
  }

  // Move
  MutableMatrix(MutableMatrix &&_other)
      : m_nbRow(std::move(_other.m_nbRow)),
        m_nbColumn(std::move(_other.m_nbColumn)),
        m_matrix(std::move(_other.m_matrix)) {
    _other.m_matrix = nullptr;
    _other.m_nbRow = 0;
    _other.m_nbColumn = 0;
  }

  MutableMatrix &operator=(MutableMatrix &&_other) {
    std::swap(m_nbRow, _other.m_nbRow);
    std::swap(m_nbColumn, _other.m_nbColumn);
    std::swap(m_matrix, _other.m_matrix);
    return *this;
  }

  ~MutableMatrix() {
    for (int i = 0; i < m_nbRow; ++i) {
      for (int j = 0; j < m_nbColumn; ++j) {
        m_matrix[m_nbColumn * j + i].~T();
      }
    }
    free(m_matrix);
  }

  void addColumnAndRow(const T &_value = T()) {
    // TODO: Exception fail realloc
    ++m_nbColumn;
    ++m_nbRow;
    m_matrix = (T *)realloc(m_matrix, sizeof(T) * m_nbRow * m_nbColumn);
    for (int i = m_nbRow - 2; i >= 0; --i) {
      for (int j = m_nbColumn - 1; j >= 0; --j) {
        T *v = m_matrix + (m_nbColumn * j + i);
        new (v) T(m_matrix[(m_nbColumn - 1) * j + i]);
      }
    }

    for (int j = 0; j < m_nbColumn; ++j) {
      int i = m_nbRow;
      T *v = m_matrix + (m_nbColumn * j + i);
      new (v) T(_value);
    }
    for (int i = 0; i < m_nbRow; ++i) {
      int j = m_nbColumn;
      T *v = m_matrix + (m_nbColumn * j + i);
      new (v) T(_value);
    }
  }

  int size1() const { return m_nbRow; }
  int size2() const { return m_nbColumn; }

  const T &operator()(int i, int j) const {
    return m_matrix[m_nbColumn * j + i];
  }

  T &operator()(int i, int j) { return m_matrix[m_nbColumn * j + i]; }

private:
  int m_nbRow;
  int m_nbColumn;
  T *m_matrix;
};
#endif