#pragma once
#if(FALSE)
#include "RayCaster.h"
class RayCasterFixed : public RayCaster
{
public:
    void Start(uint32_t playerX, uint32_t playerY, int32_t playerA);
    void Trace(uint32_t screenX, uint32_t* screenY, uint8_t* textureNo, uint8_t* textureX, uint32_t* textureY, uint32_t* textureStep);

    RayCasterFixed();
    ~RayCasterFixed();

private:
    uint32_t _playerX;
    uint32_t _playerY;
    int32_t  _playerA;
    uint8_t  _viewQuarter;
    uint8_t  _viewAngle;

    static void     CalculateDistance(uint32_t rayX, uint32_t rayY, uint32_t rayA, int32_t* deltaX, int32_t* deltaY, uint8_t* textureNo, uint8_t* textureX);
    static void     LookupHeight(uint32_t distance, uint8_t* height, uint32_t* step);
    static bool     IsWall(uint8_t tileX, uint8_t tileY);
    static int32_t  MulTan(uint8_t value, bool inverse, uint8_t quarter, uint8_t angle, const uint16_t* lookupTable);
    static int32_t  AbsTan(uint8_t quarter, uint8_t angle, const uint16_t* lookupTable);
    static uint32_t MulU(uint8_t v, uint32_t f);
    static int32_t  MulS(uint8_t v, int32_t f);
};
#endif