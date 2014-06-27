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

} // end namespace

#endif // CVUTILS_H
