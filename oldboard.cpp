#include "oldboard.h"
#include "typedefs.h"
#include <vector>
#include <stdexcept>

Oldboard::Oldboard(cv::Mat& image, Lines hlinesSorted, Lines vlinesSorted)
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

            squares.push_back(square);
        }
    }
}

void Oldboard::addRow(Squares row)
{
    if (row.empty())
        return;

    if ((int)row.size() != nCols){
        std::invalid_argument("This row has the wrong length for this board");
    }

    for (size_t i = 0; i < row.size(); i++){
        squares.push_back(row.at(i));
    }

    if (nCols == 0){ // then this is the first row added, and it will determine number of columns
        nCols = row.size();
    }
    nRows++;
}

void Oldboard::addCol(Squares col)
{
    if (col.empty())
        return;


    if ((int)col.size() != nRows){
        std::invalid_argument("This column has the wrong length for this board");
    }

    if (squares.empty()){
        squares = col;
        nCols = 1;
        return;
    }




    Squares squaresOld = squares;
    squares.clear();

    //cv::Mat img = cv::imread("/Users/benedicte/Dropbox/kings/thesis/images/chessboard1.jpg");
    int j = 0;
    for (size_t i = 0; i < squaresOld.size(); i++){

        squares.push_back(squaresOld.at(i));
        //squaresOld.at(i).drawOnImg(img);
        bool doInsert = (i+1) % nCols == 0;
        if (doInsert){
            squares.push_back(col.at(j));
            //col.at(j).drawOnImg(img);
            j++;

        }
    }

    if (nRows == 0){
        nRows = col.size();
    }
    nCols++;

    // More efficient method does not work correctly:
    //for (int i = 0; i < nRows; i++){
    //    std::vector<Square>::iterator it = squares.begin() + i*nCols + 1;
    //    Square square = col.at(i);
    //    it = squares.insert(it, square);
    //}
}


int Oldboard::getIndex(int row, int col)
{
    return row * nCols + col; // assumes squares are concatenated row after row
}

Square Oldboard::getSquare(int row, int col)
{
    int idx = getIndex(row, col);

    Square square = squares.at(idx);
    return square;
}

Square& Oldboard::getSquareRef(int rowIdx, int colIdx){
    int idx = getIndex(rowIdx, colIdx);

    if (idx > (int)squares.size()-1){
        std::invalid_argument("Trying to access squares beyond number of elements");
    }
    Square& square = squares.at(idx);
    return square;
}

Squares Oldboard::getRow(int rowIdx)
{

    if (rowIdx > nRows){
        throw std::invalid_argument("row index > number of rows in board");
    }

    int idx1 = rowIdx * nCols;
    int idx2 = idx1 + nCols - 1;

    Squares row(&squares[idx1], &squares[idx2]);
    return row;
}

Squares Oldboard::getCol(int colIdx)
{
    if (colIdx > nCols){
        throw std::invalid_argument("col index > number of columns in board");
    }

    Squares column(nRows);
    for (int i = 0; i < nRows; i++){

        int squaresIdx = colIdx + nCols * i;
        Square square = squares.at(squaresIdx);
        column.at(i) = square;
    }

    return column;
}

std::vector<int> Oldboard::getRowTypes()
{
    if (rowTypes.empty()){
        determineRowTypes();
    }
    return rowTypes;
}

std::vector<int> Oldboard::getColTypes()
{
    if (colTypes.empty()){
        determineColTypes();
    }
    return colTypes;
}

void Oldboard::draw(cv::Mat &image)
{
    if (squares.empty()){
        std::cout << "This board is empty" << std::endl;
        return;
    }

    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < squares.size(); i++) {

        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, squares.at(i).getCornerpoints(), col);
        cv::imshow("poly", image);
        cv::waitKey(10);
    }
    cv::waitKey();
}

void Oldboard::determineRowTypes()
{
    rowTypes.clear();

    for (int i = 0; i < nRows; i++){
        std::vector<int> histogram(5,0);
        for (size_t j = 0; j < nCols; j++){
            Square& square = getSquareRef(i, j);
            int type = square.getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < nCols-1) // if there are two or more votes for other categories
        {
            rowTypes.push_back(-1); // might be part of the board
        } else {
            rowTypes.push_back(vote);
        }
    }
}

void Oldboard::determineColTypes()
{
    colTypes.clear();

    for (int i = 0; i < nCols; i++){
        std::vector<int> histogram(5,0);
        for (size_t j = 0; j < nRows; j++){
            Square& square = getSquareRef(j, i);
            int type = square.getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < nCols-1) // if there are two or more votes for other categories
        {
            colTypes.push_back(-1); // might be part of the board
        } else {
            colTypes.push_back(vote);
        }
    }
}
