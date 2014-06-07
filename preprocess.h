#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>

class Preprocess
{
public:
    Preprocess(cv::Mat&);
    int edgeDetection();
    int lineDetection();
};

#endif // PREPROCESS_H
