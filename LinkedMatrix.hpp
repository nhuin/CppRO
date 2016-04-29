#ifndef LINKEDMATRIX_H
#define LINKEDMATRIX_H

#include <vector>
#include <cassert>

template<typename T>
class LinkedMatrix {		
public:
	class Cell {
		friend class LinkedMatrix;
		private:
			Cell(int i, int j, const T& value, Cell* nextOnRow, Cell* nextOnCol) :
				i(i), j(j), nextOnRow(nextOnRow), nextOnCol(nextOnCol), value(value)
			{}

			Cell(Cell&&) = delete;
			Cell& operator=(Cell&&) = delete;

			Cell(const Cell&) = delete;
			Cell& operator=(const Cell&) = delete;

			int i;
			int j;
			Cell* nextOnRow;
			Cell* nextOnCol;
		public:
			T value;
			int getRow() const { return i; }
			int getColumn() const { return j; }
			Cell* getNextOnCol() const { return nextOnCol; }
			Cell* getNextOnRow() const { return nextOnRow; }
	};

	LinkedMatrix(int _nbRow, int _nbCol) : 
		m_rowHeads(_nbRow, nullptr),
		m_colHeads(_nbCol, nullptr)
	{}
		
	~LinkedMatrix() {
		for(int i = 0; i < (int) m_rowHeads.size(); ++i) {
			Cell* nextCell = m_rowHeads[i];
			Cell* toDel;
			while(nextCell) {
				toDel = nextCell;
				nextCell = toDel->nextOnRow;
				delete toDel;
			}
		}
	}

	LinkedMatrix(LinkedMatrix&&) = default; 
	LinkedMatrix& operator=(LinkedMatrix&&) = default;

	LinkedMatrix(const LinkedMatrix& _lm) : 
		m_rowHeads(_lm.m_rowHeads.size(), nullptr),
		m_colHeads(_lm.m_colHeads.size(), nullptr)
	{
		// @TODO: Improve
		for(int s = 0; s < _lm.rowSize(); ++s) {
    		for(auto cell = _lm.m_rowHeads[s]; cell; cell = cell->nextOnRow) {
    			this->add(s, cell->getColumn(), cell->value);
    		}
    	}
	}

	LinkedMatrix& operator=(const LinkedMatrix& _lm)
	{
		// @TODO: Improve
		for(int s = 0; s < rowSize(); ++s) {
    		while(m_rowHeads[s]) {
    			this->remove(m_rowHeads[s]->getRow(), m_rowHeads[s]->getColumn());
    		}
    	}

		for(int s = 0; s < _lm.rowSize(); ++s) {
    		for(auto cell = _lm.m_rowHeads[s]; cell; cell = cell->nextOnRow) {
    			this->add(s, cell->getColumn(), cell->value);
    		}
    	}
    	return *this;

	}
	
	int rowSize() const { return m_rowHeads.size(); }
	int columnSize() const { return m_colHeads.size(); }

	Cell* find(int i, int j) {
		Cell* cell =  m_rowHeads[i];
		while(cell && cell->j <= j) {
			if(cell->j == j) {
				return cell;
			}
			cell = cell->nextOnRow;
		}
		return nullptr;
	}

	const Cell* find(int i, int j) const {
		Cell* cell =  m_rowHeads[i];
		while(cell && cell->j <= j) {
			if(cell->j == j) {
				return cell;
			}
			cell = cell->nextOnRow;
		}
		return nullptr;
	}
	
	void add(int i, int j, const T& value) {
		assert(i < (int) m_rowHeads.size());
		assert(j < (int) m_colHeads.size());
		// Add on row
		Cell* newCell;
		if(!m_rowHeads[i]) {
			newCell = m_rowHeads[i] = new Cell(i, j, value, nullptr, nullptr);
		} else {
			Cell* ptr = m_rowHeads[i];
			Cell* last = nullptr;
			while(ptr && ptr->j < j) {
				last = ptr;
				ptr = ptr->nextOnRow;
			}
			if(ptr && ptr->j == j) {
				ptr->value = value;
				return;
			}
			if(last == nullptr) {
				newCell = new Cell(i, j, value, m_rowHeads[i], nullptr);
				m_rowHeads[i] = newCell;
			} else {
				newCell = new Cell(i, j, value, ptr, nullptr);
				last->nextOnRow = newCell;
			}
		}
		
		// Add on column
		if(!m_colHeads[j]) {
			m_colHeads[j] = newCell;
		} else {
			Cell* ptr = m_colHeads[j];
			Cell* last = nullptr;
			while(ptr && ptr->i < i) {
				last = ptr;
				ptr = ptr->nextOnCol;
			}
			
			if(last == nullptr) {
				newCell->nextOnCol = m_colHeads[j];
				m_colHeads[j] = newCell;
			} else {
				newCell->nextOnCol = ptr;
				last->nextOnCol = newCell;
			}
		}
	}
	
	void remove(int i, int j) {
		assert(i < (int) m_rowHeads.size());
		assert(j < (int) m_colHeads.size());

		Cell* ptr = m_rowHeads[i];
		Cell* last = nullptr;
		while(ptr && ptr->j != j) {
			last = ptr;
			ptr = ptr->nextOnRow;
		}
		assert(ptr);
		if(last == nullptr) { // On head of row
			m_rowHeads[i] = ptr->nextOnRow;
		} else {
			last->nextOnRow = ptr->nextOnRow;
		}
		
		ptr = m_colHeads[j];
		last = nullptr;
		while(ptr && ptr->i != i) {
			last = ptr;
			ptr = ptr->nextOnCol;
		}
		assert(ptr);
		if(last == nullptr) { // On head of column
			m_colHeads[j] = ptr->nextOnCol;
		} else {
			last->nextOnCol = ptr->nextOnCol;
		}

		delete ptr;
	}

	Cell* begin_row(int i) {
		return m_rowHeads[i];
	}

	const Cell* begin_row(int i) const {
		return m_rowHeads[i];
	}

	Cell* begin_column(int i) {
		return m_colHeads[i];
	}

	const Cell* begin_column(int i) const {
		return m_colHeads[i];
	}

	private:
		std::vector<Cell*> m_rowHeads;
		std::vector<Cell*> m_colHeads;

	template<class N>
	friend std::ostream& operator<<(std::ostream& _out, LinkedMatrix<N>& _lm);
};

template<typename T>
std::ostream& operator<<(std::ostream& _out, LinkedMatrix<T>& _lm) {
	for(int i = 0; i < _lm.rowSize(); ++i) {
		if(_lm.begin_row(i)) {
			_out << "Row " << i << ":";
		}
		for(auto cell = _lm.begin_row(i); cell; cell = cell->getNextOnRow()) {
			_out << '(' << cell->getRow() << ", " << cell->getColumn() << "): " << cell->value << '\t';
		}
		if(_lm.begin_row(i)) {
			_out << std::endl;
		}
	}
	return _out;
}
#endif