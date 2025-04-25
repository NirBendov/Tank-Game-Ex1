#include "GameTest.h"
#include "TestRunner.h"
#include "../algorithms/Action.h"
#include "../game_objects/Tank.h"
#include "../game_objects/Direction.h"

void setupShellCollisionTest(TestRunner& runner) {
    // Create a board with two tanks facing each other
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

    // Create moves - both tanks shoot at each other
    std::vector<Action> p1Moves = {
        Action(Action::ActionType::SHOOT, p1Tank)
    };

    std::vector<Action> p2Moves = {
        Action(Action::ActionType::SHOOT, p2Tank)
    };

    // Expected deaths - none, as shells should collide and disappear
    std::vector<GameBoard::TankDeath> expectedDeaths = {};

    // Create and add the test
    GameTest test("ShellCollisionTest", board, p1Moves, p2Moves, 0, expectedDeaths);
    runner.addTest(test);
} 