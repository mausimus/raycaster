#pragma once

#include "Game.h"
#include "RayCaster.h"

class Renderer
{
    RayCaster* _rc;
    Game*      _g;

    inline static uint32_t GetARGB(uint8_t brightness)
    {
        return (brightness << 16) + (brightness << 8) + brightness;
    }

public:
    void TraceFrame(Game* g, uint32_t* frameBuffer);
    Renderer(RayCaster* rc);
    ~Renderer();
};
