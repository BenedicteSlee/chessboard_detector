#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include "typedefs.h"
#include <vector>

class board
{
public:
    board();

    std::vector<square> getSquares(int row, int col) const;
    //void setSquares(const std::vector<square> &value);

private:
    std::vector<square> squares;

};

#endif // BOARD_H
