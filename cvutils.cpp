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

    cv::Point2d center = cv::Point((a.x + b.x)*0.5, (a.y + b.y) * 0.5);

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


cv::Point2d cvutils::centerpoint(Points points){
    double xsum = 0;
    double ysum = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        xsum += points[i].x;
        ysum += points[i].y;
    }

    return cv::Point2d(xsum/(double) points.size(), ysum/ (double) points.size());

}

bool cvutils::pairIsLess(const std::pair<int, double> a, const std::pair<int, double> b)
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
{ // Ref: http://www.cplusplus.com/forum/general/116020/
    cv::Point2d center = centerpoint(points);

    std::vector<std::pair<int, double> > atans(points.size());

    for (size_t i = 0; i < points.size(); ++i)
    {
        atans[i].first = i;
        atans[i].second = atan2(points[i].y - center.y, points[i].x - center.x);

    }

    std::sort(atans.begin(), atans.end(),
              [] (const std::pair<int, double>& left, const std::pair<int, double>& right) -> bool
                    {return left.second > right.second;});


    Points originalPoints = points;

    for (size_t i = 0; i < points.size(); ++i) {
        points[i] = originalPoints[atans[i].first];
    }

}

int cvutils::sumderiv(std::vector<int> input)
{
    int result = 0;
    for (size_t i = 1; i < input.size(); ++i) {
        result += abs(input[i] - input[i-1]);
    }
    return result;
}


void cvutils::dilate(std::vector<int> & binaryPixels)
{
    int start = 0;
    int count = 0;
    int a = binaryPixels[0];
    for (int i = 1; i < (int) binaryPixels.size(); ++i) {
        int b = binaryPixels[i];
        if (a == b){
            count++;
        }
        else {
            if (count < (int) binaryPixels.size() / 8){
                for (int j = start; j < i; ++j) {
                    binaryPixels[j] = b;
                }
            }
            a = b;
            count = 0;
        }

    }
}
