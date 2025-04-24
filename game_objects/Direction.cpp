#include "Direction.h"

int vectorMultiply(int d1[2], int d2[2]) {
    return d1[0] * d2[0] + d1[1] * d2[1];
}

int inverseMap(int d[2]) {
    for(int i = 0; i < 8; ++i) {
        if (directions[i] == d) {
            return i;
        }
    }
}

int *getDirection(int d[2], Turn t) {
    int index = inverseMap(d);
    return (int*)d[(index + t)%8];
}