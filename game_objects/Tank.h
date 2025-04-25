#pragma once

#include "Moveable.h"
#include "Shell.h"
#include "Direction.h"           // for Turn
#include "algorithms/Algorithm.h"
#include "player/Player.h"

class Tank : public Moveable {
private:
    int ammoCount;
    Algorithm *algo;
    Player::PlayerId playerId;
    int moveBackwardCooldown;
    bool isMovingBackward;
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
        using namespace BoardConstants;
        moveBackwardCooldown = MOVE_BACKWARD_COOLDOWN;
    }
    void moveBackward() {
        using namespace BoardConstants;
        isMovingBackward = true;
        moveBackwardCooldown = MOVE_BACKWARD_COOLDOWN;
    }
};

static constexpr int MAX_AMMO = 16;
