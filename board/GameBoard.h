#pragma once
#include <vector>
#include <string>
#include <map>
#include "../game_objects/Tank.h"
#include "../game_objects/Shell.h"
#include "../algorithms/Action.h"
#include "../constants/BoardConstants.h"
#include "../game_objects/Direction.h"
#include "../algorithms/Algorithm.h"

class Action;
class Moveable;
class Tank;
class Shell;

using namespace std;

class GameBoard {
private:
    int height, width;
    vector<vector<char>> board;
    vector<string> player1Moves;
    vector<string> player2Moves;
    bool gameOver;
    int winner; // 0 for tie, 1 for player 1, 2 for player 2
    vector<Tank> player1Tanks;
    vector<Tank> player2Tanks;
    vector<Shell> bulletsPositions; // Store bullets in the main class
    map<pair<int, int>, int> tankShootCooldowns; // Maps tank position to remaining cooldown turns
    vector<Action> stepMoves; // List of moves to execute in the next step
    vector<pair<int, int>> backwardMoves; // Positions where tanks moved back
    int numOfTurnsSinceNoAmmo = 0; // Number of turns since all ammo was used
    bool isWall(int x, int y) const;
    char calculateNewPositionCharForTank(int x, int y, int playerId);
    char calculateNewPositionCharForShell(int x, int y);
    char handleMultipleTanksPosition(int x, int y);
    void performActions();
    void moveShells();
    void handleCollisions();
    void handleCollision(int i, int j, char currentChar);
    void moveTankBack(int x, int y);
    int  countShellsAtPosition(int x, int y) const;
    void removeBulletsAtPosition(int x, int y);
    void killTanksAtPosition(int x, int y, const string& cause);
    bool isShellAtPosition(int x, int y) const;
    bool checkGameOver(); // Check if game is over and set winner
    void checkNoAmoForTanks(); // Check if all tanks are out of ammo
    void updateAlgorithmsAfterShells(); // If any algorithm needs to update some data about moving shells
public:
    typedef struct {
        pair<int, int> position;
        int playerId;
        string cause;
    } TankDeath;
    
    vector<TankDeath> tankDeaths; // List of tank deaths with their positions, player IDs and causes
    
    GameBoard(const vector<vector<char>>& initialBoard);
    ~GameBoard();
    
    bool validateMove(const Action& action, int playerId) const;
    void addToStepMoves(const Action& action, int playerId);
    void executeStep();
    void saveGameMoves(const string& filename) const;
    bool isGameOver() const { return gameOver; }
    int getWinner() const { return winner; } // Get the winner (0 for tie, 1 for player 1, 2 for player 2)
    
    // Get tanks for a specific player
    const vector<Tank>& getPlayerTanks(int playerId) const {
        return playerId == 1 ? player1Tanks : player2Tanks;
    }

    // Get positions of tanks for a specific player
    vector<pair<int, int>> getPlayerTankPositions(int playerId) const;
    
    // Get positions of enemy tanks for a specific player
    vector<pair<int, int>> getEnemyTankPositions(int playerId) const;

    // Get all bullets on the board
    const vector<Shell>& getBullets() const { return bulletsPositions; }

    // Print the current state of the board
    void printBoard() const;

    // Add getBoard method
    const vector<vector<char>>& getBoard() const { return board; }
}; 