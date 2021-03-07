#pragma once

#include <stdint.h>
#include <vector>
#include <tuple>

#ifdef MSDOS
#define M_PI 3.1415926
#define TABLES_320
#define SCREEN_WIDTH (uint32_t)320
#define SCREEN_HEIGHT (uint32_t)200
#define LOOKUP_TBL
#define LOOKUP8(tbl, offset) tbl[offset]
#define LOOKUP16(tbl, offset) tbl[offset]
#define __LOOKUP8(tbl, offset) pgm_read_byte_near((tbl) + (offset))
#define __LOOKUP16(tbl, offset) pgm_read_word_near((tbl) + (offset))
#endif

#ifdef ARDUINO_AVR_UNO
#include <Arduino.h>
#define TABLES_160
#define SCREEN_WIDTH (uint16_t)160
#define SCREEN_HEIGHT (uint16_t)128
#define LOOKUP_TBL PROGMEM
#define LOOKUP8(tbl, offset) pgm_read_byte_near((tbl) + (offset))
#define LOOKUP16(tbl, offset) pgm_read_word_near((tbl) + (offset))
#endif

#if defined(LINUX) || defined(WIN32)
#define TABLES_320
#define SCREEN_WIDTH (uint16_t)320
#define SCREEN_HEIGHT (uint16_t)256
#define SCREEN_SCALE 2
#define FOV (double)(M_PI / 2)
#define INV_FACTOR (float)(SCREEN_WIDTH * 95.0f / 320.0f)
#define LOOKUP_TBL
#define LOOKUP8(tbl, offset) tbl[offset]
#define LOOKUP16(tbl, offset) tbl[offset]
#endif

#define MAP_X (uint8_t)32
#define MAP_XS (uint8_t)5
#define MAP_Y (uint8_t)32
#define INV_FACTOR_INT ((uint16_t)(SCREEN_WIDTH * 75))
#define MIN_DIST (int)((150 * ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)))
#define HORIZON_HEIGHT (SCREEN_HEIGHT / 2)
#define INVERT(x) (uint8_t)((x ^ 255) + 1)
#define ABS(x) (x < 0 ? -x : x)

class RayCaster
{
public:
    virtual void Start(uint32_t playerX, uint32_t playerY, int32_t playerA) = 0;

    struct BoxHit
    {
        BoxHit(bool isExit, int height) : isExit(isExit), height(height)
        {}
        bool isExit;
        int  height;
    };

    struct TraceHit
    {
        TraceHit(uint32_t screenY, uint8_t textureNo, uint8_t textureX, int hitX, int hitY,
        std::vector<RayCaster::BoxHit> boxHits) : screenY(screenY), textureNo(textureNo),
        textureX(textureX), hitX(hitX), hitY(hitY),
        boxHits(boxHits)
        {}

        uint32_t screenY;
        uint8_t textureNo;
        uint8_t textureX;
        int hitX;
        int hitY;
        std::vector<RayCaster::BoxHit> boxHits;
    };

    virtual std::vector<TraceHit> Trace(uint32_t screenX/*, uint32_t* screenY, uint8_t* textureNo, uint8_t* textureX*//*, uint32_t* textureY, uint32_t* textureStep*/) = 0;

    RayCaster();

    ~RayCaster();
};
