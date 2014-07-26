#ifndef STATE_H
#define STATE_H

#include "matrix.h"
#include "typedefs.h"

class State : public matrix<int>
{
public:
    State();

    // Methods
    void print(){
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

    bool isEndOfGame() const {return (nBlack == 0 || nWhite == 0);}
    State(const State *state);
    State(const State *state, size_t pieceIdx, std::pair<int,int> move);
    std::vector<int> getSurroundings(size_t element) const;
    std::vector<State> findPossibleMoves(int player) const;

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

