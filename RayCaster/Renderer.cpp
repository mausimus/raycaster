#define _USE_MATH_DEFINES

#include <math.h>
#include <cstring>
#include <unistd.h>
#include "Renderer.h"
#include "RayCasterData.h"

void Renderer::PaintFloor(int x, int startY, int endY, uint32_t* frameBuffer, int col)
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
        frameBuffer[x + SCREEN_WIDTH * startY] = (0x00004f00) >> col;//= 0x22222222;
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

    const auto texelX     = static_cast<int>(textureX >> 2) >> 2;
    uint32_t   texturePos = textureY;

    auto startPixel = (int)topOffset;
    auto endPixel = (int)(topOffset + visibleSize);
    if(!TrimPaint(&startPixel, &endPixel))
    {
        return;
    }

    // paint wall
    lineBuffer += SCREEN_WIDTH * topOffset;
    //auto visibleSize = screenY * 2;
    for(int y = 0; y < visibleSize; y++)
    {
        auto currentPixel = topOffset + y;

        // paint texture pixel
        auto texelY     = static_cast<int>(texturePos >> 10) >> 2;
        auto texelValue = g_texture8[((texelY << 6) + texelX) % 4096];

        texturePos += textureStep;

        if(textureNo == 1 && texelValue > 0)
        {
            // dark wall
            texelValue >>= 1;
        }
        if(currentPixel >= startPixel && currentPixel <= endPixel)
        {
            *lineBuffer = GetARGB(texelValue);
        }
        lineBuffer += SCREEN_WIDTH;
    }
}

int zbuffer[SCREEN_HEIGHT];

// trim draw area to only areas not drawn before
bool Renderer::TrimPaint(int* sy, int* ey)
{
    //return;

    if(*sy > *ey)
    {
        return TrimPaint(ey, sy);
    }

    if(*sy == *ey)
    {
        // TODO: handle no drawing at all
        //zbuffer[*sy] = 1;
        return false;
    }

    if(*sy < 0)
        *sy = 0;

    if(*ey < 0)
        *ey = 0;

    if(*ey >= SCREEN_HEIGHT)
        *ey = SCREEN_HEIGHT - 1;

    if(*sy >= SCREEN_HEIGHT)
        *sy = SCREEN_HEIGHT - 1;

    if(*sy == *ey)
    {
        // TODO: handle no drawing at all
        //zbuffer[*sy] = 1;
        return false;
    }

    while(zbuffer[*sy] == 1 && *sy < SCREEN_HEIGHT && *sy < *ey)
    {        
        *sy = *sy + 1;
    }
    while(zbuffer[*ey] == 1 && *ey >= 0 && *ey > *sy)
    {
        *ey = *ey - 1;
    }

    if(*sy == *ey)
    {
        // TODO: handle no drawing at all
        //zbuffer[*sy] = 1;
        return false;
    }

    for(int zy = *sy; zy <= *ey; zy++)
    {
        zbuffer[zy] = 1;
    }

    return true;
}

// paint wall and connect floor/ceiling
void Renderer::PaintLevel(
    int x, float verticalOffset, const RayCaster::TraceHit& hit, bool isExit, uint32_t* frameBuffer, int* previousPoint)
{
    uint32_t screenY   = hit.screenY;
    uint8_t  textureNo = hit.textureNo;
    uint8_t  textureX  = hit.textureX;

    auto direction  = verticalOffset >= 0 ? 1 : -1;
    auto centerLine = static_cast<int>(round(HORIZON_HEIGHT + ((2 * verticalOffset * (int)screenY) / 256.0f)));
    auto pointTop    = centerLine - direction * (((int)screenY * 2) / 256) / 2;
    auto pointBottom   = centerLine + direction * (((int)screenY * 2) / 256) / 2;

    if(*previousPoint != -1)
    {
        // fills in from previousPoint to pointTop
        if(TrimPaint(&pointTop, previousPoint))
        {
            PaintFloor(x, pointTop, *previousPoint, frameBuffer, 0/*(hit.hitX + hit.hitY) % 2*/);
        }
    }

    // fills in center +/- half height
    //auto centerLine = HORIZON_HEIGHT + ((2 * verticalOffset * (int)screenY) / 256);
    auto wallHeight = (screenY * 2) >> 8;
    PaintWall(x, centerLine, wallHeight, textureNo, textureX, frameBuffer);

    if(!isExit)
    {
        *previousPoint = pointTop;
    }
    else
    {
        *previousPoint = -1;
    }
}

int framePaints = 5;

void Renderer::TraceFrame(Game* g, uint32_t* frameBuffer)
{
    // clear frame
    memset(frameBuffer, 128, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));

    _rc->Start(static_cast<uint32_t>(g->playerX * 256.0f),
               static_cast<uint32_t>(g->playerY * 256.0f),
               static_cast<int32_t>(g->playerA / (2.0f * M_PI) * 1024.0f));

    float baseOffset = g->playerZ * 32;
    framePaints++;
    if(framePaints > 10)
        framePaints = 0;

    for(int x = 0; x < SCREEN_WIDTH; x++)
    {
        int maxPaints = framePaints;

        for(int y = 0; y < SCREEN_HEIGHT; y++)
            zbuffer[y] = 0;

        auto traces = _rc->Trace(x);

        int centerLevel[20];
        for(int cl = 0; cl < 10; cl++)
            centerLevel[cl] = -1;

        for(auto it = traces.begin(); it != traces.end(); it++)
        {
            uint32_t screenY   = it->screenY;
            uint8_t  textureNo = it->textureNo;
            uint8_t  textureX  = it->textureX;

            if(it->boxHits.size() == 2)
            {
                int z = 5;
                z++;
            }
            for(const auto& bh : it->boxHits)
            {
                float blockOffset = baseOffset - bh.height;
                //if(maxPaints--)
                    PaintLevel(x, blockOffset, *it, bh.isExit, frameBuffer, centerLevel + bh.height);
            }
        }
    }

    //sleep(1);
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
