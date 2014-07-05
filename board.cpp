#include "board.h"
#include "Line.h"
#include "square.h"
#include "typedefs.h"
#include "matrix.h"

Board::Board()
{
    nCols = 0;
    nRows = 0;
}

Board::Board(cv::Mat &image, Lines hlinesSorted, Lines vlinesSorted)
{
    nCols = vlinesSorted.size() - 1;
    nRows = hlinesSorted.size() - 1;

    for (size_t i = 0; i < hlinesSorted.size()-1; ++i) {
        Line hlineUpper = hlinesSorted.at(i);
        Line hlineLower = hlinesSorted.at(i+1);
        for (size_t j = 0; j < vlinesSorted.size()-1; ++j) {
            Line vlineLeft = vlinesSorted.at(j);
            Line vlineRight = vlinesSorted.at(j+1);

            cv::Point upperLeft, upperRight, lowerLeft, lowerRight;
            hlineUpper.Intersection(vlineLeft, upperLeft);
            hlineUpper.Intersection(vlineRight, upperRight);
            hlineLower.Intersection(vlineLeft, lowerLeft);
            hlineLower.Intersection(vlineRight, lowerRight);

            Square square(image, upperLeft, upperRight, lowerRight, lowerLeft);

            elements.push_back(square);
        }
    }
}

void Board::determineRowTypes()
{
    rowTypes.clear();

    for (int i = 0; i < nRows; i++){
        std::vector<int> histogram(5,0);
        for (int j = 0; j < nCols; j++){
            Square& square = getRef(i, j);
            int type = square.getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < nCols-1) // if there are two or more votes for other categories
        {
            rowTypes.push_back(vote);
            //rowTypes.push_back(-1); // might be part of the board
        } else {
            rowTypes.push_back(vote);
        }
    }
}

void Board::determineColTypes()
{
    colTypes.clear();

    for (int i = 0; i < nCols; i++){
        std::vector<int> histogram(5,0);
        for (int j = 0; j < nRows; j++){
            Square& square =  getRef(j, i);
            int type = square.getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < nCols-1) // if there are two or more votes for other categories
        {
            colTypes.push_back(vote);
            //colTypes.push_back(-1); // might be part of the board
        } else {
            colTypes.push_back(vote);
        }
    }
}

std::vector<int> Board::getRowTypes()
{
    if (rowTypes.empty()){
        determineRowTypes();
    }
    return rowTypes;
}

std::vector<int> Board::getColTypes()
{
    if (colTypes.empty()){
        determineColTypes();
    }
    return colTypes;
}

void Board::draw(cv::Mat &image)
{
    if (elements.empty()){
        std::cout << "This board is empty" << std::endl;
        return;
    }

    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < elements.size(); i++) {

        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, elements.at(i).getCornerpoints(), col);
        cv::imshow("poly", image);
        cv::waitKey(10);
    }
    cv::waitKey();
}


