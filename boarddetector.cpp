#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "chessboard.h"
#include <vector>
#include <boost/math/distributions/normal.hpp>

BoardDetector::~BoardDetector()
{
}

BoardDetector::BoardDetector(cv::Mat& image_, std::vector<Line> lines_)
{
    image = image_;
    lines = lines_;
    //detectChessboardRegion(); // TODO Use template matching to find rough region of chessboard
    categorizeLines();
    findIntersections();
    findVanishingPoint();
    //removeSpuriousLines(); // TODO Remove lines not belonging to the chessboard
    //createPossibleSquares();
}

Squares BoardDetector::getPossibleSquares()
{
    return possibleSquares;
}

void BoardDetector::categorizeLines(){

    for (size_t i=0; i<lines.size();i++)
    {
        if (lines[i].slope < 0.1 && lines[i].slope > -0.1) // TODO: make numbers dynamic based on iamge size!
        {
            horizontalLines.push_back((int) i);
        }
        else
        {
            nonHorizontalLines.push_back((int) i);
        }
    }
    
    // TODO: sort horizontal lines

    
    
    
}

void BoardDetector::findVanishingPoint(){
    int numlines = (int) nonHorizontalLines.size();
    Points vpoints;
    vpoints.reserve(numlines*(numlines-1)/2);

    // Calculate intersection between each possible pair of lines
    // TODO: this should be gotten from findIntersections()
    for (int i=0;i<numlines;i++){
        int idx1 = nonHorizontalLines.at(i);
        Line line1 = lines.at(idx1);
        for (int j=i+1;j<numlines;j++){
            int idx2 = nonHorizontalLines.at(j);
            Line line2 = lines.at(idx2);
            cv::Point vpoint;
            line1.Intersection(line2, vpoint);
            vpoints.push_back(vpoint);
        }
    }

    cv::Point meanpoint = cvutils::MeanPoint(vpoints);

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
    vanishingPoint = cvutils::MeanPoint(voters);
}

void BoardDetector::findIntersections()
{
    std::vector<cv::Point> points, newpoints;
    cv::Point limits(image.size[1], image.size[0]);
    std::vector<double> distances;

    Line::Intersections(lines, points, limits);
    Line::RemoveDuplicateIntersections(points,newpoints, distances);

}

void BoardDetector::createPossibleSquares()
{
    for (size_t i = 0; i < horizontalLines.size()-1; ++i) {
        Line hlineUpper = lines.at(horizontalLines.at(i));
        Line hlineLower = lines.at(horizontalLines.at(i+1));
        for (size_t j = 0; j < nonHorizontalLines.size()-1; ++j) {
            Line vlineLeft = lines.at(nonHorizontalLines.at(j));
            Line vlineRight = lines.at(nonHorizontalLines.at(j+1));

            cv::Point upperLeft, upperRight, lowerLeft, lowerRight;
            hlineUpper.Intersection(vlineLeft, upperLeft);
            hlineUpper.Intersection(vlineRight, upperRight);
            hlineLower.Intersection(vlineLeft, lowerLeft);
            hlineLower.Intersection(vlineRight, lowerRight);

            Square square(upperLeft, upperRight, lowerRight, lowerLeft);

            possibleSquares.push_back(square);
        }
    }

}

