#include "GameTest.h"
#include <iostream>

GameTest::GameTest(const std::string& name, 
                   const std::vector<std::vector<char>>& board,
                   const std::vector<Action>& p1Moves,
                   const std::vector<Action>& p2Moves,
                   int winner,
                   const std::vector<GameBoard::TankDeath>& deaths)
    : testName(name), initialBoard(board), player1Moves(p1Moves),
      player2Moves(p2Moves), expectedWinner(winner), expectedDeaths(deaths) {}

bool GameTest::runTest(GameBoard& board) const {
    // Add all moves to the board
    for (const auto& move : player1Moves) {
        board.addToStepMoves(move, 1);
    }
    for (const auto& move : player2Moves) {
        board.addToStepMoves(move, 2);
    }

    // Execute the game until it's over
    while (!board.isGameOver()) {
        board.executeStep();
    }

    // Check winner
    if (board.getWinner() != expectedWinner) {
        std::cout << "Test " << testName << " failed: Wrong winner. Expected " 
                  << expectedWinner << ", got " << board.getWinner() << std::endl;
        return false;
    }

    // Check deaths
    if (!compareDeaths(board.tankDeaths, expectedDeaths)) {
        std::cout << "Test " << testName << " failed: Deaths don't match expected" << std::endl;
        return false;
    }

    return true;
}

bool GameTest::compareDeaths(const std::vector<GameBoard::TankDeath>& actual, 
                           const std::vector<GameBoard::TankDeath>& expected) const {
    if (actual.size() != expected.size()) {
        return false;
    }

    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i].position != expected[i].position ||
            actual[i].playerId != expected[i].playerId ||
            actual[i].cause != expected[i].cause) {
            return false;
        }
    }

    return true;
}

bool GameTest::execute() const {
    GameBoard board(initialBoard);
    return runTest(board);
} 