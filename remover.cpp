#include "remover.h"



Remover::Remover(Board &board_) : matrix<size_t>(board_.getNumRows(), board_.getNumCols()), board(board_){
    std::for_each(elements.begin(), elements.end(), [](size_t& element) {element = 0;});
}

std::vector<size_t> Remover::getCurrentRowRequests(){
    std::vector<size_t> rowRequests;
    for (size_t row = 0; row < board.getNumRows(); row++){

        std::vector<size_t> thisrow = this->getRow(row);

        int sumVotes = std::count_if(thisrow.begin(), thisrow.end(), [](size_t i) {return i > 0;});

        if (sumVotes / (double) nCols > 0.5)
            rowRequests.push_back(row);
    }
    return rowRequests;
}

std::vector<size_t> Remover::getCurrentColRequests(){
    std::vector<size_t> colRequests;

    for (size_t col = 0; col < board.getNumCols(); col++){
        std::vector<size_t> thiscol = getCol(col);
        size_t sumVotes = std::count_if(thiscol.begin(), thiscol.end(), [](size_t i) {return i > 0;});

        if (sumVotes / (double) nRows > 0.9)
            colRequests.push_back(col);
    }

    return colRequests;
}

void Remover::remove(){
    std::vector<size_t> rowRequests = getCurrentRowRequests();
    std::vector<size_t> colRequests = getCurrentColRequests();

    // Request removals from board
    if (!rowRequests.empty()){
        board.removeRowsRequest(rowRequests);
        this->removeRowsRequest(rowRequests);
    }
    if (!colRequests.empty()){
        board.removeColsRequest(colRequests);
        this->removeColsRequest(colRequests);
    }
}
