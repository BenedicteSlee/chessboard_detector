#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>
#include "Line.h"
#include "cvutils.h"
#include "typedefs.h"

class Preprocess
{
public:
    Preprocess(cv::Mat&);
    void getLines(Lines&);

    int houghThreshold;
    int minLineLength;
    int maxLineGap;

    void showCanny();
    void showHoughlines();

private:
    Lines lines;
    std::vector<cv::Vec4i> houghlines;
    cv::Mat image, canny;
    void edgeDetection(bool doBlur = true);
    void lineDetection();
};

#endif // PREPROCESS_H
