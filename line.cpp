#include "Line.h"
#include "typedefs.h"
#include "chessboard.h"
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
        result.y = this->ylookup(result.x);
        return 0;
    }


    double x = (d-b)/(a-c);
    double y = a * x + b;

    result.x = x;
    result.y = y;
    return 0;
}

// Static methods
void Line::Intersections(std::vector<Line>& lines, std::vector<cv::Point>& intersections, cv::Point limits, std::vector<double>& distances)
{
    Points unsortedIntersections;
    cv::Point p;
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
    std::sort(intersections.begin(), intersections.end(), cvutils::pointIsLess);
}

void Line::RemoveDuplicateIntersections(std::vector<cv::Point> & src, std::vector<cv::Point> & dst, std::vector<double>& distances)
{
    std::cout << "Original vector length: " << src.size() << std::endl;
    double tol = 5; // distance tolerance in pixels
    std::vector<cv::Point> vec;
    vec = src;
    cv::Point p1,p2;
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
    std::cout << "New vector length: " << dst.size() << std::endl;

}

void Line::FrameIntersections(const cv::Mat& image, Points frameintersections)
{
    int cols = image.cols;
    int rows = image.rows;

    Square frame = Square(cv::Point(0,0), cv::Point(cols, 0), cv::Point(cols, rows), cv::Point(0,rows));

    cv::Point p;
    int check;
    for (size_t i = 0; i < 4; ++i) {
        check = this->Intersection(frame.borders.at(i), p);
        if (check != -1){
            frameintersections.push_back(p);
        }
    }
}






