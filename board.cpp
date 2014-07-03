#include "board.h"
#include "typedefs.h"
#include <vector>
#include <stdexcept>


Board::Board(cv::Mat& image, Lines hlinesSorted, Lines vlinesSorted)
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

void Board::addRow(Squares row)
{
    if (row.empty())
        return;

    if (nCols == 0){ // then this is the first row added, and it will determine number of columns
        nCols = row.size();
    }
    nRows++;

    if ((int)row.size() != nCols){
        std::invalid_argument("This row has the wrong length for this board");
    }

    for (size_t i = 0; i < row.size(); i++){
        squares.push_back(row.at(i));
    }


}

void Board::addCol(Squares col)
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

    int j = 0;
    for (size_t i = 0; i < squaresOld.size(); i++){
        std::cout << squaresOld.size() << std::endl;
        squares.push_back(squaresOld.at(i));
        bool doInsert = i % nCols == 0;
        if (doInsert){
            squares.push_back(col.at(j));
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

/*
Board::Board(Board oldBoard, std::vector<int> rowsToRemove, std::vector<int> colsToRemove)
{

    nCols = oldBoard.getNumCols() - colsToRemove.size();
    nRows = oldBoard.getNumRows() - rowsToRemove.size();

    for (int i = 0; i < oldBoard.getNumCols(); i++){
        if (std::count(colsToRemove.begin(), colsToRemove.end(), i) == 0){
            for (int j = 0; j < oldBoard.getNumRows(); j++){
                if (std::count(rowsToRemove.begin(), rowsToRemove.end(), j) == 0){
                    squares.push_back(oldBoard.getSquare(j,i));
                }
            }
        }
    }
}
*/

int Board::getIndex(int row, int col)
{
    return row * nCols + col; // assumes squares are concatenated row after row
}

Square Board::getSquare(int row, int col)
{
    int idx = getIndex(row, col);

    Square square = squares.at(idx);
    return square;
}

Square& Board::getSquareRef(int rowIdx, int colIdx){
    int idx = getIndex(rowIdx, colIdx);

    if (idx > (int)squares.size()-1){
        std::invalid_argument("Trying to access squares beyond number of elements");
    }
    Square& square = squares.at(idx);
    return square;
}

Squares Board::getRow(int rowIdx)
{

    if (rowIdx > nRows){
        throw std::invalid_argument("row index > number of rows in board");
    }

    int idx1 = rowIdx * nCols;
    int idx2 = idx1 + nCols - 1;

    Squares row(&squares[idx1], &squares[idx2]);
    return row;
}

Squares Board::getCol(int colIdx)
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
    if (squares.empty()){
        std::cout << "This board is empty" << std::endl;
        return;
    }

    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < squares.size(); i++) {

        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, squares.at(i).getCornerpoints(), col);
        cv::imshow("poly", image);
        cv::waitKey(100);
    }
    cv::waitKey();
}

void Board::determineRowTypes()
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

void Board::determineColTypes()
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

/*
void Board::createCorners()
{
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++){
            Square& square = getSquareRef(i,j);
            Points cpoints = square.getCornerpointsSorted();

            int radius = 10; // TODO make dynamic

            for (size_t k = 0; k < cpoints.size(); k++){
                cv::Point p = cpoints.at(k);
                Corner newcorner(image_gray, p, radius);
                square.addCorner(newcorner);

                 // look at corners without duplicates
                if (!cvutils::containsPoint(added, p)){
                    corners.push_back(newcorner);
                    added.push_back(p);

                    std::cout << newcorner.getNRegions() << std::endl;

                    cv::imshow("CORNER", newcorner.getArea());
                    cv::waitKey(2);

                }


            }

        }
    }
}
*/
