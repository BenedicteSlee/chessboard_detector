#include <algorithm>
#include <limits>
#include <cstdlib>

#include <state.h>

namespace checkers
{

int evaluatedStates = 0;

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

int minimax(const State &state, int depth, int player)
{
    if (depth == 0 || state.isEndOfGame()) {
        ++evaluatedStates;
        return evaluate(state, player);
    }
    std::vector<State> possibleMoves = state.findPossibleMoves(player);

    for (const State &move : possibleMoves){
        return minimax(move, depth - 1, player * -1);
    }

    /* TODO alpha beta pruning
    if (max) {
        for (const GameState &move : possibleMoves) {
            a = std::max(a, minimax(move, depth - 1, a, b, player * -1));
            if (b <= a)
                return b; // β cutoff.
        }
        return a;
    } else {
        for (const GameState &move : possibleMoves) {
            b = std::min(b, minimax(move, depth - 1, a, b, player * -1));
            if (b <= a)
                return a; // α cutoff.
        }
        return b;
    }
    */
}

State play(const State *state, int player)
{
    State bestMove(state);

    std::vector<State> possibleMoves;
    possibleMoves = state->findPossibleMoves(player);

    int currentBest = 0;
    for (const State &move : possibleMoves) {
        int v = minimax(move, 10, player);
        if (v > currentBest)
            bestMove = move;
    }
    //std::cerr << "Evaluated states: " << evaluatedStates << std::endl;
    return bestMove;
}

} // end namespace checkers

