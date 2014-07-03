#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include "typedefs.h"
#include <vector>
#include <opencv2/opencv.hpp>

class Board
{
public:
    Board(){nRows = 0; nCols = 0;}
    Board(cv::Mat& image, Lines sortedHorizontalLines, Lines sortedVerticalLines);
    //Board(Board oldBoard, std::vector<int> rowsToRemove, std::vector<int> colsToRemove);

    void addRow(Squares row);
    void addCol(Squares col);

    Square getSquare(int row, int col);
    Square& getSquareRef(int row, int col);
    Squares getRow(int row);
    Squares getCol(int col);

    int getNumCols(){return nCols;}
    int getNumRows(){return nRows;}

    void draw(cv::Mat& image);

private:
    int nRows;
    int nCols;
    std::vector<Square> squares;
    std::vector<int> squareTypes;

    int getIndex(int row, int col);
};

#endif // BOARD_H
