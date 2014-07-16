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
    //minLineLength = 105;
    minLineLength = 200;
    maxLineGap = 139;
    gaussianBlurSize = cv::Size(5,5); // must be odd
    gaussianBlurSigma = 3;
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
        cv::line(imgHough, cv::Point2d(l[0], l[1]), cv::Point2d(l[2], l[3]), cv::Scalar(255,0,0), 1, CV_AA);
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
        cv::GaussianBlur(gray, gray, gaussianBlurSize, gaussianBlurSigma);
    }
    cv::Canny(gray, canny, 30, 200, 3);
}

void Preprocess::lineDetection()
{
    /// Use Probabilistic Hough Transform
    cv::HoughLinesP(canny, houghlines, 1, CV_PI/180, houghThreshold, minLineLength, maxLineGap);

    for (size_t i = 0; i < houghlines.size(); i++)
    {
        cv::Point2d p1(houghlines[i][0], houghlines[i][1]);
        cv::Point2d p2(houghlines[i][2], houghlines[i][3]);
        if (cvutils::outOfBounds(image,p1) || cvutils::outOfBounds(image,p2))
            throw std::invalid_argument("A point created by Houghlines is out of bounds");
        Line l = Line(p1, p2);
        lines.push_back(l);
    }
}

