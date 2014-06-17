#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <vector>
#include "Line.h"
#include "cvutils.h"

class BoardDetector
{
public:
    ~BoardDetector();
    BoardDetector(cv::Mat&,std::vector<Line>);

    Squares getPossibleSquares();

private:
    void categorizeLines();
    void findVanishingPoint();
    void findIntersections();
    void createPossibleSquares();

    cv::Mat image;
    std::vector<Line> lines;
    std::vector<double> slopes;
    cv::Point vanishingPoint;
    std::vector<int> horizontalLines;
    std::vector<int> nonHorizontalLines;
    Squares possibleSquares;
};

#endif // POSTPROCESS_H
