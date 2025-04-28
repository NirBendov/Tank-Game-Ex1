#pragma once

#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include "../constants/BoardConstants.h"
#include "../game_objects/Direction.h"

using namespace std;

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Node {
    Point pt;
    int dist;
};

bool isValid(int x, int y, const vector<vector<int>>& grid, const vector<vector<bool>>& visited, bool includeWalls);
Point wrapPoint(int x, int y, int rows, int cols);
vector<Point> bfsPathfinder(const vector<vector<char>>& grid, Point start, Point end, bool includeWalls);
int dist(Point p1, Point p2, int rows, int cols);
void updatePathEnd(vector<Point> &path, Point &newEnd, int rows, int cols);
void updatePathStart(vector<Point> &path, Point &newStart, int rows, int cols);
bool isPathStraight(vector<Point> &path, int rows, int columns);
bool isPathClear(vector<Point> &path, const vector<vector<char>>& grid);
array<int,2> calcDirection(vector<Point> &path, int rows, int columns);