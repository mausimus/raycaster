// floating-point implementation for testing/comparison

#ifndef MSDOS

#    define _USE_MATH_DEFINES

#    include "RayCasterFloat.h"
#    include <math.h>

bool RayCasterFloat::IsWall(float rayX, float rayY, float rayA, bool* isEdge)
{
    float mapX    = 0;
    float mapY    = 0;
    float offsetX = modff(rayX, &mapX);
    float offsetY = modff(rayY, &mapY);
    int   tileX   = static_cast<int>(mapX);
    int   tileY   = static_cast<int>(mapY);

    if(tileX < 0 || tileY < 0 || tileX >= MAP_X - 1 || tileY >= MAP_Y - 1)
    {
        *isEdge = true;
        return true;
    }
    *isEdge = false;
    return g_map[(tileX >> 3) + (tileY << (MAP_XS - 3))] & (1 << (8 - (tileX & 0x7)));
}

std::vector<std::tuple<float, float, int>>
RayCasterFloat::Distance(float playerX, float playerY, float rayA /*, float* hitOffset, int* hitDirection*/)
{
    std::vector<std::tuple<float, float, int>> hits;
    int                                        numSteps = 0;

    while(rayA < 0)
    {
        rayA += 2.0f * M_PI;
    }
    while(rayA >= 2.0f * M_PI)
    {
        rayA -= 2.0f * M_PI;
    }

    int   tileStepX = 1;
    int   tileStepY = 1;
    float tileX     = 0;
    float tileY     = 0;
    if(rayA > M_PI)
    {
        tileStepX = -1;
    }
    if(rayA > M_PI_2 && rayA < 3 * M_PI_2)
    {
        tileStepY = -1;
    }

    float rayX    = playerX;
    float rayY    = playerY;
    float offsetX = modff(rayX, &tileX);
    float offsetY = modff(rayY, &tileY);

    float startDeltaX, startDeltaY;
    if(rayA <= M_PI_2)
    {
        startDeltaX = (1 - offsetY) * tan(rayA);
        startDeltaY = (1 - offsetX) / tan(rayA);
    }
    else if(rayA <= M_PI)
    {
        if(offsetY == 0)
        {
            startDeltaX = (1) * fabs(tan(rayA));
        }
        else
        {
            startDeltaX = (offsetY)*fabs(tan(rayA));
        }
        startDeltaY = -(1 - offsetX) / fabs(tan(rayA));
    }
    else if(rayA < 3 * M_PI_2)
    {
        if(offsetY == 0)
        {
            startDeltaX = -(1) * fabs(tan(rayA));
        }
        else
        {
            startDeltaX = -(offsetY)*fabs(tan(rayA));
        }
        if(offsetX == 0)
        {
            startDeltaY = -(1) / fabs(tan(rayA));
        }
        else
        {
            startDeltaY = -(offsetX) / fabs(tan(rayA));
        }
    }
    else
    {
        startDeltaX = -(1 - offsetY) * fabs(tan(rayA));
        if(offsetX == 0)
        {
            startDeltaY = (1) / fabs(tan(rayA));
        }
        else
        {
            startDeltaY = (offsetX) / fabs(tan(rayA));
        }
    }

    float interceptX    = rayX + startDeltaX;
    float interceptY    = rayY + startDeltaY;
    float stepX         = fabs(tan(rayA)) * tileStepX;
    float stepY         = fabs(1 / tan(rayA)) * tileStepY;
    bool  verticalHit   = false;
    bool  horizontalHit = false;
    bool  somethingDone = false;
    int   isTop         = 0;
    int   isRight       = 0;
    bool  isEdge        = false;

    do
    {
        somethingDone = false;
        while(((tileStepY == 1 && (interceptY <= tileY + 1)) || (tileStepY == -1 && (interceptY >= tileY))))
        {
            somethingDone = true;
            tileX += tileStepX;
            if(IsWall(tileX, interceptY, rayA, &isEdge))
            {
                if(isEdge)
                    break;

                //verticalHit  = true;
                auto hitRayX = tileX + (tileStepX == -1 ? 1 : 0);
                auto hitRayY = interceptY;
                //*hitOffset    = interceptY;
                //*hitDirection = true;

                float deltaX = hitRayX - playerX;
                float deltaY = hitRayY - playerY;
                auto  dist   = sqrt(deltaX * deltaX + deltaY * deltaY);

                hits.push_back(std::make_tuple(dist, interceptY, 1));

                //              break;
            }
            interceptY += stepY;
        }
        while(!verticalHit && ((tileStepX == 1 && (interceptX <= tileX + 1)) || (tileStepX == -1 && (interceptX >= tileX))))
        {
            somethingDone = true;
            tileY += tileStepY;
            if(IsWall(interceptX, tileY, rayA, &isEdge))
            {
                if(isEdge)
                    break;

                //horizontalHit = true;
                auto hitRayX  = interceptX;
                //*hitOffset    = interceptX;
                //*hitDirection = 0;
                auto hitRayY = tileY + (tileStepY == -1 ? 1 : 0);

                float deltaX = hitRayX - playerX;
                float deltaY = hitRayY - playerY;
                auto  dist   = sqrt(deltaX * deltaX + deltaY * deltaY);

                hits.push_back(std::make_tuple(dist, interceptX, 0));
                //                break;
            }
            interceptX += stepX;
        }
    } while(!isEdge && numSteps++ < 15 /*(!horizontalHit && !verticalHit) && somethingDone*/);

    /*if(!somethingDone)
    {
        return hits;
    }*/

    /*float deltaX = rayX - playerX;
    float deltaY = rayY - playerY;
    return sqrt(deltaX * deltaX + deltaY * deltaY);*/

    return hits;
}

std::vector<std::tuple<uint32_t, uint8_t, uint8_t>> RayCasterFloat::Trace(
    uint32_t screenX /*, uint32_t* screenY, uint8_t* textureNo, uint8_t* textureX*/ /*, uint32_t* textureY, uint32_t* textureStep*/)
{
    std::vector<std::tuple<uint32_t, uint8_t, uint8_t>> traces;

    float deltaAngle = atanf(((int16_t)screenX - SCREEN_WIDTH / 2.0f) / (SCREEN_WIDTH / 2.0f) * M_PI / 4);

    auto hits = Distance(_playerX, _playerY, _playerA + deltaAngle);
    if(!hits.size())
    {
        //screenY = 0;
        return traces;
    }

    for(auto hit : hits)
    {
//        auto  firstHit     = *hits.begin();
        float lineDistance = std::get<0>(hit);
        float hitOffset    = std::get<1>(hit);
        int   hitDirection = std::get<2>(hit);
        uint32_t screenY;

        float distance = lineDistance * cos(deltaAngle);
        float dum;
        uint8_t textureX  = (uint8_t)(256.0f * modff(hitOffset, &dum));
        //*textureNo = hitDirection;
        if(distance > 0)
        {
            screenY = 256 * (INV_FACTOR / distance);
        }
        else
        {
            screenY = 0;
        }

        traces.push_back(std::make_tuple(screenY, hitDirection, textureX));
    }

    return traces;
}

void RayCasterFloat::Start(uint32_t playerX, uint32_t playerY, int32_t playerA)
{
    _playerX = (playerX / 1024.0f) * 4.0f;
    _playerY = (playerY / 1024.0f) * 4.0f;
    _playerA = (playerA / 1024.0f) * 2.0f * M_PI;
}

RayCasterFloat::RayCasterFloat() : RayCaster() { }

RayCasterFloat::~RayCasterFloat() { }
#endif