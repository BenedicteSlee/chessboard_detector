#ifndef STATE_H
#define STATE_H

#include "matrix.h"
#include "typedefs.h"

std::vector<std::pair<int,int> > innermoves{std::make_pair{-1,-1},std::make_pair{-1,1},std::make_pair{1,1},std::make_pair{1,-1}};
std::vector<std::pair<int,int> > outermoves{std::make_pair{-2,-2},std::make_pair{-2,2},std::make_pair{2,2},std::make_pair{2,-2}};

class State : public matrix<int>
{
public:
    State();

    // Methods
    void print();
    void copyTo(State& state) {
        matrix<int>::copyTo(state);
        state.nBlack = nBlack;
        state.nWhite = nWhite;
    }

    bool isEndOfGame() const {return (nBlack == 0 || nWhite == 0);}
    State(const State *state);
    State(const State *state, size_t pieceIdx, std::pair<int,int> move);
    std::vector<int> getSurroundings(size_t element) const;
    std::vector<State> findPossibleMoves(int player) const;
    std::vector<State> findPossibleMoves(State *state, size_t pieceIdx) const{
        int piece = state->getElement(pieceIdx);
        if (piece == 0){
            throw std::invalid_argument("There is no piece at this position");
        }

        std::vector<State> noncap, cap; // non capture and capture moves
        std::vector<int> surr = state->getSurroundings(pieceIdx);
        if (!forcedOnly){
            for (int i = 0; i < 4; i++){
                if (surr[i] == 0){ // there is no piece and it is a valid move (!= inf)
                    State newstate = State(state, pieceIdx, innermoves[i]);
                    noncap.push_back(newstate);
                }
            }
        } else {
            for (int i = 0; i < 4; i++){
                if (surr[i] * piece < 0 && surr[i+4] == 0){ // if there is an opposing piece at the inner corner and the outer corner is free
                    std::pair<int,int> move = outermoves[i];
                    State newstate = State(state, pieceIdx, move);
                    int newpieceidx = pieceIdx + move.first*8 + move.second;
                    int depth = 10;
                    int captured = 1;
                    int nCaptured = evaluateMultiJumps(newstate, newpieceidx, depth, captured);
                }
            }

        }
    }
    int evaluateMultiJumps(State *state, pieceIdx, depth, captured){
        int piece = state->getElement(pieceIdx);
        std::vector<int> surr = state->getSurroundings(pieceIdx);
        for (int i = 0; i < 4; i++){
            if (surr[i] * piece < 0 && surr[i+4] == 0){
                std::pair<int,int> move = outermoves[i];
                State newstate = State(state, pieceidx, move);
                // todo todo todo
            }
        }
    }

    // Extended methods
    void addToElement(size_t row, size_t col, int increment){
        matrix<int>::addToElement(row, col, increment);
        increment < 0 ? nWhite++ : nBlack++;
    }

    // Static methods
    static State createState(int id = 1);


private:
    int nBlack;
    int nWhite;
};


#endif // STATE_H

