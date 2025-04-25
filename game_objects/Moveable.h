#pragma once
#include <array>
#include <utility>  // for std::move if you want

using namespace std;

class Moveable {
public:
    struct Info {
        array<int,2> location;
        array<int,2> dir;
    };

    Moveable(int lc[2], int d[2]);

    Moveable(Info init)
      : info(std::move(init))
    {}

    virtual ~Moveable() = default;

    const Info& getInfo() const { return info; }

    void move();

protected:
    Info info;
};
