#ifndef SETTINGS_H
#define SETTINGS_H

#include <opencv2/opencv.hpp>

namespace Settings{

struct PreprocessSettings{
    int houghThreshold, minLineLength, maxLineGap, gaussianBlurSigma, cannyLow, cannyHigh, cannySobel;
    cv::Size gaussianBlurSize;

    PreprocessSettings(){
        houghThreshold = 96;
        minLineLength = 200;
        //minLineLength = 105;
        maxLineGap = 139;
        gaussianBlurSize = cv::Size(5,5); // must be odd
        gaussianBlurSigma = 3;
        cannyLow = 30;
        cannyHigh = 200;
        cannySobel = 3;
    }
};

} // end namespace settings

#endif // SETTINGS_H
