#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "square.h"
#include "Line.h"
#include "global.h"

extern bool global::doDraw;

// S Q U A R E
Square::Square(){
    squareTypeDetermined = false;
    outOfBounds = false;
    containsPieceDetermined = false;
    doesContainPiece = false;
}

Square::Square(cv::Point2d corner1, cv::Point2d corner2, cv::Point2d corner3, cv::Point2d corner4)
{
    cv::Size imsize = global::image.size();

    squareTypeDetermined = false;
    outOfBounds = false;
    containsPieceDetermined = false;
    doesContainPiece = false;

    Points2d cp{corner1, corner2, corner3, corner4};
    bool negCoord = cvutils::anyNegCoordinate(cp);
    if (negCoord){
        throw std::invalid_argument("One of the points has a negative coordinate");
    }

    cornerpoints = cp;

    cornerpointsSorted = cvutils::sortSquareCorners(cornerpoints);

    upperLeft = cornerpointsSorted[0];
    upperRight = cornerpointsSorted[1];
    lowerRight = cornerpointsSorted[2];
    lowerLeft = cornerpointsSorted[3];

    center = cvutils::centerpoint(cornerpointsSorted);

    upperLeft.x < lowerLeft.x ? firstx = upperLeft.x : firstx = lowerLeft.x;
    upperLeft.y < lowerLeft.y ? firsty = upperLeft.y : firsty = lowerLeft.y;
    upperRight.x > lowerRight.x ? lastx = upperRight.x : lastx = lowerRight.x;
    upperRight.y > lowerRight.y ? lasty = upperRight.y : lasty = lowerRight.y;

    try{
        area = global::image(cv::Rect(firstx, firsty, lastx-firstx, lasty-firsty));

        meanGray = calcMeanGray(area);
    } catch(std::exception &e){
        outOfBounds = true;
    }

    //////////
    hlength = cv::norm(upperRight-upperLeft);
    vlength = cv::norm(upperLeft - lowerLeft);

    calcBorders();
    //calcVanishingPoints();

    if (!global::image.data){
        throw std::invalid_argument("global::image is empty, cannot create corners.");
    } else if (!outOfBounds) {
        createCorners(global::image);
    }
}

size_t Square::getVLength(){return vlength;}
size_t Square::getHLength(){return hlength;}

int Square::getMeanGray()
{
    return meanGray;
}

Points2d Square::getCornerpoints(){
    return cornerpoints;
}

void Square::determineType()
{
    if (corners.empty()){
        std::cout << "Corners have not been added yet" << std::endl;
        return;
    }

    std::vector<int> votes(4);
    for (int i = 0; i < 4; i++){
        votes.at(i) = corners[i].getNRegions();
    }

    std::vector<int> histogram(6,0); // votes can be 0 (undertermined), 1, 2, 3, 4, or 5 (more than 4)
    for( int i=0; i< (int) votes.size(); ++i )
        ++histogram[ votes[i] ];

    //int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();

    // inner 4, border 3, corner 2, frame 1, outside board 0
    if (histogram[4] >= 3){
        squareType = 4;
    }

    else if (histogram[4] == 2 && histogram[2] == 2){
        squareType = 3;
    }

    else if (histogram[4] == 1 && histogram[2] >= 2){
        squareType = 2;
    }

    else {
        squareType = 0;
    }

    squareTypeDetermined = true;
}

bool Square::containsPoint(cv::Point2d point) const {
    /*
        double test1 = upperBorder.ylookup(point.x);
        double test2 = lowerBorder.ylookup(point.x);
        double test3 = leftBorder.xlookup(point.y);
        double test4 = rightBorder.xlookup(point.y);
        */
    bool belowTop = point.y > upperBorder.ylookup(point.x);
    bool aboveBottom = point.y < lowerBorder.ylookup(point.x);
    bool rightofLeft = point.x > leftBorder.xlookup(point.y);
    bool leftofRight = point.x < rightBorder.xlookup(point.y);

    return (belowTop && aboveBottom && rightofLeft && leftofRight);
}

void Square::calcBorders()
{
    leftBorder = Line(cornerpointsSorted.at(3),cornerpointsSorted.at(0));
    upperBorder = Line(cornerpointsSorted.at(0),cornerpointsSorted.at(1));
    rightBorder = Line(cornerpointsSorted.at(1),cornerpointsSorted.at(2));
    lowerBorder = Line(cornerpointsSorted.at(2),cornerpointsSorted.at(3));
    Lines bds{leftBorder, upperBorder, rightBorder, lowerBorder};
    borders = bds;
}

int Square::calcMeanGray(cv::Mat& area)
{
    int n = 0;
    int val = 0;
    for (int y = 0; y < area.rows; ++y)
    {
        for (int x = 0; x < area.cols; ++x)
        {
            val += (int) area.at<uchar>(y,x);
            n += 1;
        }
    }
    meanGray = 0;
    if (n > 0){
        meanGray = val / n;
    }
    return meanGray;
}

void Square::calcVanishingPoints()
{
    Line border1 = borders.at(0);
    Line border2 = borders.at(1);
    Line border3 = borders.at(2);
    Line border4 = borders.at(3);

    cv::Point2d p1, p2;
    border1.Intersection(border3, p1);
    border2.Intersection(border4, p2);

    vanishingPoints.push_back(p1);
    vanishingPoints.push_back(p2);
}

void Square::draw() const{


    // TODO make dynamic
    cv::Mat im = cv::Mat::zeros(2,imsize,CV_8UC3);
    cv::line( im, borders[0].points[0], borders[0].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );
    cv::line( im, borders[1].points[0], borders[1].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );
    cv::line( im, borders[2].points[0], borders[2].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );
    cv::line( im, borders[3].points[0], borders[3].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );

    cv::namedWindow("Square");
    cv::imshow("Square",im);

    cv::waitKey( 0 );


}

void Square::drawOnImg(cv::Mat& image) const
{

    if (cvutils::anyNegCoordinate(cornerpointsSorted)){
        std::cout << "At least one point has a negative index, cannot draw" << std::endl;
        return;
    }

    if (outOfBounds){
        std::cout << "Square is fully or partially outside of the global::image, cannot draw" << std::endl;
        return;
    }

    cv::RNG rng = cv::RNG(123);
    cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
    Points cps = cvutils::doubleToInt(cornerpointsSorted);
    cv::fillConvexPoly(image, cps, col);
    cv::imshow("Square", image);
    cv::waitKey();
}

bool Square::detectPieceWithHough(cv::Vec3i &circle){
    cv::Mat binarea;
    cv::threshold(area, binarea, meanGray, 255, 0);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(binarea, circles, CV_HOUGH_GRADIENT, 1, binarea.rows, 20, 15, binarea.rows*0.3, binarea.rows*1.5);

    if (circles.size() > 0){
        circle = circles[0]; // todo use diagnostics to choose the best circle if there are more than 1
        doesContainPiece = true;
        return true;
    }
    return false;
}

bool Square::detectPieceWithHough(cv::Mat &image_channel, cv::Vec3i &circle){
    cv::Mat binarea, channelArea;

    try{
        channelArea = image_channel(cv::Rect(firstx, firsty, lastx-firstx, lasty-firsty));
    } catch(std::exception& e){
        std::cout << "image channel error" << std::endl;
    }
    int channelMeanGray =  calcMeanGray(channelArea);
    int thresh = channelMeanGray * 1.1;
    cv::threshold(channelArea, binarea, thresh, 255, 0);

    /*
    if (global::doDraw){
        cv::imshow("area", channelArea);
        cv::waitKey();
        cv::imshow("binarea", binarea);
        cv::waitKey();
    }
    */

    std::vector<cv::Vec3f> circles;

    try{
        cv::HoughCircles(binarea, circles, CV_HOUGH_GRADIENT, 1, binarea.rows, 20, 15, binarea.rows*0.1, binarea.rows*3);
    } catch(std::exception &e){
        std::cout << "HoughCircles error" << std::endl;
    }

    if (circles.size() > 0){
        circle = circles[0]; // todo use diagnostics to choose the best circle if there are more than 1

        try{
            cv::circle(channelArea, cv::Point(circle[0], circle[1]), circle[2], cv::Scalar(255,0,0));

        } catch(std::exception& e){
            std::cout << "circle error" << std::endl;
        }

        //if (global::doDraw) cv::imshow("circle", channelArea); cv::waitKey();
        doesContainPiece = true;
        return true;
    }
    return false;
}

bool Square::determinePieceColor(cv::Vec3i circle, int& color) const{
    int x = circle[0];
    int y = circle[1];
    int vsize = vlength/3;
    int hsize = hlength/3;
    cv::Mat subarea;

    int upperx = x - hsize;
    int uppery = y - vsize;

    if (upperx < 0)
        upperx = 0;
    if (uppery < 0)
        uppery = 0;
    if (upperx + hsize > area.cols)
        hsize = area.cols - upperx - 2;
    if (uppery + vsize > area.rows)
        vsize = area.rows - uppery - 2;

    try{
        subarea = area(cv::Rect(upperx, uppery, hsize, vsize));
    } catch(std::exception &e){
        std::cout << "piece too close to square edge" << std::endl;
        return false;
    }

    color = cv::mean(subarea)[0];
    return true;
}

std::vector<cv::Point2d> Square::getCornerpointsSorted() const
{
    return cornerpointsSorted;
}

Lines Square::getBordersSorted()
{
    return borders;
}

std::vector<int> Square::getSquareTypes(Squares squares)
{
    if (squares.empty()){
        throw std::invalid_argument("vector is empty");
    }

    std::vector<int> result(squares.size());
    for (size_t i = 0; i < squares.size(); i++){
        result[i] = squares[i].getSquareType();
    }
    return result;
}

void Square::createCorners(cv::Mat& image){
    if (!global::image.data){
        throw std::invalid_argument("global::image is empty, won't create new corner");
    }
    int radius = 10; // TODO make dynamic
    for (size_t i = 0; i < 4; i++){
        Corner newcorner(global::image, cornerpointsSorted.at(i), radius);
        if (newcorner.isOutOfBounds())
            outOfBounds = true;
        corners.push_back(newcorner);
    }
    if (!outOfBounds){
        determineType();
    }
}



