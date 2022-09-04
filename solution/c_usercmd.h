#pragma once
#include "Vector.h"
#include "checksum_crc.h"
class CUserCmd
{
public:
    CUserCmd()
    {
        memset(this, 0, sizeof(*this));
    };
    virtual ~CUserCmd() {};

    CRC32_t GetChecksum(void) const
    {
        CRC32_t crc;
        CRC32_Init(&crc);

        CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
        CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
        CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
        CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
        CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
        CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
        CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
        CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
        CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
        CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
        CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
        CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
        CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
        CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

        CRC32_Final(&crc);
        return crc;
    }

    int     command_number;     // 0x04 For matching server and client commands for debugging
    int     tick_count;         // 0x08 the tick the client created this command
    Vector  viewangles;         // 0x0C Player instantaneous view angles.
    Vector  aimdirection;       // 0x18
    float   forwardmove;        // 0x24
    float   sidemove;           // 0x28
    float   upmove;             // 0x2C
    int     buttons;            // 0x30 Attack button states
    char    impulse;            // 0x34
    int     weaponselect;       // 0x38 Current weapon id
    int     weaponsubtype;      // 0x3C
    int     random_seed;        // 0x40 For shared random functions
    short   mousedx;            // 0x44 mouse accel delta in x from create move
    short   mousedy;            // 0x46 mouse accel delta in y from create move
    bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
    char    pad_0x4C[0x18];     // 0x4C Current sizeof( usercmd ) =  100  = 0x64
};