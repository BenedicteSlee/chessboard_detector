#include <vector>
#include <cmath>
#include <stdexcept>

#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "square.h"
#include "typedefs.h"
#include "board.h"

BoardDetector::~BoardDetector()
{
}

BoardDetector::BoardDetector(cv::Mat& image_, std::vector<Line> lines_)
{
    image = image_;
    cv::Mat im1, im2, im3, im4;
    image.copyTo(im1);
    image.copyTo(im2);
    image.copyTo(im3);
    image.copyTo(im4);

    if (image.channels() != 1){
        cv::cvtColor(image, image_gray, CV_RGB2GRAY);
        cv::normalize(image_gray, image_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }

    lines = lines_;
    //detectChessboardRegion(); // TODO Use template matching to find rough region of chessboard
    categorizeLines();

    Board possibleBoard = Board(image_gray, hlinesSorted, vlinesSorted);

    //possibleBoard.draw(im1);
    //cv::destroyAllWindows();
    //findVanishingPoint(); // use?
    //removeSpuriousLines(); // TODO Remove lines not belonging to the chessboard

    std::vector<int> rowTypes = possibleBoard.getRowTypes();
    Board possibleBoard2 = filterBasedOnRowType(possibleBoard, rowTypes);

    possibleBoard2.draw(im2);
    cv::destroyAllWindows();
    std::vector<int> colTypes = possibleBoard2.getColTypes();
    Board possibleBoard3 = filterBasedOnColType(possibleBoard2, colTypes);

    possibleBoard3.draw(im3);


    Board possibleBoard4 = filterBasedOnSquareSize(possibleBoard3);
    possibleBoard4.draw(im4);

    cv::destroyAllWindows();
    std::cout << "hei" << std::endl;
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
    Points vpoints;
    vpoints.reserve(numlines*(numlines-1)/2);

    // Calculate intersection between each possible pair of lines
    // TODO: this should be gotten from findIntersections()
    for (int i=0;i<numlines;i++){
        int idx1 = vlinesIdx.at(i);
        Line line1 = lines.at(idx1);
        for (int j=i+1;j<numlines;j++){
            int idx2 = vlinesIdx.at(j);
            Line line2 = lines.at(idx2);
            cv::Point vpoint;
            line1.Intersection(line2, vpoint);
            vpoints.push_back(vpoint);
        }
    }

    cv::Point meanpoint = cvutils::centerpoint(vpoints);

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

    Points voters;
    for (size_t i=0;i<vpoints.size();i++){
        if (dists[i] >= quantile1 && dists[i] <= quantile2){
            voters.push_back(vpoints[i]);
        }
    }
    vanishingPoint = cvutils::centerpoint(voters);
}


Board BoardDetector::filterBasedOnSquareSize(Board &board)
{
    int nCols = board.getNumCols();
    int nRows = board.getNumRows();

    std::vector<int> hlengths(nCols);
    std::vector<int> vlengths(nRows);


    matrix<int> remove, remove1, remove2;

    std::vector<int> houtliers(nRows), voutliers(nCols);

    // Flag outliers based on horizontal lengths
    for (int i = 0; i < nRows; i++){
        Squares row = board.getRow(i);
        for (size_t j = 0; j < row.size(); j++){
            hlengths.at(j) = row.at(j).getHLength();
        }
        houtliers = cvutils::outliers(hlengths);
        remove1.addRow(houtliers);
    }

    Board colsRemoved;
    for (int i = 0; i < nCols; i++){
        std::vector<int> flags = remove1.getCol(i);
        int nkeep = std::count(flags.begin(), flags.end(), 0);
        if (nkeep / (double) nRows > 0.4){ // less than 60% flagged for removal
            colsRemoved.addCol(board.getCol(i));
        }
    }
    return colsRemoved;

    /* TODO CORRECT FOR VIEWPOINT EFFECT
    // Flag outliers based on vertical lengths
    for (int i = 0; i < nCols; i++){
        Squares col = board.getCol(i);
        for (size_t j = 0; j < col.size(); j++){
            vlengths.at(j) = col.at(j).getVLength();
        }
        voutliers = cvutils::outliers(vlengths);
        remove2.addCol(voutliers);
    }

    remove = remove1 + remove2; // any non-zero element is flagged for removal

    Board colsRemoved;
    for (int i = 0; i < rowsRemoved.getNumCols(); i++){
        std::vector<int> flags = remove.getCol(i);
        int nkeep = std::count(flags.begin(), flags.end(), 0);
        if (nkeep / (double) rowsRemoved.getNumCols() > 0.4){ // less than 60% flagged for removal
            colsRemoved.addCol(rowsRemoved.getCol(i));
        }
    }
    */
}

Board BoardDetector::filterBasedOnRowType(Board& board, std::vector<int> rowTypes)
{

    if (cv::sum(rowTypes)[0] == 0){
        std::cout << "None of the rows seem to be part of a chessboard" << std::endl;
        return board;
    }

    Board newBoard;
    for (size_t i = 0; i < rowTypes.size(); i++){
        if (rowTypes.at(i) != 0){
            Squares row = board.getRow(i);
            newBoard.addRow(row);
        }
    }

    return newBoard;
}

Board BoardDetector::filterBasedOnColType(Board& board, std::vector<int> colTypes)
{
    if (cv::sum(colTypes)[0] == 0){
        std::cout << "None of the columns seem to be part of a chessboard" << std::endl;
        return board;
    }

    Board newBoard;
    for (size_t i = 0; i < colTypes.size(); i++){
        if (colTypes.at(i) != 0){
            Squares col = board.getCol(i);
            newBoard.addCol(col);
            //cv::Mat tmp;
            //globalim.copyTo(tmp);
            //newBoard.draw(tmp);
        }
    }
    return newBoard;
}


