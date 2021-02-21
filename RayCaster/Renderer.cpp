#define _USE_MATH_DEFINES

#include <math.h>
#include "Renderer.h"
#include "RayCasterData.h"

void Renderer::TraceFrame(Game* g, uint32_t* fb)
{
    _rc->Start(static_cast<uint32_t>(g->playerX * 256.0f),
               static_cast<uint32_t>(g->playerY * 256.0f),
               static_cast<int32_t>(g->playerA / (2.0f * M_PI) * 1024.0f));

    for(int x = 0; x < SCREEN_WIDTH; x++)
    {
        uint8_t   sso;
        uint8_t   tc;
        uint8_t   tn;
        uint32_t  tso;
        uint32_t  tst;
        uint32_t* lb = fb + x;

        _rc->Trace(x, &sso, &tn, &tc, &tso, &tst);

        const auto tx = static_cast<int>(tc >> 2);
        int32_t    ws = HORIZON_HEIGHT - sso;
        if(ws < 0)
        {
            ws  = 0;
            sso = HORIZON_HEIGHT;
        }
        uint32_t to = tso;
        uint32_t ts = tst;

        for(int y = 0; y < ws; y++)
        {
            *lb = GetARGB(96 + (HORIZON_HEIGHT - y));
            lb += SCREEN_WIDTH;
        }

        for(int y = 0; y < sso * 2; y++)
        {
            // paint texture pixel
            auto ty = static_cast<int>(to >> 10);
            auto tv = g_texture8[((ty << 6) + tx) % 4096];

            to += ts;

            if(tn == 1 && tv > 0)
            {
                // dark wall
                tv >>= 1;
            }
            *lb = GetARGB(tv);
            lb += SCREEN_WIDTH;
        }

        for(int y = 0; y < ws; y++)
        {
            *lb = GetARGB(96 + (HORIZON_HEIGHT - (ws - y)));
            lb += SCREEN_WIDTH;
        }
    }
}

Renderer::Renderer(RayCaster* rc)
{
    _rc = rc;
}

Renderer::~Renderer() {}
