#include "board.h"
#include "Line.h"
#include "square.h"
#include "typedefs.h"
#include "matrix.h"
#include "squareExpander.h"
#include "state.h"

Board::Board(cv::Mat& image_) : image(image_)
{
    nCols = 0;
    nRows = 0;
}

Board::Board(cv::Mat &image_, Lines hlinesSorted, Lines vlinesSorted) : image(image_)
{
    if (hlinesSorted.empty()){
        throw std::invalid_argument("Vector with horizontal lines does not contain any elements");
    }

    if (vlinesSorted.empty()){
        throw std::invalid_argument("Vector with vertical lines does not contain any elements");
    }
    nCols = vlinesSorted.size() - 1;
    nRows = hlinesSorted.size() - 1;

    for (size_t i = 0; i < nRows; ++i) {
        Line hlineUpper = hlinesSorted.at(i);
        Line hlineLower = hlinesSorted.at(i+1);
        bool addRow = true;
        Squares row;
        for (size_t j = 0; j < nCols; ++j) {
            Line vlineLeft = vlinesSorted.at(j);
            Line vlineRight = vlinesSorted.at(j+1);

            cv::Point2d upperLeft, upperRight, lowerLeft, lowerRight;


            int check = hlineUpper.Intersection(vlineLeft, upperLeft);
            check = hlineUpper.Intersection(vlineRight, upperRight);
            check = hlineLower.Intersection(vlineLeft, lowerLeft);
            check = hlineLower.Intersection(vlineRight, lowerRight);

            Square sq;
            try{
                Square square(image, upperLeft, upperRight, lowerRight, lowerLeft);
                sq = square;
            }
            catch(std::exception& e){
                addRow = false;
            }
            row.push_back(sq);
        }
        if (addRow)
            elements.insert(elements.end(), row.begin(), row.end());

    }
    removeOutOfBounds();
}

int Board::squareId(cv::Point2d point){ // TODO TEST!
    Squares::iterator it = std::find_if(elements.begin(), elements.end(), [&](Square square){return square.containsPoint(point);});
    int squareId = std::distance(elements.begin(), it);
    return squareId;
}


void Board::determineRowTypes()
{
    rowTypes.clear();

    for (size_t i = 0; i < nRows; i++){
        std::vector<int> histogram(5,0);
        for (size_t j = 0; j < nCols; j++){
            const Square& square = getRef(i, j);
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

    for (size_t i = 0; i < nCols; i++){
        std::vector<size_t> histogram(5,0);
        for (size_t j = 0; j < nRows; j++){
            const Square& square = getRef(j, i);
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

void Board::removeOutOfBounds(){
    std::vector<size_t> delRow;
    std::vector<size_t> delCol;
    for (size_t i = 0; i < elements.size(); i++){
        std::pair<size_t,size_t> rowcol = getRowCol(i);
        int row = rowcol.first;
        int col = rowcol.second;
        if (elements[i].isOutOfBounds()){
            delRow.push_back(row);
            delCol.push_back(col);
        }
    }

    std::vector<size_t> checkColsRemoved, checkRowsRemoved;
    if (delCol.size() > 0)
        checkColsRemoved = removeColsRequest(delCol);
    if (delRow.size() > 0)
        checkRowsRemoved = removeRowsRequest(delRow);

    // if rows or cols out of bounds that are not at top/bottom or left/right then give up and ask for new image
    if (checkColsRemoved.size() != delCol.size() || checkRowsRemoved.size() != delRow.size()){
        throw std::invalid_argument("Make sure whole board is within image frame");
    }
    std::cout << "Cols removed because contained an out of bounds square: ";
    for (size_t i = 0; i < checkColsRemoved.size(); i++){
        std::cout << checkColsRemoved[i] << ",";
    }
    std::cout << std::endl;
    std::cout << "Rows removed because contained an out of bounds square: ";
    for (size_t i = 0; i < checkRowsRemoved.size(); i++){
        std::cout << checkRowsRemoved[i] << ",";
    }
    std::cout << std::endl;

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

void Board::draw()
{
    if (elements.empty()){
        std::cout << "This board is empty" << std::endl;
        return;
    }


    if (!image.data){
        throw std::invalid_argument("draw() has no image to draw on");
    }
    cv::Mat img_draw;
    image.copyTo(img_draw);
    cv::cvtColor(img_draw, img_draw, cv::COLOR_GRAY2BGR);
    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < elements.size(); i++) {

        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        Points2d cps = elements.at(i).getCornerpointsSorted();
        if (cvutils::anyNegCoordinate(cps)){
            std::cout << "At least one point has a negative index, cannot draw" << std::endl;
            return;
        } else {
            Points cornerpoints = cvutils::doubleToInt(cps);
            cv::fillConvexPoly(img_draw, cornerpoints, col);
            cv::imshow("poly", img_draw);

            cv::waitKey(1);
        }
    }
    cv::waitKey();
}

std::pair<int,int> Board::getStatus()
{
    std::pair<int,int> status; //row, col

    status.first = 8 - nRows; // number of rows needed to add/shrink
    status.second = 8 - nCols;

    return status;
}

void Board::expand(Direction dir)
{
    size_t size;
    Squares baseSquares;
    switch(dir){
    case UP:
        size = nCols;
        baseSquares = this->getRow(0);
        std::cout << "Adding row to top of board" << std::endl;
        break;
    case DOWN:
        size = nCols;
        baseSquares = this->getRow(nRows-1);
        std::cout << "Adding row to bottom of board" << std::endl;
        break;
    case LEFT:
        size = nRows;
        baseSquares = this->getCol(0);
        std::cout << "Adding column to left of board" << std::endl;
        break;
    case RIGHT:
        size = nRows;
        baseSquares = this->getCol(nCols-1);
        std::cout << "Adding column to right of board" << std::endl;
        break;

    }

    Squares newsquares(size);
    for (size_t i = 0; i < size; i++){
        SquareExpander se(image, baseSquares[i], dir);
        //if (!success)
        //  break;
        newsquares[i] = se.getSquare();
    }

    switch(dir){  // TODO: use function pointers instead and define them in the previous switch
    case UP:
        this->prependRow(newsquares);
        break;
    case DOWN:
        this->appendRow(newsquares);
        break;
    case LEFT:
        this->prependCol(newsquares);
        break;
    case RIGHT:
        this->appendCol(newsquares);
        break;
    }
}


void Board::detectPieces(){
    for (size_t i = 0; i < elements.size(); i++){
        Square &square = elements[i];
        cv::Vec3i circle;
        bool pieceDetected = square.detectPieceWithHough(circle);
        if (pieceDetected){
            circles.push_back(std::make_pair(i,circle));
        }
    }
}

int Board::determinePieceColorThreshold(){
    for (size_t i = 0; i < circles.size(); i++){
        Square square = elements[circles[i].first];
        cv::Vec3i circle = circles[i].second;
        int col = square.determinePieceColor(circle);
        pieceColors.push_back(col);
    }
    return cv::mean(pieceColors)[0];
}

State Board::initState(){
    detectPieces();
    int threshold = determinePieceColorThreshold();

    std::vector<std::pair<size_t, int>> pieces;

    for (size_t i = 0; i < circles.size(); i++){
        int piece;
        pieceColors.at(i) > threshold ? piece = 1 : piece = -1;
        pieces.push_back(std::make_pair(circles[i].first, piece));
    }
    State state(pieces);
    return state;
}

