#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <string>
#include "typedefs.h"
#include "matrix.h"
#include "state.h"

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

private:
    std::vector<int> rowTypes;
    std::vector<int> colTypes;
    std::vector<int> pieceColors;
    std::vector<std::pair<size_t, int>> pieces;
    std::vector<std::pair<size_t, cv::Vec3i>> circles;

    void determineRowTypes();
    void determineColTypes();
    void removeOutOfBounds();

    void detectCircles();
    int determinePieceColorThreshold();
    bool piecesDetected;

};


#endif // GAMEBOARD_H
