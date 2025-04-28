#pragma once
#include <array>
#include "../algorithms/PathFinder.h"
#include "../algorithms/Action.h"
class GameBoard;  // Forward declaration
struct Point;

using namespace std;

const int directions[8][2] = {
    {-1, 0}, //UP
    {-1, 1}, //UP_RIGHT
    {0, 1}, //RIGHT
    {1, 1}, //DOWN_RIGHT
    {1, 0}, //DOWN
    {1, -1}, //DOWN_LEFT
    {0, -1}, //LEFT
    {-1, -1}, //UP_LEFT
};

enum Turn {
    RIGHT_90 = 2,
    RIGHT_45 = 1,
    LEFT_45 = -1,
    LEFT_90 = -2,
    RIGHT_135 = 3,
    LEFT_135 = -3,
    COMPLETE_180 = 4,
    NONE_0 = 0
};

int vectorMultiply(array<int, 2> d1, array<int, 2> d2);
int inverseMap(array<int, 2> d);
array<int, 2> getDirection(array<int, 2> d, Turn t);
bool isInBulletPath(array<int, 2> start, array<int, 2> trajectory, array<int, 2> end, GameBoard *gameBoard);
array<int,2> directionBetweenPoints(Point &start, Point &end);
Turn rotation(array<int,2> currDir, array<int,2> newDir);
Action::Type turnToAction(Turn t);