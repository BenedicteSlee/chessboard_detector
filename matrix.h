#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

// Class stores element in a std::vector
template <class T>
class matrix
{
public:
    matrix();

    T getElement(int row, int col);

    T& getElementRef(int rowIdx, int colIdx);

    std::vector<T> getRow(int rowIdx);

    std::vector<T> getCol(int colIdx);

    void addRow(std::vector<T> row);

    void addCol(std::vector<T> col);

private:
    std::vector<T> elements;

    int nRows;
    int nCols;

    int getIndex(int row, int col);



};

#endif // MATRIX_H
