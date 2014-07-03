#include "corner.h"
#include "cvutils.h"
#include "typedefs.h"
#include <opencv2/opencv.hpp>

Corner::Corner(const cv::Mat& image, cv::Point cornerpoint, int radius)
{
    classified = false;
    this->initialCornerpoint = cornerpoint;
    this->cornerpoint = cornerpoint;
    this->radius = radius;

    // Create area around cornerpoint
    area = image(cv::Rect(cornerpoint.x - radius,cornerpoint.y - radius, radius*2 , radius*2));

    //recalculateCornerpoint(); //TODO

    classify(); // set nRegions;
}

cv::Mat Corner::getArea()
{
    return area;
}

int Corner::getNRegions()
{
    return nRegions;
}

void Corner::classify(){
    int nc = area.cols;
    int nr = area.rows;

    int nLayers = std::min(nc, nr) / 2 - 1;
    std::vector<Points> layercorners(nLayers);

    // Determine coordinates of corners in each layer
    for (int i = 0; i < nLayers; i++) {
        cv::Point upperleft = cv::Point(i, i);
        cv::Point upperright = cv::Point(nc-1-i, i);
        cv::Point lowerright = cv::Point(nc-1-i, nr-1-i);
        cv::Point lowerleft = cv::Point(i, nr-1-i);


        Points points{upperleft, upperright, lowerright, lowerleft};

        layercorners[i] = points;
    }

    for (int i = 0; i < nLayers; i++)
    {

        int n = 2 * (nc-1-i*2) + 2 * (nr-1-i*2); // number of pixels in layer
        std::vector<int> layer(n);

        Points corners = layercorners[i];

        // upper left corner to upper right corner
        int row = corners[0].y;
        int col = corners[0].x;
        int idx = 0;
        while (col < corners[1].x)
        {
            layer[idx] = (int) area.at<uchar>(col, row);
            col++;
            idx++;
        }

        // upper right corner to lower right corner
        while (row < corners[2].y)
        {
            layer[idx] = (int) area.at<uchar>(col, row);
            row++;
            idx++;
        }

        // lower right corner to lower left corner
        while (col > corners[3].x)
        {
            layer[idx] = (int) area.at<uchar>(col, row);
            col--;
            idx++;
        }

        // lower left corner to upper left corner
        while (row > corners[0].y)
        {
            layer[idx] = (int) area.at<uchar>(col, row);
            row--;
            idx++;
        }


        layers.push_back(layer);
    }

    // binarize
    for (size_t i = 0; i < layers.size(); ++i) {
        std::vector<int> grayLayer = layers[i];
        std::vector<int> binLayer(grayLayer.size());
        int meancol = (int) cv::mean(grayLayer)[0];
        for (size_t j = 0; j < grayLayer.size(); j++) {
            if (grayLayer[j] < meancol){
                binLayer[j] = 0;
            } else {
                binLayer[j] = 1;
            }
        }
        binaryLayers.push_back(binLayer);
    }

    // vote
    double nLayersConsulted = binaryLayers.size() / 2; // many corners arent centered precisely on the intersection so the innermost layers dont traverse all four regions
    std::vector<int> nRegionsVotes(nLayersConsulted);
    for (size_t i = 0; i < nLayersConsulted; ++i) {
        std::vector<int> dilated = binaryLayers[i];
        cvutils::dilate(dilated);
        int sumderiv = cvutils::sumderiv(dilated);
        if (dilated[0] != dilated[dilated.size()-1]) // unlikely case that the layer is started right on the edge of a region
            sumderiv++;
        nRegionsVotes[i] = std::min(5,sumderiv); // if more than 4 regions found, set to 5. Only care about the fact that it is more than 4.
    }

    std::vector<int> histogram(5,0);
    for( int i=0; i< (int) nRegionsVotes.size(); ++i )
        ++histogram[ nRegionsVotes[i] ];

    int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();

    int count = 0;
    for (size_t i = 0; i < binaryLayers.size(); ++i) {
        if (nRegionsVotes[i] == vote)
            count++;
    }

    double pct = count / (double) nLayersConsulted;
    if (pct > 0.7)
        nRegions = vote;
    else
        nRegions = 0;

    classified = true;
}

void Corner::recalculateCornerpoint()
{ // TODO look at: http://stackoverflow.com/questions/4292249/automatic-calculation-of-low-and-high-thresholds-for-the-canny-operation-in-open
    int meancol =  (int) cv::mean(area)[0];
    cv::Mat binArea = area;
    //cv::threshold(area, binArea, 200, 255, 1);
    //cv::GaussianBlur(area, binArea, cv::Size(1,1), 1);
    cv::Canny(binArea, binArea, 30, 200, 3);
    std::vector<cv::Vec4i> houghlines;
    cv::HoughLinesP(binArea, houghlines, 1, CV_PI/180, 10);

    for( size_t i = 0; i < houghlines.size(); i++ )
        {
            cv::line(binArea, cv::Point(houghlines[i][0], houghlines[i][1]),


                cv::Point(houghlines[i][2], houghlines[i][3]), cv::Scalar(0,255,0), 10, 8 );
        }

}
