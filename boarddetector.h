#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <vector>
#include "Line.h"
#include "cvutils.h"
#include "corner.h"
#include "typedefs.h"
#include "board.h"
#include "remover.h"




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
    void filterBasedOnSquareSize(Board& Board, Remover& remover);
    void filterBasedOnRowType(Board& Board, std::vector<int> rowTypes);
    void filterBasedOnColType(Board& Board, std::vector<int> colTypes);
    void requestColumnExpansion(Board &board);
    void requestRowExpansion(Board& board);


    cv::Mat image;
    cv::Mat image_gray;
    std::vector<Line> lines;
    std::vector<double> slopes;
    cv::Point2d vanishingPoint;
    std::vector<int> hlinesIdx;
    std::vector<int> vlinesIdx;
    Lines hlinesSorted;
    Lines vlinesSorted;
    std::vector<std::vector<int>> squareTypes;
    Corners corners;

};

#endif // POSTPROCESS_H
