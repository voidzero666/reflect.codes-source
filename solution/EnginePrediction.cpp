#include "EnginePrediction.h"
#include "checksum_md5.h"
#include "AntiAims.h"
#include "netvar_manager.h"

c_engine_prediction* engine_prediction = new c_engine_prediction();

//void CEnginePrediction::Start(CUserCmd* cmd, IBasePlayer* local) {
//    old_vars.velocity = csgo->local->GetVelocity();
//    old_vars.origin = csgo->local->GetOrigin();
//    old_vars.curtime = interfaces.global_vars->curtime;
//    old_vars.frametime = interfaces.global_vars->frametime;
//    old_vars.tickcount = interfaces.global_vars->tickcount;
//
//    interfaces.global_vars->curtime = TICKS_TO_TIME(local->GetTickBase());
//    interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
//
//    if (csgo->client_state->iDeltaTick > 0)  //-V807
//        interfaces.prediction->Update(
//            csgo->client_state->iDeltaTick, true,
//            csgo->client_state->nLastCommandAck, csgo->client_state->nLastOutgoingCommand + csgo->client_state->iChokedCommands);
//
//    if (!old_vars.prediction_random_seed)
//        old_vars.prediction_random_seed = *(int**)(csgo->Utils.FindPatternIDA(
//            GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), hs::pred_seed.s().c_str()) + 0x1);
//
//    *old_vars.prediction_random_seed = MD5_PseudoRandom(csgo->cmd->command_number) & INT_MAX;
//
//    if (!old_vars.prediction_player)
//        old_vars.prediction_player = *(int**)(csgo->Utils.FindPatternIDA(
//            GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), hs::pred_player.s().c_str()) + 0x2);
//
//    *old_vars.prediction_player = (uint32_t)csgo->local;
//
//    //interfaces.global_vars->tickcount = TIME_TO_TICKS(interfaces.global_vars->curtime);
//
//    interfaces.game_movement->StartTrackPredictionErrors(local);
//    interfaces.move_helper->SetHost(local);
//    CMoveData move_data;
//    memset(&move_data, 0, sizeof(move_data));
//
//    interfaces.prediction->SetupMove(local, cmd, interfaces.move_helper, &move_data);
//    interfaces.game_movement->ProcessMovement(local, &move_data);
//    interfaces.prediction->FinishMove(local, cmd, &move_data);
//
//    interfaces.game_movement->FinishTrackPredictionErrors(local);
//    interfaces.move_helper->SetHost(nullptr);
//}
//
//void CEnginePrediction::Finish(IBasePlayer* local) {
//
//    *old_vars.prediction_random_seed = -1;
//    *old_vars.prediction_player = 0;
//
//    //interfaces.game_movement->FinishTrackPredictionErrors(local);
//    //interfaces.move_helper->SetHost(nullptr);
//
//    interfaces.global_vars->curtime = old_vars.curtime;
//    interfaces.global_vars->frametime = old_vars.frametime;
//}

CNetData* g_NetData = new CNetData();

void CNetData::Store() {
    if (!csgo->local && !csgo->local->isAlive()) {
        Reset();
        return;
    }

    auto tickbase = csgo->local->GetTickBase();

    auto data = &m_data[tickbase % 150];

    data->m_tickbase = tickbase;
    data->m_punch = csgo->local->GetPunchAngle();
    data->m_punch_vel = csgo->local->GetPunchAngleVel();
    data->m_view_offset = csgo->local->GetViewPunchAngle();
    data->m_view_offset = csgo->local->GetVecViewOffset();
    data->m_velocity = csgo->local->GetVelocity();
    data->m_velocity_modifier = csgo->local->GetVelocityModifier();
    data->m_duck_amount = csgo->local->GetDuckAmount();
    data->m_thirdperson_recoil = csgo->local->GetThirdpersonRecoil();
    data->m_duck_speed = csgo->local->GetDuckSpeed();
    data->m_fall_velocity = csgo->local->GetFallVelocity();
}

void CNetData::Apply() {
    if (!csgo->local && !csgo->local->isAlive()) {
        Reset();
        return;
    }

    auto tickbase = csgo->local->GetTickBase();

    // get current record and validate.
    auto data = &m_data[tickbase % 150];

    if (tickbase != data->m_tickbase)
        return;

    const auto aim_punch_vel_diff = data->m_punch_vel - csgo->local->GetPunchAngleVel();
    const auto aim_punch_diff = data->m_punch - csgo->local->GetPunchAngle();
    const auto viewpunch_diff = data->m_view_punch.x - csgo->local->GetViewPunchAngle().x;
    const auto velocity_diff = data->m_velocity - csgo->local->GetVelocity();
    const auto origin_diff = data->m_origin - csgo->local->GetOrigin();

    //fix viewpunch on hit etc

    if (std::abs(aim_punch_diff.x) <= 0.03125f && std::abs(aim_punch_diff.y) <= 0.03125f && std::abs(aim_punch_diff.z) <= 0.03125f)
        csgo->local->GetPunchAngle() = data->m_punch;

    if (std::abs(aim_punch_vel_diff.x) <= 0.03125f && std::abs(aim_punch_vel_diff.y) <= 0.03125f && std::abs(aim_punch_vel_diff.z) <= 0.03125f)
        csgo->local->GetPunchAngleVel() = data->m_punch_vel;

    if (std::abs(csgo->local->GetVecViewOffset().z - data->m_view_offset.z) <= 0.25f)
        csgo->local->GetVecViewOffset().z = data->m_view_offset.z;

    if (std::abs(viewpunch_diff) <= 0.03125f)
        csgo->local->GetViewPunchAnglePtr()->x = data->m_view_punch.x;

    if (abs(csgo->local->GetDuckAmount() - data->m_duck_amount) <= 0.03125f)
        csgo->local->GetDuckAmount() = data->m_duck_amount;

    if (std::abs(velocity_diff.x) <= 0.03125f && std::abs(velocity_diff.y) <= 0.03125f && std::abs(velocity_diff.z) <= 0.03125f)
        csgo->local->GetVelocity() = data->m_velocity;

    if (abs(csgo->local->GetThirdpersonRecoil() - data->m_thirdperson_recoil) <= 0.03125f)
        csgo->local->GetThirdpersonRecoil() = data->m_thirdperson_recoil;

    if (abs(csgo->local->GetDuckSpeed() - data->m_duck_speed) <= 0.03125f)
        csgo->local->GetDuckSpeed() = data->m_duck_speed;

    if (abs(csgo->local->GetFallVelocity() - data->m_fall_velocity) <= 0.03125f)
        csgo->local->GetFallVelocity() = data->m_fall_velocity;

    if (std::abs(csgo->local->GetVelocityModifier() - data->m_velocity_modifier) <= 0.00625f)
        csgo->local->GetVelocityModifier() = data->m_velocity_modifier;
}

void CNetData::Reset() {
    m_data.fill(StoredData_t());
}

void CNetData::RecordViewmodelValues()
{
    this->viewModelData.m_hWeapon = 0;
    auto viewmodel = csgo->local->GetViewModel();
    if (viewmodel) {
        this->viewModelData.m_hWeapon = viewmodel->GetViewmodelWeapon();
        this->viewModelData.m_nViewModelIndex = viewmodel->GetViewModelIndex();
        this->viewModelData.m_nSequence = viewmodel->GetCurrentSequence();

        this->viewModelData.networkedCycle = viewmodel->GetCurrentCycle();
        this->viewModelData.m_nAnimationParity = viewmodel->GetAnimationParity();
        this->viewModelData.animationTime = viewmodel->GetModelAnimTime();
    }
}

void CNetData::ApplyViewmodelValues()
{
    auto viewmodel = csgo->local->GetViewModel();
    if (viewmodel) {
        if (this->viewModelData.m_nSequence == viewmodel->GetCurrentSequence()
            && this->viewModelData.m_hWeapon == viewmodel->GetViewmodelWeapon()
            && this->viewModelData.m_nAnimationParity == viewmodel->GetAnimationParity()) {
            viewmodel->GetCurrentCycle() = this->viewModelData.networkedCycle;
            viewmodel->GetModelAnimTime() = this->viewModelData.animationTime;
        }
    }
}