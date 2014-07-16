#ifndef REGRESSION_H
#define REGRESSION_H

#include <vector>
#include "Line.h"

template <class T>
class Regression
{
public:
    Regression(std::vector<T> data_){
        data = data_;
        linearRegression(data);
    }

    void linearRegression(const std::vector<T>& data)
    {
        // Ref: http://stackoverflow.com/questions/18939869/how-to-get-the-slope-of-a-linear-regression-line-using-c
        double xSum = 0, ySum = 0, xxSum = 0, xySum = 0, slope, intercept;
        std::vector<double> xData;
        for (size_t i = 0; i < data.size(); i++)
        {
            xData.push_back(i);
        }
        for (size_t i = 0; i < data.size(); i++)
        {
            xSum += xData[i];
            ySum += data[i];
            xxSum += xData[i] * xData[i];
            xySum += xData[i] * data[i];
        }
        slope = (data.size() * xySum - xSum * ySum) / (data.size() * xxSum - xSum * xSum);
        yIntercept = (ySum - slope * xSum) / data.size();

        line = Line(slope, yIntercept);
    }

    double squaredError(int i){
        return pow(std::abs(data[i] - line.ylookup(i)), 2);
    }

    std::vector<double> squaredErrors(){
        std::vector<double> result(data.size());
        for (size_t i = 0; i < data.size(); i++){
            result[i] = squaredError(i);
        }
        return result;
    }


private:
    std::vector<T> data;
    T slope;
    T yIntercept;
    Line line;

};

#endif // REGRESSION_H
