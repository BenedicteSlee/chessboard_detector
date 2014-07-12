#include <vector>
#include <cmath>
#include <stdexcept>

#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "square.h"
#include "typedefs.h"
#include "board.h"
#include "squareExpander.h"

BoardDetector::~BoardDetector()
{
}

BoardDetector::BoardDetector(cv::Mat& image_, std::vector<Line> lines_)
{
    image = image_;

    if (image.channels() != 1){
        throw std::invalid_argument("Give me a grayscale image");
    }

    lines = lines_;
    //detectChessboardRegion(); // TODO Use template matching to find rough region of chessboard
    categorizeLines();

    Board possibleBoard = Board(image, hlinesSorted, vlinesSorted);

    possibleBoard.draw();
    cv::destroyAllWindows();
    //findVanishingPoint(); // use?
    //removeSpuriousLines(); // TODO Remove lines not belonging to the chessboard

    std::vector<int> rowTypes = possibleBoard.getRowTypes();
    filterBasedOnRowType(possibleBoard, rowTypes);

    possibleBoard.draw();cv::destroyAllWindows();
    std::vector<int> colTypes = possibleBoard.getColTypes();
    filterBasedOnColType(possibleBoard, colTypes);

    possibleBoard.draw();

    filterBasedOnSquareSize(possibleBoard);
    possibleBoard.draw();

    std::pair<int,int> status = possibleBoard.getStatus();

    bool addColumns = false;
    bool addRows = false;

    if (status.first > 0)
        addRows = true;

    while (addRows){
        requestRowExpansion(possibleBoard);
        status = possibleBoard.getStatus();
        possibleBoard.draw();
        if (status.first <= 0)
            addRows = false;
    }

    if (status.second > 0) // cols are missing
        addColumns = true;

    while (addColumns){
        requestColumnExpansion(possibleBoard);
        status  = possibleBoard.getStatus();
        possibleBoard.draw();
        if (status.second <= 0)
            addColumns = false;
    }
}

Lines BoardDetector::get_hlinesSorted()
{
    return hlinesSorted;
}

Lines BoardDetector::get_vlinesSorted()
{
    return vlinesSorted;
}

Corners BoardDetector::getCorners()
{
    return corners;
}

void BoardDetector::categorizeLines(){
    for (size_t i=0; i<lines.size();i++)
    {
        if (lines[i].slope < 0.1 && lines[i].slope > -0.1) // TODO: make numbers dynamic based on iamge size!
        {
            hlinesIdx.push_back((int) i);
        }
        else
        {
            vlinesIdx.push_back((int) i);
        }
    }

    /////////// H O R I Z O N T A L   L I N E S ///////////
    // Sort horizontal lines
    std::vector<std::pair<int,double> > yints(hlinesIdx.size());

    std::pair<int,double> p;
    for (size_t i = 0; i < hlinesIdx.size(); ++i) {
        Line line = lines[hlinesIdx[i]];
        p.first = (int) i;
        p.second = (double) line.yIntercept;
        yints[i] = p;
    }

    std::sort(yints.begin(), yints.end(), cvutils::pairIsLess);
    // Remove duplicate horizontal lines

    std::vector<int> removeIdx1(yints.size());
    for (size_t i = 1; i < yints.size(); ++i) {
        double yint1 = yints[i-1].second;
        double yint2 = yints[i].second;
        double diff =  abs(yint1 - yint2);
        if (diff < 2)
            removeIdx1[i] = 1;
        else
            removeIdx1[i] = 0;
    }

    // Add sorted unique horizontal lines to field
    for (size_t i = 0; i < hlinesIdx.size(); ++i) {
        if (removeIdx1[i] != 1){
            Line line = lines.at(hlinesIdx[yints[i].first]);
            hlinesSorted.push_back(line);
        }
    }

    /////////// V E R T I C A L   L I N E S ///////////
    // Sort vertical lines
    std::vector<std::pair<int,double> > xints(vlinesIdx.size());

    std::pair<int,double> p2;
    for (size_t i = 0; i < vlinesIdx.size(); ++i) {
        Line line = lines[vlinesIdx[i]];
        p2.first = (int) i;
        p2.second = (double) line.xlookup(0, 1);
        xints[i] = p2;
    }

    std::sort(xints.begin(), xints.end(), cvutils::pairIsLess);

    // Remove duplicate vertical lines
    std::vector<int> removeIdx2(xints.size());
    for (size_t i = 1; i < xints.size(); ++i) {
        double xint1 = xints[i-1].second;
        double xint2 = xints[i].second;
        double diff =  abs(xint1 - xint2);
        if (diff < 10)
            removeIdx2[i] = 1;
        else
            removeIdx2[i] = 0;
    }

    // Add sorted unique vertical lines to field
    for (size_t i = 0; i < vlinesIdx.size(); ++i) {
        if (removeIdx2[i] != 1){
            Line line = lines.at(vlinesIdx[xints[i].first]);
            vlinesSorted.push_back(line);
        }
    }
}

void BoardDetector::findVanishingPoint(){
    int numlines = (int) vlinesIdx.size();
    Points2d vpoints;
    vpoints.reserve(numlines*(numlines-1)/2);

    // Calculate intersection between each possible pair of lines
    // TODO: this should be gotten from findIntersections()
    for (int i=0;i<numlines;i++){
        int idx1 = vlinesIdx.at(i);
        Line line1 = lines.at(idx1);
        for (int j=i+1;j<numlines;j++){
            int idx2 = vlinesIdx.at(j);
            Line line2 = lines.at(idx2);
            cv::Point2d vpoint;
            line1.Intersection(line2, vpoint);
            vpoints.push_back(vpoint);
        }
    }

    cv::Point2d meanpoint = cvutils::centerpoint(vpoints);

    std::vector<double> dists(vpoints.size());
    std::vector<double> stdevs(vpoints.size());
    for (size_t i=0;i<vpoints.size();i++){
        dists[i] = cv::norm(vpoints[i] - meanpoint);
    }

    std::vector<double> distsSorted(dists);
    std::sort(distsSorted.begin(),distsSorted.end());
    int quantileSize = vpoints.size()/4;
    double quantile1 = distsSorted[quantileSize-1];
    double quantile2 = distsSorted[quantileSize*3 -1];

    Points2d voters;
    for (size_t i=0;i<vpoints.size();i++){
        if (dists[i] >= quantile1 && dists[i] <= quantile2){
            voters.push_back(vpoints[i]);
        }
    }
    vanishingPoint = cvutils::centerpoint(voters);
}

void BoardDetector::filterBasedOnSquareSize(Board &board)
{
    int nCols = board.getNumCols();
    int nRows = board.getNumRows();

    std::vector<int> hlengths(nCols);
    std::vector<int> vlengths(nRows);

    matrix<int> remove, remove1, remove2;

    std::vector<int> houtliers(nCols), voutliers(nRows);

    // Flag outliers based on horizontal lengths
    for (int i = 0; i < nRows; i++){
        Squares row = board.getRow(i);
        for (size_t j = 0; j < row.size(); j++){
            hlengths.at(j) = row.at(j).getHLength();
        }
        houtliers = cvutils::outliers(hlengths);
        remove1.appendRow(houtliers);
    }

    Board colsRemoved(image);
    std::vector<int> removeIdx;
    for (int i = 0; i < board.getNumCols(); i++){
        std::vector<int> flags = remove1.getCol(i);
        int nkeep = std::count(flags.begin(), flags.end(), 0);
        if (nkeep / (double) nRows < 0.6){ // less than 60% flagged for removal
            board.removeCol(i);
            removeIdx.push_back(i);
        }
    }

    if (!removeIdx.empty()){
        std::cout << "Filter by square size removed columns: ";
        for (size_t i = 0; i < removeIdx.size()-1; i++){
            std::cout << removeIdx.at(i) << ",";
        }
        std::cout << removeIdx.at(removeIdx.size()-1) << std::endl;
    } else {
        std::cout << "Filter by square size did not remove any columns" << std::endl;
    }

    // TODO CORRECT FOR VIEWPOINT EFFECT
    // Flag outliers based on vertical lengths
    for (int i = 0; i < board.getNumCols(); i++){
        Squares col = board.getCol(i);
        for (size_t j = 0; j < col.size(); j++){
            vlengths.at(j) = col.at(j).getVLength();
        }

        //voutliers = cvutils::outliers(vlengths, 0.8);
        for (size_t j = 0; j < vlengths.size(); j++){

            if (j < vlengths.size()-1 && vlengths.at(j) > vlengths.at(j+1))
                voutliers.at(j) = 1;
            else if (j > 1 && vlengths.at(j) > 1.6 * vlengths.at(j-1))
            { // TODO make this more robust
                voutliers.at(j) = 1;
            } else {
                voutliers.at(j) = 0;
            }

        }
        remove2.appendCol(voutliers);
    }

    std::vector<int> removeIdx2;
    for (int i = 0; i < board.getNumRows(); i++){
        std::vector<int> flags = remove2.getRow(i);
        int nkeep = std::count(flags.begin(), flags.end(), 0);
        if (nkeep / (double) colsRemoved.getNumRows() < 0.6){ // less than 60% flagged for removal
            board.removeRow(i);
            removeIdx2.push_back(i);
        }
    }

    if (!removeIdx2.empty()){
        std::cout << "Filter by square size removed rows: ";
        for (size_t i = 0; i < removeIdx2.size()-1; i++){
            std::cout << removeIdx2.at(i) << ",";
        }
        std::cout << removeIdx2.at(removeIdx2.size()-1) << std::endl;
    } else {
        std::cout << "Filter by square size did not remove any rows" << std::endl;
    }

}

void BoardDetector::filterBasedOnRowType(Board& board, std::vector<int> rowTypes)
{

    if (cv::sum(rowTypes)[0] == 0){
        std::cout << "None of the rows seem to be part of a chessboard" << std::endl;
        return;
    }

    std::vector<int> actuallyRemoved;
    std::vector<int> removeIdx;
    for (size_t i = 0; i < rowTypes.size(); i++){
        if (rowTypes.at(i) != 0){
            removeIdx.push_back(i);
        }
        if (!removeIdx.empty()){
            actuallyRemoved = board.removeRows(removeIdx);
        }

        if (!actuallyRemoved.empty())
        {
            std::cout << "Filter by row type removed rows: ";
            for (size_t i = 0; i < actuallyRemoved.size(); i++){
                std::cout << actuallyRemoved.at(i) << ",";
            }
        } else {
            std::cout << "Filter by row type did not remove any rows" << std::endl;
        }
    }
}

void BoardDetector::filterBasedOnColType(Board& board, std::vector<int> colTypes)
{
    if (cv::sum(colTypes)[0] == 0){
        std::cout << "None of the columns seem to be part of a chessboard" << std::endl;
        return;
    }

    std::vector<int> actuallyRemoved;
    std::vector<int> removeIdx;
    for (size_t i = 0; i < colTypes.size(); i++){
        if (colTypes.at(i) != 0){
            removeIdx.push_back(i);
        }
        if (!removeIdx.empty()){
            actuallyRemoved = board.removeCols(removeIdx);
        }

        if (!actuallyRemoved.empty())
        {
            std::cout << "Filter by col type removed cols: ";
            for (size_t i = 0; i < actuallyRemoved.size(); i++){
                std::cout << actuallyRemoved.at(i) << ",";
            }
        } else {
            std::cout << "Filter by col type did not remove any rows" << std::endl;
        }
    }



}

void BoardDetector::requestColumnExpansion(Board& board)
{
    Squares left = board.getCol(0);
    Squares right = board.getCol(board.getNumCols()-1);

    std::vector<int> leftTypes = Square::getSquareTypes(left);
    std::vector<int> rightTypes = Square::getSquareTypes(right);

    int sumLeft = cv::sum(leftTypes)[0];
    int sumRight = cv::sum(rightTypes)[0];

    Direction dir = LEFT;
    if (sumLeft < sumRight)
        dir = RIGHT;

    board.expand(dir);
}

void BoardDetector::requestRowExpansion(Board &board)
{
    Squares top = board.getRow(0);
    Squares bottom = board.getRow(board.getNumRows()-1);

    std::vector<int> topTypes = Square::getSquareTypes(top);
    std::vector<int> bottomTypes = Square::getSquareTypes(bottom);

    int sumTop = cv::sum(topTypes)[0];
    int sumBottom = cv::sum(bottomTypes)[0];

    Direction dir = UP;
    if (sumTop < sumBottom)
        dir = DOWN;

    board.expand(dir);

}


