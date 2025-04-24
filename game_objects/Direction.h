#pragma once

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
    LEFT_90 = -2
};

int vectorMultiply(int d1[2], int d2[2]);
int inverseMap(int d[2]);
int *getDirection(int d[2], Turn t);