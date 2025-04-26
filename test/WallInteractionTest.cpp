#include "GameTest.h"
#include "TestRunner.h"
#include "../algorithms/Action.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Direction.h"
#include "../board/GameBoard.h"

void setupWallInteractionTest(TestRunner& runner) {
    // Test 1: Try to move into a wall
    {
        // Create a board with a tank next to a wall
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', '#', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - tank tries to move into the wall
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

        // Expected deaths - none, tank should stay in place
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("WallInteractionTest_MoveIntoWall", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }

    // Test 2: Try to move into a damaged wall
    {
        // Create a board with a tank next to a damaged wall
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', 'D', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - tank tries to move into the damaged wall
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

        // Expected deaths - none, tank should stay in place
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("WallInteractionTest_MoveIntoDamagedWall", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }

    // Test 3: Shoot a wall
    {
        // Create a board with a tank facing a wall
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', '#', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - tank shoots at the wall
        std::vector<Action::Type> p1Moves = {
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - none, wall should become damaged
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("WallInteractionTest_ShootWall", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }

    // Test 4: Two tanks shoot wall together
    {
        // Create a board with two tanks facing a wall
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', '#', '2', ' ', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - both tanks shoot at the wall
        std::vector<Action::Type> p1Moves = {
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - none, wall should be destroyed
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("WallInteractionTest_DoubleShootWall", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }
} 