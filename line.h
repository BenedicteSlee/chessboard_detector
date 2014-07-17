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
    Line(std::vector<cv::Point2d>);
    Line(cv::Point2d, cv::Point2d);
    Line(double slope, double yIntercept);

    bool Intersection(Line&, cv::Point2d&) const;

    static void Intersections(std::vector<Line>& lines, std::vector<cv::Point2d> &intersections, cv::Point2d limits, std::vector<double>& distances); //TODO make own class called Lines with these methods?
    static void RemoveDuplicateIntersections(std::vector<cv::Point2d> &src, std::vector<cv::Point2d> &dst, std::vector<double>& distances);

    void FrameIntersections(const cv::Mat& image, Points2d frameintersections);

    double ylookup(double, int type = 1) const;
    double xlookup(double, int type = 1) const;

    std::vector<cv::Point2d> points;
    double yIntercept;
    double slope;

private:
    double x1, y1, x2, y2;
    void calcSlope();
    void calcIntercept();
};

// Template functions

#endif // LINE_H
