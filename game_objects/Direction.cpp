#include "Direction.h"
#include "./constants/BoardConstants.h"
#include "./board/GameBoard.h"
#include <cmath>
#include <array>

using namespace std;

int vectorMultiply(int d1[2], int d2[2]) {
    return d1[0] * d2[0] + d1[1] * d2[1];
}

int inverseMap(int d[2]) {
    for(int i = 0; i < 8; ++i) {
        if (directions[i] == d) {
            return i;
        }
    }
}

int *getDirection(int d[2], Turn t) {
    int index = inverseMap(d);
    return (int*)d[(index + t)%8];
}

bool isInBulletPath(array<int, 2> start, array<int, 2> trajectory, array<int, 2> end, GameBoard *gameBoard) {
    int dy = start[0] - end[0];
    int dx = start[1] - end[1];
    dy = dy == 0 ? dy : dy/abs(dy);
    dx = dx == 0 ? dx : dx/abs(dx);
    if (!(trajectory[0] == dy && trajectory[1] == dx))
        return false;
    int m = trajectory[0] != 0 ? min(start[0], end[0]) : min(start[1], end[1]);
    int n = trajectory[0] != 0 ? max(start[0], end[0]) : max(start[1], end[1]);
    int x = trajectory[0] != 0 ? start[0] : end[0];
    int y = trajectory[0] != 0 ? start[1] : end[1];
    for (int i = m; i <= n; ++i) {
    }
}