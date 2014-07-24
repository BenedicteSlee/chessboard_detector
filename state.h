#ifndef STATE_H
#define STATE_H

#include "matrix.h"
#include "typedefs.h"

class State : public matrix<Piece>
{
public:
    State();

    std::vector<Piece> pieces(short player)
    {
        std::vector<piece> result;
        for (short element : elements){
            if (player * element.type > 0){ // player > 0 : want positive elements, player < 0, want negative elements
                result.push_back(element);
            }
        }
    }

    std::vector<short> getSourroundings(Piece piece){
        std::vector<short> surroundings(4,0);
        size_t idx = getIndex(pos.first, pos.second);

        if (piece.row == 0)
        {
            surroundings[3] = surroundings[2] = INFINITY;
        } else if (piece.row == 7)
        {
            surroundings[0] = surroundings[1] = INFINITY;
        }
        if (piece.col == 0){
            surroundings[0] = surroundings[3] = INFINITY;
        }
        else if (piece.col == 7)
        {
            surroundings[1] = surroundings[2] = INFINITY;
        }

        if (surroundings[0] != INFINITY)
            surroundings[0] = elements[idx + 7];
        if (surroundings[1] != INFINITY)
            surroundings[1] = elements[idx + 9];
        if (surroundings[2] != INFINITY)
            surroundings[2] = elements[idx - 7];
        if (surroundings[3] != INFINITY)
            surroundings[3] = elements[idx - 9];

        return surroundings;
    }

    void move(Piece piece, std::pair newPosition){
        elements[getIndex(piece.row,piece.col)];
    }

    Move chooseMove(std::vector<Move> possibleMoves){
        int nForcedMoves = std::count_if(possibleMoves.begin(), possibleMoves.end(), [](Move m) {return m.isForced;});
        if (nForcedMoves > 1){
            throw std::invalid_argument("More than one forced move, illegal state");
        }

        if (nForcedMoves == 1)
            return std::find_if(possibleMoves.begin(), possibleMoves.end(), [](Move m) {return m.isForced;});

        // TODO recursive call possible moves on the next level
    }

    std::vector<Move> possibleMoves(short player)
    {
        std::vector<Move> pmoves;

        for (Piece piece : pieces(player)){
            std::vector<short> surr = getSurroundings(piece);
            std::pair currentCoord = piece.coord();
            for (size_t i = 0; i < 4; i++){
                if (surr[i] * player < 0){ // there is a piece, and it is an opponent piece

                    Move possibleMove(currentCoord, i, true);
                    pmoves.push_back(possibleMove);

                } else if (surr[i] == 0){ // there is no piece
                    Move possibleMove(currentCoord, i, false);
                    pmoves.push_back(possibleMove);
                }
            }

        }
    }

    State copyTo(State& newstate){
        newstate.pieces = pieces;
    }
};

struct Piece{
    Piece(short type_){
        type = type_;
        std::abs(type) == 2 ? isKing = true;
    }
    short type;
    bool isKing;
    size_t row, col;
    std::pair<size_t,size_t> coord(){
        return std::make_pair(row, col);
    }
    size_t position(){
        return col + row*8;
    }
};

struct Move{
    Move(short currentCoord, short direction, bool forced){
        isForced = forced;

        switch(direction){
        size_t unit;
        forced ? unit = 2 : unit = 1;
        case 0:
            displacement = std::make_pair(-unit,-unit);
            break;
        case 1:
            displacement = std::make_pair(-unit,unit);
            break;
        case 2:
            displacement = std::make_pair(unit,unit);
            break;
        case 3:
            displacement = std::make_pair(unit,-unit);
            break;
        }

        size_t newxcoord = currentCoord.first + displacement.first;
        size_t newycoord = currentCoord.second + displacement.second;

        valid = (newxcoord >= 0 && newxcoord <= 7 && newycoord >= 0 && newycoord <= 7);
    }

    bool valid;
    bool isForced;
    std::pair<size_t,size_t> displacement;

};


#endif // STATE_H

