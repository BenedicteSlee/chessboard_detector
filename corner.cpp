#include "corner.h"
#include "cvutils.h"
#include "typedefs.h"
#include "layers.h"
#include <opencv2/opencv.hpp>

Corner::Corner(const cv::Mat& image_, cv::Point cornerpoint_, Lines lines_, int radius_)
{
    image_gray = image_;
    lines = lines_;
    initialCornerpoint = cornerpoint_;
    cornerpoint = cornerpoint_;
    radius = radius_;

    // Create area around cornerpoint
    area = image_gray(cv::Rect(cornerpoint.x - radius,cornerpoint.y - radius, radius*2 , radius*2));

    Layers layers(area);
    nRegions = layers.nRegions;

}

cv::Mat Corner::getArea()
{
    return area;
}

int Corner::getNRegions()
{
    return nRegions;
}

 /*
void Corner::createLayers()
{

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

    std::vector<std::vector<int>> layers(nLayers);

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

        layers[i] = layer;

        for (size_t i = 0; i < layer.size(); ++i) {
            std::cout << layer[i] << std::endl;
        }
    }

}
*/
