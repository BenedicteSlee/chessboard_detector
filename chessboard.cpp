#include "chessboard.h"
#include "Line.h"
#include <stdexcept>
#include <opencv2/opencv.hpp>

// S Q U A R E
Square::Square()
{
    std::cout << "Creating a square..." << std::endl;
    corners.reserve(4);
    vanishingPoints.reserve(2);
    borders.reserve(4);
}

Square::Square(cv::Mat &image_, cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4)
{
    image = image_;
    corners.reserve(4);
    vanishingPoints.reserve(2);
    borders.reserve(4);

    init(corner1, corner2, corner3, corner4);
}

int Square::get_meanGray()
{
    calcMeanGray();
    return meanGray;
}


std::vector<cv::Point> Square::getCorners(){
    return corners;
}

void Square::setCorners(cv::Mat& image_, cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4){
    image = image_;
    init(corner1, corner2, corner3, corner4);
}

void Square::init(cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4){
    corners.push_back(corner1);
    corners.push_back(corner2);
    corners.push_back(corner3);
    corners.push_back(corner4);

    sortCorners();
    calcBorders();
    calcVanishingPoints();
}

void Square::sortCorners(){
    cornersSorted = corners;
    std::sort(cornersSorted.begin(), cornersSorted.end(), cvutils::pointIsLess);
}

void Square::calcBorders()
{
    borders.push_back(Line(corners.at(0),corners.at(1)));
    borders.push_back(Line(corners.at(1),corners.at(2)));
    borders.push_back(Line(corners.at(2),corners.at(3)));
    borders.push_back(Line(corners.at(3),corners.at(0)));
}

void Square::calcMeanGray()
{
    cv::Mat gray;
    cv::cvtColor(image, gray, CV_RGB2GRAY);

    int n = 0;
    uchar val = 0;
    for (int y = 0; y < gray.rows; ++y)
    {
        for (int x = 0; x < gray.cols; ++x)
        {
            val += (int) gray.at<uchar>(y,x);
            n += 1;
        }
    }
    meanGray = val / n;
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

// C H E S S B O A R D
Chessboard::Chessboard()
{
    squares.reserve(64);
}

Square& Chessboard::at(int row, int col)
{
    if (row > 7 || col > 7){
        throw std::invalid_argument("Row and column must be between 0 and 7");
    }
    int idx = 8 * row + col;
    return squares.at(idx);
}


