#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <math.h>
#include "cvutils.h"

using namespace cv;
using namespace std;

//double duration = static_cast<double>(cv::getTickCount());
//duration = static_cast<double>(cv::getTickCount()) - duration;
//duration = duration / cv::getTickFrequency(); // get elapsed time
//std::cout << "Duration: " << duration << std::endl;


void cvutils::PrintJpg(cv::Mat& img, const std::string& filename, int quality){
    std::vector<int> imwriteparams;
    imwriteparams.push_back(CV_IMWRITE_JPEG_QUALITY);
    imwriteparams.push_back(quality);
    cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/images/"+filename+".jpg",img, imwriteparams);
}

cv::Point cvutils::MeanPoint(Points points){
    std::vector<int> xvals(points.size());
    std::vector<int> yvals(points.size());

    for (size_t i=0;i<points.size();i++){
        xvals[i] = points[i].x;
        yvals[i] = points[i].y;
    }

    double meanx =  cv::mean(xvals)[0];
    double meany = cv::mean(yvals)[0];
    return cv::Point(meanx,meany);
}

void cvutils::PrintMatToConsole(Mat& mat, int lastRow, int lastCol)
{
    if (lastRow == 0)
        lastRow = mat.rows;
    if (lastCol == 0)
        lastCol = mat.cols;

    for(int i = 0; i < lastRow; i++)
    {
        for (int j = 0; j < lastCol; j++)
        {
            cout<< mat.at<double>(i, j) << "\t";
        }
        cout<<endl;
    }
}

void cvutils::PrintMatToFile(Mat& mat, int lastRow, int lastCol, string fileName){
    if (lastRow == 0)
        lastRow = mat.rows;
    if (lastCol == 0)
        lastCol = mat.cols;

    ofstream myfile;

    myfile.open(fileName);

    if (!myfile.is_open()){
        cout << "Cannot open the file" << endl;
    } else {
        cout << "Printing..." << endl;

        for(int i = 0; i < lastRow; i++)
        {
            for (int j = 0; j < lastCol; j++)
            {
                myfile << mat.at<double>(i, j) << "\t";
            }
            myfile << endl;
        }

        myfile.close();
    }

}


bool cvutils::pointIsLess(Point a, Point b) // for sorting corners in square starting at upper left corner
{
    if (a.y < b.y)
        return true;
    if (a.y > b.y)
        return false;
    if (a.x < b.x)
        return true;

/*
// Ref: http://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order
{
    if (a.x >= 0 && b.x < 0)
        return true;
    if (a.x == 0 && b.x == 0)
        return a.y > b.y;

    std::vector<cv::Point> points;
    points.push_back(a);
    points.push_back(b);

    cv::Point c = centerpoint(points);
    double det = (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    double d1 = (a.x - c.x) * (a.x - c.x) + (a.y - c.y) * (a.y - c.y);
    double d2 = (b.x - c.x) * (b.x - c.x) + (b.y - c.y) * (b.y - c.y);

    return d1 > d2;
}

cv::Point cvutils::centerpoint(Points points){
    double xsum = 0;
    double ysum = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        xsum += points[i].x;
        ysum += points[i].y;
    }

    return cv::Point(xsum/(double) points.size(), ysum/ (double) points.size());
    */
}





bool cvutils::pairIsLess(const std::pair<int, double> a, const std::pair<int, double> b)
{
    if (a.second < b.second)
        return true;

    return false;

}
