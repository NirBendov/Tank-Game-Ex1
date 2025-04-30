#pragma once
#include "Algorithm.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "../game_objects/Direction.h"
#include "../constants/BoardConstants.h"
#include "../board/GameBoard.h"
#include "Action.h"
#include <vector>
#include <map>

using namespace std;

struct MoveOption {
    int x, y;
    Action::Type action;
    int score;  // Higher score is better
    bool hasShellInFuture;
    bool canShootFromHere;
};

class AlgorithmBasic : public Algorithm 
{
private:
    int shootingCooldown = 0;
    Tank* myTank = nullptr;
    bool isInDanger(const Tank& tank) const;
    bool canShootEnemy(const Tank& tank) const;
    bool canShootFromPosition(const Tank& tank, int x, int y) const;
    bool willShellHitPosition(int x, int y, int turnsAhead) const;
    bool isPositionSafeFromShells(int x, int y) const;
    vector<MoveOption> getPossibleMoves(const Tank& tank) const;
    Action::Type findSafeMove(const Tank& tank) const;
    Action::Type findShootingPosition(const Tank& tank) const;
    
public:
    AlgorithmBasic(int playerId, GameBoard* gameBoard);
    ~AlgorithmBasic();
    
    vector<Action> decideNextActions() override;
    void defaultMode() override;
    void update() override;
}; 