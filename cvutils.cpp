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
{   // Ref: http://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order

    cv::Point center = cv::Point((a.x + b.x)*0.5, (a.y + b.y) * 0.5);

    if (a.x >= center.x && b.x < center.x ){
        return true;
    }
    if (a.x < center.x && b.x >= center.x){
        return false;
    }
    if (a.x == center.x && b.x == center.x) {
        if (a.y - center.y >= 0 || b.y - center.y >= 0)
            return a.y > b.y;
        return b.y > a.y;
    }

    int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
    int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
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

}

template<typename T>
bool cvutils::pairIsLess(const std::pair<T, double>& a, const std::pair<T, double>& b)
{
    return a.second < b.second;
}


bool cvutils::containsPoint(const Points& points, const cv::Point& point)
{
    if (points.empty()){
        return false;
    }

    for (size_t i = 0; i < points.size(); ++i) {
        if (point == points.at(i))
            return true;
    }

    return false;

}

void cvutils::sortPoints(Points& points)
{
    cv::Point center = centerpoint(points);

    std::vector<std::pair<cv::Point, double> > atans;
    atans.reserve(points.size());

    for (size_t i = 0; i < points.size(); ++i)
    {
        atans[i].first = points[i];
        atans[i].second = atan2(points[i].x - center.x, points[i].y - center.y);
    }

    std::sort(atans.begin(), atans.end(), [] (const std::pair<cv::Point, int>& left, const std::pair<cv::Point, int>& right) {
        return left.second < right.second;});
   // std::sort(atans.begin(), atans.end(), pairIsLess);
}



