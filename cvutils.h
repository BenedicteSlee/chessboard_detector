#ifndef CVUTILS_H
#define CVUTILS_H

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "global.h"

namespace cvutils {

void PrintMatToConsole(cv::Mat&, int, int);
void PrintMatToFile(cv::Mat&, int, int, std::string);
void PrintJpg(cv::Mat&, const std::string&, int);
bool pointIsLess(cv::Point2d a, cv::Point2d b);
Points2d sortSquareCorners(Points2d&);
void rotate(cv::Mat &src, cv::Mat &dst, int angle);
void plotPoints(cv::Mat& image, Points2d& points, int radius = 2, cv::Scalar col = cv::Scalar(0,0,255), int lineThickness = 2);

cv::Point doubleToInt(cv::Point2d point2d);
Points doubleToInt(Points2d points2d);
bool negCoordinate(cv::Point2d);
std::vector<bool> negCoordinate(Points2d);
bool anyNegCoordinate(Points2d);

bool outOfBounds(cv::Mat& image, cv::Point2d point2d);

std::vector<bool> outOfBounds(cv::Mat& image, Points2d points);


void dilate(std::vector<int>&);
int sumderiv(std::vector<int>);

template <typename T>
cv::Point2d centerpoint(std::vector<T>);

cv::Point2d centerpoint(cv::Point2d point1, cv::Point2d point2);

bool pairIsLess(const std::pair<int, double> a, const std::pair<int, double> b);

bool containsPoint(const Points2d& points, const cv::Point2d& point);

template <typename T>
cv::Point_<T> posPoint(cv::Point_<T>& point){
    cv::Point_<T> newpoint;
    point.x < 0 ? newpoint.x = 0 : newpoint.x = point.x;
    point.y < 0 ? newpoint.y = 0 : newpoint.y = point.y;
    return newpoint;
}

template <typename T>
cv::Point2d centerpoint(std::vector<T> points){
    double xsum = 0;
    double ysum = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        xsum += points[i].x;
        ysum += points[i].y;
    }

    return cv::Point2d(xsum/(double) points.size(), ysum/ (double) points.size());

}



template<typename T>
std::vector<double> Stats(std::vector<T> input){
    std::vector<double> output(2);

    double mean = 0;
    double size = (double) input.size();
    for (size_t i=0; i<input.size(); i++){
        mean += input[i] / size;
    }

    double stdev = 0;
    for (size_t i=0; i<input.size(); i++){
        stdev += pow(input[i] - mean,2) / (size-1);
    }

    output[0] = mean;
    output[1] = stdev;

    return output;
}

template<typename T>
double meanNoOutliers(std::vector<T> vec){
    double mean = cv::mean(vec)[0];

    std::vector<double> dists(vec.size());

    //distance to mean
    for (size_t i = 0; i < vec.size(); i++){
        dists[i] = std::abs(vec[i] - mean);
    }

    double meandists = cv::mean(dists)[0];

    int count = 0;
    double sum = 0;
    for (size_t i = 0; i < vec.size(); i++){
        if (dists[i] < meandists){
            sum += vec[i];
            count ++;
        }
    }

    double newmean = sum / count ;
    return newmean;
}

template<typename T>
std::vector<size_t> flagOutliers(std::vector<T> vec, double tolerancePct = 0.1){
    if (vec.empty() || vec.size() == 0){
        throw std::invalid_argument("This vector is empty!");
    }

    double mean = meanNoOutliers(vec);
    std::cout << "mean: " << mean << std::endl;

    double tolerance = mean * tolerancePct;
    std::cout << "tolerance: " << tolerance << std::endl;

    std::vector<double> dists(vec.size());

    //distance to mean
    for (size_t i = 0; i < vec.size(); i++){
        dists[i] = std::abs(vec[i] - mean);
    }


    std::vector<size_t> flags(vec.size());
    for (size_t i = 0; i < vec.size(); i++){
        if (dists[i] > tolerance){
            flags.at(i) = 1;
        } else {
            flags.at(i) = 0;
        }
        std::cout << "flag[" <<i <<"]: " << flags.at(i) << std::endl;

    }
    return flags;
}

}
// end namespace

#endif // CVUTILS_H
