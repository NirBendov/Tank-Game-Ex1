#include <vector>
using namespace std;

class GameBoard
{
private:
    int height, width;
    vector<vector<char>> board;
    int tank1Position[2];
    int tank2Position[2];
    vector<int[]> bulletsPositions;

public:
    GameBoard(/* args */);
    ~GameBoard();
};

GameBoard::GameBoard(/* args */)
{
}

GameBoard::~GameBoard()
{
}
