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
#include "remover.h"
#include "regression.h"
#include "report.h"
#include "global.h"

extern bool global::doDraw;

BoardDetector::~BoardDetector()
{
}

BoardDetector::BoardDetector(std::vector<Line> lines_)
{

    lines = lines_;
    //detectChessboardRegion(); // TODO Use template matching to find rough region of chessboard
    categorizeLines();


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

bool BoardDetector::detect(Board& dst, std::string *reportPath)
{
    std::cout << "Horizontal lines: " << hlinesSorted.size() << std::endl;
    dst.initBoard(hlinesSorted, vlinesSorted);


    if (hlinesSorted.size() < 2 || vlinesSorted.size() < 2)
        return false;

    if (global::doDraw) dst.draw();

    if (reportPath != 0){
        std::string filename1 = *reportPath + "initBoard.png";
        dst.write(filename1);
    }

    // INITIAL PRUNING
    size_t initrows = dst.getNumRows();
    size_t initcols = dst.getNumCols();

    if (initrows > 10){
        std::vector<size_t> prunerows{0,1,2, initrows-3,initrows-2,initrows-1};
        dst.removeRowsRequest(prunerows);
    }
    else if (initrows > 5){
        std::vector<size_t> prunerows{0,1, initrows-2, initrows-1};
        dst.removeRowsRequest(prunerows);
    }

    if (initcols > 10){
        std::vector<size_t> prunecols{0,1,2, initcols-3,initcols-2,initcols-1};
        dst.removeColsRequest(prunecols);
    }
    else if (initcols > 5){
        std::vector<size_t> prunecols{0, 1, initcols-2,initcols-1};
        dst.removeColsRequest(prunecols);
    }

    if (global::doDraw) dst.draw();

    if (reportPath != 0){
        dst.write(*reportPath + "boardAfterPruning.png");
    }

    Remover remover(dst);

    filterBasedOnSquareSize(dst, remover);
    indices colreq1 = remover.getCurrentColRequests();
    indices rowreq1 = remover.getCurrentRowRequests();

    remover.remove();

    if (global::doDraw) dst.draw();

    if (reportPath != 0){
        dst.write(*reportPath + "boardAfterFilterBySize.png");
        // possibleBoard.writeLayerReport(*reportPath + "layerReportAfterFilterBySize.csv");
    }

    filterBasedOnRowType(dst, remover);
    indices colreq2 = remover.getCurrentColRequests();
    indices rowreq2 = remover.getCurrentRowRequests();
    //remover.remove();
    //possibleBoard.draw();

    filterBasedOnColType(dst, remover);
    indices colreq3 = remover.getCurrentColRequests();
    indices rowreq3 = remover.getCurrentRowRequests();

    remover.remove();
    if (global::doDraw) dst.draw();

    if (reportPath != 0){
        dst.write(*reportPath + "boardAfterFilterByType.png");
    }

    filterBasedOnSquareSize(dst, remover);
    remover.remove();
    if (global::doDraw) dst.draw();
    if (reportPath != 0){
        dst.write(*reportPath + "boardAfterFilterBySize2.png");
    }
    std::pair<int,int> status = dst.getStatus();

    bool addColumns = false;
    bool addRows = false;

    if (status.first > 0)
        addRows = true;

    while (addRows){
        requestRowExpansion(dst);
        status = dst.getStatus();
        if (global::doDraw) dst.draw();
        if (status.first <= 0)
            addRows = false;
    }

    if (status.second > 0) // cols are missing
        addColumns = true;

    while (addColumns){
        requestColumnExpansion(dst);
        status  = dst.getStatus();
        if (global::doDraw) dst.draw();
        if (status.second <= 0)
            addColumns = false;
    }

    //const Square& square = possibleBoard.getRef(0);
    //bool check = square.containsPoint(cv::Point2d(200,200));
    return true;
}

void BoardDetector::writeHoughAfterCategorizationToGlobal()
{
    cv::Mat output;
    global::image.copyTo(output);
    if (output.channels() != 3){
        cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
    }

    cv::Scalar col(20,110,255);
    for (size_t i = 0; i < vlinesSorted.size(); i++){
        Line &line = vlinesSorted[i];
        cv::line(output, line.points[0], line.points[1], col);
    }

    for (size_t i = 0; i < hlinesSorted.size(); i++){
        Line &line = hlinesSorted[i];
        cv::line(output, line.points[0], line.points[1], col);
    }

    global::image_hough_mod = output;
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
    std::vector<int> removeIdx1(yints.size(), 0);
    for (size_t i = 1; i < yints.size(); ++i) {
        double yint1 = yints[i-1].second;
        double yint2 = yints[i].second;
        double diff =  abs(yint1 - yint2);
        if (diff < 10)
            removeIdx1[i] = 1;
        else
            removeIdx1[i] = 0;
    }

    // remove false horizontal lines
    std::vector<double> slopes(hlinesIdx.size());
    for (size_t i = 0; i < hlinesIdx.size(); i++){
        slopes[i] = lines[hlinesIdx[i]].slope;
    }
    double meanNoOutliers = cvutils::meanNoOutliers(slopes);
    double tolerance = meanNoOutliers * 0.1;
    // Add sorted unique horizontal lines to field
    for (size_t i = 0; i < hlinesIdx.size(); ++i) {

        if (removeIdx1[i] == 0){
            Line line = lines.at(hlinesIdx[yints[i].first]);
            //if (std::abs(line.slope - meanNoOutliers) <= tolerance)
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
    for (size_t i = 0; i < vlinesIdx.size(); i++) {
        if (removeIdx2[i] == 0){
            int idx1 = xints[i].first;
            int idx2 = vlinesIdx[idx1];
            Line line = lines.at(idx2);
            vlinesSorted.push_back(line);
        }
    }

    // vanishing point

    Lines newVlines = filterBasedOnVanishingPoint(vlinesSorted);

    vlinesSorted = newVlines;

    writeHoughAfterCategorizationToGlobal();
}

Lines BoardDetector::filterBasedOnVanishingPoint(Lines vlines){
    if (vlines.size() == 0){
        throw std::invalid_argument("Vector of lines is empty");
    }

    int numlines = (int) vlines.size();
    int xvpoint; //only need x, y is too volatile
    std::vector<int> vpoints;
    vpoints.reserve(numlines*(numlines-1)/2);

    std::vector<cv::Vec3i> voterInfo;
    // Calculate intersection between each possible pair of lines
    // TODO: this should be gotten from findIntersections()
    for (int i=0;i<vlines.size();i++){
        Line line1 = vlines.at(i);
        for (int j=i+1;j<numlines;j++){
            Line line2 = vlines.at(j);
            cv::Point2d vpoint;
            line1.Intersection(line2, vpoint);
            vpoints.push_back(vpoint.x);
            cv::Vec3i info{i,j,(int) vpoint.x};
            voterInfo.push_back(info);
        }
    }

    int meanx = cv::mean(vpoints)[0];

    std::vector<double> dists(vpoints.size());
    std::vector<double> stdevs(vpoints.size());
    for (size_t i=0;i<vpoints.size();i++){
        dists[i] = cv::norm(vpoints[i] - meanx);
    }

    std::vector<double> distsSorted(dists);
    std::sort(distsSorted.begin(),distsSorted.end());
    int quantileSize = vpoints.size()/4;
    double quantile1 = distsSorted[quantileSize-1];
    double quantile2 = distsSorted[quantileSize*3 -1];

    std::vector<int> voters;
    for (size_t i=0;i<vpoints.size();i++){
        if (dists[i] >= quantile1 && dists[i] <= quantile2){
            voters.push_back(vpoints[i]);
        }
    }
    xvpoint = cv::mean(voters)[0];
    std::cout << "vanishing point" << xvpoint << std::endl;

    // find the pair of lines that voted closest to the mean
    cv::Vec3i winnerPair;
    int minDist = 1000;
    for (size_t i = 0; i < voterInfo.size(); i++){
        int dist = std::abs(voterInfo[i][2] - xvpoint);
        if (dist < minDist){
            minDist = dist;
            winnerPair = voterInfo[i];
        }
    }

    // find lines that dont give the right vanishing point



    std::vector<int> removeIdx(vlines.size(),0);
    Line line1 = vlines.at(winnerPair[0]);
    Line line2 = vlines.at(winnerPair[1]);

    cv::Mat testimg;
    global::image.copyTo(testimg);
    cv::cvtColor(testimg,testimg, cv::COLOR_GRAY2RGB);
    cv::line(testimg, line1.points[0], line1.points[1], cv::Scalar(0,0,255));
    cv::line(testimg, line2.points[0], line2.points[1], cv::Scalar(0,0,255));
    cv::Point2d ints1, ints2;
    for (size_t i = 0; i < vlines.size(); i++){
        Line line3 = vlines.at(i);
        //cv::line(testimg,line3.points[0], line3.points[1], cv::Scalar(255,0,0));
        //cv::imshow("line test", testimg); cv::waitKey();
        line3.Intersection(line1, ints1);
        line3.Intersection(line2, ints2);

        bool crit1 = ints1.y > 0 && ints2.y > 0;
        bool crit2 =  std::abs(ints1.x - xvpoint) > 100 && (std::abs(ints2.x-xvpoint) > 100);
        if (crit1 || crit2){
            // this line must be removed
            removeIdx.at(i) = 1;
        }
    }

    Lines newVlines;
    for (size_t i = 0; i < vlines.size(); i++){
        if (removeIdx.at(i) == 0){
            newVlines.push_back(vlines.at(i));
        }
    }

    std::cout << "Reduced vertical lines from " << vlines.size() << " to " << newVlines.size() << std::endl;
     return newVlines;

}

void BoardDetector::filterBasedOnSquareSize(Board &board, Remover &remover)
{
    size_t nCols = board.getNumCols();
    size_t nRows = board.getNumRows();

    std::vector<int> hlengths(nCols);
    std::vector<int> vlengths(nRows);

    std::cout << "FILTER BASED ON SQUARE SIZE" << std::endl;

    // Flag outliers based on horizontal lengths
    std::vector<size_t> houtliers(nCols,0);
    for (size_t row = 0; row < nRows; row++){
        std::cout << "ROW " << row << std::endl;
        Squares squares = board.getRow(row);
        for (size_t col = 0; col < squares.size(); col++){
            hlengths.at(col) = squares.at(col).getHLength();
            std::cout << "hlengths.at(" <<col<<"):\t" <<hlengths.at(col) << std::endl;
        }
        houtliers = cvutils::flagOutliers(hlengths);
        remover.addToRow(row, houtliers);
    }

    // Flag outliers based on vertical lengths
    for (size_t col = 0; col < board.getNumCols(); col++){
        std::cout << "COL " << col << std::endl;
        Squares squares = board.getCol(col);
        for (size_t row = 0; row < squares.size(); row++){
            vlengths.at(row) = squares.at(row).getVLength();
            std::cout << "vlengths.at(" << row << "):\t" << vlengths.at(row) << std::endl;
        }

        //double midmean = cvutils::meanNoOutliers(vlengths);
        Regression<int> reg(vlengths);
        std::vector<double> errors = reg.squaredErrors();
        double meanerror = cv::mean(errors)[0];
        for (size_t row = 0; row < squares.size(); row++){
            bool doVote = errors.at(row) > meanerror;
            if (doVote)
                remover.addToElement(row, col, 1);
        }
    }
}

void BoardDetector::filterBasedOnRowType(Board& board, Remover& remover)
{
    std::vector<int> types = board.getRowTypes();

    std::vector<size_t> votes(board.getNumCols());
    std::for_each(votes.begin(), votes.end(), [](size_t& v){v = 1;});
    for (size_t row = 0; row < types.size(); row++){
        if (types.at(row) == 0){
            remover.addToRow(row, votes);
        }
    }
}

void BoardDetector::filterBasedOnColType(Board& board, Remover& remover)
{
    std::vector<int> types = board.getColTypes();

    std::vector<size_t> votes(board.getNumRows());
    std::for_each(votes.begin(), votes.end(), [](size_t& v){v = 1;});
    for (size_t col = 0; col < types.size(); col++){
        if (types.at(col) == 0){
            remover.addToCol(col, votes);
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


