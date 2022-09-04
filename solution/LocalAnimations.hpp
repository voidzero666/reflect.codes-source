#pragma once
#include "Hooks.h"

class LocalAnimations
{
public:
	virtual void Instance();
	virtual void SetupShootPosition();
	virtual void DoAnimationEvent(int type);
	virtual bool GetCachedMatrix(matrix* aMatrix);
	virtual void OnUpdateClientSideAnimation();

	virtual void ResetData();

	virtual std::array < matrix, 128 > GetDesyncMatrix();
	virtual std::array < matrix, 128 > GetLagMatrix();
	virtual std::array < CAnimationLayer, 13 > GetAnimationLayers();
	virtual std::array < CAnimationLayer, 13 > GetFakeAnimationLayers();
};

struct
{
	std::array < CAnimationLayer, 13 > GetAnimOverlays;
	std::array < CAnimationLayer, 13 > m_FakeAnimationLayers;

	std::array < float_t, 24 > m_PoseParameters;
	std::array < float_t, 24 > m_FakePoseParameters;
	CCSGOPlayerAnimState m_FakeAnimationState;

	float_t m_flSpawnTime = 0.0f;
	float_t m_flLowerBodyYaw = 0.0f;
	float_t m_flNextLowerBodyYawUpdateTime = 0.0f;

	std::array < int32_t, 2 > m_iMoveType;
	std::array < int32_t, 2 > m_iFlags;

	std::array < Vector, 128 > m_vecBoneOrigins;
	std::array < Vector, 128 > m_vecFakeBoneOrigins;

	Vector m_vecNetworkedOrigin = Vector(0, 0, 0);
	Vector m_vecShootPosition = Vector(0, 0, 0);

	bool m_bDidShotAtChokeCycle = false;
	Vector m_angShotChokedAngle = Vector(0, 0, 0);

	float_t m_flShotTime = 0.0f;
	Vector m_angForcedAngles = Vector(0, 0, 0);

	std::array < matrix, 128 > m_aMainBones;
	std::array < matrix, 128 > m_aDesyncBones;
	std::array < matrix, 128 > m_aLagBones;
} LocalData;

inline LocalAnimations* g_LocalAnimations = new LocalAnimations();