#include "state.h"

State::State() : matrix<int>(8,8,0){
    nBlack = nWhite = 0;
}

void State::print() const{
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

void State::copyTo(State &state) const{
    state.elements = elements;
    state.nRows = nRows;
    state.nCols = nCols;
    state.nBlack = nBlack;
    state.nWhite = nWhite;
}

State::State(const State &oldstate, size_t pieceIdx, std::pair<int, int> move){
    if (oldstate.elements.size() == 0){
        throw std::invalid_argument("input state empty");
    }

    oldstate.copyTo(*this);

    int piece = oldstate.getElement(pieceIdx);
    size_t newidx = pieceIdx + move.first*8 + move.second; // assumes that only legal moves are passed in!! to avoid checking this twice
    elements[pieceIdx] = 0;
    elements[newidx] = piece;
    if (std::abs(move.first) == 2){ // attack move
        piece > 0 ? nWhite-- : nBlack--;
        std::pair<int,int> pieceCoord = getRowCol(pieceIdx);
        size_t removedPieceIdx = getIndex(pieceCoord.first+move.first/2, pieceCoord.second+move.second/2);
        elements[removedPieceIdx] = 0;
    }

    // check if moved piece should be turned into a king
    std::vector<size_t> firstrow{0,1,2,3,4,5,6,7};
    std::vector<size_t> lastrow{56,57,58,59,60,61,62,63};

    int nLastrow = std::count(lastrow.begin(), lastrow.end(), newidx);
    int nFirstrow = std::count(firstrow.begin(), firstrow.end(), newidx);

    if (piece > 0 && nLastrow > 0){
        elements[newidx] = 2;
    }

    if (piece < 0 && nFirstrow > 0) {
        elements[newidx] = -2;
    }
}

State::State(std::vector<std::pair<size_t, int> > pieces) : State(){
    for (size_t i = 0; i < pieces.size(); i++){
        std::pair<size_t, int> piece = pieces[i];
        size_t idx = piece.first;
        elements[idx] = piece.second;
        piece.second > 0 ? nBlack++ : nWhite++;
    }
}

State State::MultiMoves(size_t pieceIdx, std::vector<std::pair<int, int> > moves) const
{
    if (moves.empty())
        throw std::invalid_argument("moves vector is empty.");

    State newstate(*this);

    for (std::pair<int,int> move : moves){
        State newnewstate(newstate, pieceIdx, move);
        pieceIdx = pieceIdx + move.first*8 + move.second;
        newstate = newnewstate;
    }
    //std::cout << "Result of multimoves: " << std::endl;
    //newstate.print();
    return newstate;
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
    if (id == 3){
        State state = State();
        state.addToElement(0,1,1);
        state.addToElement(1,2,-1);
        state.addToElement(3,4,-1);
        state.addToElement(5,6,-1);
        return state;
    }
    if (id == 4){
        State state = State();
        state.addToElement(0,1,1);
        state.addToElement(1,2,-1);
        state.addToElement(3,4,-1);
        state.addToElement(5,4,-1);
        return state;
    }
}

std::vector<State> State::findMovesForPiece(int pieceIdx) const{
    int piece = getElement(pieceIdx);
    if (piece == 0){
        throw std::invalid_argument("No piece at this location");
    }
    std::vector<State> moves;
    bool isKing, isBlack;

    piece > 0 ? isBlack = true : isBlack = false;
    std::abs(piece) == 2 ? isKing = true : isKing = false;

    bool moveUp = (!isBlack || isKing);
    bool moveDown = (isBlack || isKing);

    std::vector<int> surrs = getSurroundings(pieceIdx);

    // Inner moves (non capturing moves)
    if (moveUp){
        for (int i = 0; i < 2; i++){
            if (surrs[i] == 0){
                State newstate(*this, pieceIdx, innermoves[i]);
                moves.push_back(newstate);
                std::cout << "Adding inner move: " << std::endl;
                newstate.print();
            }
        }
    }
    if (moveDown){
        for (int i = 2; i < 4; i++){
            if (surrs[i] == 0){
                State newstate(*this, pieceIdx, innermoves[i]);
                moves.push_back(newstate);

            }
        }
    }

    // Outer moves (capturing moves)
    int currentBest = 0;
    Path bestPath;
    Path currentpath;
    currentpath.start = pieceIdx;

    bool anyOutermoves = false;
    for (int i = 0; i < 4; i++){ // for each potential move
        bool doAttack = surrs[i] * piece < 0 && surrs[i+4] == 0;
        if (doAttack){
            anyOutermoves = true;

            std::pair<int,int> move = outermoves[i];
            currentpath.add(move);
            State newstate(*this, pieceIdx, move);

            if (newstate.isEndOfGame()){
                moves.push_back(newstate);
                return moves;
            }

            Path path = launchAttack(pieceIdx, i, moveUp, moveDown);
            //Path path  = attack(newstate, newpieceidx, 10, currentpath, moveUp, moveDown);
            if (path.captured > currentBest){ // only need the path with maximum captures
                currentBest = path.captured;
                bestPath = path;
            }
        }
    }
    if (anyOutermoves){
        State stateAfterBestPath = MultiMoves(bestPath.start, bestPath.moves);
        moves.push_back(stateAfterBestPath);
    }
    return moves;
}

Path State::launchAttack(int pieceIdx, int dir, bool moveUp, bool moveDown) const{

    std::pair<int,int> move = outermoves[dir];
    Path initPath;
    initPath.add(move);
    State newstate(*this, pieceIdx, move);

    if (newstate.isEndOfGame()){
        return initPath;
    }

    int newpieceidx = pieceIdx + move.first*8 + move.second;
    Path attackPath = attack(newstate, newpieceidx, 10, initPath, moveUp, moveDown);

    return attackPath;
}


std::vector<State> State::findMovesForPlayer(int player) const
{
    std::vector<State> moves;
    for (int i = 0; i < 64; i++){
        int piece = getElement(i);
        if (piece * player > 0){ // piece belongs to current player
            std::vector<State> m = findMovesForPiece(i);
            moves.insert(moves.end(), m.begin(), m.end());
        }
    }
    return moves;
}

void State::addToElement(size_t row, size_t col, int increment){
    matrix<int>::addToElement(row, col, increment);
    increment < 0 ? nWhite++ : nBlack++;
}

Path State::attack(const State &state, int pieceIdx, int depth, Path currentpath, bool moveUp, bool moveDown){
    if (depth == 0 || state.isEndOfGame()){
        return currentpath;
    }

    std::vector<int> surrs = state.getSurroundings(pieceIdx);
    int piece = state.getElement(pieceIdx);
    bool isKing = moveUp && moveDown;
    if (isKing){

        for (int i = 0; i < 4; i++){

            bool test = surrs[i] * piece < 0 && surrs[i+4] == 0;
            if (test){
                std::pair<int,int> move = outermoves[i];
                State newstate(state, pieceIdx, move);

                int newpieceidx = pieceIdx + move.first*8 + move.second;
                currentpath.add(outermoves[i]);
                currentpath.captured++;
                return attack(newstate, newpieceidx, --depth, currentpath, moveUp, moveDown);
            }
        }
    }
    else if (moveUp && !moveDown){ // white, non king
        for (int i = 0; i < 2; i++){
            if (surrs[i] * piece < 0 && surrs[i+4] == 0){
                std::pair<int,int> move = outermoves[i];
                State newstate(state, pieceIdx, move);
                int newpieceidx = pieceIdx + move.first*8 + move.second;
                currentpath.add(outermoves[i]);
                currentpath.captured++;
                return attack(newstate, newpieceidx, --depth, currentpath, moveUp, moveDown);
            }
        }
    }
    else if (!moveUp && moveDown){ // black, non king
        for (int i = 2; i < 4; i++){
            if (surrs[i] * piece < 0 && surrs[i+4] == 0){
                std::pair<int,int> move = outermoves[i];
                State newstate(state, pieceIdx, move);
                int newpieceidx = pieceIdx + move.first*8 + move.second;
                currentpath.add(outermoves[i]);
                currentpath.captured++;
                return attack(newstate, newpieceidx, --depth, currentpath, moveUp, moveDown);
            }
        }
    }
}
