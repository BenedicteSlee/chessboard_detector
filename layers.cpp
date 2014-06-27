#include "layers.h"

#include <opencv2/opencv.hpp>
#include "typedefs.h"
#include "cvutils.h"

Layers::Layers(const cv::Mat& area) : area(area)
{
    init();
    binarize();
    vote();
    classify();
}

void Layers::init(){
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

    layers.reserve(nLayers);
    for (int i = 0; i < nLayers; i++)
    {

        Points corners = layercorners[i];
        int n = 2 * (nc-i) + 2 * (nr-i) - 4;
        std::vector<int> layer(n);

        int row = corners[0].y;
        int col = corners[0].x;
        int count = 0;
        while (col <= corners[1].x)
        {
            layer[count] = (int) area.at<uchar>(col, row);
            col++;
            count++;
        }
        col--;
        while (row <= corners[2].y){
            layer[count] = (int) area.at<uchar>(col, row);
            row++;
            count++;
        }
        row--;
        while (col >= corners[3].x){
            layer[count] = (int) area.at<uchar>(col, row);
            col--;
            count++;
        }
        col++;
        while (row >= corners[0].y){
            layer[count] = (int) area.at<uchar>(col, row);
            row--;
            count++;
        }

        layers.push_back(layer);
    }
}

void Layers::binarize()
{
    for (size_t i = 0; i < layers.size(); i++) {
        std::vector<int> layer = layers[i];
        int meancol = (int) cv::mean(layer)[0];
        std::vector<int> bin(layer.size());
        for (size_t j = 0; j < bin.size(); j++) {
            if (layer[j] < meancol){
                bin[j] = 0;
            } else {
                bin[j] = 1;
            }

        }
        binary.push_back(bin);
    }
}

void Layers::vote()
{
    std::vector<int> nRegionsVotes(binary.size());
    for (size_t i = 0; i < binary.size(); ++i) {
        std::vector<int> bin = binary[i];
        nRegionsVotes[i] = cvutils::sumderiv(bin);
    }

    std::vector<int> histogram(4,0);
    for( int i=0; i< (int) nRegionsVotes.size(); ++i )
      ++histogram[ nRegionsVotes[i] ];

    int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
    int count = 0;
    for (size_t i = 0; i < nRegionsVotes.size(); ++i) {
        if (nRegionsVotes[i] == vote)
            count++;
    }

    if (nRegionsVotes.size() > 0 && count/nRegionsVotes.size() > 0.7)
        nRegions = vote;
    else
        nRegions = 0;

}

void Layers::classify(){

}
