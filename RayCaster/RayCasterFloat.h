#pragma once
#include <vector>
#include <tuple>

#include "RayCaster.h"
#include "RayCasterData.h"

class RayCasterFloat : public RayCaster
{
public:
    void Start(uint32_t playerX, uint32_t playerY, int32_t playerA);
    std::vector<std::tuple<uint32_t, uint8_t, uint8_t>> Trace(uint32_t screenX/*, uint32_t* screenY, uint8_t* textureNo, uint8_t* textureX*//*, uint32_t* textureY, uint32_t* textureStep*/);

    RayCasterFloat();
    ~RayCasterFloat();

private:
    float _playerX;
    float _playerY;
    float _playerA;

    std::vector<std::tuple<float, float, int>> Distance(float playerX, float playerY, float rayA/*, float* hitOffset, int* hitDirection*/);
    bool  IsWall(float rayX, float rayY, float rayA, bool *isEdge);
};
