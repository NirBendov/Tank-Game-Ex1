#pragma once
class Moveable
{
private:
public:
    enum Direction {
        UP = 0b1000,
        UP_RIGHT = 0b1001,
        RIGHT = 0b0001,
        DOWN_RIGHT = 0b0101,
        DOWN = 0b0100,
        DOWN_LEFT = 0b0110,
        LEFT = 0b0010,
        UP_LEFT = 0b1010
    };
    int location[2];
    Direction dir;
    Moveable(int location[2], Direction dir);
    ~Moveable();
    void move();
};