#include "Knifebot.h"
#include "Ragebot.h"

void CKnifebot::Run()
{
	work = false;
	auto can_shoot = []() {
		auto weapon = csgo->weapon;
		if (!csgo->local || !csgo->weapon || csgo->weapon->InReload() || csgo->weapon->GetAmmo(false) == 0)
			return false;

		if (csgo->cmd->weaponselect || csgo->local->GetFlags() & FL_FROZEN || csgo->game_rules->IsFreezeTime())
			return false;

		return (interfaces.global_vars->curtime >= weapon->NextPrimaryAttack()
			|| interfaces.global_vars->curtime >= weapon->NextSecondaryAttack());
	};

	if (!csgo->weapon->IsKnife())
		return;
	if (!can_shoot())
		return;
	IBasePlayer* best_entity = nullptr;
	float best_distance = 99999.f;
	for (auto i = 0; i <= interfaces.global_vars->maxClients; i++)
	{
		auto pEntity = interfaces.ent_list->GetClientEntity(i);
		if (pEntity == nullptr)
			continue;
		if (!pEntity->IsValid())
			continue;
		Vector stomach = Ragebot::Get().GetPoint(pEntity, (int)CSGOHitboxID::Stomach, pEntity->GetBoneCache().Base());
		if (stomach == Vector(0, 0, 0))
			continue;
		float dist = stomach.DistTo(csgo->local->GetEyePosition());
		if (best_distance >= dist)
		{
			best_entity = pEntity;
			best_distance = dist;
		}
	}

	if (best_entity != nullptr)
	{
		int work = 0;
		for (int i = 0; i < 2; i++) {
			animation* record = nullptr;
			if (i == 0)
				record = g_Animfix->get_latest_animation(best_entity);
			else if (i == 1)
				record = g_Animfix->get_oldest_animation(best_entity);

			if (!record || !record->player)
				return;
			Vector stomach = Ragebot::Get().GetPoint(record->player, (int)CSGOHitboxID::Stomach, record->bones);
			if (stomach == Vector(0, 0, 0))
				return;

			Vector AimAngle = Math::CalculateAngle(csgo->local->GetEyePosition(), stomach);
			if (i == 1)
				best_distance = stomach.DistTo(csgo->local->GetEyePosition());
			if (best_entity->GetHealth() <= 35 && best_distance < 65) {
				csgo->cmd->buttons |= IN_ATTACK;
				csgo->cmd->viewangles = AimAngle;
			}
			else {
				if (Math::IsNearEqual(best_entity->GetEyeAngles().y, Math::CalculateAngle(csgo->local->GetOrigin(), best_entity->GetOrigin()).y, 45.f))
				{
					if (best_distance < 50)
					{
						/*csgo->cmd->buttons |= IN_ATTACK2;
						csgo->cmd->viewangles = AimAngle;*/
						work = 1;
					}
				}
				else
				{
					if (best_distance < 50)
						work = 2;
					else if (best_distance < 65)
						work = 1;
				}
			}

			if (work > 0) {
				if (work == 1)
					csgo->cmd->buttons |= IN_ATTACK;
				else if (work == 2)
					csgo->cmd->buttons |= IN_ATTACK2;

				csgo->cmd->viewangles = AimAngle;
				csgo->cmd->tick_count = TIME_TO_TICKS(record->sim_time + Ragebot::Get().LerpTime());
				break;
			}
		}
	}
}