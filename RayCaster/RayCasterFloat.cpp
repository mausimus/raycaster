// floating-point implementation for testing/comparison

#ifndef MSDOS

#define _USE_MATH_DEFINES

#include "RayCasterFloat.h"
#include <math.h>

RayCasterFloat::MapColumn s_map[MAP_X][MAP_Y];

void RayCasterFloat::InitializeMap()
{
    srand(432789);
    for(int x = 0; x < MAP_X; x++)
        for(int y = 0; y < MAP_Y; y++)
        {
            s_map[x][y] = MapColumn();

            /*if(rand()%2 == 0)
            {
                MapBlock floor1;
                floor1.textureNo = 0;
                floor1.height = 1;
                s_map[x][y].blocks.push_back(floor1);
            }*/

            MapBlock b;
            b.textureNo = 1;
            b.height    = 1; //rand() % 4;
            if(x <= 1 || y <= 1 || x >= MAP_X - 2 || y >= MAP_Y - 2)
            {
                s_map[x][y].blocks.push_back(b);
            }
            else
            {
                MapBlock floor;
                floor.textureNo = 0;
                floor.height    = 0;
                s_map[x][y].blocks.push_back(floor);
                if(x == 3 && y == 3)
                {
                    s_map[x][y].blocks.push_back(b);

                    MapBlock b1;
                    b1.textureNo = 1;
                    b1.height    = 2; //rand() % 4;
                    s_map[x][y].blocks.push_back(b1);

                    MapBlock b2;
                    b2.textureNo = 1;
                    b2.height    = 3; //rand() % 4;
                    s_map[x][y].blocks.push_back(b2);
                }
                else if(x == 5 && y == 3)
                {
                    s_map[x][y].blocks.push_back(b);

                    /*MapBlock b1;
                b1.textureNo = 1;
                b1.height    = 1; //rand() % 4;
                s_map[x][y].blocks.push_back(b1);*/

                    MapBlock b2;
                    b2.textureNo = 1;
                    b2.height    = 3; //rand() % 4;
                    s_map[x][y].blocks.push_back(b2);
                }
                else if(x >= 8 && x <= 12)
                {
                    switch(y)
                    {
                    case 4: {
                        MapBlock b2;
                        b2.textureNo = 1;
                        b2.height    = 2;
                        s_map[x][y].blocks.push_back(b2);
                        break;
                    }
                    case 3:
                    case 5:
                        MapBlock b2;
                        b2.textureNo = 1;
                        b2.height    = 1;
                        s_map[x][y].blocks.push_back(b2);
                        break;
                    }
                }
            }
            //s_map[x][y].blocks.push_back(b);
        }
}

RayCasterFloat::MapColumn* RayCasterFloat::IsWall(float rayX, float rayY, float rayA, bool* isEdge)
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
        return nullptr;
    }
    *isEdge = false;

    RayCasterFloat::MapColumn* hitCol = &s_map[tileX][tileY];
    if(hitCol->blocks.empty())
        return nullptr;

    return hitCol;
    //return g_map[(tileX >> 3) + (tileY << (MAP_XS - 3))] & (1 << (8 - (tileX & 0x7)));
}

std::vector<RayCasterFloat::BoxHit>
RayCasterFloat::FindBoxHits(RayCasterFloat::MapColumn* currentColumn, RayCasterFloat::MapColumn* prevColumn, bool& hadHit)
{
    std::vector<BoxHit> boxHits;
    int                 curr_idx = 0, prev_idx = 0;
    while(curr_idx < currentColumn->blocks.size() || prev_idx < prevColumn->blocks.size())
    {
        if(curr_idx == currentColumn->blocks.size())
        {
            // we only have previous, meaning it's an exit
            boxHits.push_back(RayCasterFloat::BoxHit(true, prevColumn->blocks[prev_idx].height));
            prev_idx++;
        }
        else if(prev_idx == prevColumn->blocks.size())
        {
            // we only have current, meaning it's a new wall
            boxHits.push_back(RayCasterFloat::BoxHit(false, currentColumn->blocks[curr_idx].height));
            hadHit = true;
            curr_idx++;
        }
        else if(currentColumn->blocks[curr_idx].height > prevColumn->blocks[prev_idx].height)
        {
            boxHits.push_back(RayCasterFloat::BoxHit(true, prevColumn->blocks[prev_idx].height));
            prev_idx++;
        }
        else if(currentColumn->blocks[curr_idx].height < prevColumn->blocks[prev_idx].height)
        {
            boxHits.push_back(RayCasterFloat::BoxHit(false, currentColumn->blocks[curr_idx].height));
            hadHit = true;
            curr_idx++;
        }
        else
        {
            // still in a wall - do nothing
            curr_idx++;
            prev_idx++;
            hadHit = true;
        }
    }
    return boxHits;
}

std::vector<RayCasterFloat::DistanceHit> RayCasterFloat::Distance(float playerX, float playerY, float rayA)
{
    std::vector<RayCasterFloat::DistanceHit> hits;
    int                                      numSteps = 0;

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

    float                      interceptX    = rayX + startDeltaX;
    float                      interceptY    = rayY + startDeltaY;
    float                      stepX         = fabs(tan(rayA)) * tileStepX;
    float                      stepY         = fabs(1 / tan(rayA)) * tileStepY;
    bool                       verticalHit   = false;
    bool                       horizontalHit = false;
    bool                       somethingDone = false;
    int                        isTop         = 0;
    int                        isRight       = 0;
    bool                       isEdge        = false;
    bool                       hadHit        = false;
    RayCasterFloat::MapColumn  emptyCol;
    RayCasterFloat::MapColumn* prevColumn = &emptyCol;

    do
    {
        somethingDone = false;
        while(((tileStepY == 1 && (interceptY <= tileY + 1)) || (tileStepY == -1 && (interceptY >= tileY))))
        {
            somethingDone = true;
            tileX += tileStepX;
            //auto isWall = IsWall(tileX, interceptY, rayA, &isEdge);
            auto currentColumn = IsWall(tileX, interceptY, rayA, &isEdge);
            if(currentColumn != nullptr || hadHit || isEdge)
            {
                if(isEdge)
                    break;
                //if(!(isWall && hadHit))
                {
                    if(currentColumn == nullptr)
                        currentColumn = &emptyCol;

                    //verticalHit  = true;
                    auto hitRayX = tileX + (tileStepX == -1 ? 1 : 0);
                    auto hitRayY = interceptY;

                    float deltaX = hitRayX - playerX;
                    float deltaY = hitRayY - playerY;
                    auto  dist   = sqrt(deltaX * deltaX + deltaY * deltaY);

                    auto boxHits = FindBoxHits(currentColumn, prevColumn, hadHit);

                    DistanceHit dh(dist, interceptY, 1, (int)hitRayX, (int)hitRayY, boxHits);
                    hits.push_back(dh);
                    //hadHit = isWall;
                    prevColumn = currentColumn;
                }
            }
            interceptY += stepY;
        }
        while(!verticalHit && ((tileStepX == 1 && (interceptX <= tileX + 1)) || (tileStepX == -1 && (interceptX >= tileX))))
        {
            somethingDone = true;
            tileY += tileStepY;
            auto currentColumn = IsWall(interceptX, tileY, rayA, &isEdge);
            if(currentColumn != nullptr || hadHit || isEdge)
            {
                if(isEdge)
                    break;
                //if(!(isWall && hadHit))
                {
                    if(currentColumn == nullptr)
                        currentColumn = &emptyCol;

                    //horizontalHit = true;
                    auto hitRayX = interceptX;
                    auto hitRayY = tileY + (tileStepY == -1 ? 1 : 0);

                    float deltaX = hitRayX - playerX;
                    float deltaY = hitRayY - playerY;
                    auto  dist   = sqrt(deltaX * deltaX + deltaY * deltaY);

                    auto boxHits = FindBoxHits(currentColumn, prevColumn, hadHit);

                    hits.push_back(DistanceHit(dist, interceptX, 0, (int)hitRayX, (int)hitRayY, boxHits));
                    //hadHit = isWall;
                    prevColumn = currentColumn;

                    /*hits.push_back(DistanceHit(dist, interceptX, 0, hadHit && !isWall));
                    hadHit = isWall;*/
                }
            }
            interceptX += stepX;
        }
    } while(!isEdge && numSteps++ < 25);

    return hits;
}

std::vector<RayCaster::TraceHit> RayCasterFloat::Trace(uint32_t screenX)
{
    std::vector<RayCaster::TraceHit> traces;

    float deltaAngle = atanf(((int16_t)screenX - SCREEN_WIDTH / 2.0f) / (SCREEN_WIDTH / 2.0f) * M_PI / 4);

    auto hits = Distance(_playerX, _playerY, _playerA + deltaAngle);
    if(!hits.size())
    {
        return traces;
    }

    for(auto hit : hits)
    {
        float    lineDistance = hit.distance;
        float    hitOffset    = hit.hitOffset;
        int      hitDirection = hit.hitDirection;
        uint32_t screenY;

        float   distance = lineDistance * cos(deltaAngle);
        float   dum;
        uint8_t textureX = (uint8_t)(256.0f * modff(hitOffset, &dum));
        if(distance > 0)
        {
            screenY = 256 * (INV_FACTOR / distance);
        }
        else
        {
            screenY = 0;
        }

        traces.push_back(RayCaster::TraceHit(screenY, hitDirection, textureX, hit.hitX, hit.hitY, hit.hits));
    }

    return traces;
}

void RayCasterFloat::Start(uint32_t playerX, uint32_t playerY, int32_t playerA)
{
    _playerX = (playerX / 1024.0f) * 4.0f;
    _playerY = (playerY / 1024.0f) * 4.0f;
    _playerA = (playerA / 1024.0f) * 2.0f * M_PI;

    InitializeMap();
}

RayCasterFloat::RayCasterFloat() : RayCaster() { }

RayCasterFloat::~RayCasterFloat() { }
#endif