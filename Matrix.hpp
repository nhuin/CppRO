#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <new>
#include <cstring>
#include <iostream>
#include <cassert>

template<typename T>
class Matrix {
	template<typename V> friend std::ostream& operator<<(std::ostream&, Matrix<V>);

	private:
		int m_nbRow;
		int m_nbColumn;
		T* m_matrix;

	public:
		Matrix(int _nbRow, int _nbColumn, const T& _value = T()) :
			m_nbRow(_nbRow),
			m_nbColumn(_nbColumn),
			m_matrix((T*) malloc(sizeof(T) * m_nbRow * m_nbColumn))
		{
			assert(m_nbColumn > 0);
			assert(m_nbRow > 0);
			for(int i = 0; i < m_nbRow; ++i) {
				for(int j = 0; j < m_nbColumn; ++j) {
					T* v = m_matrix + (m_nbRow * j + i);
					new(v) T(_value);
				}
			}
		}

		// Copy

		Matrix(const Matrix& _other) :
			m_nbRow(_other.m_nbRow),
			m_nbColumn(_other.m_nbColumn),
			m_matrix((T*) malloc(sizeof(T) * m_nbRow * m_nbColumn))
		{
			for(int i = 0; i < m_nbRow; ++i) {
				for(int j = 0; j < m_nbColumn; ++j) {
					T* v = m_matrix + (m_nbRow * j + i);
					new(v) T(_other.m_matrix[m_nbRow * j + i]);
				}
			}
		}

		Matrix& operator=(const Matrix& _other)
		{
			for(int i = 0; i < m_nbRow; ++i) {
				for(int j = 0; j < m_nbColumn; ++j) {
					m_matrix[m_nbRow * j + i].~T();
				}
			}

			//Copy matrix
			m_nbRow = _other.m_nbRow;
			m_nbColumn = _other.m_nbColumn;
			m_matrix = (T*) realloc(m_matrix, sizeof(T) * m_nbRow * m_nbColumn);
			
			for(int i = 0; i < m_nbRow; ++i) {
				for(int j = 0; j < m_nbColumn; ++j) {
					T* v = m_matrix + (m_nbRow * j + i);
					new(v) T(_other.m_matrix[m_nbRow * j + i]);
				}
			}
			return *this;
		}

		// Move
		Matrix(Matrix&& _other) :
			m_nbRow(std::move(_other.m_nbRow)),
			m_nbColumn(std::move(_other.m_nbColumn)),
			m_matrix(std::move(_other.m_matrix))
		{
			_other.m_matrix = nullptr;
			_other.m_nbRow = 0;
			_other.m_nbColumn = 0;
		}

		Matrix& operator=(Matrix&& _other) {
			std::swap(m_nbRow, _other.m_nbRow);
			std::swap(m_nbColumn, _other.m_nbColumn);
			std::swap(m_matrix, _other.m_matrix);
			return *this;
		}

		~Matrix() {
			for(int i = 0; i < m_nbRow; ++i) {
				for(int j = 0; j < m_nbColumn; ++j) {
					m_matrix[m_nbRow * j + i].~T();
				}
			}
			free(m_matrix);
		}

		void addColumnAndRow(const T& _value = T()){
			// TODO: Exception fail realloc
			++m_nbColumn; ++m_nbRow;
			m_matrix = (T*) realloc(m_matrix, sizeof(T) * m_nbRow * m_nbColumn);

			for(int i = m_nbColumn - 2; i >= 0; --i) {
				for(int j = m_nbRow - 2; j >= 0; --j) {
					T* v = m_matrix + (m_nbRow * j + i);
					new (v) T(m_matrix[(m_nbColumn - 1) * j + i]);
				}
			}

			for(int j = 0; j < m_nbColumn; ++j) {
				int i = m_nbRow - 1;
				T* v = m_matrix + (m_nbRow * j + i);
				new(v) T(_value);
			}
			for(int i = 0; i < m_nbRow; ++i) {
				int j = m_nbColumn - 1;;
				T* v = m_matrix + (m_nbRow * j + i);
				new(v) T(_value);
			}
		}

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

};

template<typename T>
std::ostream& operator<<(std::ostream& _out, Matrix<T> _mat) {
	for(int i = 0; i < _mat.size1(); ++i) {
		for(int j = 0; j < _mat.size2(); ++j) {
			_out << _mat(i, j) << ' ';
		}	
		_out << '\n';
	}
	return _out;
}

template<typename T>
bool operator==(const Matrix<T>& _m1, const Matrix<T>& _m2) {
	if(_m1.size1() != _m2.size1() || _m1.size2() != _m2.size2()) {
		return false;
	} else {
		for(int i = 0; i < _m1.size1(); ++i) {
			for(int j = 0; j < _m1.size2(); ++j) {
				if (_m1(i, j) != _m2(i, j)) {
					return false;
				}
			}	
		}
		return true;
	}
}

#endif