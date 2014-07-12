#ifndef SQUARE_H
#define SQUARE_H

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
    Square();
    Square(cv::Mat& image, cv::Point2d corner1, cv::Point2d corner2, cv::Point2d corner3, cv::Point2d corner4);

    // Methods
    void draw();
    void drawOnImg(cv::Mat& image);

    // Get and set methods
    int getMeanGray();
    int getVLength();
    int getHLength();
    Points2d getCornerpoints();
    void setCornerpoints(cv::Mat image, cv::Point2d, cv::Point2d, cv::Point2d, cv::Point2d);
    std::vector<cv::Point2d> getCornerpointsSorted();
    Lines getBordersSorted();
    int getSquareType(){return squareType;}
    static std::vector<int> getSquareTypes(Squares);
    cv::Point2d getCenter(){return center;}
    // Add features
    //void addCorner(Corner corner);
    void determineType();
    Points2d getVanishingPoints(){return vanishingPoints;}
    bool isOutOfBounds(){return outOfBounds;}

private:
    // Variables
    int meanGray;
    std::vector<cv::Point2d> cornerpoints;
    std::vector<cv::Point2d> cornerpointsSorted;
    std::vector<Corner> corners;
    cv::Point2d center;
    std::vector<Line> borders;
    Line upperBorder, rightBorder, lowerBorder, leftBorder;
    cv::Point2d upperLeft, upperRight, lowerRight, lowerLeft;
    std::vector<cv::Point2d> vanishingPoints;
    int vlength;
    int hlength;
    int squareType;
    bool squareTypeDetermined;
    bool outOfBounds;

    // Methods
    void calcVanishingPoints();
    void calcBorders();
    int calcMeanGray(cv::Mat &image);
    void sortCornerpoints(); 
    void createCorners(cv::Mat& image);
};

#endif // SQUARE_H
