#include "corner.h"
#include "cvutils.h"
#include "typedefs.h"
#include <opencv2/opencv.hpp>


Corner::Corner(cv::Point cornerpoint, int radius_)
{
    initialCornerpoint = cornerpoint;
    radius = radius_;

}

void Corner::setNbors(Points nbors_){
   std::sort(nbors_.begin(), nbors_.end(), cvutils::pointIsLess);
   nbors = nbors_;
}

cv::Point Corner::get_topNbor()
{
    return nbors[0];
}

cv::Point Corner::get_rightNbor()
{
    return nbors[1];
}

cv::Point Corner::get_bottomNbor()
{
    return nbors[2];
}

cv::Point Corner::get_leftNbor()
{
    return nbors[3];
}
