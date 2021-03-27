#pragma once

#include <stdint.h>

class Game
{
public:
    void Move(int m, int r, int z, int s, float seconds);

    float playerX, playerY, playerZ, playerA;
    int maxPaints;

    Game();
    ~Game();
};
