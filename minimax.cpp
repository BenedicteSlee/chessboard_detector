#include <algorithm>
#include <limits>
#include <cstdlib>

/*
namespace checkers
{

int evaluatedStates = 0;

int evaluate(const GameState &state)
{
    // FIXME: Improve heuristics.
    int redScore = 0;
    int whiteScore = 0;
    int piece = 0;
    for (int i = 1; i <= 32; ++i)
    {
        piece = state.at(i);
        if (piece & CELL_RED) {
            ++redScore;
            if (piece & CELL_KING)
                redScore += 2;   // King bonus.
        } else if (piece & CELL_WHITE) {
            ++whiteScore;
            if (piece & CELL_KING)
                whiteScore += 2; // King bonus.
        }
    }
    return state.getNextPlayer() == CELL_RED ? whiteScore - redScore : redScore - whiteScore;
}

int minimax(const GameState &state, int depth, int a, int b, bool max)
{
    if (depth == 0 || state.isEOG()) {
        ++evaluatedStates;
        return evaluate(state);
    }
    std::vector<GameState> possibleMoves;
    state.findPossibleMoves(possibleMoves);
    if (max) {
        for (const GameState &move : possibleMoves) {
            a = std::max(a, minimax(move, depth - 1, a, b, false));
            if (b <= a)
                return b; // β cutoff.
        }
        return a;
    } else {
        for (const GameState &move : possibleMoves) {
            b = std::min(b, minimax(move, depth - 1, a, b, true));
            if (b <= a)
                return a; // α cutoff.
        }
        return b;
    }
}

int negamax(const GameState &state, int depth, int a, int b)
{
    if (depth == 0 || state.isEOG()) {
        ++evaluatedStates;
        return evaluate(state);
    }
    std::vector<GameState> possibleMoves;
    state.findPossibleMoves(possibleMoves);
    for (const GameState &move : possibleMoves) {
        a = std::max(a, -negamax(move, depth - 1, -b, -a));
        if (b <= a)
            return b; // β cutoff.
    }
    return a;
}

GameState Player::play(const GameState &pState, const Deadline &pDue)
{
    GameState bestMove(pState, Move());

    std::vector<GameState> possibleMoves;
    pState.findPossibleMoves(possibleMoves);

    int a = -std::numeric_limits<int>::max();
    int b = std::numeric_limits<int>::max();
    for (const GameState &move : possibleMoves) {
        int v = negamax(move, 10, a, b);
        //int v = minimax(move, 10, a, b, false);
        if (v > a) {
            a = v;
            bestMove = move;
        }
    }
    std::cerr << "Evaluated states: " << evaluatedStates << std::endl;
    return bestMove;
}

} // end namespace checkers
*/
