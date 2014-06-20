#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "square.h"
#include <vector>
#include <cmath>


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
    createPossibleSquares();
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

Squares BoardDetector::get_PossibleSquares()
{
    return possibleSquares;
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

    Line::Intersections(lines, points, limits, distances);


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

