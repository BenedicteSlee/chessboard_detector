#ifndef EXPANDSQUARE_H
#define EXPANDSQUARE_H

#include "square.h"
#include "Line.h"


class SquareExpander
{
public:
    SquareExpander(cv::Mat& image_, Square square_, Direction dir_);
    Square getSquare(){return newSquare;}

private:
    cv::Mat& image;
    Square square;
    Square newSquare;
    Points cpoints;
    Direction dir;
    Line b1, b2; // left border, rigth border relative to having base square behind you and looking in direction of expansion
    cv::Point c1, c2; // left corner, right corner
    cv::Point A, B, C, D, E, F, G, H, I, J, K, L;
    cv::Point topMid, rightMid, bottomMid, leftMid;

    void calculatePoints();
    void namePoints();
    void nameBorders();
    void calculateExtrapolatedPoints();
    void createSquare();




};

#endif // EXPANDSQUARE_H
