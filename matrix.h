#ifndef MATRIX_H
#define MATRIX_H

#include <vector>


// Class stores element in a std::vector
template <class T>
class matrix
{
public:
    matrix();
    matrix(int nRows, int nCols)
    {
        this->nCols = nCols;
        this->nRows = nRows;

        std::vector<T> tmp(nRows * nCols);
        elements = tmp;
    }

    matrix<T> operator+(const matrix<T> m);

    T getElement(int row, int col);

    T& getRef(int rowIdx, int colIdx);

    void setElement(T element, int row, int col);

    int getNumCols(){return nCols;}
    int getNumRows(){return nRows;}

    std::vector<T> getRow(int rowIdx);

    std::vector<T> getCol(int colIdx);

    void appendRow(std::vector<T> row);
    void prependRow(std::vector<T> row);

    void appendCol(std::vector<T> col);
    void prependCol(std::vector<T> col);

protected:
    std::vector<T> elements;

    int nRows;
    int nCols;

private:
    int getIndex(int row, int col);
};

template <typename T>
matrix<T>::matrix()
{
    nCols = 0;
    nRows = 0;
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

template <typename T>
T matrix<T>::getElement(int row, int col)
{
    int idx = getIndex(row, col);

    T element = elements.at(idx);
    return element;
}

template <typename T>
T &matrix<T>::getRef(int rowIdx, int colIdx){
    int idx = getIndex(rowIdx, colIdx);

    if (idx > (int)elements.size()-1){
         throw std::out_of_range("Out of range!");
    }
    T& element = elements.at(idx);
    return element;
}

template <typename T>
void matrix<T>::setElement(T element, int row, int col){
    if (row > nRows || col > nCols){
        throw std::out_of_range("Out of range!");
    }

    int idx = getIndex(row, col);
    elements.at(idx) = element;
}

template <typename T>
std::vector<T> matrix<T>::getRow(int rowIdx)
{

    if (rowIdx >= nRows){
        throw std::invalid_argument("row index > number of rows in matrix");
    }

    int idx1 = rowIdx * nCols;
    int idx2 = idx1 + nCols;

    std::vector<T> row(&elements[idx1], &elements[idx2]);
    return row;
}

template <typename T>
std::vector<T> matrix<T>::getCol(int colIdx)
{
    if (colIdx >= nCols){
        throw std::invalid_argument("col index > number of columns in matrix");
    }

    std::vector<T> column(nRows);
    for (int i = 0; i < nRows; i++){

        int elementIdx = colIdx + nCols * i;
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

    if ((int)row.size() != nCols){
        std::invalid_argument("This row has the wrong length for this matrix");
    }

    for (size_t i = 0; i < row.size(); i++){
        elements.push_back(row.at(i));
    }

    if (nCols == 0){ // then this is the first row added, and it will determine number of columns
        nCols = row.size();
    }
    nRows++;
}

template <typename T>
void matrix<T>::appendCol(std::vector<T> col)
{
    if (col.empty())
        return;


    if ((int)col.size() != nRows){
        std::invalid_argument("This column has the wrong length for this matrix");
    }

    if (elements.empty()){
        elements = col;
        nCols = 1;
        return;
    }

    std::vector<T> elementsOld = elements; //std::vector = gives a copy
    elements.clear();

    int j = 0;
    for (size_t i = 0; i < elementsOld.size(); i++){

        elements.push_back(elementsOld.at(i));

        bool doInsert = (i+1) % nCols == 0;
        if (doInsert){
            elements.push_back(col.at(j));

            j++;

        }
    }

    if (nRows == 0){
        nRows = col.size();
    }
    nCols++;
}

template <typename T>
int matrix<T>::getIndex(int row, int col)
{
    return row * nCols + col;
}


#endif // MATRIX_H
