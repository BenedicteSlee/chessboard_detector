#ifndef MATRIX_H
#define MATRIX_H

#include "square.h" // why do i need to include this. If not get incomplete type error

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

// Class stores element in a std::vector
template <class T>
class matrix
{
public:
    matrix();
    matrix(size_t nRows, size_t nCols, T initval);
    matrix(size_t nRows, size_t nCols);

    matrix<T> operator+(const matrix<T> m);

    //void copyTo(matrix<T> other) const;

    T getElement(size_t row, size_t col) const;
    T getElement(size_t index) const;


    const T& getRef(size_t rowIdx, size_t colIdx) const;
    const T& getRef(size_t index) const;
    const std::vector<T>& getRefs() const;

    void setElement(size_t row, size_t col, T element);

    size_t getNumCols() const {return nCols;}
    size_t getNumRows() const {return nRows;}

    std::vector<T> getRow(size_t rowIdx) const;

    std::vector<T> getCol(size_t colIdx) const;

    void appendRow(std::vector<T> row);
    void prependRow(std::vector<T> row);

    void appendCol(std::vector<T> col);
    void prependCol(std::vector<T> col);

    void addToElement(size_t row, size_t col, T increment);
    void addToCol(size_t col, std::vector<T> increments);
    void addToRow(size_t row, std::vector<T> increments);

    bool removeColRequest(size_t idx);
    std::vector<size_t> removeColsRequest(std::vector<size_t> cols);

    bool removeRowRequest(size_t idx);
    std::vector<size_t> removeRowsRequest(std::vector<size_t> rows);
    void removeRowsAgressive(std::vector<size_t> rows);

protected:
    std::vector<T> elements;

    size_t nRows;
    size_t nCols;

    size_t getIndex(size_t row, size_t col) const;
    std::pair<size_t,size_t> getRowCol(size_t index) const;

    bool getSmartIndex(size_t row, size_t col, size_t &idx) const;
};

template <typename T>
matrix<T>::matrix()
{
    nCols = 0;
    nRows = 0;
}

template <typename T>
matrix<T>::matrix(size_t nRows, size_t nCols, T initval)
{
    this->nCols = nCols;
    this->nRows = nRows;

    std::vector<T> tmp(nRows * nCols, initval);
    elements = tmp;

    std::cout << "Calling matrix constructor" << std::endl;

}

template <typename T>
matrix<T>::matrix(size_t nRows, size_t nCols)
{
    this->nCols = nCols;
    this->nRows = nRows;

    std::vector<T> tmp(nRows * nCols);
    elements = tmp;

}

template <typename T>
matrix<T> matrix<T>::operator+(const matrix<T> m){
    if (m.nRows != this->nRows || m.nCols != this->nCols){
        throw std::invalid_argument("Matrices must be of same size");
    }

    matrix<T> m2;
    m2.elements.reserve(nCols * nRows);
    m2.nCols = this->nCols;
    m2.nRows = this->nRows;

    for (size_t i = 0; i < this->elements.size(); i++){
        m2.elements.push_back(this->elements.at(i) + m.elements.at(i));
    }
    return m2;
}

/*template <typename T>
void matrix<T>::copyTo(matrix<T> other) const{
    other.elements = elements;
    other.nCols = nCols;
    other.nRows = nRows;
}*/

template <typename T>
T matrix<T>::getElement(size_t row, size_t col) const
{
    size_t idx = getIndex(row, col);

    T element = elements.at(idx);
    return element;
}

template <typename T>
T matrix<T>::getElement(size_t index) const
{
    if (index >= elements.size())
        throw std::out_of_range("Matrix has " + std::to_string(elements.size()) + " elements");
    return elements.at(index);
}

template <typename T>
const T &matrix<T>::getRef(size_t rowIdx, size_t colIdx) const
{
    size_t idx = getIndex(rowIdx, colIdx);

    if (idx > elements.size()-1){
        throw std::out_of_range("Out of range!");
    }
    const T& ref = elements.at(idx);
    return ref;
}

template <typename T>
const T &matrix<T>::getRef(size_t index) const{
    if (index >= elements.size())
        throw std::out_of_range("Out of range");
    const T& ref = elements.at(index);
    return ref;
}

template <typename T>
const std::vector<T> &matrix<T>::getRefs() const
{
    const std::vector<T>& refs = elements;
    return refs;
}

template <typename T>
void matrix<T>::setElement(size_t row, size_t col, T element){
    if (row > nRows || col > nCols){
        throw std::out_of_range("Out of range!");
    }

    size_t idx = getIndex(row, col);
    elements.at(idx) = element;
}

template <typename T>
void matrix<T>::addToElement(size_t row, size_t col, T increment){
    if (row > nRows || col > nCols){
        throw std::out_of_range("Out of range!");
    }

    size_t idx = getIndex(row, col);
    elements.at(idx) += increment;
}

template <typename T>
void matrix<T>::addToRow(size_t row, std::vector<T> increments){
    if (increments.size() != nCols){
        throw std::out_of_range("Row must be of length " + std::to_string(nCols));
    }

    for (size_t col = 0; col < increments.size(); col++){
        size_t idx = getIndex(row, col);
        elements.at(idx) += increments.at(col);
    }
}

template <typename T>
void matrix<T>::addToCol(size_t col, std::vector<T> increments){
    if (increments.size() != nRows){
        throw std::out_of_range("Col must be of length " + std::to_string(nRows));
    }

    for (size_t row = 0; row < increments.size(); row++){
        size_t idx = getIndex(row, col);
        elements.at(idx) += increments.at(row);
    }
}

template <typename T>
std::vector<T> matrix<T>::getRow(size_t rowIdx) const
{

    if (rowIdx >= nRows){
        throw std::invalid_argument("row index > number of rows in matrix");
    }
    if (elements.size() == 0){
        throw std::invalid_argument("empty matrix");
    }

    size_t idx1 = rowIdx * nCols;
    size_t idx2 = idx1 + nCols;

   std::vector<T> row(&elements[idx1], &elements[idx2]);
    return row;
}

template <typename T>
std::vector<T> matrix<T>::getCol(size_t colIdx) const
{
    if (colIdx >= nCols){
        throw std::invalid_argument("col index > number of columns in matrix");
    }

    std::vector<T> column(nRows);
    for (size_t i = 0; i < nRows; i++){

        size_t elementIdx = colIdx + nCols * i;
        T element = elements.at(elementIdx);
        column.at(i) = element;
    }

    return column;
}

template <typename T>
void matrix<T>::appendRow(std::vector<T> row)
{
    if (row.empty())
        return;

    if (row.size() != nCols){
        std::invalid_argument("This row has the wrong length for this matrix");
    }

    if (elements.empty()){ // then this is the first row added, and it will determine number of columns
        nCols = row.size();
    }

    for (size_t i = 0; i < row.size(); i++){
        elements.push_back(row.at(i));
    }

    nRows++;

}

template <typename T>
void matrix<T>::prependRow(std::vector<T> row){
    if (row.empty())
        return;
    if (row.size() != nCols){
        std::invalid_argument("This row has the wrong length for this matrix");
    }
    
    if (elements.empty()){
        elements = row;
        nCols = row.size();

    } else {
        elements.insert(elements.begin(), row.begin(), row.end());
    }

    nRows++;

}

template <typename T>
void matrix<T>::appendCol(std::vector<T> col)
{
    if (col.empty())
        return;


    if (col.size() != nRows){
        std::invalid_argument("This column has the wrong length for this matrix");
    }

    if (elements.empty()){
        elements = col;
        nCols = 1;
        nRows = col.size();
        return;
    }

    std::vector<T> elementsOld = elements; //std::vector = gives a copy
    elements.clear();

    size_t j = 0;
    for (size_t i = 0; i < elementsOld.size(); i++){

        elements.push_back(elementsOld.at(i));

        bool doInsert = (i+1) % nCols == 0;
        if (doInsert){
            elements.push_back(col.at(j));
            j++;
        }
    }

    nCols++;
}

template <typename T>
void matrix<T>::prependCol(std::vector<T> col){
    if (col.empty())
        return;


    if (col.size() != nRows){
        std::invalid_argument("This column has the wrong length for this matrix");
    }

    if (elements.empty()){
        elements = col;
        nCols = 1;
        nRows = col.size();
        return;
    }

    std::vector<T> elementsOld = elements; //std::vector = gives a copy
    elements.clear();

    size_t j = 0;
    for (size_t i = 0; i < elementsOld.size(); i++){
        bool doInsert = i % nCols == 0;
        if (doInsert){
            elements.push_back(col.at(j));
            j++;
        }
        elements.push_back(elementsOld.at(i));
    }

    nCols++;
}

template <typename T>
bool matrix<T>::removeRowRequest(size_t row){
    if (nRows == 1){
        throw std::invalid_argument("This matrix only has one row left, will not remove it");
    }

    if (nRows == 0){
        throw std::invalid_argument("This matrix is empty");
    }
    if (row != 0 && row != nRows-1){
        std::cout << "Request to remove row " << row << " denied, can only remove first and last row" << std::endl;
        return false;
    }

    if (row > nRows-1){
        std::cout << "Request to remove row " << row << " denied, this matrix has only " << nRows << " rows." << std::endl;
        return false;
    }

    if (row == 0){
        elements.erase(elements.begin(), elements.begin() + nCols);
    }

    else if (row == nRows - 1){
        elements.erase(elements.end() - nCols, elements.end());
    }

    nRows--;
    std::cout << "Removed row: " << row << std::endl;
    return true;
}

template <typename T>
std::vector<size_t> matrix<T>::removeRowsRequest(std::vector<size_t> rows){
    if (rows.empty()){
        throw std::invalid_argument("Input vector is empty");
    }

    std::sort(rows.begin(), rows.end());

    std::vector<size_t> isRemoved;
    //remove from top
    if (rows[0] == 0){
        size_t idx = 0;
        while (rows[idx] == idx){
            bool check = removeRowRequest(0); // remove the first row
            if (check)
                isRemoved.push_back(idx);
            idx++;
        }
    }

    //remove from bottom
    size_t decrement = isRemoved.size(); // board may have shrunk in previous block. If previously wanted row 9 to be removed, this may now be row 8
    size_t rowsIdx = rows.size()-1;
    size_t idxRequested = rows[rowsIdx] - decrement;
    size_t idxAllowed = nRows-1;
    while (idxRequested == idxAllowed){
        bool check = removeRowRequest(nRows-1); // remove the last row
        if (check)
            isRemoved.push_back(idxRequested);

        std::cout << rowsIdx << std::endl;
        idxRequested = rows[--rowsIdx] - decrement;
        std::cout << rowsIdx << std::endl;
        idxAllowed--;

    }

    return isRemoved;
}

template <typename T>
void matrix<T>::removeRowsAgressive(std::vector<size_t> rows){
    if (rows.empty()){
        throw std::invalid_argument("Input vector is empty");
    }
    // TODO
}

template <typename T>
bool matrix<T>::removeColRequest(size_t col){
    if (nCols == 1){
        throw std::invalid_argument("This board only has one column left, will not remove it");
    }

    if (nCols == 0){
        throw std::invalid_argument("This matrix is empty");
    }

    if (col != 0 && col != nCols-1){
        std::cout << "Request to remove col " << col << " denied, can only remove first and last col" << std::endl;
        return false;
    }

    if (col > nCols-1){
        std::cout << "Request to remove col " << col << " denied, this matrix has only " << nCols << " columns."  <<std::endl;
        return false;
    }

    std::vector<T> newElements(elements.size() - nRows);

    size_t count = 0;
    for (size_t i = 0; i < elements.size(); i++){
        size_t thiscol = i % nCols;
        if (thiscol != col){
            newElements[count++] = elements[i];
        }
    }
    elements.clear();
    elements = newElements;
    nCols--;
    std::cout << "Removed column: " << col << std::endl;
    return true;
}

template <typename T>
std::vector<size_t> matrix<T>::removeColsRequest(std::vector<size_t> cols){
    if (cols.empty()){
        throw std::invalid_argument("Input vector is empty");
    }
    std::vector<size_t> isRemoved;

    std::sort(cols.begin(), cols.end());

    //remove from left
    if (cols[0] == 0){
        size_t idx = 0;
        while (cols[idx] == idx){
            bool check = removeColRequest(0);
            if (check)
                isRemoved.push_back(idx);
            idx++;
        }
    }

    //remove from right
    size_t decrement = isRemoved.size(); // board may have shrunk in previous block. If previously wanted col 9 to be removed, this may now be col 8
    size_t colsIdx = cols.size()-1;
    size_t idxRequested = cols[colsIdx] - decrement;
    size_t idxAllowed = nCols-1;
    while (idxRequested == idxAllowed){
        bool check = removeColRequest(nCols-1);
        if (check)
            isRemoved.push_back(idxRequested);
        idxRequested = cols[colsIdx--] - decrement;
        idxAllowed--;
    }
    return isRemoved;
}

template <typename T>
bool matrix<T>::getSmartIndex(size_t row, size_t col, size_t &idx) const
{
    if (row < 0 || row >= nRows || col < 0 || col >= nCols){
        return false;
    }
    idx = row * nCols + col;
    return true;
}

template <typename T>
size_t matrix<T>::getIndex(size_t row, size_t col) const
{
    return row * nCols + col;
}

template <typename T>
std::pair<size_t, size_t> matrix<T>::getRowCol(size_t index) const{
    std::pair<size_t,size_t> result;
    result.first = index / nCols;
    result.second = index % nCols;
    return result;
}

#endif // MATRIX_H
