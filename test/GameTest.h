#pragma once
#include <vector>
#include <string>
#include "../board/GameBoard.h"
#include "../algorithms/Action.h"

class GameTest {
private:
    std::string testName;
    std::vector<std::vector<char>> initialBoard;
    std::vector<Action> player1Moves;
    std::vector<Action> player2Moves;
    int expectedWinner;
    std::vector<GameBoard::TankDeath> expectedDeaths;

    bool runTest(GameBoard& board) const;
    bool compareDeaths(const std::vector<GameBoard::TankDeath>& actual, 
                      const std::vector<GameBoard::TankDeath>& expected) const;

public:
    GameTest(const std::string& name, 
             const std::vector<std::vector<char>>& board,
             const std::vector<Action>& p1Moves,
             const std::vector<Action>& p2Moves,
             int winner,
             const std::vector<GameBoard::TankDeath>& deaths);

    bool execute() const;
    std::string getName() const { return testName; }
}; 