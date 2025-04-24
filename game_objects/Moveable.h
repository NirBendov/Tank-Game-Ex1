#pragma once
class Moveable
{
private:
public:
    typedef struct {
        int location[2];
        int dir[2];
    } Info;
    Info *info;
    Moveable(int location[2], int dir[2]);
    ~Moveable();
    void move();
};