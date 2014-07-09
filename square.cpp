#include "square.h"
#include "Line.h"
#include <stdexcept>
#include <opencv2/opencv.hpp>

// S Q U A R E

Square::Square(){
    squareTypeDetermined = false;
}

Square::Square(cv::Mat& image, cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4)
{
    squareTypeDetermined = false;
    cornerpoints.push_back(corner1);
    cornerpoints.push_back(corner2);
    cornerpoints.push_back(corner3);
    cornerpoints.push_back(corner4);

    cornerpointsSorted = cvutils::sortSquareCorners(cornerpoints);

    upperLeft = cornerpointsSorted[0];
    upperRight = cornerpointsSorted[1];
    lowerLeft = cornerpointsSorted[2];
    lowerRight = cornerpointsSorted[3];

    hlength = cv::norm(upperRight-upperLeft);
    vlength = cv::norm(upperLeft - lowerLeft);

    calcBorders();
    calcVanishingPoints();

    if (!image.data){
        throw std::invalid_argument("Image is empty, cannot create corners.");
    } else {

        createCorners(image);
    }
}

int Square::getVLength(){return vlength;}
int Square::getHLength(){return hlength;}

int Square::getMeanGray()
{
    return meanGray;
}

Points Square::getCornerpoints(){
    return cornerpoints;
}

void Square::sortCornerpoints(){
    cornerpointsSorted = cornerpoints;
    cvutils::sortPoints(cornerpointsSorted);
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

void Square::calcBorders()
{
    borders.push_back(Line(cornerpointsSorted.at(3),cornerpointsSorted.at(0)));
    borders.push_back(Line(cornerpointsSorted.at(0),cornerpointsSorted.at(1)));
    borders.push_back(Line(cornerpointsSorted.at(1),cornerpointsSorted.at(2)));
    borders.push_back(Line(cornerpointsSorted.at(2),cornerpointsSorted.at(3)));
}

int Square::calcMeanGray(cv::Mat& image)
{
    cv::Mat gray;

    if (image.channels() == 3){
        cv::cvtColor(image, gray, CV_RGB2GRAY);
        cv::normalize(gray, gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    } else {
        gray = image;
    }

    int n = 0;
    int val = 0;
    for (int y = upperLeft.y; y < lowerLeft.y; ++y)
    {
        for (int x = upperLeft.x; x < upperRight.x; ++x)
        {
            val += (int) gray.at<uchar>(y,x);
            n += 1;
        }
    }
    meanGray = 0;
    if (n > 0){
        meanGray = val / n;
    }
}

void Square::calcVanishingPoints()
{
    Line border1 = borders.at(0);
    Line border2 = borders.at(1);
    Line border3 = borders.at(2);
    Line border4 = borders.at(3);

    cv::Point p1, p2;
    border1.Intersection(border3, p1);
    border2.Intersection(border4, p2);

    vanishingPoints.push_back(p1);
    vanishingPoints.push_back(p2);
}

void Square::draw(){

    // TODO make dynamic
    int xmax =  500;
    int ymax = 500;

    cv::Mat image = cv::Mat::zeros(xmax*1.5,ymax*1.5,CV_8UC3);
    cv::line( image, borders[0].points[0], borders[0].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );
    cv::line( image, borders[1].points[0], borders[1].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );
    cv::line( image, borders[2].points[0], borders[2].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );
    cv::line( image, borders[3].points[0], borders[3].points[1],cv::Scalar( 110, 220, 0 ),  2, 8 );

    cv::namedWindow("Square");
    cv::imshow("Square",image);

    cv::waitKey( 0 );


}

void Square::drawOnImg(cv::Mat& image)
{
    cv::RNG rng = cv::RNG(123);
    cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
    cv::fillConvexPoly(image, cornerpointsSorted, col);
    cv::imshow("Square", image);
    cv::waitKey();
}

std::vector<cv::Point> Square::getCornerpointsSorted()
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
/*
void Square::addCorner(Corner corner)
{
    if (corners.size() >= 4){
        std::invalid_argument("This square already has 4 corners");
    }

    corners.push_back(corner); // TODO insert in sorted order

    if (corners.size() == 4){ // Once four corners have been added, can determine type
        determineType();
    }
}
*/
void Square::createCorners(cv::Mat& image){
    if (!image.data){
        throw std::invalid_argument("Image is empty, won't create new corner");
    }
    int radius = 10; // TODO make dynamic
    for (size_t i = 0; i < 4; i++){
        Corner newcorner(image, cornerpointsSorted.at(i), radius);
        corners.push_back(newcorner);
    }
    determineType();

}



