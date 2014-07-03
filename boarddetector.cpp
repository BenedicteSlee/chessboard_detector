#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "square.h"
#include "typedefs.h"
#include <vector>
#include <cmath>
#include <stdexcept>

BoardDetector::~BoardDetector()
{
}

BoardDetector::BoardDetector(cv::Mat& image_, std::vector<Line> lines_)
{
    image = image_;
    if (image.channels() != 1){
        cv::cvtColor(image, image_gray, CV_RGB2GRAY);
        cv::normalize(image_gray, image_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }

    lines = lines_;
    //detectChessboardRegion(); // TODO Use template matching to find rough region of chessboard
    categorizeLines();

    Board possibleBoard = Board(image, hlinesSorted, vlinesSorted);
    //possibleBoard.draw(image);

    //findVanishingPoint(); // use?
    //removeSpuriousLines(); // TODO Remove lines not belonging to the chessboard

    //createPossibleSquares();
    //Board possibleBoard2 = filterBasedOnSquareSize(possibleBoard);

    createCorners(possibleBoard);

    //determineSquareTypes(possibleBoard2);
    determineRowTypes(possibleBoard);
    Board possibleBoard2 = filterBasedOnRowType(possibleBoard, rowTypes);
    possibleBoard2.draw(image);
    determineColTypes(possibleBoard2);
    Board possibleBoard3 = filterBasedOnColType(possibleBoard2, colTypes);
    possibleBoard3.draw(image);

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

void BoardDetector::createPossibleSquares()
{


    /*
    for (size_t i = 0; i < hlinesSorted.size()-1; ++i) {
        Line hlineUpper = lines.at(hlinesSorted.at(i));
        Line hlineLower = lines.at(hlinesSorted.at(i+1));
        for (size_t j = 0; j < vlinesSorted.size()-1; ++j) {
            Line vlineLeft = lines.at(vlinesSorted.at(j));
            Line vlineRight = lines.at(vlinesSorted.at(j+1));

            cv::Point upperLeft, upperRight, lowerLeft, lowerRight;
            hlineUpper.Intersection(vlineLeft, upperLeft);
            hlineUpper.Intersection(vlineRight, upperRight);
            hlineLower.Intersection(vlineLeft, lowerLeft);
            hlineLower.Intersection(vlineRight, lowerRight);

            Square square(image, upperLeft, upperRight, lowerRight, lowerLeft);

            possibleSquares.push_back(square);
        }
    }
    */
}


Board BoardDetector::filterBasedOnSquareSize(Board &board)
{ // TODO REDO!! remove whole rows/columns at a time not indivudal squares
    std::vector<int> hlengths(board.getNumCols());
    std::vector<int> vlengths(board.getNumCols());
    std::vector<int> meanHLengths(board.getNumRows());
    std::vector<int> meanVLengths(board.getNumRows());

    Board newBoard;

    // get mean size per row
    for (int i = 0; i < board.getNumRows(); i++){
        Squares row = board.getRow(i);
        for (size_t j = 0; j < row.size(); j++){
            hlengths.at(j) = row.at(j).getHLength();
            vlengths.at(j) = row.at(j).getVLength();
        }
        meanHLengths.at(i) = cvutils::meanNoOutliers(hlengths);
        meanVLengths.at(i) = cvutils::meanNoOutliers(vlengths);

        Squares newRow;
        for (int j = 0; j < board.getNumCols(); j++){
            Square square = row.at(j);
            bool vlengthOk = std::abs(hlengths.at(j) - meanVLengths.at(i)) < 10; // TODO make dynamic
            bool hlengthOk = std::abs(hlengths.at(j) - meanHLengths.at(i)) < 10; // TODO make dynamic

            if (vlengthOk && hlengthOk){
                newRow.push_back(square);
            }
        }
        newBoard.addRow(newRow);
        std::cout << newBoard.getNumRows() << ";" << newBoard.getNumCols() << std::endl;
    }

    return newBoard;
}
/*
void BoardDetector::filterBasedOnSquareSizeOld()
{
    int nrows = hlinesSorted.size()-1;
    int ncols = vlinesSorted.size()-1;

    // get mean size per row (not for whole board because need to account for viewpoint effect

    std::vector<int> hlengths(ncols);
    std::vector<int> vlengths(ncols);
    std::vector<int> meanHLengths(nrows);
    std::vector<int> meanVLengths(nrows);

    for (int j=0; j<nrows;j++){
        for (int i=0; i < ncols; i++)
        {
            Square square = possibleBoard.getSquare(j, i);
            hlengths[i] = square.getHLength();
            vlengths[i] = square.getVLength();

        }
        meanHLengths[j] = cvutils::meanNoOutliers(hlengths);
        meanVLengths[j] = cvutils::meanNoOutliers(vlengths);
    }

    for (int j = 0; j < nrows; j++){
        Squares row;
        for (int i = 0; i < ncols; i++){
            Square square = possibleBoard.getSquare(j,i);
            int hlength = square.getHLength();
            int vlength = square.getVLength();

            bool vlengthOk = std::abs(vlength - meanVLengths[j]) < 10; // TODO make dynamic
            bool hlengthOk = std::abs(hlength - meanHLengths[j]) < 10; // TODO make dynamic
            if ( vlengthOk && hlengthOk){
                row.push_back(square);
            }
        }
        possibleSquares2.push_back(row);
    }
}
*/
void BoardDetector::createCorners(Board& board)
{
    //Points added;

    for (int i = 0; i < board.getNumRows(); i++)
    {

        for (int j = 0; j < board.getNumCols(); j++){
            Square& square = board.getSquareRef(i,j);
            Points cpoints = square.getCornerpointsSorted();

            int radius = 10; // TODO make dynamic

            for (size_t k = 0; k < cpoints.size(); k++){
                cv::Point p = cpoints.at(k);
                Corner newcorner(image_gray, p, radius);
                square.addCorner(newcorner);

                /* // look at corners without duplicates
                if (!cvutils::containsPoint(added, p)){
                    corners.push_back(newcorner);
                    added.push_back(p);

                    std::cout << newcorner.getNRegions() << std::endl;

                    cv::imshow("CORNER", newcorner.getArea());
                    cv::waitKey(2);

                }
                */

            }

        }
    }

}

void BoardDetector::determineRowTypes(Board& board)
{
    int nrows = board.getNumRows();
    std::vector<int> types(nrows);

    for (int i = 0; i < nrows; i++){
        Squares row = board.getRow(i);

        std::vector<int> histogram(5,0);
        for (size_t j = 0; j < row.size(); j++){
            int type = row.at(j).getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < (int)row.size()-1) // if there are two or more votes for other categories
        {
            types.at(i) = -1; // might be part of the board
        } else {

            types.at(i) = vote;
        }
    }

    rowTypes = types;
}

Board BoardDetector::filterBasedOnRowType(Board& board, std::vector<int> rowTypes)
{
    Board newBoard;
    int n = rowTypes.size();
    for (size_t i = 0; i < rowTypes.size(); i++){
        if (rowTypes.at(i) != 0){
            Squares row = board.getRow(i);
            newBoard.addRow(row);
        }
    }
    return newBoard;
}

void BoardDetector::determineColTypes(Board& board)
{
    std::vector<int> types(board.getNumCols());

    for (size_t i = 0; i < board.getNumCols(); i++){
        Squares col = board.getCol(i);

        std::vector<int> histogram(5,0);
        for (size_t j = 0; j < col.size(); j++){
            int type = col.at(j).getSquareType();
            if (type < 5 && type >= 0){
                ++histogram[ type ];
            } else {
                throw std::invalid_argument("invalid type!");
            }

        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < (int)col.size()-1) // if there are two or more votes for other categories
        {
            types.push_back(-1); // might be part of the board
        } else {

            types.push_back(vote);
        }
    }
    colTypes = types;
}


/*
void BoardDetector::determineColTypesOld()
{
    int nCols = squareTypes.at(0).size(); // squareTypes has the same number of cols in each vector element so can just look at the first element

    // access per column

    for (size_t i = 0; i < nCols; i++){
        std::vector<int> col;
        for (size_t j = 0; j < squareTypes.size(); j++){
            col.push_back(squareTypes.at(j).at(i));
        }

        std::vector<int> histogram(5,0);
        for (size_t j = 0; j < col.size(); j++){
            ++histogram[ col.at(j) ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < (int)col.size()-1) // if there are two or more votes for other categories
        {
            colTypes.push_back(-1); // might be part of the board
        } else {
            colTypes.push_back(vote);
        }
    }
}
*/


Board BoardDetector::filterBasedOnColType(Board& board, std::vector<int> colTypes)
{
    Board newBoard;
    for (size_t i = 0; i < colTypes.size(); i++){
        if (rowTypes.at(i) != 0){
            Squares col = board.getCol(i);
            newBoard.addRow(col);
        }
    }
    return newBoard;

}


