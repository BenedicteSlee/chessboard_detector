#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "square.h"
#include "typedefs.h"
#include <vector>
#include <cmath>
#include <armadillo>
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
    findVanishingPoint();
    //removeSpuriousLines(); // TODO Remove lines not belonging to the chessboard
    createPossibleSquares();
    calcSquareSize();
    createCorners();
    determineSquareTypes();
}

Lines BoardDetector::get_hlinesSorted()
{
    Lines output(hlinesSorted.size());
    for (size_t i = 0; i < hlinesSorted.size(); ++i) {
        output[i] = lines[hlinesSorted[i]];
    }
    return output;
}

Lines BoardDetector::get_vlinesSorted()
{
    Lines output(vlinesSorted.size());
    for (size_t i = 0; i < vlinesSorted.size(); ++i) {
        output[i] = lines[vlinesSorted[i]];
    }
    return output;
}

Squares BoardDetector::getPossibleSquares()
{
    return possibleSquares;
}

Square BoardDetector::getPossibleSquare(int col, int row)
{
    int nCols = vlinesSorted.size() - 1;

    int idx = row * nCols + col;
    if (idx > (int)possibleSquares.size() - 1)
        std::cout << "trying to access element > than possibleSquares.size()"<< std::endl;
    Square square = possibleSquares.at(idx);
    return square;
}

std::vector<Squares> BoardDetector::getPossibleSquares2()
{
    return possibleSquares2;
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
            hlines.push_back((int) i);
        }
        else
        {
            vlines.push_back((int) i);
        }
    }
    
    /////////// H O R I Z O N T A L   L I N E S ///////////
    // Sort horizontal lines
    std::vector<std::pair<int,double> > yints(hlines.size());

    std::pair<int,double> p;
    for (size_t i = 0; i < hlines.size(); ++i) {
        Line line = lines[hlines[i]];
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
    for (size_t i = 0; i < hlines.size(); ++i) {
        if (removeIdx1[i] != 1)
            hlinesSorted.push_back(hlines[yints[i].first]);
    }

    /////////// V E R T I C A L   L I N E S ///////////
    // Sort vertical lines
    std::vector<std::pair<int,double> > xints(vlines.size());

    std::pair<int,double> p2;
    for (size_t i = 0; i < vlines.size(); ++i) {
        Line line = lines[vlines[i]];
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
    for (size_t i = 0; i < vlines.size(); ++i) {
        if (removeIdx2[i] != 1){
            vlinesSorted.push_back(vlines[xints[i].first]);
        }
    }
}

void BoardDetector::findVanishingPoint(){
    int numlines = (int) vlines.size();
    Points vpoints;
    vpoints.reserve(numlines*(numlines-1)/2);

    // Calculate intersection between each possible pair of lines
    // TODO: this should be gotten from findIntersections()
    for (int i=0;i<numlines;i++){
        int idx1 = vlines.at(i);
        Line line1 = lines.at(idx1);
        for (int j=i+1;j<numlines;j++){
            int idx2 = vlines.at(j);
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
}

void BoardDetector::calcSquareSize()
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
            Square square = getPossibleSquare(i,j);
            hlengths[i] = square.getHLength();
            vlengths[i] = square.getVLength();

        }
        meanHLengths[j] = cvutils::meanNoOutliers(hlengths);
        meanVLengths[j] = cvutils::meanNoOutliers(vlengths);
    }

    for (int j = 0; j < nrows; j++){
        Squares row;
        for (int i = 0; i < ncols; i++){
            Square square = getPossibleSquare(i, j);
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

void BoardDetector::determineSquareTypes()
{
    for (size_t i = 0; i < possibleSquares2.size(); i++){
        Squares& row = possibleSquares2.at(i);
        for (size_t j = 0; j < row.size(); j++){
            Square& square = row.at(j);
            square.determineType();
        }
    }

}

void BoardDetector::createCorners()
{
    Points added;

    for (size_t i = 0; i < possibleSquares2.size(); i++)
    {
        Squares& row = possibleSquares2.at(i);
        for (size_t j = 0; j < row.size(); j++){
            Square& square = row.at(j);
            Points cpoints = square.getCornerpointsSorted();

            int radius = 10; // TODO make dynamic

            for (size_t k = 0; k < cpoints.size(); k++){
                cv::Point p = cpoints.at(k);
                Corner newcorner(image_gray, p, radius);
                square.addCorner(newcorner);
                if (!cvutils::containsPoint(added, p)){
                    corners.push_back(newcorner);
                    added.push_back(p);

                    std::cout << newcorner.getNRegions() << std::endl;

                    cv::imshow("CORNER", newcorner.getArea());
                    cv::waitKey(2);

                }

            }
        }
    }

}

