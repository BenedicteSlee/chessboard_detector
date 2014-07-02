#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <vector>
#include "Line.h"
#include "cvutils.h"
#include "corner.h"
#include "typedefs.h"
#include "board.h"

class BoardDetector
{
public:
    ~BoardDetector();
    BoardDetector(cv::Mat&,std::vector<Line>);

    Lines get_hlinesSorted();
    Lines get_vlinesSorted();
    Board getPossibleBoard();
    std::vector<Squares> getPossibleSquares2();
    Corners getCorners();

private:
    void categorizeLines();
    void findVanishingPoint();
    void createCorners();
    void createPossibleSquares();
    void filterBasedOnSquareSize();
    void determineSquareTypes();
    void determineRowTypes();
    void filterBasedOnRowType();
    void determineColTypes();
    void filterBasedOnColType();

    cv::Mat image;
    cv::Mat image_gray;
    std::vector<Line> lines;
    std::vector<double> slopes;
    cv::Point vanishingPoint;
    std::vector<int> hlinesIdx;
    std::vector<int> vlinesIdx;
    Lines hlinesSorted;
    Lines vlinesSorted;
    Board possibleBoard;
    std::vector<Squares > possibleSquares2;
    std::vector<Squares> possibleSquares3;
    std::vector<std::vector<int>> squareTypes;
    std::vector<int> rowTypes;
    std::vector<int> colTypes;
    Corners corners;

};

#endif // POSTPROCESS_H
