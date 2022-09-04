#pragma once
#include "scripting.h"
#include "Menu.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <winerror.h>

sol::state lua;

namespace cheat
{
    void print(std::string message)
    {
        Msg(message, { 255,255,255,255 });
    }


    void printclr(std::string message, color_t color)
    {
        Msg(message, color);
    }

    void register_callback(sol::this_state s, std::string event, sol::protected_function func)
    {
        if (event != crypt_str("on_paint") && event != crypt_str("createmove") && event != crypt_str("pred_createmove") && event != crypt_str("on_event") && event != crypt_str("frame_stage") && event != crypt_str("menu_items"))
        {
            std::string log = crypt_str("LUA Error: Invalid callback \"") + event + crypt_str("\"");
            Msg(log, color_t(255, 0, 0));
            return;
        }

        if (CLua::Get().loaded.at(CLua::Get().get_current_script_id(s)))
            CLua::Get().hooks.register_hook(event, CLua::Get().get_current_script_id(s), func);
    }
}

namespace globals
{
    float get_realtime()
    {
        return interfaces.global_vars->realtime;
    }

    int get_framecount()
    {
        return interfaces.global_vars->framecount;
    }

    float get_curtime()
    {
        return interfaces.global_vars->curtime;
    }

    float get_frametime()
    {
        return interfaces.global_vars->frametime;
    }

    float get_tickcount()
    {
        return interfaces.global_vars->tickcount;
    }

    int get_tickrate()
    {
        return interfaces.engine->GetServerTick();
    }

    float get_interval_per_tick()
    {
        return interfaces.global_vars->interval_per_tick;
    }

    float get_latency()
    {
        if (!interfaces.engine->GetNetChannelInfo())
            return 0.0f;

        return interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
    }

    int get_choked_commands()
    {
        return csgo->client_state->iChokedCommands + 1;
    }

    bool get_send_packet()
    {
        return csgo->send_packet;
    }

    void set_send_packet(bool v)
    {
        csgo->send_packet = v;
    }
}

namespace engine
{
    Vector2D get_screen_size()
    {
        int w, h;
        interfaces.engine->GetScreenSize(w, h);
        return Vector2D(w, h);
    }

    void execute_console_cmd(std::string cmd)
    {
        interfaces.engine->ClientCmd_Unrestricted(cmd.c_str(), 0);
    }

    Vector get_view_angles()
    {
        Vector nig;
        interfaces.engine->GetViewAngles(nig);
        return nig;
    }

    void set_view_angles(Vector vec)
    {
        return interfaces.engine->SetViewAngles(vec);
    }

    int get_max_clients()
    {
        return interfaces.engine->GetMaxClients();
    }

    bool is_in_game()
    {
        return interfaces.engine->IsInGame();
    }

    bool is_connected()
    {
        return interfaces.engine->IsConnected();
    }

    bool is_paused()
    {
        return interfaces.engine->IsPaused();
    }

    bool is_hltv()
    {
        return interfaces.engine->IsHLTV();
    }

    player_info_t get_player_info(int entindex)
    {
        player_info_t info;
        if (interfaces.engine->GetPlayerInfo(entindex, &info))
        {
            return info;
        }
    }

    int get_player_by_userid(int userid)
    {
        return interfaces.engine->GetPlayerForUserID(userid);
    }
}

//<std::string, ImFont*> lua_fonts;
static vector<ImVec2> poly_points;

namespace render
{
    void create_font(const char* file_name, float size, std::string fontkey)
    {
        font_4process proc{};
        proc.filename = file_name;
        proc.size = size;
        proc.key = fontkey;
        csgo->font_process_queue.push_back(proc);
    }

    void text(float x, float y, color_t color, int flags, std::string fontkey , const char* message, ...)
    {
        auto sexfont = csgo->lua_fonts.find(fontkey);

        ImFont* font = fonts::menu_main;

        if (sexfont != csgo->lua_fonts.end())
            font = sexfont->second;

        g_Render->DrawString(x, y, color, flags, font, message);
    }

    Vector2D calc_text_size(std::string nigger, std::string fontkey)
    {
        auto sexfont = csgo->lua_fonts.find(fontkey);

        ImFont* font = fonts::menu_main;

        if (sexfont != csgo->lua_fonts.end())
            font = sexfont->second;

        ImGui::PushFont(font);
        auto size = ImGui::CalcTextSize(nigger.c_str());
        ImGui::PopFont();
        return Vector2D{ size.x, size.y };
    }

    void line(float x1, float y1, float x2, float y2, color_t color, float thickness)
    {
        g_Render->DrawLine(x1, y1, x2, y2, color, thickness);
    }

    void line_gradient(float x1, float y1, float x2, float y2, color_t color_1, color_t color_2, float thickness)
    {
        g_Render->DrawLineGradient(x1, y1, x2, y2, color_1, color_2, thickness);
    }

    void rect(float x, float y, float w, float h, color_t color, float rounding)
    {
        g_Render->Rect(x, y, w, h, color, rounding);
    }

    void filled_rect(float x, float y, float w, float h, color_t color, float rounding)
    {
        g_Render->FilledRect(x, y, w, h, color, rounding);
    }

    void filled_rect_gradient(float x, float y, float w, float h, color_t col_upr_left, color_t col_upr_right, color_t col_bot_right, color_t col_bot_left)
    {
        g_Render->filled_rect_gradient(x, y, w, h, col_upr_left, col_upr_right, col_bot_right, col_bot_right);
    }

    void arc(float x, float y, float radius, float min_angle, float max_angle, color_t col, float thickness)
    {
        g_Render->Arc(x, y, radius, min_angle, max_angle, col, thickness);
    }

    void triangle(float x1, float y1, float x2, float y2, float x3, float y3, color_t clr, float thickness)
    {
        g_Render->Triangle(x1, y1, x2, y2, x3, y3, clr, thickness);
    }

    void filled_triangle(float x1, float y1, float x2, float y2, float x3, float y3, color_t clr)
    {
        g_Render->TriangleFilled(x1, y1, x2, y2, x3, y3, clr);
    }

    void circle(float x1, float y1, float radius, color_t col, int segments, int thickness)
    {
        g_Render->Circle(x1, y1, radius, col, segments, thickness);
    }

    void circle_filled(float x1, float y1, float radius, color_t col, int segments)
    {
        g_Render->CircleFilled(x1, y1, radius, col, segments);
    }

    void add_poly(Vector2D point)
    {
        ImVec2 p{ point.x,point.y };
        poly_points.push_back(p);
    }

    void poly_line(color_t col, float thickness)
    {
        ImGui::GetBackgroundDrawList()->AddPolyline(poly_points.data(), poly_points.size(), col.u32(), ImDrawListFlags_None, thickness);
        poly_points.clear();
    }

    void poly_filled(color_t col)
    {
        ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(poly_points.data(), poly_points.size(), col.u32());
        poly_points.clear();
    }

    Vector2D world_to_screen(Vector in)
    {
        Vector out{0.f, 0.f, 0.f};
        Math::WorldToScreen(in, out);
        return { out.x,out.y };
    }
}

namespace cvar
{
    ConVar* find_var(const char* name)
    {
        return interfaces.cvars->FindVar(name);
    }
}

namespace entity
{
    IBasePlayer* get_local_player()
    {
        return csgo->local;
    }

    IBasePlayer* get_entity_from_index(int index)
    {
        return interfaces.ent_list->GetClientEntity(index);
    }

    IBasePlayer* get_entity_from_handle(int handle)
    {
        return interfaces.ent_list->GetClientEntityFromHandle(handle);
    }

    int get_highest_ent_index()
    {
        return interfaces.ent_list->GetHighestEntityIndex();
    }
}

namespace luamenu
{
    bool checkbox(std::string label, bool variable)
    {
        bool d = variable;
        ImGui::Checkbox(label.c_str(), &d);
        return d;
    }

    float slider(std::string label, float variable, float min, float max, std::string format, float power)
    {
        float d = variable; 
        ImGui::Text(label.c_str());
        ImGui::SetNextItemWidth(270.f);
        ImGui::SliderFloat(std::string(crypt_str("##") + label).c_str(), &d, min, max, format.c_str(), power);
        return d;
    }

    void text(std::string txt)
    {
        ImGui::Text(txt.c_str());
    }


    void separator()
    {
        ImGui::Separator();
    }

    int combo(std::string label, int cur_item, std::string items)
    {
        int d = cur_item;
        ImGui::Text(label.c_str());
        ImGui::SetNextItemWidth(305.f);
        ImGui::Combo(std::string(crypt_str("##") + label).c_str(), &d, items.c_str());
        return d;
    }


    color_t color_picker(std::string label, color_t clr)
    {
        color_t d = clr;
        g_Menu->color_picker4(label.c_str(), d, true);
        return d;
    }

    void spacing()
    {
        ImGui::Spacing();
    }

    std::string input_text(std::string label, std::string text)
    {
        std::string txt = text;
        txt.resize(256);
        char buf[256];
        strcpy_s(buf, 256, txt.data());
        ImGui::InputText(label.c_str(), buf, 256);
        txt = buf;
        return txt;
    }

    bool button(std::string label, Vector2D size = {0.f,0.f})
    {
        return ImGui::Button(label.c_str(), {size.x,size.y});
    }
}

namespace utils
{
    // 1.0 if hit
    /*float trace_ray(Vector start, Vector end, int mask, IBasePlayer* filter)
    {
        CTraceFilter nigger;
        nigger.pSkip = filter;

        CGameTrace trace;
        interfaces.trace->TraceRay({ start, end }, mask, &nigger, &trace);

        return trace.fraction;
    }*/

    void chat_printf(std::string msg)
    {
        if (auto chatelement = (IHudChat*)H::findHudElement(H::hud, crypt_str("CHudChat")))
        {
            chatelement->ChatPrintf(0, 0, msg.c_str());
        }
    }

    void set_clantag(const char* clantag)
    {
        csgo->pSetClantag(clantag, clantag);
    }
}

namespace luaconfig {
    namespace ragebot {
        //master
        void setRagebotMaster(bool bState) {
            vars.ragebot.enable = bState;
        }
        bool getRagebotMaster() {
            return vars.ragebot.enable;
        }
        //onkey
        void setRagebotOnKey(bool bState) {
            vars.ragebot.onkey = bState;
        }
        bool getRagebotOnKey() {
            return vars.ragebot.onkey;
        }
        //silent
        void setRagebotSilent(bool bState) {
            vars.ragebot.silent = bState;
        }
        bool getRagebotSilent() {
            return vars.ragebot.silent;
        }
        //fov
        void setRagebotFov(int fov) {
            vars.ragebot.silent = fov;
        }
        int getRagebotFov() {
            return vars.ragebot.fov;
        }
        //autoshoot
        void setRagebotAutoshoot(bool bState) {
            vars.ragebot.autoshoot = bState;
        }
        bool getRagebotAutoshoot() {
            return vars.ragebot.autoshoot;
        }
        //autoscope
        void setRagebotAutoscope(bool bState) {
            vars.ragebot.autoscope = bState;
        }
        bool getRagebotAutoscope() {
            return vars.ragebot.autoscope;
        }
        //resolvo
        void setRagebotResolver(bool bState) {
            vars.ragebot.resolver = bState;
        }
        bool getRagebotResolver() {
            return vars.ragebot.resolver;
        }
        //zeusbot
        void setZeusbotEnable(bool bState) {
            vars.ragebot.zeusbot = bState;
        }
        bool getZeusbotEnable() {
            return vars.ragebot.zeusbot;
        }
        //zeuschance
        void setZeuschance(int c) {
            vars.ragebot.zeuschance = c;
        }
        int getZeuschance() {
            return vars.ragebot.zeuschance;
        }
        //knifebot
        void setKnifebot(bool c) {
            vars.misc.knifebot = c;
        }
        bool getKnifebot() {
            return vars.misc.knifebot;
        }
        //teleportdt
        void setDtTeleport(bool c) {
            vars.ragebot.dt_teleport = c;
        }
        bool getDtTeleport() {
            return  vars.ragebot.dt_teleport;
        }

        // weapon group settings (including default)
        void setRagebotWeaponMaster(bool bState, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].enable = bState;
        }
        bool getRagebotWeaponMaster(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].enable;
        }

        //hitchance
        void setRagebotWeaponHitchance(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].hitchance = hc;
        }
        int getRagebotWeaponHitchance(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].hitchance;
        }

        //DT hitchance
        void setRagebotWeaponDTHitchance(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].doubletap_hc = hc;
        }
        int getRagebotWeaponDTHitchance(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].doubletap_hc;
        }

        //mindmg
        void setRagebotWeaponMinDmg(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].mindamage = hc;
        }
        int getRagebotWeaponMinDmg(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].mindamage;
        }

        //overridemindmg
        void setRagebotWeaponMinDmgOverride(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].mindamage_override = hc;
        }
        int getRagebotWeaponMinDmgOverride(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].mindamage_override;
        }

        //autostopmode
        void setRagebotWeaponAutostopMode(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].autostopMode = hc;
        }
        int getRagebotWeaponAutostopMode(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].autostopMode;
        }

        //hitboxes
        void setRagebotWeaponHitboxEnabled(bool enable, int hitboxIdx, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].hitboxes[hitboxIdx] = enable;
        }
        int getRagebotWeaponHitboxEnabled(int hitboxIdx, int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].hitboxes[hitboxIdx];
        }

        //static_scale
        void setRagebotWeaponStaticScale(bool hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].static_scale = hc;
        }
        bool getRagebotWeaponStaticScale(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].static_scale;
        }

        //body
        void setRagebotWeaponBodyScale(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].scale_body = hc;
        }
        int getRagebotWeaponBodyScale(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].scale_body;
        }

        //facial
        void setRagebotWeaponHeadScale(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].scale_head = hc;
        }
        int getRagebotWeaponHeadScale(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].scale_head;
        }

        //max misses to baim safe
        void setRagebotWeaponMaxMisses(int hc, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].max_misses = hc;
        }
        int getRagebotWeaponMaxMisses(int weaponIdx = 0) {
            return vars.ragebot.weapon[weaponIdx].max_misses;
        }

        //prefersafe
        void setRagebotWeaponPreferSafe(bool bState, int weaponIdx) {
            vars.ragebot.weapon[weaponIdx].prefer_safepoint = bState;
        }
        bool getRagebotWeaponPreferSafe(int weaponIdx) {
            return vars.ragebot.weapon[weaponIdx].prefer_safepoint;
        }



    }

    namespace antiaim {
        //master
        void setAntiAimMaster(bool bState) {
            vars.antiaim.enable = bState;
        }
        bool getAntiAimMaster() {
            return vars.antiaim.enable;
        }

        //aa on use
        void setAntiAimOnUse(bool bState) {
            vars.antiaim.aa_on_use = bState;
        }
        bool getAntiAimOnUse() {
            return vars.antiaim.aa_on_use;
        }

        //pitch
        void setAntiAimPitch(int pitch) {
            vars.antiaim.pitch = pitch; //disabled down up
        }
        int getAntiAimPitch() {
            return vars.antiaim.pitch;
        }

        //yaw
        void setAntiAimYaw(int yaw) {
            vars.antiaim.pitch = yaw; // backward forward
        }
        int getAntiAimYaw() {
            return vars.antiaim.pitch;
        }

        //inverter
        void setAntiAimInverter(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }
            
            vars.antiaim.inverter.type = parse; // backward forward
        }

        //jitter
        void setAntiAimJitter(int jitter) {
            vars.antiaim.jitter_angle = jitter; // backward forward
        }
        int getAntiAimJitter() {
            return vars.antiaim.jitter_angle;
        }

        //at target
        void setAntiAimAtTargets(bool bState) {
            vars.antiaim.attarget = bState;
        }

        bool getAntiAimAtTargets() {
            return vars.antiaim.attarget;
        }

        //static
        void setAntiAimStaticDelta(bool bState) {
            vars.antiaim.static_delta = bState;
        }

        bool getAntiAimStaticDelta() {
            return vars.antiaim.static_delta;
        }

        //desync amt
        void setAntiAimDesyncDelta(int desyncAmount) {
            vars.antiaim.desync_amount = desyncAmount;
        }

        int getAntiAimDesyncDelta() {
            return vars.antiaim.desync_amount;
        }

        //extended
        void setAntiAimExtended(bool bState) {
            vars.antiaim.extendlby = bState;
        }

        bool getAntiAimExtended() {
            return vars.antiaim.extendlby;
        }

        //zleanable
        void setAntiAimZLean(bool bState) {
            vars.antiaim.zleanenable = bState;
        }

        bool getAntiAimZLean() {
            return vars.antiaim.zleanenable;
        }

        //lean amt
        void setAntiAimZLeanAngle(int degrees) {
            vars.antiaim.zlean = degrees;
        }

        int getAntiAimZLeanAngle() {
            return vars.antiaim.zlean;
        }

        //fakehead
        void setAntiAimFakehead(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.fakehead.type = parse; // on /off
        }

        //manualAA
        void setAntiAimManual(bool bState) {
            vars.antiaim.manual_antiaim = bState;
        }

        bool getAntiAimManual() {
            return vars.antiaim.manual_antiaim;
        }

        //ignore AT
        void setAntiAimIgnoreAtTarget(bool bState) {
            vars.antiaim.ignore_attarget = bState;
        }

        bool getAntiAimIgnoreAtTarget() {
            return vars.antiaim.ignore_attarget;
        }

        //manual_left
        void setAntiAimManualLeft(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.manual_left.type = parse; // on /off
        }

        //manual_right
        void setAntiAimManualRight(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.manual_right.type = parse; // on /off
        }

        //manual_back
        void setAntiAimManualBack(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.manual_back.type = parse; // on /off
        }

        //manual_forward
        void setAntiAimManualForward(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.manual_forward.type = parse; // on /off
        }

        //FAKELAG
        void setFakeLagMaster(bool bState) {
            vars.antiaim.fakelag = bState;
        }

        bool getFakeLagMaster() {
            return vars.antiaim.fakelag;
        }

        //STANDBY CHOKE
        void setFakeLagStandbyChoke(bool bState) {
            vars.antiaim.fakelag_when_standing = bState;
        }

        bool getFakeLagStandbyChoke() {
            return vars.antiaim.fakelag_when_standing;
        }

        //FACTOR
        void setFakeLagChoke(int max) {
            vars.antiaim.fakelagfactor = max;
        }

        int getFakeLagChoke() {
            return vars.antiaim.fakelagfactor;
        }

        //JITTER
        void setFakeLagJitter(int jitter) {
            vars.antiaim.fakelagvariance = jitter;
        }

        int getFakeLagJitter() {
            return vars.antiaim.fakelagvariance;
        }

        //fakeducker
        void setAntiAimFakeduck(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.fakeduck.type = parse; // on /off
        }

        //slowwalk
        void setAntiAimslowwalk(bool bEnabled) {
            int parse = 0;
            if (bEnabled) {
                parse = 4;
            }

            vars.antiaim.slowwalk.type = parse; // on /off
        }

        //slw speed
        void setSlowWalkSpeed(int speed) {
            vars.antiaim.slowwalkspeed = speed;
        }

        int getSlowWalkSpeed() {
            return  vars.antiaim.slowwalkspeed;
        }

    }

}

void lua_panic(sol::optional<std::string> message)
{
    if (!message)
        return;

    auto log = crypt_str("LUA Error: ") + message.value_or(crypt_str("unknown"));
    Msg(log, color_t(255, 0, 0));
}

void CLua::initialize()
{
    lua = sol::state(sol::c_call<decltype(&lua_panic), &lua_panic>);
    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::debug, sol::lib::package);

    lua[crypt_str("collectgarbage")] = sol::nil;
    lua[crypt_str("dofilsse")] = sol::nil;
    lua[crypt_str("load")] = sol::nil;
    lua[crypt_str("loadfile")] = sol::nil;
    lua[crypt_str("pcall")] = sol::nil;
    lua[crypt_str("print")] = sol::nil;
    lua[crypt_str("xpcall")] = sol::nil;
    lua[crypt_str("getmetatable")] = sol::nil;
    lua[crypt_str("setmetatable")] = sol::nil;
    lua[crypt_str("__nil_callback")] = []() {};
    lua[crypt_str("sleep")] = sol::nil;

    auto cheat = lua.create_table();
    cheat[crypt_str("print")] = sol::overload(cheat::print, cheat::printclr);
    cheat[crypt_str("register_callback")] = cheat::register_callback;

    auto globals = lua.create_table();
    globals[crypt_str("get_realtime")] = globals::get_realtime;
    globals[crypt_str("get_framecount")] = globals::get_framecount;
    globals[crypt_str("get_curtime")] = globals::get_curtime;
    globals[crypt_str("get_frametime")] = globals::get_frametime;
    globals[crypt_str("get_tickcount")] = globals::get_tickcount;
    globals[crypt_str("get_interval_per_tick")] = globals::get_interval_per_tick;
    globals[crypt_str("get_latency")] = globals::get_latency;

    auto ragebot = lua.create_table();
    ragebot[crypt_str("setRagebotMaster")] = luaconfig::ragebot::setRagebotMaster; //master switch 
    ragebot[crypt_str("getRagebotMaster")] = luaconfig::ragebot::getRagebotMaster;
    ragebot[crypt_str("getRagebotOnKey")] = luaconfig::ragebot::setRagebotOnKey; //onkey
    ragebot[crypt_str("getRagebotOnKey")] = luaconfig::ragebot::getRagebotOnKey;
    ragebot[crypt_str("setRagebotSilent")] = luaconfig::ragebot::setRagebotSilent; //silent
    ragebot[crypt_str("getRagebotSilent")] = luaconfig::ragebot::getRagebotSilent;
    ragebot[crypt_str("setRagebotFov")] = luaconfig::ragebot::setRagebotFov; //fov
    ragebot[crypt_str("getRagebotFov")] = luaconfig::ragebot::getRagebotFov;
    ragebot[crypt_str("setRagebotAutoshoot")] = luaconfig::ragebot::setRagebotAutoshoot; //autoshoot
    ragebot[crypt_str("getRagebotAutoshoot")] = luaconfig::ragebot::getRagebotAutoshoot;
    ragebot[crypt_str("setRagebotAutoscope")] = luaconfig::ragebot::setRagebotAutoscope; //autoscope
    ragebot[crypt_str("getRagebotAutoscope")] = luaconfig::ragebot::getRagebotAutoscope;
    ragebot[crypt_str("setRagebotResolver")] = luaconfig::ragebot::setRagebotResolver; //resolver
    ragebot[crypt_str("getRagebotResolver")] = luaconfig::ragebot::getRagebotResolver;
    ragebot[crypt_str("setZeusbotEnable")] = luaconfig::ragebot::setZeusbotEnable; //zeusbot
    ragebot[crypt_str("getZeusbotEnable")] = luaconfig::ragebot::getZeusbotEnable; 
    //zeuschance
    ragebot[crypt_str("setZeuschance")] = luaconfig::ragebot::setZeuschance;
    ragebot[crypt_str("getZeuschance")] = luaconfig::ragebot::getZeuschance;
    //knifebot
    ragebot[crypt_str("setKnifebot")] = luaconfig::ragebot::setKnifebot;
    ragebot[crypt_str("getKnifebot")] = luaconfig::ragebot::getKnifebot;
    //teleportdt
    ragebot[crypt_str("setDtTeleport")] = luaconfig::ragebot::setDtTeleport;
    ragebot[crypt_str("getDtTeleport")] = luaconfig::ragebot::getDtTeleport;

    //WEAPON FNCS
    ragebot[crypt_str("setRagebotWeaponMaster")] = luaconfig::ragebot::setRagebotWeaponMaster; //master switch override (int, bool) first arg 0=Default 1=Autosniper 2=Scout 3=AWP 4=Rifles 5=Pistols 6=Heavy Pistols
    ragebot[crypt_str("getRagebotWeaponMaster")] = luaconfig::ragebot::getRagebotWeaponMaster;


    //hitchance
    ragebot[crypt_str("setRagebotWeaponHitchance")] = luaconfig::ragebot::setRagebotWeaponHitchance;
    ragebot[crypt_str("getRagebotWeaponHitchance")] = luaconfig::ragebot::getRagebotWeaponHitchance;

    //DT hitchance
    ragebot[crypt_str("setRagebotWeaponDTHitchance")] = luaconfig::ragebot::setRagebotWeaponDTHitchance;
    ragebot[crypt_str("getRagebotWeaponDTHitchance")] = luaconfig::ragebot::getRagebotWeaponDTHitchance;

    //mindmg
    ragebot[crypt_str("setRagebotWeaponMinDmg")] = luaconfig::ragebot::setRagebotWeaponMinDmg;
    ragebot[crypt_str("getRagebotWeaponMinDmg")] = luaconfig::ragebot::getRagebotWeaponMinDmg;

    //overridemindmg
    ragebot[crypt_str("setRagebotWeaponMinDmgOverride")] = luaconfig::ragebot::setRagebotWeaponMinDmgOverride;
    ragebot[crypt_str("getRagebotWeaponMinDmgOverride")] = luaconfig::ragebot::getRagebotWeaponMinDmgOverride;

    //autostopmode
    ragebot[crypt_str("setRagebotWeaponAutostopMode")] = luaconfig::ragebot::setRagebotWeaponAutostopMode; //0 = disable, 1= normal 2 = quick 3 = preduct
    ragebot[crypt_str("getRagebotWeaponAutostopMode")] = luaconfig::ragebot::getRagebotWeaponAutostopMode;

    //hitboxes
    ragebot[crypt_str("setRagebotWeaponHitboxEnabled")] = luaconfig::ragebot::setRagebotWeaponHitboxEnabled;
    ragebot[crypt_str("getRagebotWeaponHitboxEnabled")] = luaconfig::ragebot::getRagebotWeaponHitboxEnabled;

    //static_scale
    ragebot[crypt_str("setRagebotWeaponStaticScale")] = luaconfig::ragebot::setRagebotWeaponStaticScale;
    ragebot[crypt_str("getRagebotWeaponStaticScale")] = luaconfig::ragebot::getRagebotWeaponStaticScale;

    //body
    ragebot[crypt_str("setRagebotWeaponBodyScale")] = luaconfig::ragebot::setRagebotWeaponBodyScale;
    ragebot[crypt_str("getRagebotWeaponBodyScale")] = luaconfig::ragebot::getRagebotWeaponBodyScale;

    //facial
    ragebot[crypt_str("setRagebotWeaponHeadScale")] = luaconfig::ragebot::setRagebotWeaponHeadScale;
    ragebot[crypt_str("getRagebotWeaponHeadScale")] = luaconfig::ragebot::getRagebotWeaponHeadScale;

    //max misses to baim safe
    ragebot[crypt_str("setRagebotWeaponMaxMisses")] = luaconfig::ragebot::setRagebotWeaponMaxMisses;
    ragebot[crypt_str("getRagebotWeaponMaxMisses")] = luaconfig::ragebot::getRagebotWeaponMaxMisses;

    //prefersafe
    ragebot[crypt_str("setRagebotWeaponPreferSafe")] = luaconfig::ragebot::setRagebotWeaponPreferSafe;
    ragebot[crypt_str("getRagebotWeaponPreferSafe")] = luaconfig::ragebot::getRagebotWeaponPreferSafe;


    // ---- ANTIAIM ---
    auto antiaim = lua.create_table();
    //master switch 
    antiaim[crypt_str("setAntiAimMaster")] = luaconfig::antiaim::setAntiAimMaster;
    antiaim[crypt_str("getAntiAimMaster")] = luaconfig::antiaim::getAntiAimMaster;

    //aa on use
    antiaim[crypt_str("setAntiAimOnUse")] = luaconfig::antiaim::setAntiAimOnUse;
    antiaim[crypt_str("getAntiAimOnUse")] = luaconfig::antiaim::getAntiAimOnUse;

    //pitch
    antiaim[crypt_str("setAntiAimPitch")] = luaconfig::antiaim::setAntiAimPitch;
    antiaim[crypt_str("getAntiAimPitch")] = luaconfig::antiaim::getAntiAimPitch;

    //yaw
    antiaim[crypt_str("setAntiAimYaw")] = luaconfig::antiaim::setAntiAimYaw;
    antiaim[crypt_str("getAntiAimYaw")] = luaconfig::antiaim::getAntiAimYaw;

    //inverter
    antiaim[crypt_str("setAntiAimInverter")] = luaconfig::antiaim::setAntiAimInverter;

    //jitter
    antiaim[crypt_str("setAntiAimJitter")] = luaconfig::antiaim::setAntiAimJitter;
    antiaim[crypt_str("getAntiAimJitter")] = luaconfig::antiaim::getAntiAimJitter;

    //at target
    antiaim[crypt_str("setAntiAimAtTargets")] = luaconfig::antiaim::setAntiAimAtTargets;
    antiaim[crypt_str("getAntiAimAtTargets")] = luaconfig::antiaim::getAntiAimAtTargets;

    //static
    antiaim[crypt_str("setAntiAimStaticDelta")] = luaconfig::antiaim::setAntiAimStaticDelta;
    antiaim[crypt_str("getAntiAimStaticDelta")] = luaconfig::antiaim::getAntiAimStaticDelta;

    //desync amt
    antiaim[crypt_str("setAntiAimDesyncDelta")] = luaconfig::antiaim::setAntiAimDesyncDelta;
    antiaim[crypt_str("getAntiAimDesyncDelta")] = luaconfig::antiaim::getAntiAimDesyncDelta;

    //extended
    antiaim[crypt_str("setAntiAimExtended")] = luaconfig::antiaim::setAntiAimExtended;
    antiaim[crypt_str("getAntiAimExtended")] = luaconfig::antiaim::getAntiAimExtended;

    //zleanable
    antiaim[crypt_str("setAntiAimZLean")] = luaconfig::antiaim::setAntiAimZLean;
    antiaim[crypt_str("getAntiAimZLean")] = luaconfig::antiaim::getAntiAimZLean;

    //lean amt
    antiaim[crypt_str("setAntiAimZLeanAngle")] = luaconfig::antiaim::setAntiAimZLeanAngle;
    antiaim[crypt_str("getAntiAimZLeanAngle")] = luaconfig::antiaim::getAntiAimZLeanAngle;

    //fakehead
    antiaim[crypt_str("setAntiAimFakehead")] = luaconfig::antiaim::setAntiAimFakehead;


    //manualAA
    antiaim[crypt_str("setAntiAimManual")] = luaconfig::antiaim::setAntiAimManual;
    antiaim[crypt_str("getAntiAimManual")] = luaconfig::antiaim::getAntiAimManual;

    //ignore AT
    antiaim[crypt_str("setAntiAimIgnoreAtTarget")] = luaconfig::antiaim::setAntiAimIgnoreAtTarget;
    antiaim[crypt_str("getAntiAimIgnoreAtTarget")] = luaconfig::antiaim::getAntiAimIgnoreAtTarget;

    //manual_left
    antiaim[crypt_str("setAntiAimManualLeft")] = luaconfig::antiaim::setAntiAimManualLeft;

    //manual_right
    antiaim[crypt_str("setAntiAimManualRight")] = luaconfig::antiaim::setAntiAimManualRight;

    //manual_back
    antiaim[crypt_str("setAntiAimManualBack")] = luaconfig::antiaim::setAntiAimManualBack;

    //manual_forward
    antiaim[crypt_str("setAntiAimManualForward")] = luaconfig::antiaim::setAntiAimManualForward;

    //FAKELAG
    antiaim[crypt_str("setFakeLagMaster")] = luaconfig::antiaim::setFakeLagMaster;
    antiaim[crypt_str("getFakeLagMaster")] = luaconfig::antiaim::getFakeLagMaster;

    //STANDBY CHOKE
    antiaim[crypt_str("setFakeLagStandbyChoke")] = luaconfig::antiaim::setFakeLagStandbyChoke;
    antiaim[crypt_str("getFakeLagStandbyChoke")] = luaconfig::antiaim::getFakeLagStandbyChoke;

    //FACTOR
    antiaim[crypt_str("setFakeLagChoke")] = luaconfig::antiaim::setFakeLagChoke;
    antiaim[crypt_str("getFakeLagChoke")] = luaconfig::antiaim::getFakeLagChoke;

    //JITTER
    antiaim[crypt_str("setFakeLagJitter")] = luaconfig::antiaim::setFakeLagJitter;
    antiaim[crypt_str("getFakeLagJitter")] = luaconfig::antiaim::getFakeLagJitter;

    //fakeducker
    antiaim[crypt_str("setAntiAimFakeduck")] = luaconfig::antiaim::setAntiAimFakeduck;

    //slowwalk
    antiaim[crypt_str("setAntiAimSlowWalk")] = luaconfig::antiaim::setAntiAimslowwalk;

    //slw speed
    antiaim[crypt_str("setSlowWalkSpeed")] = luaconfig::antiaim::setSlowWalkSpeed;
    antiaim[crypt_str("getSlowWalkSpeed")] = luaconfig::antiaim::getSlowWalkSpeed;

    auto config = lua.create_table();
    config[crypt_str("ragebot")] = ragebot;
    config[crypt_str("antiaim")] = antiaim;

    auto engine = lua.create_table();
    engine[crypt_str("execute_console_cmd")] = engine::execute_console_cmd;
    engine[crypt_str("get_screen_size")] = engine::get_screen_size;
    engine[crypt_str("get_view_angles")] = engine::get_view_angles;
    engine[crypt_str("set_view_angles")] = engine::set_view_angles;
    engine[crypt_str("get_max_clients")] = engine::get_max_clients;
    engine[crypt_str("is_in_game")] = engine::is_in_game;
    engine[crypt_str("is_connected")] = engine::is_connected;
    engine[crypt_str("is_paused")] = engine::is_paused;
    engine[crypt_str("is_hltv")] = engine::is_hltv;
    engine[crypt_str("get_player_info")] = engine::get_player_info;
    engine[crypt_str("get_player_by_userid")] = engine::get_player_by_userid;

    auto render = lua.create_table();
    render[crypt_str("text")] = render::text;
    render[crypt_str("create_font")] = render::create_font;
    render[crypt_str("calc_text_size")] = render::calc_text_size;
    render[crypt_str("line")] = render::line;
    render[crypt_str("line_gradient")] = render::line_gradient;
    render[crypt_str("rect")] = render::rect;
    render[crypt_str("filled_rect")] = render::filled_rect;
    render[crypt_str("filled_rect_gradient")] = render::filled_rect_gradient;
    render[crypt_str("arc")] = render::arc;
    render[crypt_str("triangle")] = render::triangle;
    render[crypt_str("filled_triangle")] = render::filled_triangle;
    render[crypt_str("circle")] = render::circle;
    render[crypt_str("circle_filled")] = render::circle_filled;
    render[crypt_str("add_poly")] = render::add_poly;
    render[crypt_str("poly_line")] = render::poly_line;
    render[crypt_str("poly_line_filled")] = render::poly_filled;
    render[crypt_str("world_to_screen")] = render::world_to_screen;

    auto cvar = lua.create_table();
    cvar[crypt_str("find_var")] = cvar::find_var;

    auto entity = lua.create_table();
    entity[crypt_str("get_highest_ent_index")] = entity::get_highest_ent_index;
    entity[crypt_str("get_entity_from_index")] = entity::get_entity_from_index;
    entity[crypt_str("get_entity_from_handle")] = entity::get_entity_from_handle;
    entity[crypt_str("get_local_player")] = entity::get_local_player;

    auto menu = lua.create_table();
    menu[crypt_str("checkbox")] = luamenu::checkbox;
    menu[crypt_str("slider")] = luamenu::slider;
    menu[crypt_str("text")] = luamenu::text;
    menu[crypt_str("separator")] = luamenu::separator;
    menu[crypt_str("combo")] = luamenu::combo;
    menu[crypt_str("color_picker")] = luamenu::color_picker;
    menu[crypt_str("input_text")] = luamenu::input_text;
    menu[crypt_str("spacing")] = luamenu::spacing;
    menu[crypt_str("button")] = luamenu::button;

    auto utils = lua.create_table();
    utils[crypt_str("set_clantag")] = utils::set_clantag;
    utils[crypt_str("chat_printf")] = utils::chat_printf;

    lua.new_enum<CSGOHitboxID>(crypt_str("hitbox"), {
        { crypt_str("head"), CSGOHitboxID::Head },
        { crypt_str("neck"), CSGOHitboxID::Neck },
        { crypt_str("pelvis"), CSGOHitboxID::Pelvis },
        { crypt_str("stomach"), CSGOHitboxID::Stomach },
        { crypt_str("lower_chest"), CSGOHitboxID::LowerChest },
        { crypt_str("chest"), CSGOHitboxID::Chest },
        { crypt_str("upper_chest"), CSGOHitboxID::UpperChest },
        { crypt_str("right_thigh"), CSGOHitboxID::RightThigh },
        { crypt_str("left_thigh"), CSGOHitboxID::LeftThigh },
        { crypt_str("right_shin"), CSGOHitboxID::RightShin },
        { crypt_str("left_shin"), CSGOHitboxID::LeftShin },
        { crypt_str("left_foot"), CSGOHitboxID::LeftFoot },
        { crypt_str("right_foot"), CSGOHitboxID::RightFoot },
        { crypt_str("right_hand"), CSGOHitboxID::RightHand },
        { crypt_str("left_hand"), CSGOHitboxID::LeftHand },
        { crypt_str("right_upper_arm"), CSGOHitboxID::RightUpperArm },
        { crypt_str("right_lower_arm"), CSGOHitboxID::RightLowerArm },
        { crypt_str("left_upper_arm"), CSGOHitboxID::LeftUpperArm },
        { crypt_str("left_lower_arm"), CSGOHitboxID::LeftLowerArm }
        });

    lua.new_enum<weap_type>(crypt_str("weapon_type"), {
        { crypt_str("default"), weap_type::def },
        { crypt_str("autosniper"), weap_type::scar },
        { crypt_str("scout"), weap_type::scout },
        { crypt_str("awp"), weap_type::_awp },
        { crypt_str("rifle"), weap_type::rifles },
        { crypt_str("pistol"), weap_type::pistols },
        { crypt_str("heavy_pistol"), weap_type::heavy_pistols }
        });

    lua.new_enum<ClientFrameStage_t>(crypt_str("frame_stage"), {
        { crypt_str("FRAME_UNDEFINED"), ClientFrameStage_t::FRAME_UNDEFINED },
        { crypt_str("FRAME_START"), ClientFrameStage_t::FRAME_START },
        { crypt_str("FRAME_NET_UPDATE_START"), ClientFrameStage_t::FRAME_NET_UPDATE_START },
        { crypt_str("FRAME_NET_UPDATE_POSTDATAUPDATE_START"), ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START },
        { crypt_str("FRAME_NET_UPDATE_POSTDATAUPDATE_END"), ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END },
        { crypt_str("FRAME_NET_UPDATE_END"), ClientFrameStage_t::FRAME_NET_UPDATE_END },
        { crypt_str("FRAME_RENDER_START"), ClientFrameStage_t::FRAME_RENDER_START },
        { crypt_str("FRAME_RENDER_END"), ClientFrameStage_t::FRAME_RENDER_END }
        });

    lua.new_usertype<color_t>(crypt_str("color"), sol::constructors<color_t(), color_t(int, int, int, int)>(),
        crypt_str("r"), &color_t::get_red,
        crypt_str("g"), &color_t::get_green,
        crypt_str("b"), &color_t::get_blue,
        crypt_str("a"), &color_t::get_alpha,
        crypt_str("setred"), &color_t::set_red,
        crypt_str("setgreen"), &color_t::set_green,
        crypt_str("setblue"), &color_t::set_blue,
        crypt_str("setalpha"), &color_t::set_alpha
        );

    lua.new_usertype<Vector>(crypt_str("vec3"), sol::constructors<Vector(), Vector(float, float, float)>(),
        crypt_str("x"), &Vector::x,
        crypt_str("y"), &Vector::y,
        crypt_str("z"), &Vector::z,
        crypt_str("length"), &Vector::Length,
        crypt_str("length2d"), &Vector::Length2D,
        crypt_str("length_sqr"), &Vector::LengthSqr,
        crypt_str("clamp"), &Vector::Clamp,
        crypt_str("normalize"), &Vector::Normalize,
        crypt_str("distance"), &Vector::DistTo,
        crypt_str("to_angle"), &Vector::toAngle
        );

    lua.new_usertype<Vector2D>("vec2", sol::constructors<Vector2D(), Vector2D(float, float)>(),
        crypt_str("x"), &Vector2D::x,
        crypt_str("y"), &Vector2D::y,
        crypt_str("length"), &Vector2D::Length
        );

    lua.new_usertype<CUserCmd>(crypt_str("usercmd"),
        crypt_str("cmd_number"), &CUserCmd::command_number,
        crypt_str("tick_count"), &CUserCmd::tick_count,
        crypt_str("viewangles"), &CUserCmd::viewangles,
        crypt_str("aimdirection"), &CUserCmd::aimdirection,
        crypt_str("forwardmove"), &CUserCmd::forwardmove,
        crypt_str("sidemove"), &CUserCmd::sidemove,
        crypt_str("upmove"), &CUserCmd::upmove,
        crypt_str("buttons"), &CUserCmd::buttons,
        crypt_str("impulse"), &CUserCmd::impulse,
        crypt_str("weaponselect"), &CUserCmd::weaponselect,
        crypt_str("weaponsubtype"), &CUserCmd::weaponsubtype,
        crypt_str("random_seed"), &CUserCmd::random_seed,
        crypt_str("mousedx"), &CUserCmd::mousedx,
        crypt_str("mousedy"), &CUserCmd::mousedy,
        crypt_str("predicted"), &CUserCmd::hasbeenpredicted
        );

    lua.new_usertype<ClientClass>(crypt_str("client_class"),
        crypt_str("class_id"), &ClientClass::m_ClassID,
        crypt_str("name"), &ClientClass::m_pNetworkName
        );

    lua.new_usertype<IBasePlayer>(crypt_str("cbaseentity"),
        crypt_str("origin"), &IBasePlayer::GetOrigin,
        crypt_str("velocity"), &IBasePlayer::GetVelocity,
        crypt_str("name"), &IBasePlayer::GetName,
        crypt_str("index"), &IBasePlayer::EntIndex,
        crypt_str("flags"), &IBasePlayer::GetFlags,
        crypt_str("health"), &IBasePlayer::GetHealth,
        crypt_str("movetype"), &IBasePlayer::GetMoveType,
        crypt_str("client_class"), &IBasePlayer::GetClientClass,
        crypt_str("team"), &IBasePlayer::GetTeam,
        crypt_str("is_player"), &IBasePlayer::IsPlayer,
        crypt_str("eye_angles"), &IBasePlayer::GetEyeAngles,
        crypt_str("eye_position"), &IBasePlayer::GetEyePosition,
        crypt_str("mins"), &IBasePlayer::GetMins,
        crypt_str("maxs"), &IBasePlayer::GetMaxs,
        crypt_str("get_prop_int"), &IBasePlayer::get_prop,
        crypt_str("get_prop_fl"), &IBasePlayer::get_prop_fl,
        crypt_str("get_prop_b"), &IBasePlayer::get_prop_b,
        crypt_str("get_prop_vec"), &IBasePlayer::get_prop_vec,
        crypt_str("set_prop_int"), &IBasePlayer::set_prop,
        crypt_str("set_prop_fl"), &IBasePlayer::set_prop_fl,
        crypt_str("set_prop_b"), &IBasePlayer::set_prop_b,
        crypt_str("set_prop_vec"), &IBasePlayer::set_prop_vec,

        //crypt_str("get_prop_int"), sol::overload(&IBasePlayer::get_prop, &IBasePlayer::get_prop_fl, &IBasePlayer::get_prop_b, &IBasePlayer::get_prop_vec),
        //crypt_str("set_prop"), sol::overload(&IBasePlayer::set_prop, &IBasePlayer::set_prop_fl, &IBasePlayer::set_prop_b, &IBasePlayer::set_prop_vec),
        crypt_str("is_enemy"), &IBasePlayer::isEnemy
        );

    lua.new_usertype<player_info_t>(crypt_str("player_info"),
        crypt_str("xuidLow"), &player_info_t::xuidLow,
        crypt_str("xuidHigh"), &player_info_t::xuidHigh,
        crypt_str("name"), &player_info_t::name,
        crypt_str("userid"), &player_info_t::userId,
        crypt_str("isteamid"), &player_info_t::iSteamID,
        crypt_str("friends_name"), &player_info_t::friendsName,
        crypt_str("fakeplayer"), &player_info_t::fakeplayer,
        crypt_str("is_hltv"), &player_info_t::ishltv
        );

    lua.new_usertype<IGameEvent>(crypt_str("game_event"),
        crypt_str("get_name"), &IGameEvent::GetName,
        crypt_str("get_int"), &IGameEvent::GetInt,
        crypt_str("get_float"), &IGameEvent::GetFloat,
        crypt_str("get_bool"), &IGameEvent::GetBool,
        crypt_str("get_string"), &IGameEvent::GetString
        );

    lua.new_usertype<ConVar>(crypt_str("convar"),
        crypt_str("get_string"), &ConVar::GetString,
        crypt_str("get_int"), &ConVar::GetInt,
        crypt_str("get_float"), &ConVar::GetFloat,
        crypt_str("set_string"), &ConVar::SetString,
        crypt_str("set_int"), &ConVar::SetInt,
        crypt_str("set_float"), &ConVar::SetFloat
        );

    lua[crypt_str("cheat")] = cheat;
    lua[crypt_str("globals")] = globals;
    lua[crypt_str("engine")] = engine;
    lua[crypt_str("config")] = config;
    lua[crypt_str("render")] = render;
    lua[crypt_str("cvar")] = cvar;
    lua[crypt_str("entity")] = entity;
    lua[crypt_str("menu")] = menu;
    lua[crypt_str("utils")] = utils;

    printf(crypt_str("[ Hook ] Initialized LUA scripting\n"));
}

void CLua::refresh_scripts()
{
    auto oldLoaded = loaded;
    auto oldScripts = scripts;

    loaded.clear();
    paths.clear();
    scripts.clear();

    std::string folder;
    static TCHAR path[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
    {
        folder = std::string(path) + crypt_str("\\Reflect\\Scripts\\");
        CreateDirectory(folder.c_str(), NULL);

        auto i = 0;

        for (auto& entry : std::filesystem::directory_iterator(folder))
        {
            if (entry.path().extension() == crypt_str(".lua") || entry.path().extension() == crypt_str(".luac"))
            {
                auto path = entry.path();
                auto filename = path.filename().string();

                auto didPut = false;

                for (auto i = 0; i < oldScripts.size(); i++)
                {
                    if (filename == oldScripts.at(i))
                    {
                        loaded.emplace_back(oldLoaded.at(i));
                        didPut = true;
                    }
                }

                if (!didPut)
                    loaded.emplace_back(false);

                paths.emplace_back(path);
                scripts.emplace_back(filename);
                vars.menu.loaded_luas.emplace_back(filename);
                ++i;
            }
        }
    }
}

void CLua::load_script(int idx)
{
    if (idx == -1)
        return;

    if (this->loaded.at(idx))
        return;

    auto path = get_script_path(idx);

    if (path == "")
        return;

    auto error_load = false;

    this->loaded.at(idx) = true;

    auto on_error = [&error_load](lua_State*, sol::protected_function_result result)
    {
        if (!result.valid())
        {
            sol::error error = result;
            auto log = crypt_str("LUA Error: ") + (std::string)error.what();
            Msg(log, color_t(255, 0, 0));
            error_load = true;
        }

        return result;
    };

    lua.script_file(path, on_error);

    if (error_load || this->loaded.at(idx) == false)
    {
        this->loaded.at(idx) = false;
        return;
    }

    csgo->script_loaded = true;
}

void CLua::unload_script(int idx)
{
    if (idx == -1)
        return;

    if (!loaded.at(idx))
        return;

    if (CLua::Get().events.find(idx) != CLua::Get().events.end())
        CLua::Get().events.at(idx).clear();

    hooks.unregister_hooks(idx);
    loaded.at(idx) = false;
}

std::string CLua::open_script(int idx)
{
    std::ifstream file(paths.at(idx), std::ios::binary);

    if (file.good())
    {
        std::string string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return string;
    }
}

void CLua::save_script(int idx, std::string text)
{
    std::ofstream file(paths.at(idx), std::ios::binary);

    if (file.good())
    {
        file << text;
        file.close();
    }
}

void CLua::reload_all_scripts()
{
    for (auto current : this->scripts)
    {
        if (!loaded.at(get_script_id(current)))
            continue;

        this->unload_script(get_script_id(current));
        this->load_script(get_script_id(current));
    }
}

void CLua::unload_all_scripts()
{
    for (auto s : this->scripts)
        this->unload_script(get_script_id(s));
}

int CLua::get_script_id(const std::string& name)
{
    for (auto i = 0; i < this->scripts.size(); i++)
        if (this->scripts.at(i) == name)
            return i;

    return -1;
}

int CLua::get_script_id_by_path(const std::string& path)
{
    for (auto i = 0; i < this->paths.size(); i++)
        if (this->paths.at(i).string() == path)
            return i;

    return -1;
}

std::string CLua::get_current_script(sol::this_state s)
{
    sol::state_view lua_state(s);
    sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
    std::string source = rs["source"];
    std::string filename = std::filesystem::path(source.substr(1)).filename().string();

    return filename;
}

int CLua::get_current_script_id(sol::this_state s)
{
    return this->get_script_id(get_current_script(s));
}

std::string CLua::get_script_path(const std::string& name)
{
    return this->get_script_path(get_script_id(name));
}

std::string CLua::get_script_path(int id)
{
    if (id == -1)
        return crypt_str("");

    return paths.at(id).string();
}

void CLuaHookManager::register_hook(std::string eventName, int scriptId, sol::protected_function func)
{
    lua_hook_t hk = { scriptId, func };

    this->hooks[eventName].push_back(hk);
}

void CLuaHookManager::unregister_hooks(int scriptId)
{
    for (auto& ev : this->hooks)
    {
        int pos = 0;

        for (auto& hk : ev.second)
        {
            if (hk.index == scriptId)
                ev.second.erase(ev.second.begin() + pos);

            pos++;
        }
    }
}

std::vector<lua_hook_t> CLuaHookManager::get_hooks(std::string eventName)
{
    return this->hooks[eventName];
}