#include "TankCollisionTest.h"
#include "GameTest.h"
#include "TestRunner.h"
#include "../algorithms/Action.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Direction.h"
#include "../board/GameBoard.h"
#include <iostream>
#include <vector>
#include <string>

void setupTankCollisionTest(TestRunner& runner) {
    // Test 1: Basic tank collision
    {
        // Create a simple board with two tanks facing each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', ' ', '1', '2', ' ', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - both tanks move forward to collide
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_FORWARD
        };

        std::vector<Action::Type> p2Moves = {
        };

        // Expected deaths
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 4}, 1, "Multiple tanks collision"},
            {{2, 4}, 2, "Multiple tanks collision"}
        };

        // Create and add the test
        GameTest test("TankCollisionTest_Basic", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }

    // Test 2: Tank dodging collision
    {
        // Create a board with two tanks facing each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', ' ', '1', ' ', '2', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - tank 1 moves forward, tank 2 turns and moves away
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_FORWARD,
            Action::Type::MOVE_FORWARD
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::TURN_R_90,
            Action::Type::MOVE_FORWARD
        };

        // Expected deaths - none, as tank 2 dodges
        std::vector<GameBoard::TankDeath> expectedDeaths = {};

        // Create and add the test
        GameTest test("TankCollisionTest_Dodge", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }

    // Test 3: Tank pass each other
    {
        // Create a simple board with two tanks facing each other
        std::vector<std::vector<char>> board = {
            {'#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', ' ', '1', '2', ' ', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#'}
        };

        // Create moves - both tanks move forward to collide
        std::vector<Action::Type> p1Moves = {
            Action::Type::MOVE_FORWARD
        };

        std::vector<Action::Type> p2Moves = {
            Action::Type::MOVE_FORWARD
        };

        // Expected deaths
        std::vector<GameBoard::TankDeath> expectedDeaths = {
            {{2, 3}, 1, "Multiple tanks collision"},
            {{2, 3}, 2, "Multiple tanks collision"}
        };

        // Create and add the test
        GameTest test("TankCollisionTest_HopOver", board, p1Moves, p2Moves, 0, expectedDeaths);
        runner.addTest(test);
    }
}
