#include "layer.h"

#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "cvutils.h"

Layer::Layer(const std::vector<int> pixels)
{
    this->pixels = pixels;
    this->binaryPixels.reserve(pixels.size());
    binarize();
    vote();
}

Layers Layer::createLayers(const cv::Mat& area)
{
    Layers layers;
    int nc = area.cols;
    int nr = area.rows;

    int nLayers = std::min(nc, nr) / 2 - 1;
    std::vector<Points> layercorners(nLayers);
    for (int i = 0; i < nLayers; i++) {
        cv::Point upperleft = cv::Point(i, i);
        cv::Point upperright = cv::Point(nc-1-i, i);
        cv::Point lowerleft = cv::Point(i, nr-1-i);
        cv::Point lowerright = cv::Point(nc-1-i, nr-1-i);

        Points points{upperleft, upperright, lowerleft, lowerright};

        layercorners[i] = points;
    }

    for (int i = 0; i < nLayers; i++)
    {
        Points corners = layercorners[i];
        int n = 2 * (nc-i) + 2 * (nr-i) - 4;
        std::vector<int> pixels(n);

        int row = corners[0].y;
        int col = corners[0].x;
        int count = 0;
        while (col <= corners[1].x)
        {
            pixels[count] = (int) area.at<uchar>(col, row);
            col++;
            count++;
        }
        col--;
        while (row <= corners[2].y){
            pixels[count] = (int) area.at<uchar>(col, row);
            row++;
            count++;
        }
        row--;
        while (col >= corners[3].x){
            pixels[count] = (int) area.at<uchar>(col, row);
            col--;
            count++;
        }
        col++;
        while (row >= corners[0].y){
            pixels[count] = (int) area.at<uchar>(col, row);
            row--;
            count++;
        }

        Layer newlayer = Layer(pixels);

        layers.push_back(newlayer);
    }
    return layers;
}

void Layer::binarize()
{
    int meancol = (int) cv::mean(pixels)[0];
    for (size_t j = 0; j < pixels.size(); j++) {
        if (pixels[j] < meancol){
            binaryPixels.push_back(0);
        } else {
            binaryPixels.push_back(1);
        }

    }
    // TODO dilate/erode
}

void Layer::vote()
{
    nRegions = cvutils::sumderiv(binaryPixels);
}

int Layer::classify(const Layers& layers)
{
    int nRegions;
    if (layers.empty()){
        std::cout << "No layers" << std::endl; // TODO throw error
    }
    std::vector<int> histogram(4,0);
    for( int i=0; i< (int) layers.size(); ++i )
        ++histogram[ layers[i].nRegions ];

    int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
    int count = 0;
    for (size_t i = 0; i < layers.size(); ++i) {
        if (layers[i].nRegions == vote)
            count++;
    }

    if (count/layers.size() > 0.7)
        nRegions = vote;
    else
        nRegions = 0;
    return nRegions;
}

