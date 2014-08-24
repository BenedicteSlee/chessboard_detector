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
    cv::imwrite("/Users/benedicte/Dropbox/kings/thesis/global::images/"+filename+".jpg",img, imwriteparams);
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

bool cvutils::pointIsLess(cv::Point2d a, cv::Point2d b) // for sorting corners in square starting at upper left corner
{   // Ref: http://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order

    cv::Point2d center = cv::Point2d((a.x + b.x)*0.5, (a.y + b.y) * 0.5);

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

cv::Point2d cvutils::centerpoint(cv::Point2d point1, cv::Point2d point2)
{
    Points2d points{cv::Point2d(point1.x,point1.y), cv::Point2d(point2.x, point2.y)};
    return cvutils::centerpoint(points);
}

bool cvutils::pairIsLess(const std::pair<int, double> a, const std::pair<int, double> b)
{
    return a.second < b.second;
}

bool cvutils::containsPoint(const Points2d& points, const cv::Point2d& point)
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

Points2d cvutils::sortSquareCorners(Points2d &points){
    if (points.size() != 4){
        throw std::invalid_argument("Give me four points");
    }

    Points2d sortedPoints(4);

    Points2d pointsSortedByY = points;
    std::sort(pointsSortedByY.begin(), pointsSortedByY.end(), [] (cv::Point2d a, cv::Point2d b){return a.y < b.y;});

    if (pointsSortedByY[0].x < pointsSortedByY[1].x){
        sortedPoints[0] = pointsSortedByY[0];
        sortedPoints[1] = pointsSortedByY[1];
    } else {
        sortedPoints[0] = pointsSortedByY[1];
        sortedPoints[1] = pointsSortedByY[0];
    }

    if (pointsSortedByY[2].x > pointsSortedByY[3].x){
        sortedPoints[2] = pointsSortedByY[2];
        sortedPoints[3] = pointsSortedByY[3];
    } else {
        sortedPoints[2] = pointsSortedByY[3];
        sortedPoints[3] = pointsSortedByY[2];
    }

    return sortedPoints;

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


Point cvutils::doubleToInt(Point2d point2d){
    cv::Point point;
    int x = (int) point2d.x;
    int y = (int) point2d.y;
    return cv::Point(x,y);
}


Points cvutils::doubleToInt(Points2d points2d)
{
    Points points(points2d.size());
    for (size_t i = 0; i < points2d.size(); i++){
        points[i] = doubleToInt(points2d[i]);
    }
    return points;
}


bool cvutils::negCoordinate(Point2d point2d)
{
    bool check;
    (point2d.x < 0 || point2d.y < 0) ? check = true : check = false;
    return check;
}


std::vector<bool> cvutils::negCoordinate(Points2d points2d)
{
    std::vector<bool> checks(points2d.size());
    for (size_t i = 0; i < points2d.size(); i++){
        checks[i] = negCoordinate(points2d[i]);
    }
    return checks;
}


bool cvutils::anyNegCoordinate(Points2d points2d)
{
    std::vector<bool> checks = negCoordinate(points2d);
    return std::any_of(checks.begin(), checks.end(), [] (bool b){return b;});
}


bool cvutils::outOfBounds(Mat &global::image, Point2d point2d){
    bool isOutOfBounds = (point2d.x > global::image.cols || point2d.x < 0 || point2d.y > global::image.rows || point2d.y < 0);
    return isOutOfBounds;
}


std::vector<bool> cvutils::outOfBounds(Mat &image, Points2d points){
    std::vector<bool> result(points.size());
    for (size_t i = 0; i < points.size(); i++){
        result[i] = outOfBounds(global::image, points[i]);
    }
    return result;
}


void cvutils::plotPoints(Mat &global::image, Points2d &points, int radius, cv::Scalar col, int lineThickness){
    cv::Mat rgb;


    if (global::image.channels() == 1){
        global::image.copyTo(rgb);
        cv::cvtColor(global::image, rgb, cv::COLOR_GRAY2RGB);
    } else {
        rgb = global::image;
    }

    for (auto it = points.begin(); it != points.end(); it++){
        cv::Point2d pt = *it;

        if (!negCoordinate(pt))
            cv::circle(rgb, pt, radius, col, lineThickness);
        else
            std::cout << "negative point" << std::endl;
    }
    cv::imshow("points", rgb);
    cv::waitKey();
}


void cvutils::rotate(Mat &src, Mat &dst, int angle)
{
    cv::Point2f p(src.cols/2.0, src.rows/2.0);
    Mat r = cv::getRotationMatrix2D(p, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));
}
