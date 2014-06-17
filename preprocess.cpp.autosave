#include "preprocess.h"
#include "Line.h"
#include "typedefs.h"
#include "chessboard.h"
#include <opencv2/opencv.hpp>
#include <vector>

Preprocess::Preprocess(cv::Mat & image_)
{
    image = image_;
    houghThreshold = 96;
    minLineLength = 105;
    maxLineGap = 139;
    cv::Mat dst;
    edgeDetection(dst);
    lineDetection(dst);
}

void Preprocess::getLines(Lines& lines_){
    lines_ = lines;
}

void Preprocess::edgeDetection(cv::Mat& dst){
    cv::Mat gray, blur;
    cv::cvtColor(image,gray,cv::COLOR_RGB2GRAY);
    cv::GaussianBlur(gray, blur, cv::Size(0,0), 2);
    cv::Canny(blur, dst, 30, 200, 3);
}

void Preprocess::lineDetection(cv::Mat& src)
{
    /// Use Probabilistic Hough Transform

    std::vector<cv::Vec4i> houghlines;
    cv::HoughLinesP(src, houghlines, 1, CV_PI/180, houghThreshold, minLineLength, maxLineGap);

    for (size_t i = 0; i < houghlines.size(); i++)
    {
        Line l = Line(cv::Point(houghlines[i][0], houghlines[i][1]), cv::Point(houghlines[i][2], houghlines[i][3]));
        lines.push_back(l);
    }

    int i = 1;



    /// Show the result
    /*
    for( size_t i = 0; i < houghlines.size(); i++ )
    {
        cv::Vec4i l = houghlines[i];
        cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 3, CV_AA);
    }

    cv::imshow("Probabilistic Hough", src);
    cv::waitKey(0);
    */
}

