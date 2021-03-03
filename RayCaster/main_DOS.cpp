#ifdef MSDOS

#include "RayCaster.h"
#include "RayCasterData.h"
#include "RayCasterFixed.h"
#include "stdio.h"
#include <dos.h>
#include <bios.h>
#include <dpmi.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <go32.h>
#include <pc.h>
#include <sys/nearptr.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <cstring>
#include <time.h>

uint8_t* vgamemory;
uint8_t* vgabuffer;

int init_vga_mode(void)
{
    union REGS regs;
    __djgpp_nearptr_enable();
    vgamemory = (uint8_t*)(0xA0000 + __djgpp_conventional_base);
    if((vgabuffer = (uint8_t*)malloc(320 * 200)) == NULL)
    {
        return (1);
    }
    regs.h.ah = 0x00; //Procedure 0x00: Set Video Mode
    regs.h.al = 0x13; //The mode to set (0x# = #h)
    int86(0x10, &regs, &regs); //Call BIOS intr 0x10
    return (0);
}

void done_vga_mode(void)
{
    union REGS regs;
    regs.h.ah = 0x00; //Procedure 0x00: Set Video Mode
    regs.h.al = 0x03; //The mode to set (0x# = #h)
    int86(0x10, &regs, &regs); //Call BIOS intr 0x10
    free(vgabuffer);
    __djgpp_nearptr_disable();
}

void set_color(uint8_t color, uint8_t red, uint8_t green, uint8_t blue)
{
    outportb(0x3C8, color);
    outportb(0x3C9, red);
    outportb(0x3C9, green);
    outportb(0x3C9, blue);
}

int32_t  playerA = 0;
uint32_t playerX = 23 << 8;
uint32_t playerY = 7 << 8;
/*
RayCasterFixed rayCaster;

void dos_draw()
{
    rayCaster.Start(playerX, playerY, playerA);

    uint8_t  wallHeight;
    uint8_t  textureX;
    uint8_t  textureNo;
    uint32_t textureY;
    uint32_t textureStep;

    memset(vgabuffer, 120, 32000); // sky
    memset(vgabuffer + 32000, 80, 32000); // earth

    for(int x = SCREEN_WIDTH - 1; x >= 0; x--)
    {
        uint8_t* lb = vgabuffer + x;
        rayCaster.Trace(x, &wallHeight, &textureNo, &textureX, &textureY, &textureStep);

        const auto tx        = static_cast<int>(textureX >> 2);
        int32_t    wallStart = HORIZON_HEIGHT - wallHeight;
        if(wallStart < 0)
        {
            wallStart  = 0;
            wallHeight = HORIZON_HEIGHT;
        }
        uint32_t       to = textureY >> 2;
        const uint32_t ts = textureStep >> 2;

        lb += wallStart * SCREEN_WIDTH;

        const auto textureColumn = g_texture8 + (tx << 6) + major(to);
        const auto endCond       = ((wallHeight - 1) * ts << 1);
        for(to = endCond; to > 0; to -= ts)
        {
            *lb = textureColumn[major(to)]; // >> 8
            lb += SCREEN_WIDTH;
        }
    }

    memcpy(vgamemory, vgabuffer, 64000);
}
*/
int main(void)
{
    int i;
    printf("Hello 386+!\n");

    init_vga_mode();

    for(i = 0; i <= 255; i++)
    {
        set_color(i, i / 4, i / 4, i / 4);
    }

    uclock_t startTime = uclock();
/*    int      c         = 0;
    for(c = 0; c < 100; c++)
    {
        dos_draw();

        playerA += 10;
        if(playerA > 1024)
        {
            playerA -= 1024;
        }
    }*/
    uclock_t endTime = uclock();
    done_vga_mode();
    const auto timeElapsed = 1000 * (endTime - startTime) / UCLOCKS_PER_SEC;
    printf("Done in %d milliseconds\n", timeElapsed);
    printf("%d fps", 100000 / timeElapsed);
    return 0;
}
#endif
