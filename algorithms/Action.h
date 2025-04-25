#pragma once
#include <utility>
#include "../game_objects/Tank.h"
class Action {
public:
    enum ActionType {
        NOP,
        SHOOT,
        MOVE_FORWARD,
        MOVE_BACKWARD,
        TURN_R_45,
        TURN_R_90,
        TURN_L_45,
        TURN_L_90
    };

    Action(ActionType type, Tank tank)
        : actionType(type), tank(tank) {}

    ActionType getActionType() const { return actionType; }
    Tank getTargetTank() const { return tank; }

private:
    ActionType actionType;
    Tank tank;
}; 