#include <algorithm>
#include <limits>
#include <cstdlib>
#include "state.h"

namespace checkers
{

int evaluate(const State &state, int player) /// send inn matrix of piece placements, return heuristic for current player
{
    // FIXME: Improve heuristics.
    int playerScore = 0;
    int opponentScore = 0;
    int piece = 0;
    for (size_t i = 0; i < 64; i++)
    {
        piece = state.getElement(i);
        int pieceId = piece * player;
        if (pieceId == 1)
            playerScore++;
        else if (pieceId == 2)
            playerScore += 2;   // King bonus
        else if (pieceId == -1)
            opponentScore++;
        else if (pieceId == -2)
            opponentScore += 2; // King bonus
    }
    return playerScore - opponentScore;
}

int minimax(const State &state, int depth, int a, int b, int player, bool max)
{
    if (depth == 0 || state.isEndOfGame()) {
        return evaluate(state, player);
    }
    std::vector<State> possibleMoves = state.findMovesForPlayer(player);

    int bestVal = 0;
    for (const State &move : possibleMoves){
        int newVal = minimax(move, depth-1,a,b,player,false);
        if (newVal > bestVal){
                bestVal = newVal;
        }
    }
    return bestVal;

    
    /*
    if (max) {
        for (const State &move : possibleMoves) {
            a = std::max(a, minimax(move, depth - 1, a, b, player, false));
            if (b <= a)
                return b; // β cutoff.
        }
        return a;
    } else {
        for (const State &move : possibleMoves) {
            b = std::min(b, minimax(move, depth - 1, a, b, player, true));
            if (b <= a)
                return a; // α cutoff.
        }
        return b;
    }
    */
}

State play(const State &state, int player)
{
    State bestMove;
    state.copyTo(bestMove);

    std::vector<State> possibleMoves = state.findMovesForPlayer(player);
    std::cout << "Found: " << possibleMoves.size() << " initial moves." << std::endl;

    bool foundWinner = std::any_of(possibleMoves.begin(), possibleMoves.end(), [](State s){return s.isEndOfGame();});
    if (foundWinner){
        std::vector<State>::iterator it = std::find_if(possibleMoves.begin(), possibleMoves.end(), [](State s){return s.isEndOfGame();});
        return *it;
    }

    int a = -std::numeric_limits<int>::max();
    int b = std::numeric_limits<int>::max();
    for (const State &move : possibleMoves) {
        int v = minimax(move, 10, a, b, player, true);
        if (v > a)
            bestMove = move;
    }
    return bestMove;
}

} // end namespace checkers

