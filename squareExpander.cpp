#include "squareExpander.h"
#include "typedefs.h"
#include <vector>


void SquareExpander::calculatePoints()
{// Ref TAM--Automatic_grid_segmentation_of_populated_chessboard_taken_at_a_lower_angle_view.pdf
    C = baseSquare.getCenter();


    topMid = cvutils::centerpoint(cpoints.at(1), cpoints.at(0));
    rightMid = cvutils::centerpoint(cpoints.at(2), cpoints.at(1));
    bottomMid = cvutils::centerpoint(cpoints.at(3), cpoints.at(2));
    leftMid = cvutils::centerpoint(cpoints.at(0), cpoints.at(3));
}

SquareExpander::SquareExpander(cv::Mat& image_, Square square_, Direction dir_) : global::image(global::image_)
{

    canExpand = false;
    baseSquare = square_;
    dir = dir_;

    cpoints = baseSquare.getCornerpointsSorted();

    calculatePoints();
    namePoints();
    nameBorders();
    canExpand = calculateExtrapolatedPoints();
    //draw(global::image);
    createSquare();
    //if (canExpand){
    //    createSquare();
    //    success = true;
    //}

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
    Lines borders = baseSquare.getBordersSorted();

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

bool SquareExpander::calculateExtrapolatedPoints()
{
    // Find H
    Line fe(F,E);
    fe.Intersection(b1, H);
    if (H.y < 0 || H.x < 0)
        return false;

    // Find J
    Line de(D,E);
    de.Intersection(b2, J);
    if (J.x < 0 || J.y < 0)
        return false;

    // Find I
    Line ge(G,E);
    Line hj(H,J);
    ge.Intersection(hj, I);
    if (I.x < 0 || I.y < 0)
       return false;

    // Find K
    Line c2i(c2,I);
    c2i.Intersection(b1, K);
    if (K.x < 0 || K.y < 0)
        return false;

    // Find L
    Line c1i(c1,I);
    c1i.Intersection(b2, L);
    if (L.x < 0 || L.y < 0)
        return false;

    return true;

}

void SquareExpander::createSquare()
{
    if (!canExpand)
        return;
    Square sq(c1, c2, K, L);
    newSquare = sq;
}

void SquareExpander::draw(cv::Mat global::image)
{
    cv::Mat rgb;
    if (global::image.channels() == 1){
        cv::cvtColor(global::image, rgb, cv::COLOR_GRAY2RGB);
    }
    else rgb = global::image;

    cv::Scalar col1 = cv::Scalar(0,255,0);
    cv::Scalar col2 = cv::Scalar(0,0,255);
    for (int i = 0; i < 4; i++){
        int idx2 = (i+1) % 4;
        cv::line(rgb, cpoints.at(i), cpoints.at(idx2), col1, 2);
    }

    cv::line(rgb, c1, cvutils::posPoint(K), col2,2);
    cv::line(rgb, c2, cvutils::posPoint(L), col2,2);
    cv::line(rgb, cvutils::posPoint(K), cvutils::posPoint(L), col2,2);
    cv::circle(rgb, cvutils::posPoint(H), 2, col2,2);
    cv::circle(rgb, cvutils::posPoint(J), 2, col2,2);

    if (cvutils::posPoint(K) != K)
        cv::circle(rgb, cvutils::posPoint(K), 5,  cv::Scalar(255,0,0), 2);

    if (cvutils::posPoint(L) != L)
        cv::circle(rgb, cvutils::posPoint(L), 5, cv::Scalar(255,0,0), 2);

    if (cvutils::posPoint(H) != H)
        cv::circle(rgb, cvutils::posPoint(H), 5, cv::Scalar(255,0,0), 2);

    if (cvutils::posPoint(J) != J)
        cv::circle(rgb, cvutils::posPoint(J), 5, cv::Scalar(255,0,0), 2);

    cv::imshow("expanded square", rgb);
    cv::waitKey();


}

