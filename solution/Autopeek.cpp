#include "Autopeek.h"
#include "Ragebot.h"

void CAutopeek::GotoStart(CUserCmd* cur_cmd)
{
    if (!cur_cmd) {
        cur_cmd = csgo->cmd;
    }

    float wish_yaw = csgo->original.y;
    auto difference = csgo->local->GetRenderOrigin() - quickpeekstartpos;

    if (difference.Length2D() > 5.0f)
    {
        auto velocity = Vector(
            difference.x * cos(wish_yaw / 180.0f * PI) + difference.y * sin(wish_yaw / 180.0f * PI),
            difference.y * cos(wish_yaw / 180.0f * PI) - difference.x * sin(wish_yaw / 180.0f * PI),
            difference.z);

        cur_cmd->forwardmove = -velocity.x * 20.f;
        cur_cmd->sidemove = velocity.y * 20.f;
    }
    else {
        Reset();
    }
}

void CAutopeek::Draw()
{
    if (!csgo->is_local_alive)
        return;

    if (!vars.misc.peek_assist.properstate())
        return;

    if (quickpeekstartpos != Vector{ 0, 0, 0 })
    {
        static constexpr float Step = PI * 2.0f / 20;
        std::vector<ImVec2> points;
        for (float lat = 0.f; lat <= PI * 2.0f; lat += Step)
        {
            const auto& point3d = Vector(sin(lat), cos(lat), 0.f) * 15.f;
            Vector point2d;
            if (Math::WorldToScreen(quickpeekstartpos + point3d, point2d))
                points.push_back(ImVec2(point2d.x, point2d.y));
        }
        auto flags_backup = g_Render->_drawList->Flags;
        g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill;
        g_Render->_drawList->AddConvexPolyFilled(points.data(), points.size(), color_t(0, 255, 128, 127).u32());
        g_Render->_drawList->AddPolyline(points.data(), points.size(), color_t(0, 179, 125, 127).u32(), true, 2.f);
        g_Render->_drawList->Flags = flags_backup;
    }
}

void CAutopeek::Run()
{
    if (!(csgo->local->GetFlags() & FL_ONGROUND))
        return;


    if (vars.misc.peek_assist.properstate()) {
        if (quickpeekstartpos == Vector(0, 0, 0))
            quickpeekstartpos = csgo->local->GetRenderOrigin();

        bool is_revolver = csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER;

        if (!csgo->weapon->IsMiscWeapon() && !is_revolver && csgo->cmd->buttons & IN_ATTACK || is_revolver && Ragebot::Get().shot)
            has_shot = true;

        if (has_shot)
            GotoStart();
    }
    else {
        Reset();
    }
}
