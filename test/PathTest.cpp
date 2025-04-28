#include <iostream>
#include <vector>
#include <cassert>
#include "../algorithms/PathFinder.h"

using namespace std;

void printPath(const vector<Point>& path) {
    for (const auto& p : path) {
        cout << "(" << p.x << "," << p.y << ") ";
    }
    cout << endl;
}

void wrapPointTest() {
    Point p({1,1});
    vector<Point> points = {{1,1}, {4,2}, {2,4}, {0,4}, {4,0}};
    vector<pair<int, int>> dims = {{5,5}, {5,8}, {8,5}, {7,9}, {6,7}};
    for (pair<int, int> d: dims) {
        int rows = d.first; int cols = d.second;
        for (Point p: points) {
            Point q = wrapPoint(rows + p.x%rows, cols + p.y%cols, rows, cols);
            assert(q == p);
        }
    }
    cout << "wrapPoint test completed" << endl;
}

void distTest() {
    int rows = 8; int cols = 6;
    Point p1({0, 0}); Point p2({1, cols - 2});
    assert(dist(p1, p2, rows, cols) == 2);

    Point p3({3, cols - 2});
    assert(dist(p1, p3, rows, cols) == 3);
    assert(dist(p2, p3, rows, cols) == 2);

    Point p4({rows - 1, 0});
    assert(dist(p1, p4, rows, cols) == 1);
    assert(dist(p2, p4, rows, cols) == 2);
    assert(dist(p3, p4, rows, cols) == 4);

    cout << "dist test completed" << endl;
}

void updatePathEndTest() {
    vector<Point> path = {{0, 0}, {0, 1}};
    int rows = 4; int columns = 4;

    Point p1 = {0, 2};
    updatePathEnd(path, p1, rows, columns);
    vector<Point> expected1 = {{0, 0}, {0, 1}, {0, 2}};
    assert(path == expected1);

    Point p2 = {1,2};
    updatePathEnd(path, p2, rows, columns);
    vector<Point> expected2 = {{0, 0}, {0, 1}, {1, 2}};
    assert(path == expected2);

    Point p3 = {0, 1};
    updatePathEnd(path, p3, rows, columns);
    vector<Point> expected3 = {{0, 0}, {0, 1}};
    assert(path == expected3);

    Point p4 = {rows - 1, 1};
    updatePathEnd(path, p4, rows, columns);
    vector<Point> expected4 = {{0, 0}, {rows - 1, 1}};
    assert(path == expected4);

    cout << "updateEndPath test completed" << endl;
}

void updatePathStartTest() {
    vector<Point> path = {{0, 0}, {0, 1}};
    int rows = 4; int columns = 4;

    Point p1 = {1, 0};
    updatePathStart(path, p1, rows, columns);
    vector<Point> expected1 = {{1, 0}, {0, 1}};
    assert(path == expected1);

    Point p2 = {2,0};
    updatePathStart(path, p2, rows, columns);
    vector<Point> expected2 = {{2, 0},{1, 0}, {0, 1}};
    assert(path == expected2);

    Point p3 = {2, columns - 1};
    updatePathStart(path, p3, rows, columns);
    vector<Point> expected3 = {{2, columns - 1},{1, 0}, {0, 1}};
    assert(path == expected3);

    Point p4 = {2,0};
    updatePathStart(path, p4, rows, columns);
    vector<Point> expected4 = {{2, 0},{1, 0}, {0, 1}};
    assert(path == expected4);

    Point p5 = {1,1};
    updatePathStart(path, p5, rows, columns);
    vector<Point> expected5 = {{1, 1}, {0, 1}};
    assert(path == expected5);

    cout << "updateStartPath test completed" << endl;
}

void calcDirectionTest() {
    int rows = 5, cols = 5;

    vector<Point> p1 = {{0, 0}, {0, 1}};
    auto dir1 = calcDirection(p1, rows, cols);
    assert(dir1[0] == 0 && dir1[1] == 1);

    vector<Point> p2 = {{0, 0}, {0, 4}};
    auto dir2 = calcDirection(p2, rows, cols);
    assert(dir2[0] == 0 && dir2[1] == -1);

    vector<Point> p3 = {{0, 0}, {1, 0}};
    auto dir3 = calcDirection(p3, rows, cols);
    assert(dir3[0] == 1 && dir3[1] == 0);

    vector<Point> p4 = {{0, 0}, {4, 0}};
    auto dir4 = calcDirection(p4, rows, cols);
    assert(dir4[0] == -1 && dir4[1] == 0);

    vector<Point> p5 = {{0, 0}, {1, 1}};
    auto dir5 = calcDirection(p5, rows, cols);
    assert(dir5[0] == 1 && dir5[1] == 1);

    cout << "calcDirection test completed" << endl;
}

void isPathStraightTest() {
    int rows = 5, cols = 5;

    vector<Point> p1 = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
    assert(isPathStraight(p1, rows, cols));

    vector<Point> p2 = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
    assert(isPathStraight(p2, rows, cols));

    vector<Point> p3 = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    assert(isPathStraight(p3, rows, cols));

    vector<Point> p4 = {{0, 0}, {1, 0}, {1, 1}};
    assert(!isPathStraight(p4, rows, cols));

    vector<Point> p5 = {{0, 4}, {0, 0}, {0, 1}};
    assert(isPathStraight(p5, rows, cols));

    vector<Point> p6 = {{4, 0}, {0, 0}, {1, 0}};
    assert(isPathStraight(p6, rows, cols));

    cout << "isPathStraight test completed" << endl;
}

int main() {
    wrapPointTest();
    distTest();
    updatePathEndTest();
    updatePathStartTest();
    calcDirectionTest();
    isPathStraightTest();
    return 0;
}