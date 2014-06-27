#ifndef CORNER_H
#define CORNER_H

#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "Line.h"

class Corner
{

public:
    Corner(const cv::Mat&, cv::Point, Lines, int);
    cv::Mat getArea();
    int getNRegions();

private:
    cv::Mat image_gray;
    cv::Mat area;
    cv::Point initialCornerpoint;
    cv::Point cornerpoint;
    int radius;
    Points nbors;
    std::vector<int> meancols;
    std::vector<int> stdevcols;
    Lines lines;
    int nRegions;

};

#endif // CORNER_H
