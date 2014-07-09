#include "squareExpander.h"
#include "typedefs.h"
#include <vector>


void SquareExpander::calculatePoints()
{// Ref TAM--Automatic_grid_segmentation_of_populated_chessboard_taken_at_a_lower_angle_view.pdf
    C = square.getCenter();


    topMid = cvutils::centerpoint(cpoints.at(1), cpoints.at(0));
    rightMid = cvutils::centerpoint(cpoints.at(2), cpoints.at(1));
    bottomMid = cvutils::centerpoint(cpoints.at(3), cpoints.at(2));
    leftMid = cvutils::centerpoint(cpoints.at(0), cpoints.at(3));
}

SquareExpander::SquareExpander(cv::Mat &image_, Square square_, Direction dir_) : image(image_)
{
    square = square_;
    dir = dir_;

    cpoints = square.getCornerpointsSorted();

    calculatePoints();
    namePoints();
    nameBorders();
    calculateExtrapolatedPoints();
    createSquare();

}

void SquareExpander::namePoints()
{
   switch(dir){
   case UP:
       c1 = cpoints.at(0);
       c2 = cpoints.at(1);

       D = leftMid;
       E = topMid;
       F = rightMid;
       G = bottomMid;
       break;
   case RIGHT:
       c1 = cpoints.at(1);
       c2 = cpoints.at(2);

       D = topMid;
       E = rightMid;
       F = bottomMid;
       G = leftMid;
       break;
   case DOWN:
       c1 = cpoints.at(2);
       c2 = cpoints.at(3);

       D = rightMid;
       E = bottomMid;
       F = leftMid;
       G = topMid;
       break;
   case LEFT:
       c1 = cpoints.at(3);
       c2 = cpoints.at(0);

       D = bottomMid;
       E = leftMid;
       F = topMid;
       G = rightMid;
       break;
   }
}

void SquareExpander::nameBorders()
{
    Lines borders = square.getBordersSorted();

    switch(dir){
    case UP:
        b1 = borders.at(0);
        b2 = borders.at(2);
        break;
    case RIGHT:
        b1 = borders.at(1);
        b2 = borders.at(3);
        break;
    case DOWN:
        b1 = borders.at(2);
        b2 = borders.at(0);
        break;
    case LEFT:
        b1 = borders.at(3);
        b2 = borders.at(1);
        break;
    }
}

void SquareExpander::calculateExtrapolatedPoints()
{
    // Find H
    Line fe(F,E);
    fe.Intersection(b1, H);

    // Find J
    Line de(D,E);
    de.Intersection(b2, J);

    // Find I
    Line ge(G,E);
    Line hj(H,J);
    ge.Intersection(hj, I);

    // Find K
    Line c2i(c2,I);
    c2i.Intersection(b1, K);

    // Find L
    Line c1i(c1,I);
    c1i.Intersection(b2, L);

}

void SquareExpander::createSquare()
{
    Square sq(image, c1, c2, K, L);
    newSquare = sq;
}
