#pragma once
#include <vector>
#include <tuple>

#include "RayCaster.h"
#include "RayCasterData.h"

class RayCasterFloat : public RayCaster
{
public:
    void Start(uint32_t playerX, uint32_t playerY, int32_t playerA);
    std::vector<RayCaster::TraceHit> Trace(uint32_t screenX);

    RayCasterFloat();
    ~RayCasterFloat();

private:
    float _playerX;
    float _playerY;
    float _playerA;

    struct DistanceHit {
        DistanceHit(float distance, float hitOffset, int hitDirection, bool isExit) :
         distance(distance), hitOffset(hitOffset), hitDirection(hitDirection), isExit(isExit)
        {
        }
        float distance;
        float hitOffset;
        int hitDirection;
        bool isExit;
    };

    std::vector<DistanceHit> Distance(float playerX, float playerY, float rayA);
    bool  IsWall(float rayX, float rayY, float rayA, bool *isEdge);
};
