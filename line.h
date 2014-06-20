#ifndef LINE_H
#define LINE_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "cvutils.h"
#include "typedefs.h"

class Line
{
public:
    Line();
    Line(std::vector<cv::Point>);
    Line(cv::Point, cv::Point);
    int Intersection(Line&, cv::Point&);
    static void Intersections(std::vector<Line>& lines, std::vector<cv::Point>& intersections, cv::Point limits, std::vector<double>& distances); //TODO make own class called Lines with these methods?
    static void RemoveDuplicateIntersections(std::vector<cv::Point>& src, std::vector<cv::Point>& dst, std::vector<double>& distances);

    void FrameIntersections(const cv::Mat& image, Points frameintersections);

    int ylookup(int, int type = 0);
    int xlookup(int, int type = 0);

    std::vector<cv::Point> points;
    double yIntercept;
    double slope;

private:
    double x1, y1, x2, y2;
    void calcSlope();
    void calcIntercept();
};


#endif // LINE_H
