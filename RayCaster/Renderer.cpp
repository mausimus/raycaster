#define _USE_MATH_DEFINES

#include <math.h>
#include <cstring>
#include "Renderer.h"
#include "RayCasterData.h"

void Renderer::PaintFloor(int x, int startY, int endY, uint32_t* frameBuffer)
{
    if(startY > endY)
    {
        auto temp = startY;
        startY    = endY;
        endY      = temp;
    }

    if(startY < 0)
        startY = 0;

    if(endY >= SCREEN_HEIGHT)
        endY = SCREEN_HEIGHT - 1;

    while(startY <= endY)
    {
        frameBuffer[x + SCREEN_WIDTH * startY] = 0x22222222;
        startY++;
    }
}

void Renderer::PaintWall(int x, int centerLine, uint32_t wallHeight, uint8_t textureNo, uint8_t textureX, uint32_t* frameBuffer)
{
    if(wallHeight <= 0)
    {
        return;
    }

    uint32_t* lineBuffer  = frameBuffer + x;
    uint32_t  textureY    = 0;
    int       visibleSize = wallHeight; // size of wall after screen clipping
    uint32_t  textureStep;
    uint32_t  topOffset; // wall offset from the top

    textureStep = ((256 * 256) / wallHeight);

    // trim wall to visible
    auto textureTop    = centerLine - (int)(wallHeight / 2);
    auto textureBottom = centerLine + (int)(wallHeight / 2);
    if(textureTop < 0)
    {
        topOffset = 0;
        visibleSize += textureTop;
        textureY = -textureTop * textureStep;
    }
    else
    {
        topOffset = textureTop;
    }
    if(textureBottom >= SCREEN_HEIGHT)
    {
        visibleSize -= (textureBottom - SCREEN_HEIGHT);
    }

    const auto texelX     = static_cast<int>(textureX >> 2);
    uint32_t   texturePos = textureY;

    // paint wall
    lineBuffer += SCREEN_WIDTH * topOffset;
    //auto visibleSize = screenY * 2;
    for(int y = 0; y < visibleSize; y++)
    {
        // paint texture pixel
        auto texelY     = static_cast<int>(texturePos >> 10);
        auto texelValue = g_texture8[((texelY << 6) + texelX) % 4096];

        texturePos += textureStep;

        if(textureNo == 1 && texelValue > 0)
        {
            // dark wall
            texelValue >>= 1;
        }
        *lineBuffer = GetARGB(texelValue);
        lineBuffer += SCREEN_WIDTH;
    }
}

// paint wall and connect floor/ceiling
void Renderer::PaintLevel(int x, float verticalOffset, const RayCaster::TraceHit& hit, bool isExit, uint32_t* frameBuffer, int* previousPoint)
{
    uint32_t screenY   = hit.screenY;
    uint8_t  textureNo = hit.textureNo;
    uint8_t  textureX  = hit.textureX;

    auto direction  = verticalOffset >= 0 ? 1 : -1;
    auto centerLine = static_cast<int>(HORIZON_HEIGHT + ((2 * verticalOffset * (int)screenY) / 256.0f));
    auto pointTop   = centerLine - direction * (((int)screenY * 2) / 256) / 2;
    if(*previousPoint != -1)
    {
        PaintFloor(x, pointTop, *previousPoint, frameBuffer);
    }

    if(/*hit.*/isExit)
    {
        *previousPoint = pointTop;
    }
    else
    {
        *previousPoint = -1;
    }
    PaintWall(x, HORIZON_HEIGHT + ((2 * verticalOffset * (int)screenY) / 256), (screenY * 2) >> 8, textureNo, textureX, frameBuffer);
}

void Renderer::TraceFrame(Game* g, uint32_t* frameBuffer)
{
    // clear frame
    memset(frameBuffer, 128, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));

    _rc->Start(static_cast<uint32_t>(g->playerX * 256.0f),
               static_cast<uint32_t>(g->playerY * 256.0f),
               static_cast<int32_t>(g->playerA / (2.0f * M_PI) * 1024.0f));

    float baseOffset = g->playerZ * 32;

    for(int x = 0; x < SCREEN_WIDTH; x++)
    {
        auto traces = _rc->Trace(x);

        // floor/ceiling tracking per level
        /*int drawLevel[5];
        drawLevel[0] = drawLevel[1] = drawLevel[2] = drawLevel[3] = drawLevel[4] = -1;
        int* centerLevel = drawLevel + 2;*/
        int centerLevel[10];
        for(int cl = 0; cl < 10; cl++)
            centerLevel[cl] = -1;

        for(auto it = traces.rbegin(); it != traces.rend(); it++)
        {
            uint32_t screenY   = it->screenY;
            uint8_t  textureNo = it->textureNo;
            uint8_t  textureX  = it->textureX;

            //PaintLevel(x, baseOffset + 2, *it, false, frameBuffer, centerLevel + 2);
            //PaintLevel(x, baseOffset - 2, *it, frameBuffer, centerLevel - 2);
            if(it->boxHits.size() == 2)
            {
                int z = 5; z++;
            }
            for(const auto& bh: it->boxHits)
            {
                float blockOffset = baseOffset - bh.height;
                PaintLevel(x, blockOffset, *it, bh.isExit, frameBuffer, centerLevel + bh.height);
            }
        }
    }
}

Renderer::Renderer(RayCaster* rc)
{
    _rc = rc;
}

Renderer::~Renderer() { }

// paint floor/ground
/*
        for(int y = 0; y < topOffset; y++)
        {
            *lineBuffer = GetARGB(96 + (HORIZON_HEIGHT - (topOffset - y)));
            lineBuffer += SCREEN_WIDTH;
        }*/
// paint sky/ceiling
/*
        for(int y = 0; y < topOffset; y++)
        {
            *lineBuffer = GetARGB(96 + (HORIZON_HEIGHT - y));
            lineBuffer += SCREEN_WIDTH;
        }*/
