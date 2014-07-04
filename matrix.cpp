#include "matrix.h"

template <typename T>
matrix<T>::matrix()
{

}

template <typename T>
T matrix<T>::getElement(int row, int col)
{
    int idx = getIndex(row, col);

    T element = elements.at(idx);
    return element;
}

template <typename T>
T &matrix<T>::getElementRef(int rowIdx, int colIdx){
    int idx = getIndex(rowIdx, colIdx);

    if (idx > (int)elements.size()-1){
        std::invalid_argument("Trying to access squares beyond number of elements");
    }
    T& element = elements.at(idx);
    return element;
}

template <typename T>
std::vector<T> matrix<T>::getRow(int rowIdx)
{

    if (rowIdx > nRows){
        throw std::invalid_argument("row index > number of rows in matrix");
    }

    int idx1 = rowIdx * nCols;
    int idx2 = idx1 + nCols - 1;

    std::vector<T> row(&elements[idx1], &elements[idx2]);
    return row;
}

template <typename T>
std::vector<T> matrix<T>::getCol(int colIdx)
{
    if (colIdx > nCols){
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
void matrix<T>::addRow(std::vector<T> row)
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
void matrix<T>::addCol(std::vector<T> col)
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
