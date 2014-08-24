#ifndef EXPANDSQUARE_H
#define EXPANDSQUARE_H

#include "square.h"
#include "Line.h"


class SquareExpander
{
public:
    SquareExpander(cv::Mat& image_, Square square_, Direction dir_);
    Square getSquare(){return newSquare;}
    bool hasExpanded(){return canExpand;}

private:
    cv::Mat& image;
    Square baseSquare;
    Square newSquare;
    Points2d cpoints;
    Direction dir;
    bool canExpand;
    Line b1, b2; // left border, rigth border relative to having base square behind you and looking in direction of expansion
    cv::Point2d c1, c2; // left corner, right corner
    cv::Point2d A, B, C, D, E, F, G, H, I, J, K, L;
    cv::Point2d topMid, rightMid, bottomMid, leftMid;

    void calculatePoints();
    void namePoints();
    void nameBorders();
    bool calculateExtrapolatedPoints();
    void createSquare();
    void draw(cv::Mat global::image);




};

#endif // EXPANDSQUARE_H
