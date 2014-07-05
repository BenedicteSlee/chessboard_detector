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
    Corners getCorners();


private:
    void categorizeLines();
    void findVanishingPoint();
    Board filterBasedOnSquareSize(Board& Board);
    Board filterBasedOnRowType(Board& Board, std::vector<int> rowTypes);
    Board filterBasedOnColType(Board& Board, std::vector<int> colTypes);

    cv::Mat image;
    cv::Mat image_gray;
    std::vector<Line> lines;
    std::vector<double> slopes;
    cv::Point vanishingPoint;
    std::vector<int> hlinesIdx;
    std::vector<int> vlinesIdx;
    Lines hlinesSorted;
    Lines vlinesSorted;
    std::vector<std::vector<int>> squareTypes;

    Corners corners;

};

#endif // POSTPROCESS_H
