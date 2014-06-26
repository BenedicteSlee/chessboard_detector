#include "corner.h"
#include "cvutils.h"
#include "typedefs.h"
#include <opencv2/opencv.hpp>

Corner::Corner(const cv::Mat& image_, cv::Point cornerpoint_, Lines lines_, int radius_)
{
    image_gray = image_;
    lines = lines_;
    initialCornerpoint = cornerpoint_;
    cornerpoint = cornerpoint_;
    radius = radius_;
    calcCols();
}

void Corner::setNbors(Points nbors_){
   std::sort(nbors_.begin(), nbors_.end(), cvutils::pointIsLess);
   nbors = nbors_;
}

void Corner::calcCols()
{
    // Create area around cornerpoint
    area = image_gray(cv::Rect(cornerpoint.x - radius,cornerpoint.y - radius, radius*2 , radius*2));
    cv::imshow("corner", area);
    cv::waitKey();
    // NEXTSTEP calculate colors
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
