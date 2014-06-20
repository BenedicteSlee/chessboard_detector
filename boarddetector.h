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

    Lines get_hlinesSorted();
    Lines get_vlinesSorted();
    Squares get_PossibleSquares();

private:
    void categorizeLines();
    void findVanishingPoint();
    void findIntersections();
    void createPossibleSquares();

    cv::Mat image;
    std::vector<Line> lines;
    std::vector<double> slopes;
    cv::Point vanishingPoint;
    std::vector<int> hlines;
    std::vector<int> vlines;
    std::vector<int> hlinesSorted;
    std::vector<int> vlinesSorted;
    Squares possibleSquares;
};

#endif // POSTPROCESS_H
