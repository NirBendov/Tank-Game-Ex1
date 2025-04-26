#include "Tank.h"
#include "Direction.h"
#include "./constants/BoardConstants.h"

Tank::Tank(int location[2], int dir[2])
  : Moveable(location, dir)
  , ammoCount(MAX_AMMO)
  , shootingCooldown(0)
  , algo(nullptr)
  , playerId(Player::PlayerId(/* some default, e.g. Player::None */))
{}

Tank::~Tank() {
    delete algo;
}

void Tank::turn(Turn t) {
    array<int, 2> newDir = getDirection(info.dir, t);
    info.dir[0] = newDir[0];
    info.dir[1] = newDir[1];
    moveForward();
    decreaseShootingCooldown();
}

Shell* Tank::shoot() {
    if (ammoCount > 0 && shootingCooldown == 0) {
        --ammoCount;
        moveForward();
        shootingCooldown = BoardConstants::SHOOT_COOLDOWN;
        auto shell = new Shell(info.location.data(), info.dir.data());
        shell->setJustFired(true);
        return shell;
    }
    // TODO: throw or handle out-of-ammo
    return nullptr;
}

void Tank::assignAlgorithm(Algorithm *a) {
    algo = a;
}

const Algorithm* Tank::getAlgorithm() const {
    return algo;
}

void Tank::assignPlayerId(Player::PlayerId id) {
    playerId = id;
}

Player::PlayerId Tank::getPlayerId() const {
    return playerId;
}

void Tank::decreaseShootingCooldown() {
    if (shootingCooldown > 0) {
        shootingCooldown -= 1;
    }
}

int Tank::getShootingCooldown() {
    return shootingCooldown;
}
