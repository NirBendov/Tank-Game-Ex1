#pragma once
#include <vector>
#include <string>
#include "./game_objects/Moveable.h"
#include "./game_objects/Tank.h"
#include "../algorithms/Action.h"
#include <map>

using namespace std;

class GameBoard {
private:
    int height, width;
    vector<vector<char>> board;
    vector<string> player1Moves;
    vector<string> player2Moves;
    bool gameOver;
    vector<Tank> player1Tanks;
    vector<Tank> player2Tanks;
    map<pair<int, int>, int> tankShootCooldowns; // Maps tank position to remaining cooldown turns
    vector<Action> stepMoves; // List of moves to execute in the next step

    bool isWall(int x, int y) const;
    void updateCooldowns();

public:
    typedef struct {
        vector<Tank> player1Tanks;
        vector<Tank> player2Tanks;
        vector<Shell> bulletsPositions;
    } BoardData;
    
    BoardData *boardData;
    
    GameBoard(const vector<vector<char>>& initialBoard);
    ~GameBoard();
    
    bool validateMove(const Action& action, int playerId) const;
    void addToStepMoves(const Action& action, int playerId);
    void executeStep();
    void saveGameMoves(const string& filename) const;
    bool isGameOver() const { return gameOver; }
    void setGameOver(bool status) { gameOver = status; }
    
    // Get positions of tanks for a specific player
    vector<pair<int, int>> getPlayerTankPositions(int playerId) const;
    
    // Get positions of enemy tanks for a specific player
    vector<pair<int, int>> getEnemyTankPositions(int playerId) const;

    // Get tanks for a specific player
    const vector<Tank>& getPlayerTanks(int playerId) const {
        return playerId == 1 ? player1Tanks : player2Tanks;
    }
}; 