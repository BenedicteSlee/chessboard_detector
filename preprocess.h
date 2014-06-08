#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>
#include "Line.h"

class Preprocess
{
public:
    Preprocess();
    void getLines(cv::Mat&, cv::Mat&, std::vector<Line>&);

    int houghThreshold;
    int minLineLength;
    int maxLineGap;

private:
    void edgeDetection(cv::Mat&, cv::Mat&);
    void lineDetection(cv::Mat&, std::vector<Line>&);
};

#endif // PREPROCESS_H
