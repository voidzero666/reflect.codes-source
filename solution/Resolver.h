#pragma once
#include <optional>
struct animation;
class CResolver
{
private:
	float GetLeftYaw(IBasePlayer*);
	float GetRightYaw(IBasePlayer*);
	int LastFreestandSide[64];
	bool UseFreestandAngle[65];
public:
	int FreestandSide[64];
	int resolvedForward[64];
	float ResolvedAngle[65];
	float GetAngle(IBasePlayer*);
	float GetForwardYaw(IBasePlayer*);
	float GetAwayAngle(IBasePlayer* player);
	float GetBackwardYaw(IBasePlayer*);
	float LastAppliedAngle[64];
	std::optional<float> Do(IBasePlayer*, animation*, animation*, float);
	void DetectSide(IBasePlayer*, int*);
};
extern CResolver* resolver;
extern std::string ResolverMode[65];
extern std::string LastAppliedAngle2[65];