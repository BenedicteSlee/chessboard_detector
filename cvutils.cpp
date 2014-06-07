#ifndef CVUTILS_CPP
#define CVUTILS_CPP

#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

namespace utils {

void PrintMatToConsole(Mat& mat, int lastRow, int lastCol)
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

void PrintMatToFile(Mat& mat, int lastRow, int lastCol, string fileName){
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
}

#endif
