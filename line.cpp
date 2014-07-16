#include "Line.h"
#include "typedefs.h"
#include "square.h"
#include <vector>

Line::Line(){

}

Line::Line(std::vector<cv::Point2d> points_)
{
    points = points_;
    calcSlope();
    calcIntercept();

    x1 = points.at(0).x;
    y1 = points.at(0).y;
    x2 = points.at(1).x;
    y2 = points.at(1).y;
}

Line::Line(cv::Point2d point1, cv::Point2d point2)
{

    bool check1 = cvutils::negCoordinate(point1);
    bool check2 = cvutils::negCoordinate(point2);
    if (check1 || check2){
        throw std::invalid_argument("One of the points has a negative coordinate");
    }
    points.push_back(point1);
    points.push_back(point2);

    if (point1.x < point2.x){
        x1 = point1.x;
        x2 = point2.x;
        y1 = point1.y;
        y2 = point2.y;
    } else {
        x1 = point2.x;
        x2 = point1.x;
        y1 = point2.y;
        y2 = point1.y;

    }


    calcSlope();
    calcIntercept();


}

Line::Line(double slope_, double yIntercept_)
{
    x1 = -INFINITY;
    x2 = INFINITY;
    y1 = -INFINITY;
    y2 = INFINITY;

    slope = slope_;
    yIntercept = yIntercept_;

}

double Line::ylookup(double x, int type) const
{
    if (type == 0){
        if (x < std::min(x1,x2) || x > std::max(x1,x2))
            return -1;
    }
    double y = slope*x + yIntercept;
    return y;
}

double Line::xlookup(double y, int type) const
{
    if (type == 0){
        if (y < std::min(y1,y2) || y > std::max(y1,y2))
            return -1;
    }
    if (slope == INFINITY){
        return x1;
    }
    if (slope != 0){
        return - yIntercept / slope;
    }
    throw std::invalid_argument("Horizontal Line");
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

int Line::Intersection(Line& otherline, cv::Point2d& result) const {
    // y = ax + b; y = cx + d; solve for intersection

    double a = slope;
    double b = yIntercept;

    double c = otherline.slope;
    double d = otherline.yIntercept;

    if ((a-c) == 0){
        return -1; // lines are parallel
    }

    if (slope == INFINITY){
        result.x = x1;
        result.y = otherline.ylookup(x1);
        return 0;
    }

    if (otherline.slope == INFINITY){
        result.x = otherline.points.at(0).x;
        result.y = ylookup(result.x);


        return 0;
    }


    double x = (d-b)/(a-c);
    double y = a * x + b;

    result.x = x;
    result.y = y;
    return 0;
}

// Static methods
void Line::Intersections(std::vector<Line>& lines, std::vector<cv::Point2d>& intersections, cv::Point2d limits, std::vector<double>& distances)
{
    std::vector<cv::Point2d> unsortedIntersections;
    cv::Point2d p;
    for (size_t i=0; i < lines.size(); i++){
        Line line1 = lines.at(i);
        for (size_t j=i+1; j < lines.size(); j++){
            //std::cout << i << "," << j << std::endl;
            Line line2 = lines.at(j);
            int check = line1.Intersection(line2,p);
            //if (line1.ylookup(p.x) != -1)
            if (check != -1 && p.x >= 0 && p.y >= 0 && p.x <= limits.x && p.y <= limits.y)
                unsortedIntersections.push_back(p);
        }
    }

    Line::RemoveDuplicateIntersections(unsortedIntersections, intersections, distances);
    std::sort(intersections.begin(), intersections.end(), cvutils::pointIsLess); // TODO !!! This sorting does not work
}

void Line::RemoveDuplicateIntersections(std::vector<cv::Point2d> & src, std::vector<cv::Point2d> & dst, std::vector<double>& distances)
{

    double tol = 5; // distance tolerance in pixels
    std::vector<cv::Point2d> vec;
    vec = src;
    cv::Point2d p1,p2;
    double enorm;
    size_t i = 0;
    size_t j = 0;
    while (i < vec.size()){
        p1 = vec.at(i);
        j = i + 1;
        while (j < vec.size()){
            p2 = vec.at(j);
            enorm = cv::norm(p1-p2); // calc distance. TODO:Slow?
            if (enorm < tol){
                //std::cout << "p1: " << p1 << std::endl;
                //std::cout << "p2: " << p2 << std::endl;
                //std::cout << "distance: " << enorm << std::endl;
                vec.erase(vec.begin()+j);
            } else {
                j++;
                distances.push_back(enorm);
            }
        }
        i++;
    }

    dst = vec;


}







