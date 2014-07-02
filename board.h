#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include "typedefs.h"
#include <vector>
#include <opencv2/opencv.hpp>

class Board
{
public:
    Board();
    Board(cv::Mat& image, Lines sortedHorizontalLines, Lines sortedVerticalLines);
    Board(Board oldBoard, std::vector<int> rowsToRemove, std::vector<int> colsToRemove);

    Square getSquare(int row, int col);
    Square& getSquareRef(int row, int col);
    Squares getRow(int row) const;
    Squares getCol(int col) const;
    //Squares& getColRef(int col);

    int getNumCols(){return nCols;}
    int getNumRows(){return nRows;}

private:
    int nRows;
    int nCols;
    std::vector<Square> squares;
    std::vector<bool> include;

    int getIndex(int row, int col);
};

#endif // BOARD_H
