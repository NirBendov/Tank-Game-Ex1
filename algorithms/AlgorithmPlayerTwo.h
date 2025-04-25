#pragma once
#include "Algorithm.h"
using namespace std;

class AlgorithmPlayerTwo: public Algorithm 
{
private:
    /* data */
public:
    AlgorithmPlayerTwo(int playerId, GameBoard* gameBoard);
    ~AlgorithmPlayerTwo();

    vector<Action> decideNextActions() override;
};