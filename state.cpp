#include "state.h"

State::State() : matrix<int>(8,8,0){
    nBlack = nWhite = 0;
}

void State::print(){
    if (elements.empty()){
        std::cout << "Cannot print empty board" << std::endl;
        return;
    }
    std::cout << "-----------------------------------------" << std::endl;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            std::cout << elements[getIndex(i,j)] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------" << std::endl;
}

State::State(const State *state){
    if (!state)
        throw std::invalid_argument("Input state points to null");
    elements = state->elements;
    nBlack = state->nBlack;
    nWhite = state->nWhite;
    nRows = state->nRows;
    nCols = state->nCols;
}

State::State(const State *state, size_t pieceIdx, std::pair<int, int> move) : State(state){
    int piece = elements[pieceIdx];
    elements[pieceIdx] = 0;
    size_t idx = pieceIdx + move.first + move.second*8; // assumes that only legal moves are passed in!! to avoid checking this twice
    elements[idx] = piece;
    if (std::abs(move.first) == 2){ // jumping accross
        piece > 0 ? nWhite-- : nBlack--;
        std::pair<int,int> pieceCoord = getRowCol(pieceIdx);
        size_t removedPieceIdx = getIndex(pieceCoord.first+move.first/2, pieceCoord.second+move.second/2);
        elements[removedPieceIdx] = 0;
    }
    int hei = 1;
}

std::vector<int> State::getSurroundings(size_t element) const{
    std::vector<int> surroundings(8,INFINITY);
    std::pair<size_t,size_t> coord = getRowCol(element);
    size_t row = coord.first;
    size_t col = coord.second;

    size_t idx;

    bool valid;

    // Set surroundings: circle around inner loop then outer loop, starting in upper left corner
    valid = getSmartIndex(row-1, col-1, idx);
    if (valid)
        surroundings[0] = elements[idx];

    valid = getSmartIndex(row-1, col+1, idx);
    if (valid)
        surroundings[1] = elements[idx];

    valid = getSmartIndex(row+1, col+1, idx);
    if (valid)
        surroundings[2] = elements[idx];

    valid = getSmartIndex(row+1, col-1, idx);
    if (valid)
        surroundings[3] = elements[idx];

    valid = getSmartIndex(row-2, col-2, idx);
    if (valid)
        surroundings[4] = elements[idx];

    valid = getSmartIndex(row-2, col+2, idx);
    if (valid)
        surroundings[5] = elements[idx];

    valid = getSmartIndex(row+2, col+2, idx);
    if (valid)
        surroundings[6] = elements[idx];

    valid = getSmartIndex(row+2, col-2, idx);
    if (valid)
        surroundings[7] = elements[idx];


    return surroundings;
}

std::vector<State> State::findPossibleMoves(int player) const
{
    std::vector<State> pmoves; // maximum 4 possible moves

    for (int i = 0; i < 64; i++){
        int pieceId = elements[i] * player;
        if (pieceId > 0){ // this piece can be moved by the current player
            std::vector<int> surr = getSurroundings(i);
            int oul = surr[0];
            int our = surr[1];
            int iul = surr[2];
            int iur = surr[3];
            int ill = surr[4];
            int ilr = surr[5];
            int oll = surr[6];
            int olr = surr[7];

            if (player < 0 || pieceId == 2){ // for player 2 and for player 1 kings
                // Move towards row 0
                if (iul == 0 && !onlyForced){
                    State newstate(this, i, std::make_pair(-1,-1));
                    pmoves.push_back(newstate);
                } else if (iul * player < 0 && oul == 0){ // then this is a forced move: clear any previous moves and just return this one
                    pmoves.clear();
                    State newstate(this, i, std::make_pair(-2,-2));
                    // TODO
                    // getSuroundings
                    // any forced moves?
                    // perform forced move
                    // repeat
                    pmoves.push_back(newstate);
                    return pmoves;
                }
                if (iur == 0 && !onlyForced){
                    State newstate(this, i, std::make_pair(-1,1));
                    pmoves.push_back(newstate);
                } else if (iur * player < 0 && our == 0){
                    pmoves.clear();
                    State newstate(this, i, std::make_pair(-2,2));
                    pmoves.push_back(newstate);
                    return pmoves;
                }
            }
            if (player > 0 || pieceId == 2){ // for player 1 and for player 2 kings
                if (ill == 0 && !onlyForced){
                    State newstate(this, i, std::make_pair(1,-1));
                    pmoves.push_back(newstate);
                } else if (ill * player < 0 && oll == 0){
                    pmoves.clear();
                    State newstate(this, i, std::make_pair(2,-2));
                    pmoves.push_back(newstate);
                    return pmoves;
                }
                if (ilr == 0 && !onlyForced){
                    State newstate(this, i, std::make_pair(1,1));
                    pmoves.push_back(newstate);
                } else if (ilr * player < 0 && olr == 0){
                    pmoves.clear();
                    State newstate(this, i, std::make_pair(2,2));
                    pmoves.push_back(newstate);
                    return pmoves;
                }
            }
        }
    }
    return pmoves;

    std::cout << "Number of moves found: " << pmoves.size() << std::endl;
}

State State::createState(int id){
    if (id == 1){
        State state = State();
        state.addToElement(2,3,1);
        state.addToElement(3,4,-1);
        return state;
    }
    if (id == 2){
        State state = State();
        state.addToElement(2,3,1);
        state.addToElement(3,4,-1);
        state.addToElement(5,6,-1);
        return state;
    }
}


