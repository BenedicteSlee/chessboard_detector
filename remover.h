#ifndef REMOVER_H
#define REMOVER_H

#include <vector>
#include <numeric>

#include "matrix.h"
#include "board.h"

class Remover : public matrix<size_t>
{
public:
    Remover(Board& board_);

    std::vector<size_t> getCurrentRowRequests();
    std::vector<size_t> getCurrentColRequests();
    void remove();

private:
    Board& board;

};

#endif // REMOVER_H
