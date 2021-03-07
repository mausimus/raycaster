#pragma once
#include <vector>
#include <tuple>

#include "RayCaster.h"
#include "RayCasterData.h"

class RayCasterFloat : public RayCaster
{
public:
    void                             Start(uint32_t playerX, uint32_t playerY, int32_t playerA);
    std::vector<RayCaster::TraceHit> Trace(uint32_t screenX);

    struct MapBlock
    {
        int textureNo;
        int height;
    };

    struct MapColumn
    {
        std::vector<MapBlock> blocks;
    };

    RayCasterFloat();
    ~RayCasterFloat();

private:
    float _playerX;
    float _playerY;
    float _playerA;

    struct DistanceHit
    {
        DistanceHit(float distance, float hitOffset, int hitDirection, int hitX, int hitY, std::vector<BoxHit> hits) :
            distance(distance), hitOffset(hitOffset), hitDirection(hitDirection), hitX(hitX), hitY(hitY), hits(hits)
        { }
        float distance;
        float hitOffset;
        int   hitDirection;
        int hitX;
        int hitY;

        std::vector<BoxHit> hits;
    };

    std::vector<DistanceHit> Distance(float playerX, float playerY, float rayA);
    std::vector<RayCasterFloat::BoxHit> FindBoxHits(RayCasterFloat::MapColumn* currentColumn,
                                                               RayCasterFloat::MapColumn* prevColumn, bool &hadHit);
    MapColumn*               IsWall(float rayX, float rayY, float rayA, bool* isEdge);
    void                     InitializeMap();
};
