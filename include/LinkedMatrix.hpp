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
        Cell(const std::size_t _i, const std::size_t _j, T _value, Cell* _nextOnRow,
            Cell* _nextOnCol)
            : i(_i)
            , j(_j)
            , nextOnRow(_nextOnRow)
            , nextOnCol(_nextOnCol)
            , value(std::move(_value)) {}

        ~Cell() = default;

        std::size_t i;
        std::size_t j;
        Cell* nextOnRow;
        Cell* nextOnCol;

      public:
        Cell(Cell&&) = delete;
        Cell& operator=(Cell&&) = delete;
        Cell(const Cell&) = delete;
        Cell& operator=(const Cell&) = delete;

        T value;
        std::size_t getRow() const {
            return i;
        }

        std::size_t getColumn() const {
            return j;
        }

        Cell* getNextOnCol() const {
            return nextOnCol;
        }

        Cell* getNextOnRow() const {
            return nextOnRow;
        }
    };

    LinkedMatrix(const std::size_t _nbRow, const std::size_t _nbCol)
        : m_rowHeads(_nbRow, nullptr)
        , m_colHeads(_nbCol, nullptr) {}

    ~LinkedMatrix() {
        for (std::size_t i = 0; i < m_rowHeads.size(); ++i) {
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
        for (std::size_t s = 0; s < _lm.rowSize(); ++s) {
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
        for (std::size_t s = 0; s < rowSize(); ++s) {
            while (m_rowHeads[s]) {
                this->remove(m_rowHeads[s]->getRow(), m_rowHeads[s]->getColumn());
            }
        }

        for (std::size_t s = 0; s < _lm.rowSize(); ++s) {
            for (auto cell = _lm.m_rowHeads[s]; cell; cell = cell->nextOnRow) {
                this->add(s, cell->getColumn(), cell->value);
            }
        }
        return *this;
    }

    std::size_t rowSize() const {
        return m_rowHeads.size();
    }

    std::size_t columnSize() const {
        return m_colHeads.size();
    }

    Cell* find(const std::size_t i, const std::size_t j) {
        Cell* cell = m_rowHeads[i];
        while (cell && cell->j <= j) {
            if (cell->j == j) {
                return cell;
            }
            cell = cell->nextOnRow;
        }
        return nullptr;
    }

    const Cell* find(const std::size_t i, const std::size_t j) const {
        Cell* cell = m_rowHeads[i];
        while (cell && cell->j <= j) {
            if (cell->j == j) {
                return cell;
            }
            cell = cell->nextOnRow;
        }
        return nullptr;
    }

    void add(const std::size_t i, const std::size_t j, const T& value) {
        assert(i < m_rowHeads.size());
        assert(j < m_colHeads.size());
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

    void remove(const std::size_t i, const std::size_t j) {
        assert(i < m_rowHeads.size());
        assert(j < m_colHeads.size());

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

    Cell* begin_row(const std::size_t i) {
        return m_rowHeads[i];
    }

    const Cell* begin_row(const std::size_t i) const {
        return m_rowHeads[i];
    }

    Cell* begin_column(const std::size_t i) {
        return m_colHeads[i];
    }

    const Cell* begin_column(const std::size_t i) const {
        return m_colHeads[i];
    }

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
    for (std::size_t i = 0; i < _lm.rowSize(); ++i) {
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