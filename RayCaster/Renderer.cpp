#define _USE_MATH_DEFINES

#include <math.h>
#include "Renderer.h"
#include "RayCasterData.h"

void Renderer::TraceFrame(Game* g, unsigned char* fb)
{
    _rc->Start(static_cast<uint16_t>(g->px * 256.0f),
               static_cast<uint16_t>(g->py * 256.0f),
               static_cast<int16_t>(g->pa / (2.0f * M_PI) * 1024.0f));

    for(int x = 0; x < SCREEN_WIDTH; x++)
    {
        uint8_t        sso;
        uint8_t        tc;
        uint8_t        tn;
        uint16_t       tso;
        uint16_t       tst;
        unsigned char* lb = fb + x;

        _rc->Trace(x, &sso, &tn, &tc, &tso, &tst);

        int16_t ws = HORIZON_HEIGHT - sso;
        if(ws < 0)
        {
            ws  = 0;
            sso = HORIZON_HEIGHT;
        }
        uint16_t to = tso;
        uint16_t ts = tst;

        for(int y = 0; y < ws; y++)
        {
            *lb = 96 + (HORIZON_HEIGHT - y);
            lb += SCREEN_WIDTH;
        }

        for(int y = 0; y < sso * 2; y++)
        {
            // paint texture pixel
            auto ty = static_cast<int>(to >> 10);
            auto tx = static_cast<int>(tc >> 2);
            auto tv = _tex256[(ty << 6) + tx];

            to += ts;

            if(tn == 1 && tv > 0)
            {
                // dark wall
                tv >>= 1;
            }
            *lb = tv;
            lb += SCREEN_WIDTH;
        }

        for(int y = 0; y < ws; y++)
        {
            *lb = 96 + (HORIZON_HEIGHT - (ws - y));
            lb += SCREEN_WIDTH;
        }
    }
}

Renderer::Renderer(RayCaster* rc)
{
    _rc = rc;
}

Renderer::~Renderer() {}
