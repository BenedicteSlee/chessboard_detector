#include "Line.h"
#include <vector>

Line::Line(){

}

Line::Line(std::vector<cv::Point> points_)
{
    points = points_;
    calcSlope();
    calcIntercept();

    x1 = points.at(0).x;
    y1 = points.at(0).y;
    x2 = points.at(1).x;
    y2 = points.at(1).y;
}

Line::Line(cv::Point point1, cv::Point point2)
{
    points.push_back(point1);
    points.push_back(point2);
    x1 = point1.x;
    y1 = point1.y;
    x2 = point2.x;
    y2 = point2.y;

    calcSlope();
    calcIntercept();
}

int Line::ylookup(int x)
{
    if (x < std::min(x1,x2) || x > std::max(x1,x2))
        return -1;
    return slope*x + yIntercept;
}

void Line::calcSlope(){
        if ((x2 - x1) != 0){
            slope = (y2-y1)/(x2-x1);
        } else {
            slope = INFINITY;
        }
}

void Line::calcIntercept(){
        yIntercept = y1 - slope*x1;
}

int Line::Intersection(Line& otherline, cv::Point& result){
    // y = ax + b; y = cx + d; solve for intersection
    int a = slope;
    int b = yIntercept;

    int c = otherline.slope;
    int d = otherline.yIntercept;

    if ((a-c) == 0){
        return -1; // lines are parallel
    }

    double x = (d-b)/(a-c);
    double y = a * x + b;


    result.x = x;
    result.y = y;
    return 0;
}

void Line::Intersections(std::vector<Line>& lines, std::vector<cv::Point>& intersections)
{
    cv::Point p;
    for (size_t i=0; i < lines.size(); i++){
        Line line1 = lines.at(i);
        for (size_t j=i+1; j < lines.size(); j++){
            std::cout << i << "," << j << std::endl;
            Line line2 = lines.at(j);
            line1.Intersection(line2,p);
            if (line1.ylookup(p.x) != -1)
                intersections.push_back(p);
        }
    }



}



