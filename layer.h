#ifndef LAYER_H
#define LAYER_H

#include <opencv2/opencv.hpp>
#include <typedefs.h>

class Layer
{
    friend class Corner;
public:
    Layer(const std::vector<int> pixels);
private:
    std::vector<int> pixels;
    std::vector<int> binaryPixels;
    int nRegions;

    static Layers createLayers(const cv::Mat&);
    void binarize();
    void vote();
    static int classify(const Layers&);
};

#endif // LAYER_H
