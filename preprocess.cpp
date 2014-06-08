#include "preprocess.h"
#include "Line.h"
#include <opencv2/opencv.hpp>
#include <vector>

Preprocess::Preprocess()
{
    houghThreshold = 96;
    minLineLength = 105;
    maxLineGap = 139;
}

void Preprocess::getLines(cv::Mat& image, cv::Mat& dst, std::vector<Line>& lines){
    std::cout << "preprocess..." << std::endl;
    edgeDetection(image, dst);
    lineDetection(dst, lines);
}

void Preprocess::edgeDetection(cv::Mat& src, cv::Mat& dst){
    std::cout << "edge detection..." << std::endl;
    cv::Mat gray, blur;
    cv::cvtColor(src,gray,cv::COLOR_RGB2GRAY);
    cv::GaussianBlur(gray, blur, cv::Size(0,0), 2);
    cv::Canny(blur, dst, 30, 200, 3);
}

void Preprocess::lineDetection(cv::Mat& src, std::vector<Line>& lines)
{
    std::cout << "line detection..." << std::endl;
    /// Use Probabilistic Hough Transform

    std::vector<cv::Vec4i> houghlines;
    cv::HoughLinesP(src, houghlines, 1, CV_PI/180, houghThreshold, minLineLength, maxLineGap);

    for (size_t i = 0; i < houghlines.size(); i++)
    {
        Line l = Line(cv::Point(houghlines[i][0], houghlines[i][1]), cv::Point(houghlines[i][2], houghlines[i][3]));
        lines.push_back(l);
    }
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

