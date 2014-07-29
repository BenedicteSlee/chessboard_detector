#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "typedefs.h"
#include "matrix.h"

class Board : public matrix<Square>
{
public:
    Board(cv::Mat &image_);
    Board(cv::Mat& image_, Lines sortedHorizontalLines, Lines sortedVerticalLines);

    std::vector<int> getRowTypes();
    std::vector<int> getColTypes();
    size_t size(){return nRows * nCols;}

    //int squareId(cv::Points2d point);

    void draw();

    std::pair<int,int> getStatus();

    void expand(Direction dir);

    int squareId(cv::Point2d point);
private:
    cv::Mat& image;
    std::vector<int> rowTypes;
    std::vector<int> colTypes;

    void determineRowTypes();
    void determineColTypes();

    void removeOutOfBounds();
};


#endif // GAMEBOARD_H
