#include "Moveable.h"
#include "Shell.h"
class Tank: public Moveable
{
private:
    int ammoCount;
public:
    Tank(int lc[2], Direction d):Moveable(lc, d) {}
    ~Tank();
    void aim(Direction newDir);
    Shell shoot();
};

Tank::~Tank()
{
}
