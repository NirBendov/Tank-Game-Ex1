#include "GameTest.h"
#include "TestRunner.h"
#include "../algorithms/Action.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Direction.h"
#include "../board/GameBoard.h"

void setupShellCollisionTest(TestRunner& runner) {
    // Test 1: Basic shell collision
    {
        // Create a board with two tanks facing each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', ' ', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - both tanks shoot at each other
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

        // Expected deaths - none, as shells should collide and disappear
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("ShellCollisionTest_Basic", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }

    // Test 2: Shell hits tank
    {
        // Create a board with two tanks facing each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', ' ', ' ', ' ', '2', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - player 1 shoots, player 2 moves towards the shell
        std::vector<Action::Type> p1Moves = {
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::MOVE_FORWARD,
            Action::Type::MOVE_FORWARD,
            Action::Type::MOVE_FORWARD
        };

        // Expected deaths - player 2's tank should be hit by the shell
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 4},2, "Shell hit"}
        };

        // Create and add the test
        GameTest test("ShellCollisionTest_TankIntoShell", board, p1Moves, p2Moves, 1, expectedDeaths);
        runner.addTest(test);
    }

    // Test 3: Tank moves into shell
    {
        // Create a board with two tanks facing each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', ' ', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - player 1 shoots, player 2 moves towards the shell
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

        // Expected deaths - player 2's tank should be hit by the shell
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 5}, 2, "Shell hit"}
        };

        // Create and add the test
        GameTest test("ShellCollisionTest_TankHit", board, p1Moves, p2Moves, 1, expectedDeaths);
        runner.addTest(test);
    }

    // Test 4: Adjacent tanks shoot at each other
    {
        // Create a board with two tanks next to each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '1', '2', ' ', ' ', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - both tanks shoot at each other
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

        // Expected deaths - none, as shells should collide immediately
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("ShellCollisionTest_AdjacentTanks", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }
    // Test 5: shoot threw border
    {
        // Create a board with two tanks next to each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {' ', ' ', ' ', '2', ' ', '1', ' ', ' '},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - both tanks shoot at each other
        std::vector<Action::Type> p1Moves = {
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - none, as shells should collide immediately
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 3}, 2, "Shell hit"}
        };

        // Create and add the test
        GameTest test("ShellCollisionTest_Border", board, p1Moves, p2Moves, 1, expectedDeaths);
        runner.addTest(test);
    }
        // Test 6: shoot threw border top
    {
        // Create a board with two tanks next to each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', ' ', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', '1', ' ', '#'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' ', '2', ' ', '#'},
            {'#', '#', '#', '#', '#', ' ', '#', '#'}
        };

        // Create moves - both tanks shoot at each other
        std::vector<Action::Type> p1Moves = {
            Action::Type::TURN_L_90,
            Action::Type::SHOOT,
            Action::Type::SHOOT,
            Action::Type::SHOOT,
            Action::Type::SHOOT,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - none, as shells should collide immediately
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{3, 5}, 2, "Shell hit"}
        };

        // Create and add the test
        GameTest test("ShellCollisionTest_BorderTop", board, p1Moves, p2Moves, 1, expectedDeaths);
        runner.addTest(test);
    }
    // Test 7: move backword
    {
                // Create a board with two tanks next to each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', ' ', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', '2', ' ', '#'},
            {' ', '1', ' ', ' ', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', ' ', '#', '#'}
        };

        // Create moves - both tanks shoot at each other
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_BACKWARD,
            Action::Type::MOVE_BACKWARD,
            Action::Type::MOVE_BACKWARD,
            Action::Type::SHOOT,
            Action::Type::SHOOT,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::NOP,
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - none, as shells should collide immediately
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 5}, 2, "Shell hit"}
        };

        // Create and add the test
        GameTest test("ShellCollisionTest_MoveBackward", board, p1Moves, p2Moves, 1, expectedDeaths);
        runner.addTest(test);
    }
        // Test 8: move backword canceled
    {
                // Create a board with two tanks next to each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', ' ', '#', '#'},
            {'#', '1', ' ', ' ', ' ', '2', ' ', '#'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', ' ', '#', '#'}
        };

        // Create moves - both tanks shoot at each other
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_BACKWARD,
            Action::Type::MOVE_FORWARD,
            Action::Type::MOVE_BACKWARD,
            Action::Type::SHOOT,
            Action::Type::SHOOT,
            Action::Type::NOP
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::NOP,
            Action::Type::SHOOT,
            Action::Type::NOP,
            Action::Type::NOP,
            Action::Type::NOP
        };

        // Expected deaths - none, as shells should collide immediately
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{1, 5}, 2, "Shell hit"}
        };

        // Create and add the test
        GameTest test("ShellCollisionTest_MoveBackwardCanceled", board, p1Moves, p2Moves, 1, expectedDeaths);
        runner.addTest(test);
    }
} 