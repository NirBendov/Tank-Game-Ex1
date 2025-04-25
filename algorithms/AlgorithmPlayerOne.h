#pragma once
#include "Algorithm.h"
using namespace std;

class AlgorithmPlayerOne: public Algorithm 
{
public:
    AlgorithmPlayerOne(int playerId, GameBoard* gameBoard);
    ~AlgorithmPlayerOne();
    
    vector<Action> decideNextActions() override;
}; 