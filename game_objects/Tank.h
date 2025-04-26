#pragma once

#include "Moveable.h"
#include "Shell.h"
#include "Direction.h"           
#include "../algorithms/Algorithm.h"
#include "../player/Player.h"
#include "../constants/BoardConstants.h"

class Tank : public Moveable {
private:
    int ammoCount;
    Algorithm *algo;
    Player::PlayerId playerId;
    int moveBackwardCooldown;
    int shootingCooldown;
    bool isMovingBackward = false;
    bool finishedMoveBackward = false;
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
    bool getIsMovingBackward() const { return isMovingBackward && !finishedMoveBackward; }
    bool getFinishedMoveBackward() const { return finishedMoveBackward; }
    void setFinishedMoveBackward() { finishedMoveBackward = true; }
    void cancelMoveBackward() {
        isMovingBackward = false;
        moveBackwardCooldown = BoardConstants::MOVE_BACKWARD_COOLDOWN;
        finishedMoveBackward = false;
    }
    bool getIsTankAlive() const { return isTankAlive; }
    void killTank() { isTankAlive = false; }
    void decreaseShootingCooldown();
    int getShootingCooldown();
    int getAmmoCount() const { return ammoCount; }
};

static constexpr int MAX_AMMO = 16;
