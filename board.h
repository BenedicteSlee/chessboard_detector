#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <string>
#include "typedefs.h"
#include "matrix.h"
#include "state.h"

const static std::vector<int> blackSquareIdx{1,3,5,7, 8,10,12,14, 17,19,21,23, 24,26,28,30, 33,35,37,39, 40,42,44,46, 49,51,53,55, 56,58,60,62};

class Board : public matrix<Square>
{
public:
    Board();

    void initBoard(Lines sortedHorizontalLines, Lines sortedVerticalLines);
    std::vector<int> getRowTypes();
    std::vector<int> getColTypes();
    size_t size(){return nRows * nCols;}

    //int squareId(cv::Points2d point);

    void draw();
    void drawWithPieces();
    void write(std::string filename);
    void writeLayerReport(std::string filename);

    std::pair<int,int> getStatus();

    void expand(Direction dir);

    int squareId(cv::Point2d point);
    void detectPieces();
    State initState();
    void writeImgWithPiecesToGlobal();

private:
    std::vector<int> rowTypes;
    std::vector<int> colTypes;
    std::vector<int> pieceColors;
    std::vector<std::pair<size_t, int>> pieces;
    std::vector<std::pair<size_t, cv::Vec3i>> circles;
    std::vector<Square> blackSquares;

    void setBlackSquares();
    void determineRowTypes();
    void determineColTypes();
    void removeOutOfBounds();

    void detectCircles();
    int determinePieceColorThreshold();
    bool piecesDetected;
};


#endif // GAMEBOARD_H
