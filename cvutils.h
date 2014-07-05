#ifndef CVUTILS_H
#define CVUTILS_H


#include <opencv2/opencv.hpp>
#include "typedefs.h"

namespace cvutils {

void PrintMatToConsole(cv::Mat&, int, int);
void PrintMatToFile(cv::Mat&, int, int, std::string);
void PrintJpg(cv::Mat&, const std::string&, int);
bool pointIsLess(cv::Point a, cv::Point b);
void sortPoints(Points &);
void dilate(std::vector<int>&);
int sumderiv(std::vector<int>);

cv::Point2d centerpoint(Points);

bool pairIsLess(const std::pair<int, double> a, const std::pair<int, double> b);


bool containsPoint(const Points& points, const cv::Point& point);

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
T meanNoOutliers(std::vector<T> vec){
    T mean = cv::mean(vec)[0];

    std::vector<double> dists(vec.size());

    //distance to mean
    for (size_t i = 0; i < vec.size(); i++){
        dists[i] = std::abs(vec[i] - mean);
    }

    int meandists = cv::mean(dists)[0];

    int count = 0;
    double sum = 0;
    for (size_t i = 0; i < vec.size(); i++){
        if (dists[i] < meandists){
            sum += vec[i];
            count ++;
        }
    }

    double newmean = sum / count ;
    return (T) newmean;
}

template<typename T>
std::vector<int> outliers(std::vector<T> vec){
    if (vec.empty()){
        throw std::invalid_argument("This vector is empty!");
    }

    double mean = meanNoOutliers(vec);

    double tolerance = mean * 0.1;

    std::vector<double> dists(vec.size());

    //distance to mean
    for (size_t i = 0; i < vec.size(); i++){
        dists[i] = std::abs(vec[i] - mean);
    }

    std::vector<int> flags(vec.size());
    for (size_t i = 0; i < vec.size(); i++){
        if (dists[i] > tolerance){
            flags.at(i) = 1;
        } else {
            flags.at(i) = 0;
        }

    }

    return flags;

}

}
// end namespace

#endif // CVUTILS_H
