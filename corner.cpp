#include "corner.h"
#include "cvutils.h"
#include "typedefs.h"
#include <opencv2/opencv.hpp>


corner::corner(cv::Point cornerpoint, Points nbors_, int radius_)
{
    initialCornerpoint = cornerpoint;
    radius = radius_;
    nbors = nbors_;
    std::sort(nbors.begin(), nbors.end(), cvutils::pointIsLess);
}

cv::Point corner::get_topNbor()
{
    return nbors[0];
}

cv::Point corner::get_rightNbor()
{
    return nbors[1];
}

cv::Point corner::get_bottomNbor()
{
    return nbors[2];
}

cv::Point corner::get_leftNbor()
{
    return nbors[3];
}
