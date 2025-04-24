#pragma once
#include <vector>
#include "./game_objects/Moveable.h"
using namespace std;
struct BoardData {
    Moveable::Info *tank1Position[2];
    Moveable::Info *tank2Position[2];
    vector<Moveable::Info*> bulletsPositions;
};
class GameBoard
{
private:
    int height, width;
    vector<vector<char>> board;
public:
    typedef struct {
        Moveable::Info *tank1Position[2];
        Moveable::Info *tank2Position[2];
        vector<Moveable::Info*> bulletsPositions;
    } BoardData;
    BoardData data;
    GameBoard(/* args */);
    ~GameBoard();
    void repaintBoard(); 
    // should add game turn summary object that summarizes all events that happened in one turn
    // events include moving objects, destroyed or damages walls and game over scenario
};