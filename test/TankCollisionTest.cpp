#include "GameTest.h"
#include "TestRunner.h"
#include "../algorithms/Action.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Direction.h"

void setupTankCollisionTest(TestRunner& runner) {
    // Create a simple board with two tanks facing each other
    std::vector<std::vector<char>> board = {
        {'#', '#', '#', '#', '#', '#', '#', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '1', ' ', ' ', '2', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#'}
    };

    // Create tanks for the moves
    int p1Location[2] = {2, 2};
    int p1Direction[2] = {0, 1}; // Facing right
    Tank p1Tank(p1Location, p1Direction);
    p1Tank.assignPlayerId(Player::PlayerId::P1);

    int p2Location[2] = {2, 5};
    int p2Direction[2] = {0, -1}; // Facing left
    Tank p2Tank(p2Location, p2Direction);
    p2Tank.assignPlayerId(Player::PlayerId::P2);

    // Create moves - both tanks move forward to collide
    std::vector<Action> p1Moves = {
        Action(Action::ActionType::MOVE_FORWARD, p1Tank)
    };

    std::vector<Action> p2Moves = {
        Action(Action::ActionType::MOVE_FORWARD, p2Tank)
    };

    // Expected deaths
    std::vector<GameBoard::TankDeath> expectedDeaths = {
        {{3, 3}, 1, "Multiple tanks collision"},
        {{3, 3}, 2, "Multiple tanks collision"}
    };

    // Create and add the test
    GameTest test("TankCollisionTest", board, p1Moves, p2Moves, 0, expectedDeaths);
    runner.addTest(test);
} 