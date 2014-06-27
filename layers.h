#ifndef LAYER_H
#define LAYER_H

#include <opencv2/opencv.hpp>
#include <typedefs.h>

class Layers
{
    friend class Corner;
public:
    Layers(const cv::Mat& area);
private:
    const cv::Mat& area;
    std::vector<std::vector<int>> layers;
    std::vector<std::vector<int>> binary;
    int nRegions;

    void init();
    void binarize();
    void vote();
    void classify();
};

#endif // LAYER_H
