#include <opencv2/opencv.hpp>
#include <vector>
#include "preprocess.h"
#include "Line.h"
#include "typedefs.h"
#include "settings.h"
#include "square.h"
#include "global.h"

cv::Mat global::image_r, global::image_g, global::image_b, global::image_rgb_resized; // forward decleration;

Preprocess::Preprocess()
{
    // Create global images
    cv::cvtColor(global::image_rgb, global::image_gray, CV_RGB2GRAY);
    cv::normalize(global::image_gray, global::image_norm, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    cv::resize(global::image_norm, global::image, cv::Size(1000, global::image_norm.rows * 1000/global::image_norm.cols));

    cv::resize(global::image_rgb, global::image_rgb_resized, cv::Size(global::image.cols, global::image.rows));
    cv::split(global::image_rgb_resized, global::channels); //splits into red, green, blue channels
    global::image_r = global::channels[0];
    global::image_g = global::channels[1];
    global::image_b = global::channels[2];
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

void Preprocess::detectLines(Settings::PreprocessSettings settings_)
{
    settings = settings_;
    edgeDetection();
    lineDetection();
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

    if (doBlur){
        //cv::GaussianBlur(gray, blurred, gaussianBlurSize, gaussianBlurSigma);
        cv::GaussianBlur(global::image, blurred, settings.gaussianBlurSize, settings.gaussianBlurSigma);
    } else {
    blurred = global::image;
    }
    cv::Canny(blurred, canny, settings.cannyLow, settings.cannyHigh, settings.cannySobel);
}

void Preprocess::lineDetection()
{
    /// Use Probabilistic Hough Transform
    cv::HoughLinesP(canny, houghlines, 1, CV_PI/180, settings.houghThreshold, settings.minLineLength, settings.maxLineGap);

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

