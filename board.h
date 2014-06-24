#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include "typedefs.h"
#include <vector>

class board
{
public:
    board();

    std::vector<Square> getSquares(int row, int col) const;
    //void setSquares(const std::vector<square> &value);

private:
    std::vector<Square> squares;

};

#endif // BOARD_H
