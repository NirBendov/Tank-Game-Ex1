#pragma once

class Tank;

class Action {
public:
    enum class Type {
        NOP,
        SHOOT,
        MOVE_FORWARD,
        MOVE_BACKWARD,
        TURN_R_45,
        TURN_R_90,
        TURN_L_45,
        TURN_L_90
    };

    Action(Type type, const Tank& target)
      : type_(type), target_(target) {}

    Type type()   const noexcept { return type_; }
    const Tank&  target() const noexcept { return target_; }
    void setType(Type newType) noexcept { type_ = newType; }

private:
    Type        type_;
    const Tank& target_;
};
