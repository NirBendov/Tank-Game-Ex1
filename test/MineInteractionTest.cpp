#include "GameTest.h"
#include "TestRunner.h"
#include "../algorithms/Action.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Direction.h"
#include "../board/GameBoard.h"

void setupMineInteractionTest(TestRunner& runner) {
    // Test 1: Tank steps on a mine
    {
        // Create a board with a tank next to a mine
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', '@', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - tank moves onto the mine
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_FORWARD,
            Action::Type::NOP,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - tank should be destroyed by the mine
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 3}, 1, "Mine explosion"}
        };

        // Create and add the test
        GameTest test("MineInteractionTest_StepOnMine", board, p1Moves, p2Moves, 2, expectedDeaths);
        runner.addTest(test);
    }
    // Test 2: Both Tanks step on a mine
    {
        // Create a board with a tank next to a mine
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', '@', '@', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - tank moves onto the mine
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_FORWARD,
            Action::Type::NOP,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::MOVE_FORWARD,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - tank should be destroyed by the mine
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 3}, 1, "Mine explosion"},
            {{2, 4}, 2, "Mine explosion"}
        };

        // Create and add the test
        GameTest test("MineInteractionTest_StepOnMine", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }
} 