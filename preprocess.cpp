#include "preprocess.h"
#include "Line.h"
#include "typedefs.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "settings.h"
#include "square.h"


Preprocess::Preprocess(cv::Mat & global::image_, Settings::PreprocessSettings settings_)
{
    settings = settings_;
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
    if (!imgHough.data){
        getHough();
    }
    cv::imshow("Probabilistic Hough", imgHough);
    cv::waitKey(0);
}

cv::Mat Preprocess::getHough(){
    if (!imgHough.data){
        global::image.copyTo(imgHough);
        cv::cvtColor(imgHough, imgHough, cv::COLOR_GRAY2BGR);
        for( size_t i = 0; i < houghlines.size(); i++ )
        {
            cv::Vec4i l = houghlines[i];
            cv::line(imgHough, cv::Point2d(l[0], l[1]), cv::Point2d(l[2], l[3]), cv::Scalar(20,110,255), 1, CV_AA);
        }
    }
    return imgHough;
}

void Preprocess::edgeDetection(bool doBlur){
    cv::Mat gray;
    if (global::image.channels() == 3){
        cv::cvtColor(global::image,gray,cv::COLOR_RGB2GRAY);
    } else {
        gray = global::image;
    }
    if (doBlur){
        //cv::GaussianBlur(gray, blurred, gaussianBlurSize, gaussianBlurSigma);
        cv::GaussianBlur(gray, gray, gaussianBlurSize, gaussianBlurSigma);
    }
    blurred = gray;
    cv::Canny(blurred, canny, 30, 200, 3);
}

void Preprocess::lineDetection()
{
    /// Use Probabilistic Hough Transform
    cv::HoughLinesP(canny, houghlines, 1, CV_PI/180, houghThreshold, minLineLength, maxLineGap);

    for (size_t i = 0; i < houghlines.size(); i++)
    {
        cv::Point2d p1(houghlines[i][0], houghlines[i][1]);
        cv::Point2d p2(houghlines[i][2], houghlines[i][3]);
        if (cvutils::outOfBounds(global::image,p1) || cvutils::outOfBounds(global::image,p2))
            throw std::invalid_argument("A point created by Houghlines is out of bounds");
        Line l = Line(p1, p2);
        lines.push_back(l);
    }
}

