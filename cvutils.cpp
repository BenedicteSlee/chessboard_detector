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



