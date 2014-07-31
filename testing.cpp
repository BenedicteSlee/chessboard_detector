#include "testing.h"
#include "state.h"
#include <string>


void test_minimax::test_multimoves()
{
    State state = State::createState(3);

    std::vector<std::pair<int,int>> moves = {std::make_pair(2,2), std::make_pair(2,2), std::make_pair(2,2)};
    State newstate = state.MultiMoves(1, moves);

    std::string pass = newstate.isEndOfGame() ? "true" : "false";
    std::cout << "Test passed: " <<  pass << std::endl;
}

