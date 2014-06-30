#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <vector>
#include "Line.h"
#include "cvutils.h"
#include "corner.h"
#include "typedefs.h"

class BoardDetector
{
public:
    ~BoardDetector();
    BoardDetector(cv::Mat&,std::vector<Line>);

    Lines get_hlinesSorted();
    Lines get_vlinesSorted();
    Squares getPossibleSquares();
    Square getPossibleSquare(int col,int row);
    std::vector<Squares> getPossibleSquares2();
    Corners getCorners();

private:
    void categorizeLines();
    void findVanishingPoint();
    void createCorners();
    void createPossibleSquares();
    void calcSquareSize();

    cv::Mat image;
    cv::Mat image_gray;
    std::vector<Line> lines;
    std::vector<double> slopes;
    cv::Point vanishingPoint;
    std::vector<int> hlines;
    std::vector<int> vlines;
    std::vector<int> hlinesSorted;
    std::vector<int> vlinesSorted;
    Squares possibleSquares;
    std::vector<Squares > possibleSquares2;
    Corners corners;

};

#endif // POSTPROCESS_H
