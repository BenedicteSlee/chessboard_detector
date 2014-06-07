#ifndef LINE_H
#define LINE_H

#include <opencv2/opencv.hpp>
#include <vector>

class Line
{
public:
    Line();
    Line(std::vector<cv::Point>);
    Line(cv::Point, cv::Point);
    int Intersection(Line&, cv::Point&);

    std::vector<cv::Point> points;
    double yIntercept;
    double slope;

private:
    double x1, y1, x2, y2;
    void calcSlope();
    void calcIntercept();

};

#endif // LINE_H
