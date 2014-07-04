#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "typedefs.h"
#include "matrix.h"

class Board : public matrix<Square>
{
public:
    Board();
    Board(cv::Mat& image, Lines sortedHorizontalLines, Lines sortedVerticalLines);

    std::vector<int> getRowTypes();
    std::vector<int> getColTypes();

    void draw(cv::Mat& image);

private:
    std::vector<int> rowTypes;
    std::vector<int> colTypes;

    void determineRowTypes();
    void determineColTypes();
};



#endif // GAMEBOARD_H
