#ifndef CORNER_H
#define CORNER_H

#include <opencv2/opencv.hpp>
#include "typedefs.h"

class Corner
{
public:
    Corner(cv::Point cornerpoint, int radius);

    Points get_nbors();
    cv::Point get_leftNbor();
    cv::Point get_rightNbor();
    cv::Point get_topNbor();
    cv::Point get_bottomNbor();

    void setNbors(Points nbors);

private:
    cv::Point initialCornerpoint;
    int radius;
    Points nbors;



};

#endif // CORNER_H
