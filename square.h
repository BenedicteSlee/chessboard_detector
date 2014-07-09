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
    Square(cv::Mat& image, cv::Point corner1, cv::Point corner2, cv::Point corner3, cv::Point corner4);

    // Methods
    void draw();
    void drawOnImg(cv::Mat& image);

    // Get and set methods
    int getMeanGray();
    int getVLength();
    int getHLength();
    Points getCornerpoints();
    void setCornerpoints(cv::Mat image, cv::Point, cv::Point, cv::Point, cv::Point);
    std::vector<cv::Point> getCornerpointsSorted();
    Lines getBordersSorted();
    int getSquareType(){return squareType;}
    static std::vector<int> getSquareTypes(Squares);
    cv::Point getCenter(){return center;}
    // Add features
    //void addCorner(Corner corner);
    void determineType();
    Points getVanishingPoints(){return vanishingPoints;}

private:
    // Variables
    int meanGray;
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
    bool squareTypeDetermined;

    // Methods
    void calcVanishingPoints();
    void calcBorders();
    int calcMeanGray(cv::Mat &image);
    void sortCornerpoints(); 
    void createCorners(cv::Mat& image);
};

#endif // SQUARE_H
