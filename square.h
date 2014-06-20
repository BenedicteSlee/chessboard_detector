#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "cvutils.h"
#include "Line.h"
#include "typedefs.h"

class Square
{
public:

    ~Square(){}
    Square(cv::Mat& image, cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4);

    int get_meanGray();
    std::vector<cv::Point> getCorners();
    void setCorners(cv::Mat& image, cv::Point, cv::Point, cv::Point, cv::Point);
    void init(cv::Point, cv::Point, cv::Point, cv::Point);
    cv::Point center;
    std::vector<Line> borders;
    Line upperBorder, rightBorder, lowerBorder, leftBorder;
    cv::Point upperLeft, upperRight, lowerRight, lowerLeft;

    std::vector<cv::Point> vanishingPoints;
    void draw();

protected:
    void calcVanishingPoints();
    void calcBorders();
    void calcMeanGray();
    void sortCorners();

private:
    int meanGray;
    cv::Mat& image;
    std::vector<cv::Point> corners;
    std::vector<cv::Point> cornersSorted;
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
