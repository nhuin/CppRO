#ifndef LINKEDMATRIX_H
#define LINKEDMATRIX_H

#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
class LinkedMatrix {
  public:
    class Cell {
        friend class LinkedMatrix;

      private:
        Cell(const int _i, const int _j, T _value, Cell* _nextOnRow,
            Cell* _nextOnCol)
            : i(_i)
            , j(_j)
            , nextOnRow(_nextOnRow)
            , nextOnCol(_nextOnCol)
            , value(std::move(_value)) {}

        ~Cell() = default;

        int i;
        int j;
        Cell* nextOnRow;
        Cell* nextOnCol;

      public:
        Cell(Cell&&) = delete;
        Cell& operator=(Cell&&) = delete;
        Cell(const Cell&) = delete;
        Cell& operator=(const Cell&) = delete;

        T value;
        int getRow() const { return i; }

        int getColumn() const { return j; }

        Cell* getNextOnCol() const { return nextOnCol; }

        Cell* getNextOnRow() const { return nextOnRow; }
    };

    LinkedMatrix(const int _nbRow, const int _nbCol)
        : m_rowHeads(_nbRow, nullptr)
        , m_colHeads(_nbCol, nullptr) {}

    ~LinkedMatrix() {
        for (int i = 0; i < static_cast<int>(m_rowHeads.size()); ++i) {
            Cell* nextCell = m_rowHeads[i];
            Cell* toDel;
            while (nextCell) {
                toDel = nextCell;
                nextCell = toDel->nextOnRow;
                delete toDel;
            }
        }
    }

    LinkedMatrix(LinkedMatrix&&) noexcept = default;
    LinkedMatrix& operator=(LinkedMatrix&&) noexcept = default;

    LinkedMatrix(const LinkedMatrix& _lm)
        : m_rowHeads(_lm.m_rowHeads.size(), nullptr)
        , m_colHeads(_lm.m_colHeads.size(), nullptr) {
        // @TODO: Improve
        for (int s = 0; s < _lm.rowSize(); ++s) {
            for (auto cell = _lm.m_rowHeads[s]; cell; cell = cell->nextOnRow) {
                this->add(s, cell->getColumn(), cell->value);
            }
        }
    }

    /**
   * Copy assignment of LinkedMatrix
   */
    LinkedMatrix& operator=(const LinkedMatrix& _lm) {
        // @TODO: Improve
        for (int s = 0; s < rowSize(); ++s) {
            while (m_rowHeads[s]) {
                this->remove(m_rowHeads[s]->getRow(), m_rowHeads[s]->getColumn());
            }
        }

        for (int s = 0; s < _lm.rowSize(); ++s) {
            for (auto cell = _lm.m_rowHeads[s]; cell; cell = cell->nextOnRow) {
                this->add(s, cell->getColumn(), cell->value);
            }
        }
        return *this;
    }

    int rowSize() const { return m_rowHeads.size(); }

    int columnSize() const { return m_colHeads.size(); }

    Cell* find(const int i, const int j) {
        Cell* cell = m_rowHeads[i];
        while (cell && cell->j <= j) {
            if (cell->j == j) {
                return cell;
            }
            cell = cell->nextOnRow;
        }
        return nullptr;
    }

    const Cell* find(const int i, const int j) const {
        Cell* cell = m_rowHeads[i];
        while (cell && cell->j <= j) {
            if (cell->j == j) {
                return cell;
            }
            cell = cell->nextOnRow;
        }
        return nullptr;
    }

    void add(const int i, const int j, const T& value) {
        assert(i < static_cast<int>(m_rowHeads.size()));
        assert(j < static_cast<int>(m_colHeads.size()));
        // Add on row
        Cell* newCell;
        if (!m_rowHeads[i]) { // No Cell on this row, the new cell become the new
                              // head of the row
            newCell = m_rowHeads[i] = new Cell(i, j, value, nullptr, nullptr);
        } else {
            // Search for the first available Cell position
            Cell* ptr = m_rowHeads[i];
            Cell* last = nullptr;
            while (ptr != nullptr && ptr->j < j) {
                last = ptr;
                ptr = ptr->nextOnRow;
            }
            // If there is already a Cell, change value of the cell
            if (ptr != nullptr && ptr->j == j) {
                ptr->value = value;
                return;
            }
            //
            if (last == nullptr) {
                m_rowHeads[i] = newCell = new Cell(i, j, value, m_rowHeads[i], nullptr);
            } else {
                newCell = new Cell(i, j, value, ptr, nullptr);
                last->nextOnRow = newCell;
            }
        }

        // Add on column
        if (!m_colHeads[j]) {
            m_colHeads[j] = newCell;
        } else {
            Cell* ptr = m_colHeads[j];
            Cell* last = nullptr;
            while (ptr && ptr->i < i) {
                last = ptr;
                ptr = ptr->nextOnCol;
            }

            if (last == nullptr) {
                newCell->nextOnCol = m_colHeads[j];
                m_colHeads[j] = newCell;
            } else {
                newCell->nextOnCol = ptr;
                last->nextOnCol = newCell;
            }
        }
    }

    void remove(const int i, const int j) {
        assert(i < (int)m_rowHeads.size());
        assert(j < (int)m_colHeads.size());

        Cell* ptr = m_rowHeads[i];
        Cell* last = nullptr;
        while (ptr && ptr->j != j) {
            last = ptr;
            ptr = ptr->nextOnRow;
        }
        assert(ptr);
        if (last == nullptr) { // On head of row
            m_rowHeads[i] = ptr->nextOnRow;
        } else {
            last->nextOnRow = ptr->nextOnRow;
        }

        ptr = m_colHeads[j];
        last = nullptr;
        while (ptr && ptr->i != i) {
            last = ptr;
            ptr = ptr->nextOnCol;
        }
        assert(ptr);
        if (last == nullptr) { // On head of column
            m_colHeads[j] = ptr->nextOnCol;
        } else {
            last->nextOnCol = ptr->nextOnCol;
        }

        delete ptr;
    }

    Cell* begin_row(const int i) { return m_rowHeads[i]; }

    const Cell* begin_row(const int i) const { return m_rowHeads[i]; }

    Cell* begin_column(const int i) { return m_colHeads[i]; }

    const Cell* begin_column(const int i) const { return m_colHeads[i]; }

    class iterator {
      private:
      public:
    };

  private:
    std::vector<Cell*> m_rowHeads;
    std::vector<Cell*> m_colHeads;

    template <class N>
    friend std::ostream& operator<<(std::ostream& _out, LinkedMatrix<N>& _lm);
};

template <typename T>
std::ostream& operator<<(std::ostream& _out, const LinkedMatrix<T>& _lm) {
    for (int i = 0; i < _lm.rowSize(); ++i) {
        if (_lm.begin_row(i)) {
            _out << "Row " << i << ":";
        }
        for (auto cell = _lm.begin_row(i); cell; cell = cell->getNextOnRow()) {
            _out << '(' << cell->getRow() << ", " << cell->getColumn()
                 << "): " << cell->value << '\t';
        }
        if (_lm.begin_row(i)) {
            _out << '\n';
        }
    }
    return _out;
}
#endif