#pragma once
#include "Hooks.h"
#include "Variables.h"
#include <deque>
#include <vector>

class Chams {
public:
    Chams() noexcept;
    bool render(void*, void*, const ModelRenderInfo_t&, matrix*) noexcept;
private:
    void renderPlayer(IBasePlayer* player) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;

    enum ChamsId {
        ALLIES_VISIBLE = 0,
        ALLIES_OCCLUDED,

        ENEMIES_VISIBLE,
        ENEMIES_OCCLUDED,

        BACKTRACK,
        BACKTACK_OCCLUDED,

        LOCALPLAYER,
        
        WEAPONS,
       
        HANDS,
        
       
        SLEEVES,
        
        DESYNC,
    };

    IMaterial* normal;
    IMaterial* flat;
    IMaterial* animated;
    IMaterial* platinum;
    IMaterial* glass;
    IMaterial* crystal;
    IMaterial* chrome;
    IMaterial* silver;
    IMaterial* gold;
    IMaterial* plastic;
    IMaterial* glow;
    IMaterial* pearlescent;
    IMaterial* metallic;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return animated;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return silver;
        case 8: return gold;
        case 9: return plastic;
        case 10: return glow;
        case 11: return pearlescent;
        case 12: return metallic;
        }
    }

    bool appliedChams;
    void* ctx;
    void* state;
    const ModelRenderInfo_t* info;
    matrix* customBoneToWorld;
    void applyChams(const std::vector<CGlobalVariables::Chams::Material>& chams, bool ignorez = false, int health = 0, matrix* customMatrix = nullptr) noexcept;
};




/*
class CChams
{
private:
    IMaterial* Regular;
    IMaterial* Flat;
    IMaterial* Metallic;
    IMaterial* Glow;
    IMaterial* GlowFade;
    struct CHitMatrixEntry {
        int ent_index;
        ModelRenderInfo_t info;
        DrawModelState_t state;
        matrix pBoneToWorld[128] = {};
        float time;
        matrix model_to_world;
    };
    std::deque<CHitMatrixEntry> m_Hitmatrix;
public:
	void AddHitmatrix(animation* record);
    void OnPostScreenEffects();
	void Draw(void* thisptr, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix* pCustomBoneToWorld);
};

extern CChams* Chams;
*/