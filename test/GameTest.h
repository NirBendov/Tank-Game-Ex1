#pragma once
#include <vector>
#include <string>
#include "../board/GameBoard.h"
#include "../algorithms/Action.h"

class GameTest {
protected:
    std::string testName;
    std::vector<std::vector<char>> initialBoard;
    std::vector<Action::Type> player1MoveTypes;
    std::vector<Action::Type> player2MoveTypes;
    int expectedWinner;
    std::vector<GameBoard::TankDeath> expectedDeaths;

    bool runTest(GameBoard& board) const;
    bool compareDeaths(const std::vector<GameBoard::TankDeath>& actual, 
                      const std::vector<GameBoard::TankDeath>& expected) const;

public:
    GameTest(const std::string& name, 
             const std::vector<std::vector<char>>& board,
             const std::vector<Action::Type>& p1MoveTypes,
             const std::vector<Action::Type>& p2MoveTypes,
             int winner,
             const std::vector<GameBoard::TankDeath>& deaths)
        : testName(name), initialBoard(board), 
          player1MoveTypes(p1MoveTypes), player2MoveTypes(p2MoveTypes),
          expectedWinner(winner), expectedDeaths(deaths) {}

    virtual bool execute() const;
    std::string getName() const { return testName; }
}; 