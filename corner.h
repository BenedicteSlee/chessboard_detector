#ifndef CORNER_H
#define CORNER_H

#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "Line.h"


class Corner
{

public:
    Corner(){classified = false;}
    Corner(const cv::Mat&, cv::Point2d, int);
    cv::Mat getArea();
    int getNRegions();
    bool isOutOfBounds(){return outOfBounds;}

private:
    cv::Mat area;
    cv::Point2d initialCornerpoint;
    cv::Point2d cornerpoint;
    int radius;
    int nRegions;
    std::vector<std::vector<int>> layers;
    std::vector<std::vector<int>> binaryLayers;
    bool classified;
    bool outOfBounds;
    void classify();
    void recalculateCornerpoint();
};

#endif // CORNER_H
