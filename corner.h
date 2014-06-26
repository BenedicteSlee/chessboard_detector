#ifndef CORNER_H
#define CORNER_H

#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "Line.h"

class Corner
{
public:
    Corner(const cv::Mat&, cv::Point, Lines, int);

    Points get_nbors();
    cv::Point get_leftNbor();
    cv::Point get_rightNbor();
    cv::Point get_topNbor();
    cv::Point get_bottomNbor();

    void setNbors(Points nbors);

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
    void calcCols();
};

#endif // CORNER_H
