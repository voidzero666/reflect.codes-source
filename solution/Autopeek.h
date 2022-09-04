#pragma once
#include "Hooks.h"

class CAutopeek : public Singleton<CAutopeek> {
public:
    Vector quickpeekstartpos;
    bool has_shot;

    void Reset() {
        quickpeekstartpos = Vector{ 0, 0, 0 };
        has_shot = false;
    }
    void GotoStart(CUserCmd* cur_cmd = nullptr);
    void Draw();
    void Run();
};