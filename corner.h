#ifndef CORNER_H
#define CORNER_H

#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "Line.h"


class Corner
{

public:
    Corner(){classified = false;}
    Corner(const cv::Mat&, cv::Point, int);
    cv::Mat getArea();
    int getNRegions();

private:
    cv::Mat area;
    cv::Point initialCornerpoint;
    cv::Point cornerpoint;
    int radius;
    int nRegions;
    std::vector<std::vector<int>> layers;
    std::vector<std::vector<int>> binaryLayers;
    bool classified;

    void classify();
    void recalculateCornerpoint();
};

#endif // CORNER_H
