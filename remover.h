#ifndef REMOVER_H
#define REMOVER_H

#include <vector>
#include "matrix.h"
#include "board.h"

class Remover : public matrix<int>
{
public:
    Remover(Board& board_) : matrix<int>(board_.getNumRows(), board_.getNumRows()), board(board_){

    }

    void remove(){

    }
private:
    Board& board;

};

#endif // REMOVER_H
