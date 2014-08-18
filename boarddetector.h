#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <vector>
#include <string>
#include "Line.h"
#include "cvutils.h"
#include "corner.h"
#include "typedefs.h"
#include "board.h"
#include "remover.h"
#include "report.h"

class BoardDetector
{
public:
    ~BoardDetector();
    BoardDetector(cv::Mat&, std::vector<Line>);

    Lines get_hlinesSorted();
    Lines get_vlinesSorted();
    Corners getCorners();

    Board detect(bool doDraw = false, bool doWrite = false, Report *report = 0);
    void printHoughAfterCategorization(std::string filename);


private:
    void categorizeLines();
    void findVanishingPoint();
    void filterBasedOnSquareSize(Board& Board, Remover& remover, Report *report);
    void filterBasedOnRowType(Board& Board, Remover& remover, Report *report);
    void filterBasedOnColType(Board& Board, Remover& remover, Report *report);
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
