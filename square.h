#ifndef SQUARE_H
#define SQUARE_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "cvutils.h"
#include "Line.h"
#include "typedefs.h"
#include "corner.h"
#include "global.h"

class Square
{
public:
    // Constructors
    ~Square(){doesContainPiece = false;}
    Square();
    Square(cv::Point2d corner1, cv::Point2d corner2, cv::Point2d corner3, cv::Point2d corner4);

    // Methods
    void draw() const;
    void drawOnImg(cv::Mat& image) const;

    // Get and set methods
    int getMeanGray();
    size_t getVLength();
    size_t getHLength();
    Points2d getCornerpoints();
    void setCornerpoints(cv::Mat& image, cv::Point2d, cv::Point2d, cv::Point2d, cv::Point2d);
    std::vector<cv::Point2d> getCornerpointsSorted() const;
    Lines getBordersSorted();
    int getSquareType() const {return squareType;}
    bool isOutOfBounds(){return outOfBounds;}
    cv::Point2d getCenter(){return center;}
    Points2d getVanishingPoints(){return vanishingPoints;}

    // Methods: diagnostics on square
    void determineType();
    bool detectPieceWithHough(cv::Vec3i &);
    bool detectPieceWithHough(cv::Mat &image_channel, cv::Vec3i &circle);
    bool determinePieceColor(cv::Vec3i circle, int &color) const;

    bool containsPoint(cv::Point2d point) const;
    std::vector<Corner> getCorners() const {return corners;}
    bool containsPiece(){return doesContainPiece;}

    // static methoda
    static std::vector<int> getSquareTypes(Squares);

private:
    // Variables
    int meanGray;
    cv::Mat area;
    double firstx, firsty, lastx, lasty;
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
    int imsize;
    int draughtsPiece;
    bool containsPieceDetermined;
    bool doesContainPiece;

    // Methods
    void calcVanishingPoints();
    void calcBorders();
    int calcMeanGray(cv::Mat& area);
    void createCorners(cv::Mat& image);
};

#endif // SQUARE_H
