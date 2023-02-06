#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cassert>
#include <iostream>
#include <vector>

template <typename T, typename Container = std::vector<T>>
class Matrix {
    // template <typename V>
    // friend std::ostream& operator<<(std::ostream&, const Matrix<V>&);

  private:
    int m_nbRow;
    int m_nbColumn;
    Container m_matrix;

  public:
    Matrix(const int _nbRow, const int _nbColumn, const T& _value = T())
        : m_nbRow(_nbRow)
        , m_nbColumn(_nbColumn)
        , m_matrix(m_nbRow * m_nbColumn, _value) {
        assert(!m_matrix.empty());
    }

    Matrix(const Matrix& _other) = default;
    Matrix(Matrix&& _other) noexcept = default;
    Matrix& operator=(const Matrix& _other) = default;
    Matrix& operator=(Matrix&& _other) noexcept = default;
    ~Matrix() = default;

    int size1() const { return m_nbRow; }

    int size2() const { return m_nbColumn; }

    const T& operator()(const int _i, const int _j) const {
        assert(0 <= _i && _i < m_nbRow);
        assert(0 <= _j && _j < m_nbColumn);
        return m_matrix[m_nbRow * _j + _i];
    }

    T& operator()(const int _i, const int _j) {
        assert(0 <= _i && _i < m_nbRow);
        assert(0 <= _j && _j < m_nbColumn);
        return m_matrix[m_nbRow * _j + _i];
    }

    void fill(const T& _value) {
        std::fill(m_matrix.begin(), m_matrix.end(), _value);
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& _out, const Matrix<T>& _mat) {
    for (int i = 0; i < _mat.size1(); ++i) {
        for (int j = 0; j < _mat.size2(); ++j) {
            _out << _mat(i, j) << ' ';
        }
        _out << '\n';
    }
    return _out;
}

template <typename T>
bool operator==(const Matrix<T>& _m1, const Matrix<T>& _m2) {
    if (_m1.size1() != _m2.size1() || _m1.size2() != _m2.size2()) {
        return false;
    }
    for (int i = 0; i < _m1.size1(); ++i) {
        for (int j = 0; j < _m1.size2(); ++j) {
            if (_m1(i, j) != _m2(i, j)) {
                return false;
            }
        }
    }
    return true;
}

#endif
