#pragma once

#include "Game.h"
#include "RayCaster.h"

class Renderer
{
    RayCaster* _rc;
    Game*      _g;

    void PaintWall(int x, int centerLine, uint32_t wallHeight, uint8_t textureNo, uint8_t textureX, uint32_t* frameBuffer);
    void PaintFloor(int x, int startY, int endY, uint32_t* frameBuffer, int col);
    void PaintLevel(int x, float verticalOffset, const RayCaster::TraceHit& hit, bool isHit, uint32_t* frameBuffer, int* point, int height);
    bool TrimPaint(int* sy, int* ey);
    std::vector<std::pair<int, int>> TrimFloor(int* sy, int* ey);

    inline static uint32_t GetARGB(uint8_t brightness)
    {
        int r = brightness >> 1;
        int g = brightness >> 2;
        int b = brightness >> 4;
        return (r << 16) + (g << 8) + b;// + brightness;
        //return (brightness << 16) + (brightness << 8) + brightness;
    }

public:
    void TraceFrame(Game* g, uint32_t* frameBuffer);
    Renderer(RayCaster* rc);
    ~Renderer();
};
