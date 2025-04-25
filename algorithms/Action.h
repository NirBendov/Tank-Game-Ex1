#pragma once
#include "../game_objects/Tank.h"

class Action {
public:
    enum class Type {
        NOP,
        Shoot,
        MoveForward,
        MoveBackward,
        TurnR45,
        TurnR90,
        TurnL45,
        TurnL90
    };

    Action(Type type, const Tank& target)
      : type_(type), target_(target) {}

    Type        type()   const noexcept { return type_; }
    const Tank& target() const noexcept { return target_; }

private:
    Type        type_;
    const Tank& target_;
};
