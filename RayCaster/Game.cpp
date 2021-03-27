#define _USE_MATH_DEFINES

#include <cstdlib>
#include "math.h"

#include "Game.h"
#include "RayCaster.h"

void Game::Move(int m, int r, int z, float seconds)
{
    playerA += 0.05f * r * seconds * 25.0f;
    playerZ += 0.05f * z * seconds;
    playerX += 0.5f * m * sin(playerA) * seconds * 5.0f;
    playerY += 0.5f * m * cos(playerA) * seconds * 5.0f;

    while(playerA < 0)
    {
        playerA += 2.0f * M_PI;
    }
    while(playerA >= 2.0f * M_PI)
    {
        playerA -= 2.0f * M_PI;
    }

    if(playerX < 1)
    {
        playerX = 1.01f;
    }
    else if(playerX > MAP_X - 2)
    {
        playerX = MAP_X - 2 - 0.01f;
    }
    if(playerY < 1)
    {
        playerY = 1.01f;
    }
    else if(playerY > MAP_Y - 2)
    {
        playerY = MAP_Y - 2 - 0.01f;
    }
}

Game::Game()
{
    //playerX = 23.03f;
    //playerY = 6.8f;
    playerX = 5.85f;
    playerY = 7.15f;
    playerA = 3.65f;
    playerZ = 0;
    maxPaints = 0;
}

Game::~Game() {}
