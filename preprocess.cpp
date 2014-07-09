#include "preprocess.h"
#include "Line.h"
#include "typedefs.h"
#include "square.h"
#include <opencv2/opencv.hpp>
#include <vector>

Preprocess::Preprocess(cv::Mat & image_)
{
    image = image_;
    houghThreshold = 96;
    minLineLength = 105;
    maxLineGap = 139;

    edgeDetection();
    lineDetection();
}

void Preprocess::getLines(Lines& lines_){
    lines_ = lines;
}

void Preprocess::showCanny()
{
    cv::imshow("Canny", canny);
    cv::waitKey();
}

void Preprocess::showHoughlines()
{
    cv::Mat imgHough;
    image.copyTo(imgHough);
    for( size_t i = 0; i < houghlines.size(); i++ )
    {
        cv::Vec4i l = houghlines[i];
        cv::line(imgHough, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 3, CV_AA);
    }

    cv::imshow("Probabilistic Hough", imgHough);
    cv::waitKey(0);
}

void Preprocess::edgeDetection(bool doBlur){
    cv::Mat gray;
    if (image.channels() == 3){
        cv::cvtColor(image,gray,cv::COLOR_RGB2GRAY);
    } else {
        gray = image;
    }

    if (doBlur){
        cv::GaussianBlur(gray, gray, cv::Size(5,5), 2);
    }
    cv::Canny(gray, canny, 30, 200, 3);
}

void Preprocess::lineDetection()
{
    /// Use Probabilistic Hough Transform
    cv::imshow("canny1", canny); cv::waitKey();
    cv::HoughLinesP(canny, houghlines, 1, CV_PI/180, houghThreshold, minLineLength, maxLineGap);

    for (size_t i = 0; i < houghlines.size(); i++)
    {
        Line l = Line(cv::Point(houghlines[i][0], houghlines[i][1]), cv::Point(houghlines[i][2], houghlines[i][3]));
        lines.push_back(l);
    }

}

