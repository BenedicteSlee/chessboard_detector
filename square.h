#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "cvutils.h"
#include "Line.h"
#include "typedefs.h"
#include "corner.h"

class Square
{
public:

    // Constructors
    ~Square(){}
    Square(cv::Mat& image, cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4);

    // Init
    void init(cv::Point, cv::Point, cv::Point, cv::Point);

    // Get and set methods
    int get_meanGray();
    std::vector<cv::Point> get_Corners();
    void set_Corners(cv::Mat& image, cv::Point, cv::Point, cv::Point, cv::Point);

    // Methods
    void draw();

private:
    // Variables
    int meanGray;
    cv::Mat& image;
    std::vector<cv::Point> corners;
    std::vector<cv::Point> cornersSorted;
    cv::Point center;
    std::vector<Line> borders;
    Line upperBorder, rightBorder, lowerBorder, leftBorder;
    cv::Point upperLeft, upperRight, lowerRight, lowerLeft;
    std::vector<cv::Point> vanishingPoints;
    std::vector<corner> corners;

    // Methods
    void calcVanishingPoints();
    void calcBorders();
    void calcMeanGray();
    void sortCorners();
};

class Chessboard : public Square
{
public:
    Chessboard();
    ~Chessboard(){}
    Square& at(int, int);
    std::vector<Square> squares;
};

#endif // CHESSBOARD_H
