#pragma once

#include "Moveable.h"
#include "Shell.h"
#include "Direction.h"           // for Turn
#include "../algorithms/Algorithm.h"
#include "../player/Player.h"
using namespace BoardConstants;
class Tank : public Moveable {
private:
    int ammoCount;
    Algorithm *algo;
    Player::PlayerId playerId;
    int moveBackwardCooldown;
    bool isMovingBackward;
    bool isTankAlive = true;
public:
    Tank(int lc[2], int d[2]);
    ~Tank() override;

    void turn(Turn t);
    Shell* shoot();

    void assignAlgorithm(Algorithm *a);
    const Algorithm* getAlgorithm() const;

    void assignPlayerId(Player::PlayerId id);
    Player::PlayerId getPlayerId() const;
    int getMoveBackwardCooldown() const { return moveBackwardCooldown; }
    void decreaseMoveBackwardCooldown() { 
        if (moveBackwardCooldown > 0) {
            --moveBackwardCooldown; 
        }
    }
    void moveForward() {
        moveBackwardCooldown = BoardConstants::MOVE_BACKWARD_COOLDOWN;
        isMovingBackward = false;
    }
    void moveBackward() {
        isMovingBackward = true;
        moveBackwardCooldown = BoardConstants::MOVE_BACKWARD_COOLDOWN;
    }
    bool getIsMovingBackward() const { return isMovingBackward; }
    bool getIsTankAlive() const { return isTankAlive; }
    void killTank() { isTankAlive = false; }
};

static constexpr int MAX_AMMO = 16;
