#pragma once

#include "Game.h"
#include "RayCaster.h"

class Renderer
{
    RayCaster* _rc;
    Game*      _g;

    void PaintWall(int x, int centerLine, uint32_t wallHeight, uint8_t textureNo, uint8_t textureX, uint32_t* frameBuffer);
    void PaintFloor(int x, int startY, int endY, uint32_t* frameBuffer);
    void PaintLevel(int x, float verticalOffset, const RayCaster::TraceHit& hit, uint32_t* frameBuffer, int* point);

    inline static uint32_t GetARGB(uint8_t brightness)
    {
        return (brightness << 16) + (brightness << 8) + brightness;
    }

public:
    void TraceFrame(Game* g, uint32_t* frameBuffer);
    Renderer(RayCaster* rc);
    ~Renderer();
};
