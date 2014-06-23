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

private:
    Lines lines;
    cv::Mat image;
    void edgeDetection(cv::Mat&, bool doBlur = true);
    void lineDetection(cv::Mat&);
};

#endif // PREPROCESS_H
