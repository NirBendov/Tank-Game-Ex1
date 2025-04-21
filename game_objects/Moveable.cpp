#include "Moveable.h"
class Moveable
{
private:
    int location[2];
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
    Direction dir;
    Moveable(int lc[2], Direction d);
    ~Moveable();
    void Moveable::move() {
        if (0b1000 ^ dir) {
            location[0] += 1;
        }
        else if (0b0100 ^ dir) {
            location[0] -= 1;
        }
        if (0b0010 ^ dir) {
            location[1] -= 1;
        }
        else if (0b0001 ^ dir) {
            location[1] += 1;
        }
    }
};

Moveable::Moveable(int lc[2], Direction d): dir(d) 
{
    *location = *lc;
}

Moveable::~Moveable()
{
}
