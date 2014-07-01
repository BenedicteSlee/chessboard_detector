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

    // Methods
    void draw();

    // Get and set methods
    int getMeanGray();
    int getVLength();
    int getHLength();
    Points getCornerpoints();
    void setCornerpoints(cv::Mat& image, cv::Point, cv::Point, cv::Point, cv::Point);
    std::vector<cv::Point> getCornerpointsSorted() const;
    Lines getBordersSorted() const;
    int getSquareType(){return squareType;}

    // Add features
    void addCorner(Corner corner);
    void determineType();

private:
    // Variables
    int meanGray;
    cv::Mat& image;
    std::vector<cv::Point> cornerpoints;
    std::vector<cv::Point> cornerpointsSorted;
    std::vector<Corner> corners;
    cv::Point center;
    std::vector<Line> borders;
    Line upperBorder, rightBorder, lowerBorder, leftBorder;
    cv::Point upperLeft, upperRight, lowerRight, lowerLeft;
    std::vector<cv::Point> vanishingPoints;
    int vlength;
    int hlength;
    int squareType;

    // Methods
    void calcVanishingPoints();
    void calcBorders();
    void calcMeanGray();
    void sortCornerpoints();

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
