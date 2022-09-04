#pragma once
#include "Vector.h"
#include "cvar.h"
#include "UTLVector.h"
#include "itemdefinitions.hpp"

#pragma region Defines

//LIFESTATE
#define	LIFE_ALIVE				0
#define	LIFE_DYING				1
#define	LIFE_DEAD				2
#define LIFE_RESPAWNABLE		3
#define LIFE_DISCARDBODY		4

//Player flags
#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define	FL_WATERJUMP			(1<<3)	// player jumping out of water
#define FL_ONTRAIN				(1<<4) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<5)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<7) // Player can't move, but keeps key inputs for controlling another entity
#define	FL_CLIENT				(1<<8)	// Is a player
#define FL_FAKECLIENT			(1<<9)	// Fake client, simulated server side; don't send network messages to them
#define	FL_INWATER				(1<<10)	// In water

//USERCMD BUTTONS
#define IN_ATTACK		(1 << 0) 
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client_panorama.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2

#pragma endregion

#pragma region Classes

class CCSGameRules
{
public:
	bool IsValveDS();
	bool IsBombDropped();
	bool IsBombPlanted();
	bool IsFreezeTime();
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};
enum class CSGOHitboxID
{
	Head = 0,
	Neck,
	Pelvis,
	Stomach,
	LowerChest,
	Chest,
	UpperChest,
	RightThigh,
	LeftThigh,
	RightShin,
	LeftShin,
	LeftFoot,
	RightFoot,
	RightHand,
	LeftHand,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
};
enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};


class VarMapEntry_t
{
public:
	unsigned short		type;
	unsigned short		m_bNeedsToInterpolate;	// Set to false when this var doesn't
												// need Interpolate() called on it anymore.
	void				*data;
	void	*watcher;
};

struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	VarMapEntry_t* m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};
struct player_info_t
{
	__int64         unknown;            //0x0000 
	union
	{
		__int64       steamID64;          //0x0008 - SteamID64
		struct
		{
			__int32     xuidLow;
			__int32     xuidHigh;
		};
	};
	char            name[128];        //0x0010 - Player Name
	int             userId;             //0x0090 - Unique Server Identifier
	char            szSteamID[20];      //0x0094 - STEAM_X:Y:Z
	char            pad_0x00A8[0x10];   //0x00A8
	unsigned long   iSteamID;           //0x00B8 - SteamID 
	char            friendsName[128];
	bool            fakeplayer;
	bool            ishltv;
	unsigned int    customFiles[4];
	unsigned char   filesDownloaded;
	//unsigned __int64 unknown;
	//unsigned __int64 xuid;
	//char name[32];

	//char unknown01[96];

	//int userID;
	//char guid[32 + 1];
	//__int32 friendsID;
	//char friendsName[32];
	//bool fakeplayer;
	//bool ishltv;
	//unsigned long customFiles[4];
	//unsigned char filesDownloaded;
	//char pad[304];
};

#pragma endregion

#pragma region ShitClasses

struct datamap_t;
class typedescription_t;

enum
{
	PC_NON_NETWORKED_ONLY = 0,
	PC_NETWORKED_ONLY,

	PC_COPYTYPE_COUNT,
	PC_EVERYTHING = PC_COPYTYPE_COUNT,
};

enum
{
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,

	// Must be last
	TD_OFFSET_COUNT,
};


class typedescription_t
{
public:
	int32_t fieldType; //0x0000
	char* fieldName; //0x0004
	int fieldOffset[TD_OFFSET_COUNT]; //0x0008
	int16_t fieldSize_UNKNWN; //0x0010
	int16_t flags_UNKWN; //0x0012
	char pad_0014[12]; //0x0014
	datamap_t* td; //0x0020
	char pad_0024[24]; //0x0024
}; //Size: 0x003C

struct datamap_t
{
	typedescription_t    *dataDesc;
	int                    dataNumFields;
	char const            *dataClassName;
	datamap_t            *baseMap;

	bool                chains_validated;
	// Have the "packed" offsets been computed
	bool                packed_offsets_computed;
	int                    packed_size;
};


//class CCSWeaponInfo
//{
//public:
//	std::uint8_t pad_0x0000[0x4]; //0x0000
//	char* WeaponName; //0x0004
//	std::uint8_t pad_0x0008[0xC]; //0x0008
//	std::uint32_t MaxClip; //0x0014
//	std::uint8_t pad_0x0018[0x68]; //0x0018
//	char* AmmoName; //0x0080
//	char* AmmoName_2; //0x0084
//	char* HudName; //0x0088
//	char* WeaponId; //0x008C
//	std::uint8_t pad_0x0090[0x3C]; //0x0090
//	std::uint32_t type; //0x00CC
//	std::uint32_t WeaponPrice; //0x00D0
//	std::uint32_t WeaponReward; //0x00D4
//	std::uint8_t pad_0x00D8[0x14]; //0x00D8
//	std::uint8_t WeaponFullAuto; //0x00EC
//	std::uint8_t pad_0x00ED[0x3]; //0x00ED
//	std::uint32_t damage; //0x00F0
//	float armor_ratio; //0x00F4
//	std::uint32_t WeaponBullets; //0x00F8
//	float penetration; //0x00FC
//	std::uint8_t pad_0x0100[0x8]; //0x0100
//	float range; //0x0108
//	float range_modifier; //0x010C
//	std::uint8_t pad_0x0110[0x20]; //0x0110
//	float MaxSpeed; //0x0130
//	float MaxSpeedAlt; //0x0134
//	std::uint8_t pad_0x0138[0x108]; //0x0138
//};

// Created with ReClass.NET by KN4CK3R
class CCSWeaponInfo
{
public:
	PAD(20)
	int m_iMaxClip1;
	PAD(112)
	char* name;
	PAD(60)
	int m_iWeaponType;
	PAD(4)
	int price;
	PAD(0x8)
	float m_flCycleTime;
	PAD(12)
	bool m_bFullAuto;
	PAD(3)
	int m_iDamage; //
	float flHeadshotMultiplier; //headshot multiplier
	float m_flArmorRatio; 
	int m_iBullets;
	float m_flPenetration;
	float		flinch_velocity_modifier_large;
	float		flinch_velocity_modifier_small;
	float m_flRange;
	float m_flRangeModifier;
	PAD(32)
	float m_flMaxSpeed;
	float m_flMaxSpeedAlt;
	/*
	int m_iBullets; //wrong
	float m_flArmorRatio;
	float m_flPenetration;
	float		flinch_velocity_modifier_large;
	float		flinch_velocity_modifier_small;
	float m_flRange;
	float m_flRangeModifier;
	PAD(0x10)
	bool m_bHasSilencer;
	PAD(15)
	float m_flMaxSpeed;
	float m_flMaxSpeedAlt;
	PAD(100)
	float recoilMagnitude;
	float recoilMagnitudeAlt;
	PAD(0x14)
	float recoveryTimeStand;
	*/
};

class CGameTrace;
struct Ray_t;
typedef CGameTrace trace_t;
//struct model_t
//{
//	void*   fnHandle;               //0x0000 
//	char    name[260];            //0x0004 
//	__int32 nLoadFlags;             //0x0108 
//	__int32 nServerCount;           //0x010C 
//	__int32 type;                   //0x0110 
//	__int32 flags;                  //0x0114 
//	Vector  vecMins;                //0x0118 
//	Vector  vecMaxs;                //0x0124 
//	float   radius;                 //0x0130 
//	char    pad[0x1C];              //0x0134
//};

struct brushdata_t
{
	void* pShared;
	int				firstmodelsurface;
	int				nummodelsurfaces;

	// track the union of all lightstyles on this brush.  That way we can avoid
	// searching all faces if the lightstyle hasn't changed since the last update
	int				nLightstyleLastComputedFrame;
	unsigned short	nLightstyleIndex;	// g_ModelLoader actually holds the allocated data here
	unsigned short	nLightstyleCount;

	unsigned short	renderHandle;
	unsigned short	firstnode;
};

struct spritedata_t
{
	int				numframes;
	int				width;
	int				height;
	void* sprite;
};

struct model_t
{
	void* fnHandle;
	char name[260];
	__int32 nLoadFlags;
	__int32 nServerCount;
	__int32 type;
	__int32 flags;
	Vector mins, maxs;
	float radius;
	void* m_pKeyValues;
	union
	{
		brushdata_t brush;
		MDLHandle_t studio;
		spritedata_t sprite;
	};
};

class IHandleEntity;
class ICollideable
{
public:
	virtual IHandleEntity*      GetEntityHandle() = 0;
	virtual Vector&				OBBMins() const = 0;
	virtual Vector&				OBBMaxs() const = 0;
	virtual void world_space_trigger_bounds(Vector* mins, Vector* maxs) const = 0;
	virtual bool test_collision(const Ray_t& ray, unsigned int mask, trace_t& trace) = 0;
	virtual bool test_hitboxes(const Ray_t& ray, unsigned int mask, trace_t& trace) = 0;
	virtual int get_collision_model_index() = 0;
	virtual const model_t* get_collision_model() = 0;
	virtual Vector&	get_collision_origin() const = 0;
	virtual Vector& get_collision_angles() const = 0;
	virtual const matrix& collision_to_world_transform() const = 0;
	virtual int get_solid() const = 0;
	virtual int get_solid_flags() const = 0;
	virtual void* get_client_unknown() = 0;
	virtual int get_collision_group() const = 0;
	virtual void world_space_surrounding_bounds(Vector* mins, Vector* maxs) = 0;
	virtual bool should_touch_trigger(int flags) const = 0;
	virtual const matrix* get_root_parent_to_world_transform() const = 0;
};

class IBaseCombatWeapon;
class IBasePlayer;
class IClientUnknown;

typedef unsigned short ModelInstanceHandle_t;
typedef unsigned char uint8;
class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector& const GetRenderOrigin(void) = 0;
	virtual Vector& const GetRenderAngles(void) = 0;
	virtual bool ShouldDraw(void) = 0;
	virtual int GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void Unused(void) const
	{
	}
	virtual HANDLE GetShadowHandle() const = 0;
	virtual HANDLE& RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int DrawModel(int flags, const int /*RenderableInstance_t*/ & instance) = 0;
	virtual int GetBody() = 0;
	virtual void Getcolor_tModulation(float* color) = 0;
	virtual bool LODTest() = 0;
	virtual void SetupWeights(const matrix* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void DoAnimationEvents(void) = 0;
	virtual void* /*IPVSNotify*/ GetPVSNotifyInterface() = 0;
	virtual void GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void GetShadowRenderBounds(Vector& mins, Vector& maxs, int /*ShadowType_t*/ shadowType) = 0;
	virtual bool ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool GetShadowCastDistance(float* pDist, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool GetShadowCastDirection(Vector* pDirection, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual int /*ShadowType_t*/ ShadowCastType() = 0;
	virtual void CreateModelInstance() = 0;
	virtual HANDLE GetModelInstance() = 0;
	virtual const matrix& RenderableToWorldTransform() = 0;
	virtual int LookupAttachment(const char* pAttachmentName) = 0;
	virtual bool GetAttachment(int number, Vector& origin, Vector& angles) = 0;
	virtual bool GetAttachment(int number, matrix& matrix) = 0;
	virtual float* GetRenderClipPlane(void) = 0;
	virtual int GetSkin() = 0;
	virtual void OnThreadedDrawSetup() = 0;
	virtual bool UsesFlexDelayedWeights() = 0;
	virtual void RecordToolMessage() = 0;
	virtual bool ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8 OverrideAlphaModulation(uint8 nAlpha) = 0;
	virtual uint8 OverrideShadowAlphaModulation(uint8 nAlpha) = 0;
};
class IClientNetworkable
{
public:
	virtual IClientUnknown*  GetIClientUnknown() = 0;
	virtual void             Release() = 0;
	virtual ClientClass*     GetClientClass() = 0;
	virtual void             NotifyShouldTransmit(int state) = 0;
	virtual void             OnPreDataChanged(int updateType) = 0;
	virtual void             OnDataChanged(int updateType) = 0;
	virtual void             PreDataUpdate(int updateType) = 0;
	virtual void             PostDataUpdate(int updateType) = 0;
	virtual void             __unkn(void) = 0;
	virtual bool             IsDormant(void) = 0;
	virtual int              EntIndex(void) const = 0;
	virtual void             ReceiveMessage() = 0;
	virtual void*            GetDataTableBasePtr() = 0;
	virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientThinkable;
using CBaseHandle = uint32_t;
class IHandleEntity
{
public:
	virtual ~IHandleEntity() = default;
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};



class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable*       GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IBasePlayer*		GetIClientEntity() = 0;
	virtual IBasePlayer*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual void*				GetThinkHandle() = 0;
	virtual void				SetThinkHandle(void* hThink) = 0;
	virtual void				Release() = 0;
};
struct studiohdr_t;
struct mstudiohitboxset_t;
struct mstudiobbox_t
{
	//int bone;
	//int group; // intersection group
	//Vector bbmin; // bounding box 
	//Vector bbmax;
	//int hitboxnameindex; // offset to the name of the hitbox.
	//Vector rotation;
	//float radius;
	//int pad2[0x10];
	int     bone;                 // 0x0000
	int     group;                // 0x0004
	Vector  bbmin;                 // 0x0008
	Vector  bbmax;                 // 0x0014
	int     name_id;				// 0x0020
	Vector   rotation;                // 0x0024
	float   radius;               // 0x0030
	PAD(0x10);                    // 0x0034
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	//    DATA_UPDATE_ENTERED_PVS,
	DATA_UPDATE_DATATABLE_CHANGED
	//    DATA_UPDATE_LEFT_PVS,
	//DATA_UPDATE_DESTROYED,
};

class CAnimationLayer
{
public:
	bool            m_bClientBlend;            //0x00
	float            m_flBlendIn;                //0x04
	void* m_pStudioHdr;                //0x08
	int                m_nDispatchedSrc;            //0x0C
	int                m_nDispatchedDst;            //0x10
	std::uintptr_t  m_iOrder;                    //0x14
	std::uintptr_t  m_nSequence;                //0x18
	float            m_flPrevCycle;            //0x1C
	float            m_flWeight;                //0x20
	float            m_flWeightDeltaRate;        //0x24
	float            m_flPlaybackRate;            //0x28
	float            m_flCycle;                //0x2C
	void* m_pOwner;                    //0x30
	int                m_nInvalidatePhysicsBits;    //0x34
};


enum animStateLayer
{
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT,
};

//class CAnimationLayer
//{
//	char pad_0000[20] = {};
//public:
//	uint32_t order{};
//	uint32_t sequence{};
//	float prev_cycle{};
//	float weight{};
//	float weight_delta_rate{};
//	float playback_rate{};
//	float cycle{};
//	void* owner{};
//private:
//	char pad_0038[4] = {};
//};
class CCSGOPlayerAnimState1
{
public:
	char pad[3];
	char bUnknown; //0x4
	char pad2[87];
	void* m_pLastBoneSetupWeapon; //0x5C
	void* m_pBaseEntity; //0x60
	void* m_pActiveWeapon; //0x64
	void* m_pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flUpdateTimeDelta; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float m_velocity; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	bool m_bOnGround; //0x108
	bool m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flMovingFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];

	float& time_since_in_air()
	{
		return *(float*)((uintptr_t)this + 0x110);
	}

	float& yaw_desync_adjustment()
	{
		return *(float*)((uintptr_t)this + 0x334);
	}
};

struct proceduralFoot
{
	Vector m_vecPosAnim;
	Vector m_vecPosAnimLast;
	Vector m_vecPosPlant;
	Vector m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;
};//Size: 0x38(56)


struct CCSGOPlayerAnimState //This is the client one
{
	std::byte pad[4];
	//PAD(4);
	bool					m_bFirstRunSinceInit; //4 = 0x4
	std::byte				pad1[91]; //PAD(91); //5
	void*					m_pBaseEntity; //96 = 0x60
	void*					m_pWeapon; //100 = 0x64
	void*					m_pWeaponLast; //104 = 0x68

	float					m_flLastUpdateTime; //108 = 0x6C LastClientSideAnimationUpdateTime
	int						m_nLastUpdateFrame; //112 = 0x70 LastClientSideAnimationUpdateFramecount
	float					m_flLastUpdateIncrement; //116 = 0x74

	float					m_flEyeYaw; //120 = 0x78
	float					m_flEyePitch; //124 = 0x7C
	float					m_flFootYaw; //128 = 0x80 //GoalFeetYaw;
	float					m_flFootYawLast; //132 = 0x84 //CurrentFeetYaw
	float					m_flMoveYaw; //136 = 0x88
	float					m_flMoveYawIdeal; //140 = 0x8C
	float					m_flMoveYawCurrentToIdeal; //144 = 0x90
	float					m_flTimeToAlignLowerBody; //148 = 0x94

	float					m_flPrimaryCycle; //152 = 0x98
	float					m_flMoveWeight; //156 = 09C
	float					m_flMoveWeightSmoothed; //T 160
	float					m_flAnimDuckAmount; //T 164 duckAmount
	float					m_flDuckAdditional; //168 = 0xA8 landingDuckAmount
	float					m_flRecrouchWeight; //T 172

	Vector					m_vecPositionCurrent; //T 176(X),180(Y),184(Z) Origin
	Vector					m_vecPositionLast; //T 188(X),192(Y),196(Z) LastOrigin

	Vector					m_vecVelocity; //T 200(X),204(Y),208(Z)
	Vector					m_vecVelocityNormalized; //T 212(X),216(Y),220(Z)
	Vector					m_vecVelocityNormalizedNonZero; //T 224(X),228(Y),232(Z)
	float					m_flVelocityLengthXY; //236 = 0xEC
	float					m_flVelocityLengthZ; // T 240

	float					m_flSpeedAsPortionOfRunTopSpeed; //T 244
	float					m_flSpeedAsPortionOfWalkTopSpeed; //T 248 footSpeed
	float					m_flSpeedAsPortionOfCrouchTopSpeed; //T 252 footSpeed2

	float					m_flDurationMoving; //T 256 TimeSinceStartedMoving
	float					m_flDurationStill; //T 260 TimeSinceStoppedMoving

	bool					m_bOnGround; //264 = 0x108 OnGround
	bool					m_bLanding; //265 = 0x109 InHitGroundAnimation
	std::byte pad2[2]; //PAD(2);
	float					m_flJumpToFall; //268
	float					m_flDurationInAir; //272 = 0x110
	float					m_flLeftGroundHeight; //276 = 0x114
	float					m_flLandAnimMultiplier; //280 = 0x118 HeadHeightOrOffsetFromHittingGroundAnimation

	float					m_flWalkToRunTransition; //284 = 0x11C stopToFullRunningFraction

	bool					m_bLandedOnGroundThisFrame; //288 = 0x120
	bool					m_bLeftTheGroundThisFrame; //289 = 0x121
	std::byte pad3[2]; //PAD(2);
	float					m_flInAirSmoothValue; //292 = 0x124

	bool					m_bOnLadder; //296 = 0x128
	std::byte pad4[3]; //PAD(3);
	float					m_flLadderWeight; //300 = 0x12C
	float					m_flLadderSpeed; //304 = 0x130

	bool					m_bWalkToRunTransitionState; //308 = 0x134

	bool					m_bDefuseStarted; //T 309
	bool					m_bPlantAnimStarted;//T 310
	bool					m_bTwitchAnimStarted;//T 311
	bool					m_bAdjustStarted;//T 312
	std::byte pad5[3]; //PAD(3);
	UtlVector<uint16_t>	m_ActivityModifiers;//T 316 (size 20)

	float					m_flNextTwitchTime;//T 336

	float					m_flTimeOfLastKnownInjury;//T 340

	float					m_flLastVelocityTestTime; //344 = 0x158
	Vector					m_vecVelocityLast; //T 348(X), 352(Y), 356(Z)
	Vector					m_vecTargetAcceleration; //T 360(X), 364(Y), 368(Z)
	Vector					m_vecAcceleration; //T 372(X), 376(Y), 380(Z)
	float					m_flAccelerationWeight; //T 384

	float					m_flAimMatrixTransition;//T 388
	float					m_flAimMatrixTransitionDelay; //T 392

	bool					m_bFlashed;// T 396
	std::byte pad6[3];//PAD(3);
	float					m_flStrafeChangeWeight; //400 = 0x190
	float					m_flStrafeChangeTargetWeight; // T 404
	float					m_flStrafeChangeCycle; //408 = 0x198
	int						m_nStrafeSequence; //412 = 0x19C
	bool					m_bStrafeChanging; //416 = 0x1A0
	std::byte pad7[3]; //PAD(3);
	float					m_flDurationStrafing; //420 = 0x1A4

	float					m_flFootLerp; // T 424

	bool					m_bFeetCrossed; // T 428

	bool					m_bPlayerIsAccelerating; //429 = 0x1AD
	std::byte pad8[2]; //PAD(2);
	std::byte pad9[240]; //PAD(240); //animstatePoseParamCache m_tPoseParamMappings[20];//T 432

	float					m_flDurationMoveWeightIsTooHigh; //672 = 0x2A0
	float					m_flStaticApproachSpeed; //676 =  0x2A4

	int						m_nPreviousMoveState; //680 = 0x2A8
	float					m_flStutterStep; //684 = 0x2AC

	float					m_flActionWeightBiasRemainder; //688 = 0x2B0 //Literally useless??

	proceduralFoot	m_footLeft; //692 = 0x2B4
	proceduralFoot	m_footRight;//T 748

	float					m_flCameraSmoothHeight; //804 = 0x324
	bool					m_bSmoothHeightValid; //808 = 0x328
	std::byte pad10[3]; //PAD(3);
	float					m_flLastTimeVelocityOverTen; //T 812 = 0x329
	std::byte pad11[4]; // Thanks UKnown Chit!!
	float					m_flAimYawMin; //T 816
	float					m_flAimYawMax; //T 820
	float					m_flAimPitchMin; //T 824
	float					m_flAimPitchMax; //T 828

	int						m_nAnimstateModelVersion; //832 = 0x340

	float& time_since_in_air()
	{
		return *(float*)((uintptr_t)this + 0x110);
	}

	float& yaw_desync_adjustment()
	{
		return *(float*)((uintptr_t)this + 0x334);
	}

};


class CBoneAccessor
{
public:
	inline matrix* get_bone_array_for_write(void) const
	{
		return m_pBones;
	}
	inline void set_bone_array_for_write(matrix* bonearray)
	{
		m_pBones = bonearray;
	}
	alignas(16) matrix* m_pBones;

	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};
#pragma endregion

#define offset(func, type, offset) type& func { return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }

#define NO_INTERP 8
enum
{
	EFL_KILLME = (1 << 0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT = (1 << 1),	// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE = (1 << 2),	// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES = (1 << 3),	// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4), // This is a special entity that should not be deleted when we restart entities only

	EFL_DIRTY_SHADOWUPDATE = (1 << 5),	// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY = (1 << 6),	// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

	EFL_BOT_FROZEN = (1 << 8),	// This is set on bots that are frozen.
	EFL_SERVER_ONLY = (1 << 9),	// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH = (1 << 10), // Don't attach the edict; we're doing it explicitly

	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM = (1 << 11),
	EFL_DIRTY_ABSVELOCITY = (1 << 12),
	EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	EFL_HAS_PLAYER_CHILD = (1 << 16),	// One of the child entities is a player.

	EFL_IN_SKYBOX = (1 << 17),	// This is set if the entity detects that it's in the skybox.
	// This forces it to pass the "in PVS" for transmission.
	EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
	EFL_TOUCHING_FLUID = (1 << 19),	// Used to determine if an entity is floating
	
	// FIXME: Not really sure where I should add this...
	EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
	EFL_NO_ROTORWASH_PUSH = (1 << 21),		// I shouldn't be pushed by the rotorwash
	EFL_NO_THINK_FUNCTION = (1 << 22),
	EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),
	
	EFL_CHECK_UNTOUCH = (1 << 24),
	EFL_DONTBLOCKLOS = (1 << 25),		// I shouldn't block NPC line-of-sight
	EFL_DONTWALKON = (1 << 26),		// NPC;s should not walk on this entity
	EFL_NO_DISSOLVE = (1 << 27),		// These guys shouldn't dissolve
	EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),	// Mega physcannon can't ragdoll these guys.
	EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),	// Don't adjust this entity's velocity when transitioning into water
	EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),	// Physcannon can't pick these up or punt them
	EFL_NO_DAMAGE_FORCES = (1 << 31),	// Doesn't accept forces from physics damage
};

class c_studio_hdr;
class Quaternion;

enum InvalidatePhysicsBits_t
{
	POSITION_CHANGED = 0x1,
	ANGLES_CHANGED = 0x2,
	VELOCITY_CHANGED = 0x4,
	ANIMATION_CHANGED = 0x8,		// Means cycle has changed, or any other event which would cause render-to-texture shadows to need to be rerendeded
	BOUNDS_CHANGED = 0x10,		// Means render bounds have changed, so shadow decal projection is required, etc.
	SEQUENCE_CHANGED = 0x20,		// Means sequence has changed, only interesting when surrounding bounds depends on sequence																				
};

enum Activity
{
	ACT_RESET,
	ACT_IDLE,
	ACT_TRANSITION,
	ACT_COVER,
	ACT_COVER_MED,
	ACT_COVER_LOW,
	ACT_WALK,
	ACT_WALK_AIM,
	ACT_WALK_CROUCH,
	ACT_WALK_CROUCH_AIM,
	ACT_RUN,
	ACT_RUN_AIM,
	ACT_RUN_CROUCH,
	ACT_RUN_CROUCH_AIM,
	ACT_RUN_PROTECTED,
	ACT_SCRIPT_CUSTOM_MOVE,
	ACT_RANGE_ATTACK1,
	ACT_RANGE_ATTACK2,
	ACT_RANGE_ATTACK1_LOW,
	ACT_RANGE_ATTACK2_LOW,
	ACT_DIESIMPLE,
	ACT_DIEBACKWARD,
	ACT_DIEFORWARD,
	ACT_DIEVIOLENT,
	ACT_DIERAGDOLL,
	ACT_FLY,
	ACT_HOVER,
	ACT_GLIDE,
	ACT_SWIM,
	ACT_JUMP,
	ACT_HOP,
	ACT_LEAP,
	ACT_LAND,
	ACT_CLIMB_UP,
	ACT_CLIMB_DOWN,
	ACT_CLIMB_DISMOUNT,
	ACT_SHIPLADDER_UP,
	ACT_SHIPLADDER_DOWN,
	ACT_STRAFE_LEFT,
	ACT_STRAFE_RIGHT,
	ACT_ROLL_LEFT,
	ACT_ROLL_RIGHT,
	ACT_TURN_LEFT,
	ACT_TURN_RIGHT,
	ACT_CROUCH,
	ACT_CROUCHIDLE,
	ACT_STAND,
	ACT_USE,
	ACT_ALIEN_BURROW_IDLE,
	ACT_ALIEN_BURROW_OUT,
	ACT_SIGNAL1,
	ACT_SIGNAL2,
	ACT_SIGNAL3,
	ACT_SIGNAL_ADVANCE,
	ACT_SIGNAL_FORWARD,
	ACT_SIGNAL_GROUP,
	ACT_SIGNAL_HALT,
	ACT_SIGNAL_LEFT,
	ACT_SIGNAL_RIGHT,
	ACT_SIGNAL_TAKECOVER,
	ACT_LOOKBACK_RIGHT,
	ACT_LOOKBACK_LEFT,
	ACT_COWER,
	ACT_SMALL_FLINCH,
	ACT_BIG_FLINCH,
	ACT_MELEE_ATTACK1,
	ACT_MELEE_ATTACK2,
	ACT_RELOAD,
	ACT_RELOAD_START,
	ACT_RELOAD_FINISH,
	ACT_RELOAD_LOW,
	ACT_ARM,
	ACT_DISARM,
	ACT_DROP_WEAPON,
	ACT_DROP_WEAPON_SHOTGUN,
	ACT_PICKUP_GROUND,
	ACT_PICKUP_RACK,
	ACT_IDLE_ANGRY,
	ACT_IDLE_RELAXED,
	ACT_IDLE_STIMULATED,
	ACT_IDLE_AGITATED,
	ACT_IDLE_STEALTH,
	ACT_IDLE_HURT,
	ACT_WALK_RELAXED,
	ACT_WALK_STIMULATED,
	ACT_WALK_AGITATED,
	ACT_WALK_STEALTH,
	ACT_RUN_RELAXED,
	ACT_RUN_STIMULATED,
	ACT_RUN_AGITATED,
	ACT_RUN_STEALTH,
	ACT_IDLE_AIM_RELAXED,
	ACT_IDLE_AIM_STIMULATED,
	ACT_IDLE_AIM_AGITATED,
	ACT_IDLE_AIM_STEALTH,
	ACT_WALK_AIM_RELAXED,
	ACT_WALK_AIM_STIMULATED,
	ACT_WALK_AIM_AGITATED,
	ACT_WALK_AIM_STEALTH,
	ACT_RUN_AIM_RELAXED,
	ACT_RUN_AIM_STIMULATED,
	ACT_RUN_AIM_AGITATED,
	ACT_RUN_AIM_STEALTH,
	ACT_CROUCHIDLE_STIMULATED,
	ACT_CROUCHIDLE_AIM_STIMULATED,
	ACT_CROUCHIDLE_AGITATED,
	ACT_WALK_HURT,
	ACT_RUN_HURT,
	ACT_SPECIAL_ATTACK1,
	ACT_SPECIAL_ATTACK2,
	ACT_COMBAT_IDLE,
	ACT_WALK_SCARED,
	ACT_RUN_SCARED,
	ACT_VICTORY_DANCE,
	ACT_DIE_HEADSHOT,
	ACT_DIE_CHESTSHOT,
	ACT_DIE_GUTSHOT,
	ACT_DIE_BACKSHOT,
	ACT_FLINCH_HEAD,
	ACT_FLINCH_CHEST,
	ACT_FLINCH_STOMACH,
	ACT_FLINCH_LEFTARM,
	ACT_FLINCH_RIGHTARM,
	ACT_FLINCH_LEFTLEG,
	ACT_FLINCH_RIGHTLEG,
	ACT_FLINCH_PHYSICS,
	ACT_FLINCH_HEAD_BACK,
	ACT_FLINCH_HEAD_LEFT,
	ACT_FLINCH_HEAD_RIGHT,
	ACT_FLINCH_CHEST_BACK,
	ACT_FLINCH_STOMACH_BACK,
	ACT_FLINCH_CROUCH_FRONT,
	ACT_FLINCH_CROUCH_BACK,
	ACT_FLINCH_CROUCH_LEFT,
	ACT_FLINCH_CROUCH_RIGHT,
	ACT_IDLE_ON_FIRE,
	ACT_WALK_ON_FIRE,
	ACT_RUN_ON_FIRE,
	ACT_RAPPEL_LOOP,
	ACT_180_LEFT,
	ACT_180_RIGHT,
	ACT_90_LEFT,
	ACT_90_RIGHT,
	ACT_STEP_LEFT,
	ACT_STEP_RIGHT,
	ACT_STEP_BACK,
	ACT_STEP_FORE,
	ACT_GESTURE_RANGE_ATTACK1,
	ACT_GESTURE_RANGE_ATTACK2,
	ACT_GESTURE_MELEE_ATTACK1,
	ACT_GESTURE_MELEE_ATTACK2,
	ACT_GESTURE_RANGE_ATTACK1_LOW,
	ACT_GESTURE_RANGE_ATTACK2_LOW,
	ACT_MELEE_ATTACK_SWING_GESTURE,
	ACT_GESTURE_SMALL_FLINCH,
	ACT_GESTURE_BIG_FLINCH,
	ACT_GESTURE_FLINCH_BLAST,
	ACT_GESTURE_FLINCH_BLAST_SHOTGUN,
	ACT_GESTURE_FLINCH_BLAST_DAMAGED,
	ACT_GESTURE_FLINCH_BLAST_DAMAGED_SHOTGUN,
	ACT_GESTURE_FLINCH_HEAD,
	ACT_GESTURE_FLINCH_CHEST,
	ACT_GESTURE_FLINCH_STOMACH,
	ACT_GESTURE_FLINCH_LEFTARM,
	ACT_GESTURE_FLINCH_RIGHTARM,
	ACT_GESTURE_FLINCH_LEFTLEG,
	ACT_GESTURE_FLINCH_RIGHTLEG,
	ACT_GESTURE_TURN_LEFT,
	ACT_GESTURE_TURN_RIGHT,
	ACT_GESTURE_TURN_LEFT45,
	ACT_GESTURE_TURN_RIGHT45,
	ACT_GESTURE_TURN_LEFT90,
	ACT_GESTURE_TURN_RIGHT90,
	ACT_GESTURE_TURN_LEFT45_FLAT,
	ACT_GESTURE_TURN_RIGHT45_FLAT,
	ACT_GESTURE_TURN_LEFT90_FLAT,
	ACT_GESTURE_TURN_RIGHT90_FLAT,
	ACT_BARNACLE_HIT,
	ACT_BARNACLE_PULL,
	ACT_BARNACLE_CHOMP,
	ACT_BARNACLE_CHEW,
	ACT_DO_NOT_DISTURB,
	ACT_SPECIFIC_SEQUENCE,
	ACT_VM_DRAW,
	ACT_VM_HOLSTER,
	ACT_VM_IDLE,
	ACT_VM_FIDGET,
	ACT_VM_PULLBACK,
	ACT_VM_PULLBACK_HIGH,
	ACT_VM_PULLBACK_LOW,
	ACT_VM_THROW,
	ACT_VM_PULLPIN,
	ACT_VM_PRIMARYATTACK,
	ACT_VM_SECONDARYATTACK,
	ACT_VM_RELOAD,
	ACT_VM_DRYFIRE,
	ACT_VM_HITLEFT,
	ACT_VM_HITLEFT2,
	ACT_VM_HITRIGHT,
	ACT_VM_HITRIGHT2,
	ACT_VM_HITCENTER,
	ACT_VM_HITCENTER2,
	ACT_VM_MISSLEFT,
	ACT_VM_MISSLEFT2,
	ACT_VM_MISSRIGHT,
	ACT_VM_MISSRIGHT2,
	ACT_VM_MISSCENTER,
	ACT_VM_MISSCENTER2,
	ACT_VM_HAULBACK,
	ACT_VM_SWINGHARD,
	ACT_VM_SWINGMISS,
	ACT_VM_SWINGHIT,
	ACT_VM_IDLE_TO_LOWERED,
	ACT_VM_IDLE_LOWERED,
	ACT_VM_LOWERED_TO_IDLE,
	ACT_VM_RECOIL1,
	ACT_VM_RECOIL2,
	ACT_VM_RECOIL3,
	ACT_VM_PICKUP,
	ACT_VM_RELEASE,
	ACT_VM_ATTACH_SILENCER,
	ACT_VM_DETACH_SILENCER,
	ACT_VM_EMPTY_FIRE,
	ACT_VM_EMPTY_RELOAD,
	ACT_VM_EMPTY_DRAW,
	ACT_VM_EMPTY_IDLE,
	ACT_SLAM_STICKWALL_IDLE,
	ACT_SLAM_STICKWALL_ND_IDLE,
	ACT_SLAM_STICKWALL_ATTACH,
	ACT_SLAM_STICKWALL_ATTACH2,
	ACT_SLAM_STICKWALL_ND_ATTACH,
	ACT_SLAM_STICKWALL_ND_ATTACH2,
	ACT_SLAM_STICKWALL_DETONATE,
	ACT_SLAM_STICKWALL_DETONATOR_HOLSTER,
	ACT_SLAM_STICKWALL_DRAW,
	ACT_SLAM_STICKWALL_ND_DRAW,
	ACT_SLAM_STICKWALL_TO_THROW,
	ACT_SLAM_STICKWALL_TO_THROW_ND,
	ACT_SLAM_STICKWALL_TO_TRIPMINE_ND,
	ACT_SLAM_THROW_IDLE,
	ACT_SLAM_THROW_ND_IDLE,
	ACT_SLAM_THROW_THROW,
	ACT_SLAM_THROW_THROW2,
	ACT_SLAM_THROW_THROW_ND,
	ACT_SLAM_THROW_THROW_ND2,
	ACT_SLAM_THROW_DRAW,
	ACT_SLAM_THROW_ND_DRAW,
	ACT_SLAM_THROW_TO_STICKWALL,
	ACT_SLAM_THROW_TO_STICKWALL_ND,
	ACT_SLAM_THROW_DETONATE,
	ACT_SLAM_THROW_DETONATOR_HOLSTER,
	ACT_SLAM_THROW_TO_TRIPMINE_ND,
	ACT_SLAM_TRIPMINE_IDLE,
	ACT_SLAM_TRIPMINE_DRAW,
	ACT_SLAM_TRIPMINE_ATTACH,
	ACT_SLAM_TRIPMINE_ATTACH2,
	ACT_SLAM_TRIPMINE_TO_STICKWALL_ND,
	ACT_SLAM_TRIPMINE_TO_THROW_ND,
	ACT_SLAM_DETONATOR_IDLE,
	ACT_SLAM_DETONATOR_DRAW,
	ACT_SLAM_DETONATOR_DETONATE,
	ACT_SLAM_DETONATOR_HOLSTER,
	ACT_SLAM_DETONATOR_STICKWALL_DRAW,
	ACT_SLAM_DETONATOR_THROW_DRAW,
	ACT_SHOTGUN_RELOAD_START,
	ACT_SHOTGUN_RELOAD_FINISH,
	ACT_SHOTGUN_PUMP,
	ACT_SMG2_IDLE2,
	ACT_SMG2_FIRE2,
	ACT_SMG2_DRAW2,
	ACT_SMG2_RELOAD2,
	ACT_SMG2_DRYFIRE2,
	ACT_SMG2_TOAUTO,
	ACT_SMG2_TOBURST,
	ACT_PHYSCANNON_UPGRADE,
	ACT_RANGE_ATTACK_AR1,
	ACT_RANGE_ATTACK_AR2,
	ACT_RANGE_ATTACK_AR2_LOW,
	ACT_RANGE_ATTACK_AR2_GRENADE,
	ACT_RANGE_ATTACK_HMG1,
	ACT_RANGE_ATTACK_ML,
	ACT_RANGE_ATTACK_SMG1,
	ACT_RANGE_ATTACK_SMG1_LOW,
	ACT_RANGE_ATTACK_SMG2,
	ACT_RANGE_ATTACK_SHOTGUN,
	ACT_RANGE_ATTACK_SHOTGUN_LOW,
	ACT_RANGE_ATTACK_PISTOL,
	ACT_RANGE_ATTACK_PISTOL_LOW,
	ACT_RANGE_ATTACK_SLAM,
	ACT_RANGE_ATTACK_TRIPWIRE,
	ACT_RANGE_ATTACK_THROW,
	ACT_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_RANGE_ATTACK_RPG,
	ACT_MELEE_ATTACK_SWING,
	ACT_RANGE_AIM_LOW,
	ACT_RANGE_AIM_SMG1_LOW,
	ACT_RANGE_AIM_PISTOL_LOW,
	ACT_RANGE_AIM_AR2_LOW,
	ACT_COVER_PISTOL_LOW,
	ACT_COVER_SMG1_LOW,
	ACT_GESTURE_RANGE_ATTACK_AR1,
	ACT_GESTURE_RANGE_ATTACK_AR2,
	ACT_GESTURE_RANGE_ATTACK_AR2_GRENADE,
	ACT_GESTURE_RANGE_ATTACK_HMG1,
	ACT_GESTURE_RANGE_ATTACK_ML,
	ACT_GESTURE_RANGE_ATTACK_SMG1,
	ACT_GESTURE_RANGE_ATTACK_SMG1_LOW,
	ACT_GESTURE_RANGE_ATTACK_SMG2,
	ACT_GESTURE_RANGE_ATTACK_SHOTGUN,
	ACT_GESTURE_RANGE_ATTACK_PISTOL,
	ACT_GESTURE_RANGE_ATTACK_PISTOL_LOW,
	ACT_GESTURE_RANGE_ATTACK_SLAM,
	ACT_GESTURE_RANGE_ATTACK_TRIPWIRE,
	ACT_GESTURE_RANGE_ATTACK_THROW,
	ACT_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_GESTURE_MELEE_ATTACK_SWING,
	ACT_IDLE_RIFLE,
	ACT_IDLE_SMG1,
	ACT_IDLE_ANGRY_SMG1,
	ACT_IDLE_PISTOL,
	ACT_IDLE_ANGRY_PISTOL,
	ACT_IDLE_ANGRY_SHOTGUN,
	ACT_IDLE_STEALTH_PISTOL,
	ACT_IDLE_PACKAGE,
	ACT_WALK_PACKAGE,
	ACT_IDLE_SUITCASE,
	ACT_WALK_SUITCASE,
	ACT_IDLE_SMG1_RELAXED,
	ACT_IDLE_SMG1_STIMULATED,
	ACT_WALK_RIFLE_RELAXED,
	ACT_RUN_RIFLE_RELAXED,
	ACT_WALK_RIFLE_STIMULATED,
	ACT_RUN_RIFLE_STIMULATED,
	ACT_IDLE_AIM_RIFLE_STIMULATED,
	ACT_WALK_AIM_RIFLE_STIMULATED,
	ACT_RUN_AIM_RIFLE_STIMULATED,
	ACT_IDLE_SHOTGUN_RELAXED,
	ACT_IDLE_SHOTGUN_STIMULATED,
	ACT_IDLE_SHOTGUN_AGITATED,
	ACT_WALK_ANGRY,
	ACT_POLICE_HARASS1,
	ACT_POLICE_HARASS2,
	ACT_IDLE_MANNEDGUN,
	ACT_IDLE_MELEE,
	ACT_IDLE_ANGRY_MELEE,
	ACT_IDLE_RPG_RELAXED,
	ACT_IDLE_RPG,
	ACT_IDLE_ANGRY_RPG,
	ACT_COVER_LOW_RPG,
	ACT_WALK_RPG,
	ACT_RUN_RPG,
	ACT_WALK_CROUCH_RPG,
	ACT_RUN_CROUCH_RPG,
	ACT_WALK_RPG_RELAXED,
	ACT_RUN_RPG_RELAXED,
	ACT_WALK_RIFLE,
	ACT_WALK_AIM_RIFLE,
	ACT_WALK_CROUCH_RIFLE,
	ACT_WALK_CROUCH_AIM_RIFLE,
	ACT_RUN_RIFLE,
	ACT_RUN_AIM_RIFLE,
	ACT_RUN_CROUCH_RIFLE,
	ACT_RUN_CROUCH_AIM_RIFLE,
	ACT_RUN_STEALTH_PISTOL,
	ACT_WALK_AIM_SHOTGUN,
	ACT_RUN_AIM_SHOTGUN,
	ACT_WALK_PISTOL,
	ACT_RUN_PISTOL,
	ACT_WALK_AIM_PISTOL,
	ACT_RUN_AIM_PISTOL,
	ACT_WALK_STEALTH_PISTOL,
	ACT_WALK_AIM_STEALTH_PISTOL,
	ACT_RUN_AIM_STEALTH_PISTOL,
	ACT_RELOAD_PISTOL,
	ACT_RELOAD_PISTOL_LOW,
	ACT_RELOAD_SMG1,
	ACT_RELOAD_SMG1_LOW,
	ACT_RELOAD_SHOTGUN,
	ACT_RELOAD_SHOTGUN_LOW,
	ACT_GESTURE_RELOAD,
	ACT_GESTURE_RELOAD_PISTOL,
	ACT_GESTURE_RELOAD_SMG1,
	ACT_GESTURE_RELOAD_SHOTGUN,
	ACT_BUSY_LEAN_LEFT,
	ACT_BUSY_LEAN_LEFT_ENTRY,
	ACT_BUSY_LEAN_LEFT_EXIT,
	ACT_BUSY_LEAN_BACK,
	ACT_BUSY_LEAN_BACK_ENTRY,
	ACT_BUSY_LEAN_BACK_EXIT,
	ACT_BUSY_SIT_GROUND,
	ACT_BUSY_SIT_GROUND_ENTRY,
	ACT_BUSY_SIT_GROUND_EXIT,
	ACT_BUSY_SIT_CHAIR,
	ACT_BUSY_SIT_CHAIR_ENTRY,
	ACT_BUSY_SIT_CHAIR_EXIT,
	ACT_BUSY_STAND,
	ACT_BUSY_QUEUE,
	ACT_DUCK_DODGE,
	ACT_DIE_BARNACLE_SWALLOW,
	ACT_GESTURE_BARNACLE_STRANGLE,
	ACT_PHYSCANNON_DETACH,
	ACT_PHYSCANNON_ANIMATE,
	ACT_PHYSCANNON_ANIMATE_PRE,
	ACT_PHYSCANNON_ANIMATE_POST,
	ACT_DIE_FRONTSIDE,
	ACT_DIE_RIGHTSIDE,
	ACT_DIE_BACKSIDE,
	ACT_DIE_LEFTSIDE,
	ACT_DIE_CROUCH_FRONTSIDE,
	ACT_DIE_CROUCH_RIGHTSIDE,
	ACT_DIE_CROUCH_BACKSIDE,
	ACT_DIE_CROUCH_LEFTSIDE,
	ACT_OPEN_DOOR,
	ACT_DI_ALYX_ZOMBIE_MELEE,
	ACT_DI_ALYX_ZOMBIE_TORSO_MELEE,
	ACT_DI_ALYX_HEADCRAB_MELEE,
	ACT_DI_ALYX_ANTLION,
	ACT_DI_ALYX_ZOMBIE_SHOTGUN64,
	ACT_DI_ALYX_ZOMBIE_SHOTGUN26,
	ACT_READINESS_RELAXED_TO_STIMULATED,
	ACT_READINESS_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_AGITATED_TO_STIMULATED,
	ACT_READINESS_STIMULATED_TO_RELAXED,
	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED,
	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_PISTOL_AGITATED_TO_STIMULATED,
	ACT_READINESS_PISTOL_STIMULATED_TO_RELAXED,
	ACT_IDLE_CARRY,
	ACT_WALK_CARRY,
	ACT_STARTDYING,
	ACT_DYINGLOOP,
	ACT_DYINGTODEAD,
	ACT_RIDE_MANNED_GUN,
	ACT_VM_SPRINT_ENTER,
	ACT_VM_SPRINT_IDLE,
	ACT_VM_SPRINT_LEAVE,
	ACT_FIRE_START,
	ACT_FIRE_LOOP,
	ACT_FIRE_END,
	ACT_CROUCHING_GRENADEIDLE,
	ACT_CROUCHING_GRENADEREADY,
	ACT_CROUCHING_PRIMARYATTACK,
	ACT_OVERLAY_GRENADEIDLE,
	ACT_OVERLAY_GRENADEREADY,
	ACT_OVERLAY_PRIMARYATTACK,
	ACT_OVERLAY_SHIELD_UP,
	ACT_OVERLAY_SHIELD_DOWN,
	ACT_OVERLAY_SHIELD_UP_IDLE,
	ACT_OVERLAY_SHIELD_ATTACK,
	ACT_OVERLAY_SHIELD_KNOCKBACK,
	ACT_SHIELD_UP,
	ACT_SHIELD_DOWN,
	ACT_SHIELD_UP_IDLE,
	ACT_SHIELD_ATTACK,
	ACT_SHIELD_KNOCKBACK,
	ACT_CROUCHING_SHIELD_UP,
	ACT_CROUCHING_SHIELD_DOWN,
	ACT_CROUCHING_SHIELD_UP_IDLE,
	ACT_CROUCHING_SHIELD_ATTACK,
	ACT_CROUCHING_SHIELD_KNOCKBACK,
	ACT_TURNRIGHT45,
	ACT_TURNLEFT45,
	ACT_TURN,
	ACT_OBJ_ASSEMBLING,
	ACT_OBJ_DISMANTLING,
	ACT_OBJ_STARTUP,
	ACT_OBJ_RUNNING,
	ACT_OBJ_IDLE,
	ACT_OBJ_PLACING,
	ACT_OBJ_DETERIORATING,
	ACT_OBJ_UPGRADING,
	ACT_DEPLOY,
	ACT_DEPLOY_IDLE,
	ACT_UNDEPLOY,
	ACT_CROSSBOW_DRAW_UNLOADED,
	ACT_GAUSS_SPINUP,
	ACT_GAUSS_SPINCYCLE,
	ACT_VM_PRIMARYATTACK_SILENCED,
	ACT_VM_RELOAD_SILENCED,
	ACT_VM_DRYFIRE_SILENCED,
	ACT_VM_IDLE_SILENCED,
	ACT_VM_DRAW_SILENCED,
	ACT_VM_IDLE_EMPTY_LEFT,
	ACT_VM_DRYFIRE_LEFT,
	ACT_VM_IS_DRAW,
	ACT_VM_IS_HOLSTER,
	ACT_VM_IS_IDLE,
	ACT_VM_IS_PRIMARYATTACK,
	ACT_PLAYER_IDLE_FIRE,
	ACT_PLAYER_CROUCH_FIRE,
	ACT_PLAYER_CROUCH_WALK_FIRE,
	ACT_PLAYER_WALK_FIRE,
	ACT_PLAYER_RUN_FIRE,
	ACT_IDLETORUN,
	ACT_RUNTOIDLE,
	ACT_VM_DRAW_DEPLOYED,
	ACT_HL2MP_IDLE_MELEE,
	ACT_HL2MP_RUN_MELEE,
	ACT_HL2MP_IDLE_CROUCH_MELEE,
	ACT_HL2MP_WALK_CROUCH_MELEE,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,
	ACT_HL2MP_GESTURE_RELOAD_MELEE,
	ACT_HL2MP_JUMP_MELEE,
	ACT_VM_FIZZLE,
	ACT_MP_STAND_IDLE,
	ACT_MP_CROUCH_IDLE,
	ACT_MP_CROUCH_DEPLOYED_IDLE,
	ACT_MP_CROUCH_DEPLOYED,
	ACT_MP_DEPLOYED_IDLE,
	ACT_MP_RUN,
	ACT_MP_WALK,
	ACT_MP_AIRWALK,
	ACT_MP_CROUCHWALK,
	ACT_MP_SPRINT,
	ACT_MP_JUMP,
	ACT_MP_JUMP_START,
	ACT_MP_JUMP_FLOAT,
	ACT_MP_JUMP_LAND,
	ACT_MP_JUMP_IMPACT_N,
	ACT_MP_JUMP_IMPACT_E,
	ACT_MP_JUMP_IMPACT_W,
	ACT_MP_JUMP_IMPACT_S,
	ACT_MP_JUMP_IMPACT_TOP,
	ACT_MP_DOUBLEJUMP,
	ACT_MP_SWIM,
	ACT_MP_DEPLOYED,
	ACT_MP_SWIM_DEPLOYED,
	ACT_MP_VCD,
	ACT_MP_ATTACK_STAND_PRIMARYFIRE,
	ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_STAND_SECONDARYFIRE,
	ACT_MP_ATTACK_STAND_GRENADE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_SECONDARYFIRE,
	ACT_MP_ATTACK_CROUCH_GRENADE,
	ACT_MP_ATTACK_SWIM_PRIMARYFIRE,
	ACT_MP_ATTACK_SWIM_SECONDARYFIRE,
	ACT_MP_ATTACK_SWIM_GRENADE,
	ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,
	ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE,
	ACT_MP_ATTACK_AIRWALK_GRENADE,
	ACT_MP_RELOAD_STAND,
	ACT_MP_RELOAD_STAND_LOOP,
	ACT_MP_RELOAD_STAND_END,
	ACT_MP_RELOAD_CROUCH,
	ACT_MP_RELOAD_CROUCH_LOOP,
	ACT_MP_RELOAD_CROUCH_END,
	ACT_MP_RELOAD_SWIM,
	ACT_MP_RELOAD_SWIM_LOOP,
	ACT_MP_RELOAD_SWIM_END,
	ACT_MP_RELOAD_AIRWALK,
	ACT_MP_RELOAD_AIRWALK_LOOP,
	ACT_MP_RELOAD_AIRWALK_END,
	ACT_MP_ATTACK_STAND_PREFIRE,
	ACT_MP_ATTACK_STAND_POSTFIRE,
	ACT_MP_ATTACK_STAND_STARTFIRE,
	ACT_MP_ATTACK_CROUCH_PREFIRE,
	ACT_MP_ATTACK_CROUCH_POSTFIRE,
	ACT_MP_ATTACK_SWIM_PREFIRE,
	ACT_MP_ATTACK_SWIM_POSTFIRE,
	ACT_MP_STAND_PRIMARY,
	ACT_MP_CROUCH_PRIMARY,
	ACT_MP_RUN_PRIMARY,
	ACT_MP_WALK_PRIMARY,
	ACT_MP_AIRWALK_PRIMARY,
	ACT_MP_CROUCHWALK_PRIMARY,
	ACT_MP_JUMP_PRIMARY,
	ACT_MP_JUMP_START_PRIMARY,
	ACT_MP_JUMP_FLOAT_PRIMARY,
	ACT_MP_JUMP_LAND_PRIMARY,
	ACT_MP_SWIM_PRIMARY,
	ACT_MP_DEPLOYED_PRIMARY,
	ACT_MP_SWIM_DEPLOYED_PRIMARY,
	ACT_MP_ATTACK_STAND_PRIMARY,
	ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_PRIMARY,
	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_SWIM_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_PRIMARY,
	ACT_MP_RELOAD_STAND_PRIMARY,
	ACT_MP_RELOAD_STAND_PRIMARY_LOOP,
	ACT_MP_RELOAD_STAND_PRIMARY_END,
	ACT_MP_RELOAD_CROUCH_PRIMARY,
	ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP,
	ACT_MP_RELOAD_CROUCH_PRIMARY_END,
	ACT_MP_RELOAD_SWIM_PRIMARY,
	ACT_MP_RELOAD_SWIM_PRIMARY_LOOP,
	ACT_MP_RELOAD_SWIM_PRIMARY_END,
	ACT_MP_RELOAD_AIRWALK_PRIMARY,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_END,
	ACT_MP_ATTACK_STAND_GRENADE_PRIMARY,
	ACT_MP_ATTACK_CROUCH_GRENADE_PRIMARY,
	ACT_MP_ATTACK_SWIM_GRENADE_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_PRIMARY,
	ACT_MP_STAND_SECONDARY,
	ACT_MP_CROUCH_SECONDARY,
	ACT_MP_RUN_SECONDARY,
	ACT_MP_WALK_SECONDARY,
	ACT_MP_AIRWALK_SECONDARY,
	ACT_MP_CROUCHWALK_SECONDARY,
	ACT_MP_JUMP_SECONDARY,
	ACT_MP_JUMP_START_SECONDARY,
	ACT_MP_JUMP_FLOAT_SECONDARY,
	ACT_MP_JUMP_LAND_SECONDARY,
	ACT_MP_SWIM_SECONDARY,
	ACT_MP_ATTACK_STAND_SECONDARY,
	ACT_MP_ATTACK_CROUCH_SECONDARY,
	ACT_MP_ATTACK_SWIM_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_SECONDARY,
	ACT_MP_RELOAD_STAND_SECONDARY,
	ACT_MP_RELOAD_STAND_SECONDARY_LOOP,
	ACT_MP_RELOAD_STAND_SECONDARY_END,
	ACT_MP_RELOAD_CROUCH_SECONDARY,
	ACT_MP_RELOAD_CROUCH_SECONDARY_LOOP,
	ACT_MP_RELOAD_CROUCH_SECONDARY_END,
	ACT_MP_RELOAD_SWIM_SECONDARY,
	ACT_MP_RELOAD_SWIM_SECONDARY_LOOP,
	ACT_MP_RELOAD_SWIM_SECONDARY_END,
	ACT_MP_RELOAD_AIRWALK_SECONDARY,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_END,
	ACT_MP_ATTACK_STAND_GRENADE_SECONDARY,
	ACT_MP_ATTACK_CROUCH_GRENADE_SECONDARY,
	ACT_MP_ATTACK_SWIM_GRENADE_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_SECONDARY,
	ACT_MP_STAND_MELEE,
	ACT_MP_CROUCH_MELEE,
	ACT_MP_RUN_MELEE,
	ACT_MP_WALK_MELEE,
	ACT_MP_AIRWALK_MELEE,
	ACT_MP_CROUCHWALK_MELEE,
	ACT_MP_JUMP_MELEE,
	ACT_MP_JUMP_START_MELEE,
	ACT_MP_JUMP_FLOAT_MELEE,
	ACT_MP_JUMP_LAND_MELEE,
	ACT_MP_SWIM_MELEE,
	ACT_MP_ATTACK_STAND_MELEE,
	ACT_MP_ATTACK_STAND_MELEE_SECONDARY,
	ACT_MP_ATTACK_CROUCH_MELEE,
	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY,
	ACT_MP_ATTACK_SWIM_MELEE,
	ACT_MP_ATTACK_AIRWALK_MELEE,
	ACT_MP_ATTACK_STAND_GRENADE_MELEE,
	ACT_MP_ATTACK_CROUCH_GRENADE_MELEE,
	ACT_MP_ATTACK_SWIM_GRENADE_MELEE,
	ACT_MP_ATTACK_AIRWALK_GRENADE_MELEE,
	ACT_MP_STAND_ITEM1,
	ACT_MP_CROUCH_ITEM1,
	ACT_MP_RUN_ITEM1,
	ACT_MP_WALK_ITEM1,
	ACT_MP_AIRWALK_ITEM1,
	ACT_MP_CROUCHWALK_ITEM1,
	ACT_MP_JUMP_ITEM1,
	ACT_MP_JUMP_START_ITEM1,
	ACT_MP_JUMP_FLOAT_ITEM1,
	ACT_MP_JUMP_LAND_ITEM1,
	ACT_MP_SWIM_ITEM1,
	ACT_MP_ATTACK_STAND_ITEM1,
	ACT_MP_ATTACK_STAND_ITEM1_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM1,
	ACT_MP_ATTACK_CROUCH_ITEM1_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM1,
	ACT_MP_ATTACK_AIRWALK_ITEM1,
	ACT_MP_STAND_ITEM2,
	ACT_MP_CROUCH_ITEM2,
	ACT_MP_RUN_ITEM2,
	ACT_MP_WALK_ITEM2,
	ACT_MP_AIRWALK_ITEM2,
	ACT_MP_CROUCHWALK_ITEM2,
	ACT_MP_JUMP_ITEM2,
	ACT_MP_JUMP_START_ITEM2,
	ACT_MP_JUMP_FLOAT_ITEM2,
	ACT_MP_JUMP_LAND_ITEM2,
	ACT_MP_SWIM_ITEM2,
	ACT_MP_ATTACK_STAND_ITEM2,
	ACT_MP_ATTACK_STAND_ITEM2_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM2,
	ACT_MP_ATTACK_CROUCH_ITEM2_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM2,
	ACT_MP_ATTACK_AIRWALK_ITEM2,
	ACT_MP_GESTURE_FLINCH,
	ACT_MP_GESTURE_FLINCH_PRIMARY,
	ACT_MP_GESTURE_FLINCH_SECONDARY,
	ACT_MP_GESTURE_FLINCH_MELEE,
	ACT_MP_GESTURE_FLINCH_ITEM1,
	ACT_MP_GESTURE_FLINCH_ITEM2,
	ACT_MP_GESTURE_FLINCH_HEAD,
	ACT_MP_GESTURE_FLINCH_CHEST,
	ACT_MP_GESTURE_FLINCH_STOMACH,
	ACT_MP_GESTURE_FLINCH_LEFTARM,
	ACT_MP_GESTURE_FLINCH_RIGHTARM,
	ACT_MP_GESTURE_FLINCH_LEFTLEG,
	ACT_MP_GESTURE_FLINCH_RIGHTLEG,
	ACT_MP_GRENADE1_DRAW,
	ACT_MP_GRENADE1_IDLE,
	ACT_MP_GRENADE1_ATTACK,
	ACT_MP_GRENADE2_DRAW,
	ACT_MP_GRENADE2_IDLE,
	ACT_MP_GRENADE2_ATTACK,
	ACT_MP_PRIMARY_GRENADE1_DRAW,
	ACT_MP_PRIMARY_GRENADE1_IDLE,
	ACT_MP_PRIMARY_GRENADE1_ATTACK,
	ACT_MP_PRIMARY_GRENADE2_DRAW,
	ACT_MP_PRIMARY_GRENADE2_IDLE,
	ACT_MP_PRIMARY_GRENADE2_ATTACK,
	ACT_MP_SECONDARY_GRENADE1_DRAW,
	ACT_MP_SECONDARY_GRENADE1_IDLE,
	ACT_MP_SECONDARY_GRENADE1_ATTACK,
	ACT_MP_SECONDARY_GRENADE2_DRAW,
	ACT_MP_SECONDARY_GRENADE2_IDLE,
	ACT_MP_SECONDARY_GRENADE2_ATTACK,
	ACT_MP_MELEE_GRENADE1_DRAW,
	ACT_MP_MELEE_GRENADE1_IDLE,
	ACT_MP_MELEE_GRENADE1_ATTACK,
	ACT_MP_MELEE_GRENADE2_DRAW,
	ACT_MP_MELEE_GRENADE2_IDLE,
	ACT_MP_MELEE_GRENADE2_ATTACK,
	ACT_MP_ITEM1_GRENADE1_DRAW,
	ACT_MP_ITEM1_GRENADE1_IDLE,
	ACT_MP_ITEM1_GRENADE1_ATTACK,
	ACT_MP_ITEM1_GRENADE2_DRAW,
	ACT_MP_ITEM1_GRENADE2_IDLE,
	ACT_MP_ITEM1_GRENADE2_ATTACK,
	ACT_MP_ITEM2_GRENADE1_DRAW,
	ACT_MP_ITEM2_GRENADE1_IDLE,
	ACT_MP_ITEM2_GRENADE1_ATTACK,
	ACT_MP_ITEM2_GRENADE2_DRAW,
	ACT_MP_ITEM2_GRENADE2_IDLE,
	ACT_MP_ITEM2_GRENADE2_ATTACK,
	ACT_MP_STAND_BUILDING,
	ACT_MP_CROUCH_BUILDING,
	ACT_MP_RUN_BUILDING,
	ACT_MP_WALK_BUILDING,
	ACT_MP_AIRWALK_BUILDING,
	ACT_MP_CROUCHWALK_BUILDING,
	ACT_MP_JUMP_BUILDING,
	ACT_MP_JUMP_START_BUILDING,
	ACT_MP_JUMP_FLOAT_BUILDING,
	ACT_MP_JUMP_LAND_BUILDING,
	ACT_MP_SWIM_BUILDING,
	ACT_MP_ATTACK_STAND_BUILDING,
	ACT_MP_ATTACK_CROUCH_BUILDING,
	ACT_MP_ATTACK_SWIM_BUILDING,
	ACT_MP_ATTACK_AIRWALK_BUILDING,
	ACT_MP_ATTACK_STAND_GRENADE_BUILDING,
	ACT_MP_ATTACK_CROUCH_GRENADE_BUILDING,
	ACT_MP_ATTACK_SWIM_GRENADE_BUILDING,
	ACT_MP_ATTACK_AIRWALK_GRENADE_BUILDING,
	ACT_MP_STAND_PDA,
	ACT_MP_CROUCH_PDA,
	ACT_MP_RUN_PDA,
	ACT_MP_WALK_PDA,
	ACT_MP_AIRWALK_PDA,
	ACT_MP_CROUCHWALK_PDA,
	ACT_MP_JUMP_PDA,
	ACT_MP_JUMP_START_PDA,
	ACT_MP_JUMP_FLOAT_PDA,
	ACT_MP_JUMP_LAND_PDA,
	ACT_MP_SWIM_PDA,
	ACT_MP_ATTACK_STAND_PDA,
	ACT_MP_ATTACK_SWIM_PDA,
	ACT_MP_GESTURE_VC_HANDMOUTH,
	ACT_MP_GESTURE_VC_FINGERPOINT,
	ACT_MP_GESTURE_VC_FISTPUMP,
	ACT_MP_GESTURE_VC_THUMBSUP,
	ACT_MP_GESTURE_VC_NODYES,
	ACT_MP_GESTURE_VC_NODNO,
	ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY,
	ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY,
	ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY,
	ACT_MP_GESTURE_VC_NODYES_PRIMARY,
	ACT_MP_GESTURE_VC_NODNO_PRIMARY,
	ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY,
	ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY,
	ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY,
	ACT_MP_GESTURE_VC_NODYES_SECONDARY,
	ACT_MP_GESTURE_VC_NODNO_SECONDARY,
	ACT_MP_GESTURE_VC_HANDMOUTH_MELEE,
	ACT_MP_GESTURE_VC_FINGERPOINT_MELEE,
	ACT_MP_GESTURE_VC_FISTPUMP_MELEE,
	ACT_MP_GESTURE_VC_THUMBSUP_MELEE,
	ACT_MP_GESTURE_VC_NODYES_MELEE,
	ACT_MP_GESTURE_VC_NODNO_MELEE,
	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM1,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM1,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM1,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM1,
	ACT_MP_GESTURE_VC_NODYES_ITEM1,
	ACT_MP_GESTURE_VC_NODNO_ITEM1,
	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM2,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM2,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM2,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM2,
	ACT_MP_GESTURE_VC_NODYES_ITEM2,
	ACT_MP_GESTURE_VC_NODNO_ITEM2,
	ACT_MP_GESTURE_VC_HANDMOUTH_BUILDING,
	ACT_MP_GESTURE_VC_FINGERPOINT_BUILDING,
	ACT_MP_GESTURE_VC_FISTPUMP_BUILDING,
	ACT_MP_GESTURE_VC_THUMBSUP_BUILDING,
	ACT_MP_GESTURE_VC_NODYES_BUILDING,
	ACT_MP_GESTURE_VC_NODNO_BUILDING,
	ACT_MP_GESTURE_VC_HANDMOUTH_PDA,
	ACT_MP_GESTURE_VC_FINGERPOINT_PDA,
	ACT_MP_GESTURE_VC_FISTPUMP_PDA,
	ACT_MP_GESTURE_VC_THUMBSUP_PDA,
	ACT_MP_GESTURE_VC_NODYES_PDA,
	ACT_MP_GESTURE_VC_NODNO_PDA,
	ACT_VM_UNUSABLE,
	ACT_VM_UNUSABLE_TO_USABLE,
	ACT_VM_USABLE_TO_UNUSABLE,
	ACT_PRIMARY_VM_DRAW,
	ACT_PRIMARY_VM_HOLSTER,
	ACT_PRIMARY_VM_IDLE,
	ACT_PRIMARY_VM_PULLBACK,
	ACT_PRIMARY_VM_PRIMARYATTACK,
	ACT_PRIMARY_VM_SECONDARYATTACK,
	ACT_PRIMARY_VM_RELOAD,
	ACT_PRIMARY_VM_DRYFIRE,
	ACT_PRIMARY_VM_IDLE_TO_LOWERED,
	ACT_PRIMARY_VM_IDLE_LOWERED,
	ACT_PRIMARY_VM_LOWERED_TO_IDLE,
	ACT_SECONDARY_VM_DRAW,
	ACT_SECONDARY_VM_HOLSTER,
	ACT_SECONDARY_VM_IDLE,
	ACT_SECONDARY_VM_PULLBACK,
	ACT_SECONDARY_VM_PRIMARYATTACK,
	ACT_SECONDARY_VM_SECONDARYATTACK,
	ACT_SECONDARY_VM_RELOAD,
	ACT_SECONDARY_VM_DRYFIRE,
	ACT_SECONDARY_VM_IDLE_TO_LOWERED,
	ACT_SECONDARY_VM_IDLE_LOWERED,
	ACT_SECONDARY_VM_LOWERED_TO_IDLE,
	ACT_MELEE_VM_DRAW,
	ACT_MELEE_VM_HOLSTER,
	ACT_MELEE_VM_IDLE,
	ACT_MELEE_VM_PULLBACK,
	ACT_MELEE_VM_PRIMARYATTACK,
	ACT_MELEE_VM_SECONDARYATTACK,
	ACT_MELEE_VM_RELOAD,
	ACT_MELEE_VM_DRYFIRE,
	ACT_MELEE_VM_IDLE_TO_LOWERED,
	ACT_MELEE_VM_IDLE_LOWERED,
	ACT_MELEE_VM_LOWERED_TO_IDLE,
	ACT_PDA_VM_DRAW,
	ACT_PDA_VM_HOLSTER,
	ACT_PDA_VM_IDLE,
	ACT_PDA_VM_PULLBACK,
	ACT_PDA_VM_PRIMARYATTACK,
	ACT_PDA_VM_SECONDARYATTACK,
	ACT_PDA_VM_RELOAD,
	ACT_PDA_VM_DRYFIRE,
	ACT_PDA_VM_IDLE_TO_LOWERED,
	ACT_PDA_VM_IDLE_LOWERED,
	ACT_PDA_VM_LOWERED_TO_IDLE,
	ACT_ITEM1_VM_DRAW,
	ACT_ITEM1_VM_HOLSTER,
	ACT_ITEM1_VM_IDLE,
	ACT_ITEM1_VM_PULLBACK,
	ACT_ITEM1_VM_PRIMARYATTACK,
	ACT_ITEM1_VM_SECONDARYATTACK,
	ACT_ITEM1_VM_RELOAD,
	ACT_ITEM1_VM_DRYFIRE,
	ACT_ITEM1_VM_IDLE_TO_LOWERED,
	ACT_ITEM1_VM_IDLE_LOWERED,
	ACT_ITEM1_VM_LOWERED_TO_IDLE,
	ACT_ITEM2_VM_DRAW,
	ACT_ITEM2_VM_HOLSTER,
	ACT_ITEM2_VM_IDLE,
	ACT_ITEM2_VM_PULLBACK,
	ACT_ITEM2_VM_PRIMARYATTACK,
	ACT_ITEM2_VM_SECONDARYATTACK,
	ACT_ITEM2_VM_RELOAD,
	ACT_ITEM2_VM_DRYFIRE,
	ACT_ITEM2_VM_IDLE_TO_LOWERED,
	ACT_ITEM2_VM_IDLE_LOWERED,
	ACT_ITEM2_VM_LOWERED_TO_IDLE,
	ACT_RELOAD_SUCCEED,
	ACT_RELOAD_FAIL,
	ACT_WALK_AIM_AUTOGUN,
	ACT_RUN_AIM_AUTOGUN,
	ACT_IDLE_AUTOGUN,
	ACT_IDLE_AIM_AUTOGUN,
	ACT_RELOAD_AUTOGUN,
	ACT_CROUCH_IDLE_AUTOGUN,
	ACT_RANGE_ATTACK_AUTOGUN,
	ACT_JUMP_AUTOGUN,
	ACT_IDLE_AIM_PISTOL,
	ACT_WALK_AIM_DUAL,
	ACT_RUN_AIM_DUAL,
	ACT_IDLE_DUAL,
	ACT_IDLE_AIM_DUAL,
	ACT_RELOAD_DUAL,
	ACT_CROUCH_IDLE_DUAL,
	ACT_RANGE_ATTACK_DUAL,
	ACT_JUMP_DUAL,
	ACT_IDLE_SHOTGUN,
	ACT_IDLE_AIM_SHOTGUN,
	ACT_CROUCH_IDLE_SHOTGUN,
	ACT_JUMP_SHOTGUN,
	ACT_IDLE_AIM_RIFLE,
	ACT_RELOAD_RIFLE,
	ACT_CROUCH_IDLE_RIFLE,
	ACT_RANGE_ATTACK_RIFLE,
	ACT_JUMP_RIFLE,
	ACT_SLEEP,
	ACT_WAKE,
	ACT_FLICK_LEFT,
	ACT_FLICK_LEFT_MIDDLE,
	ACT_FLICK_RIGHT_MIDDLE,
	ACT_FLICK_RIGHT,
	ACT_SPINAROUND,
	ACT_PREP_TO_FIRE,
	ACT_FIRE,
	ACT_FIRE_RECOVER,
	ACT_SPRAY,
	ACT_PREP_EXPLODE,
	ACT_EXPLODE,
	ACT_DOTA_IDLE,
	ACT_DOTA_RUN,
	ACT_DOTA_ATTACK,
	ACT_DOTA_ATTACK_EVENT,
	ACT_DOTA_DIE,
	ACT_DOTA_FLINCH,
	ACT_DOTA_DISABLED,
	ACT_DOTA_CAST_ABILITY_1,
	ACT_DOTA_CAST_ABILITY_2,
	ACT_DOTA_CAST_ABILITY_3,
	ACT_DOTA_CAST_ABILITY_4,
	ACT_DOTA_OVERRIDE_ABILITY_1,
	ACT_DOTA_OVERRIDE_ABILITY_2,
	ACT_DOTA_OVERRIDE_ABILITY_3,
	ACT_DOTA_OVERRIDE_ABILITY_4,
	ACT_DOTA_CHANNEL_ABILITY_1,
	ACT_DOTA_CHANNEL_ABILITY_2,
	ACT_DOTA_CHANNEL_ABILITY_3,
	ACT_DOTA_CHANNEL_ABILITY_4,
	ACT_DOTA_CHANNEL_END_ABILITY_1,
	ACT_DOTA_CHANNEL_END_ABILITY_2,
	ACT_DOTA_CHANNEL_END_ABILITY_3,
	ACT_DOTA_CHANNEL_END_ABILITY_4,
	ACT_MP_RUN_SPEEDPAINT,
	ACT_MP_LONG_FALL,
	ACT_MP_TRACTORBEAM_FLOAT,
	ACT_MP_DEATH_CRUSH,
	ACT_MP_RUN_SPEEDPAINT_PRIMARY,
	ACT_MP_DROWNING_PRIMARY,
	ACT_MP_LONG_FALL_PRIMARY,
	ACT_MP_TRACTORBEAM_FLOAT_PRIMARY,
	ACT_MP_DEATH_CRUSH_PRIMARY,
	ACT_DIE_STAND,
	ACT_DIE_STAND_HEADSHOT,
	ACT_DIE_CROUCH,
	ACT_DIE_CROUCH_HEADSHOT,
	ACT_CSGO_NULL,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM,
};

class CUserCmd;
class IBaseViewModel;

class IBasePlayer : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	__forceinline void* renderable() {
		return (void*)((uintptr_t)this + 0x4);
	}
	__forceinline int GetIndex() {
		return EntIndex();
	}
	__forceinline const model_t* GetModel2() {
		return getvfunc< const model_t* (__thiscall*)(void*) >(renderable(), 8)(renderable());
	}

	float& m_flNextAttack();
	Vector& GetMins();
	Vector& GetMaxs();
	IBasePlayer* GetObserverTarget() noexcept;
	int GetObserverTargetHandle();
	Vector& GetAbsVelocity();
	int &GetGroundEntity();
	__forceinline void SetLowerBodyYaw(float value);
	void InvalidateBoneCache();
	__forceinline void SetVelocity(Vector velocity);
	void SetupBonesFix(int mask);
	bool SetupBonesFixed(matrix* out, int maxBones, int boneMask, float currentTime);
	__forceinline void SetFlags(int flags);
	float &HealthShotBoostExpirationTime();
	void ResetAnimationState(CCSGOPlayerAnimState *state);
	void CreateAnimationState(CCSGOPlayerAnimState *state);
	void UpdateAnimationState(CCSGOPlayerAnimState *state, Vector angle);
	IBasePlayer* GetDefuser();
	float GetTimerLength();
	float GetDefuseCooldown();
	float GetC4Blow();
	float GetBombTicking();
	bool IsBombDefused();
	int* GetButtons();
	int& GetButtonLast();
	int& GetButtonPressed();
	int& GetButtonReleased();
	uint64_t getSteamID64();
	bool isEnemy(IBasePlayer* local);
	int getSurvivalTeam();

	int get_prop(std::string table, std::string prop);
	float get_prop_fl(std::string table, std::string prop);
	bool get_prop_b(std::string table, std::string prop);
	Vector get_prop_vec(std::string table, std::string prop);
	void set_prop(std::string table, std::string prop, int val);
	void set_prop_fl(std::string table, std::string prop, float val);
	void set_prop_b(std::string table, std::string prop, bool val);
	void set_prop_vec(std::string table, std::string prop, Vector val);

	int m_fireCount();
	bool* m_bFireIsBurning();
	int* m_fireXDelta();
	int* m_fireYDelta();
	int* m_fireZDelta();

	bool m_bDidSmokeEffect();
	int m_nSmokeEffectTickBegin();

	__forceinline int GetButtonDisabled();
	__forceinline int GetButtonForced();
	__forceinline void UpdateCollisionBounds();
	__forceinline void SetSequence(int iSequence);
	__forceinline void StudioFrameAdvance();
	int PostThink();
	int* GetNextThinkTick();
	__forceinline Vector GetBaseVelocity();
	int &GetTickBasePtr();
	__forceinline bool SelectItem(const char* string, int sub_type = 0);
	__forceinline //CHandle<IBasePlayer> GetVehicle();
	__forceinline void StandardBlendingRules(c_studio_hdr *hdr, Vector *pos, Quaternion *q, float curtime, int boneMask);
	__forceinline void BuildTransformations(c_studio_hdr *hdr, Vector *pos, Quaternion *q, const matrix &cameraTransform, int boneMask, BYTE *computed);
	float GetLastSeenTime();
	bool BadMatrix();
	
	int& GetTakeDamage() {
		return *(int*)((DWORD)this + 0x280);
	}
	int GetHitboxSet();
	std::string GetName();
	bool PhysicsRunThink(int unk01);

	__forceinline int GetNumAnimOverlays()
	{
		return *(int*)((DWORD)this + 0x299C);
	}

	__forceinline int GetMoveType()
	{
		return *(int*)((DWORD)this + 0x25C);
	}

	__forceinline CAnimationLayer *GetAnimOverlays()
	{
		return *(CAnimationLayer**)((DWORD)this + 0x2990);
	}

	__forceinline CAnimationLayer *GetAnimOverlay(int index)
	{
		if (index < 15)
			return &GetAnimOverlays()[index];
		else
			return nullptr;
	}

	Vector GetAimPunchAngle();

	__forceinline Vector GetRagdollPos();

	bool& GetClientSideAnims();

	float GetDSYDelta();

	CUtlVector<CAnimationLayer>& GetAnimOverlayVector();

	__forceinline Vector* EyeAngles()
	{
		typedef Vector*(__thiscall* OriginalFn)(IBasePlayer*);
		return getvfunc<OriginalFn>(this, 164)(this);
	}

	__forceinline void PreThink()
	{
		typedef void(__thiscall *OriginalFn)(PVOID);
		getvfunc<OriginalFn>(this, 317)(this);
	}

	__forceinline void Think()
	{
		typedef void(__thiscall *OriginalFn)(PVOID);
		getvfunc<OriginalFn>(this, 138)(this);
	}
	int32_t& GetThinkTick()
	{
		return *(int32_t*)((DWORD)this + 0x40);
	}
	void UnkFunc();

	void ForceBoneCache();

	 void SetAbsAngles(const Vector &angles);
	 void SetAbsOrigin(const Vector &origin);
	__forceinline float_t m_surfaceFriction();
	void SetLocalViewAngles(Vector angle);
	datamap_t *GetDataDescMap();
	datamap_t *GetPredDescMap();
	bool IsWeapon();
	std::array<float, 24> &m_flPoseParameter();
	float& GetFlashDuration();
	bool IsFlashed();
	bool HaveDefuser();
	Vector GetPunchAngle();
	Vector& GetPunchAngleVel();
	Vector* GetPunchAnglePtr();
	Vector GetViewPunchAngle();
	Vector* GetViewPunchAnglePtr();
	
	void UpdateClientSideAnimation()
	{
		typedef void(__thiscall *o_updateClientSideAnimation)(void*);
		getvfunc<o_updateClientSideAnimation>(this, 224)(this);
	}

	IBaseCombatWeapon* GetWeapon();
	int GetArmor();

	offset(get_most_recent_model_bone_counter(), uint32_t, 0x2690)
	offset(get_last_bone_setup_time(), float, 0x2924)

	__forceinline bool isAlive()
	{
		if (!this)
			return false;
		BYTE lifestate = *(BYTE*)((DWORD)this + 0x25F);
		return (lifestate == 0);
	}

	bool IsValid();
	
	uint32_t& GetOcclusionFlags()
	{
		return *(uint32_t*)((DWORD)this + 0xA28);
	}

	uint32_t& GetOcclusionFramecount()
	{
		return *(uint32_t*)((DWORD)this + 0xA30);
	}

	uint32_t& LastSkipFramecount()
	{
		return *(uint32_t*)((DWORD)this + 0xA68);
	}

	bool& MaintainSequenceTransition()
	{
		return *(bool*)((DWORD)this + 0x9F0);
	}

	int& GetClientEffects() {
		return *(int*)((uintptr_t)this + 0x68);
	}


	uint32_t& GetReadableBones()
	{
		return *(uint32_t*)((DWORD)this + 0x26AC);
	}

	float SequenceDuration(int sequence);

	float GetSequenceCycleRate(int sequence);

	float GetSequenceMoveDist(studiohdr_t* pStudioHdr, int iSequence);

	void getSequenceLinearMotion(c_studio_hdr* studioHdr, int sequence, Vector& v) noexcept;

	float GetSequenceMoveDist_2(c_studio_hdr* studioHdr, int sequence) noexcept;

	//void IBasePlayer::getSequenceLinearMotion(studiohdr_t* studioHdr, int sequence, Vector& v);

	uint32_t& GetWritableBones()
	{
		return *(uint32_t*)((DWORD)this + 0x26B0);
	}

	matrix* &GetBoneArrayForWrite()
	{
		return *(matrix**)((DWORD)this + 0x26A8);
	}

	void InvalidatePhysicsRecursive(int32_t flags);
	CCSGOPlayerAnimState* GetPlayerAnimState();
	void SetAnimState(CCSGOPlayerAnimState* anims);
	Vector& GetVecForce();
	__forceinline int GetFlags()
	{
		return *(int*)((DWORD)this + 0x104);
	}

	__forceinline int &GetFlagsPtr()
	{
		return *(int*)((DWORD)this + 0x104);
	}
	offset(get_unknown_ptr(), uint8_t, 0x274)
	int& GetEFlags();
	int& GetEffects();
	__forceinline int GetHealth()
	{
		if (!this)
			return -1;
		return *(int*)((DWORD)this + 0x100);
	}
	void ParsePoseParameter(std::array<float, 24>& param);
	void SetPoseParameter(std::array<float, 24> param);
	void ParseAnimOverlays(CAnimationLayer* anim);
	void SetAnimOverlays(CAnimationLayer* anim);

	bool ComputeHitboxSurroundingBox(Vector* mins, Vector* maxs);

	bool& TargetSpotted();

	c_studio_hdr* GetModelPtr();
	void AttachmentHelper();
	int& GetPlayerState();
	Vector& origin();
	bool& IsDefusing();
	bool& WaitForNoAttack();
	CBoneAccessor* GetBoneAccessor();
	CUtlVector<matrix>& GetBoneCache();
	void SetBoneCache(matrix* m);
	__forceinline void ShootPos(Vector* vec);
	void UpdateVisibilityAllEntities();
	void ModifyEyePosition(CCSGOPlayerAnimState*, Vector&);
	Vector GetEyePosition();
	IBaseCombatWeapon* GetWeaponWorldModel();

	void DrawServerHitboxes();
	__forceinline uint8_t* GetServerEdict();
	float &GetVelocityModifier();
	Vector GetEyeAngles();
	int GetSequence();
	float& GetFallVelocity();
	Vector* GetEyeAnglesPointer();
	bool IsPlayer();
	bool DormantWrapped();
	int GetTeam();
	__forceinline player_info_t GetPlayerInfo();
	__forceinline Vector GetOrigin()
	{
		if (!this)
			return Vector(0, 0, 0);
		return *(Vector*)((DWORD)this + 0x138);
	}
	bool GrenadeExploded();
	Vector& GetVecViewOffset();	
	float& GetDuckSpeed();
	const char* GetLastPlace();

	float & GetDuckAmount();
	void SetObserverMode(int value);

	int32_t GetObserverMode();
	Vector& GetVelocity()
	{
		return *(Vector*)((DWORD)this + 0x114);
	}

	__forceinline int GetShootsFired()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0xA390);
	}
	//NETVAR(thrower, "CBaseGrenade", "m_hThrower", int)
	__forceinline bool IsHostage();
	float& GetLBY();
	__forceinline bool IsChicken();
	Vector& GetAbsAngles();
	Vector& GetAbsOriginVirtual();
	__forceinline float m_angAbsRotation();
	Vector GetAbsOrigin();
	int Thrower();
	bool HasHelmet();
	bool HeavyArmor();
	Vector GetAbsOriginVec();
	int GetSequenceActivity(int sequence);
	matrix& GetrgflCoordinateFrame();
	IBaseViewModel* GetViewModel();
	std::vector< IBaseCombatWeapon* > GetWeapons();
	float GetOldSimulationTime();
	__forceinline void SetCurrentCommand(CUserCmd* cmd);
	__forceinline int GetTickBase();
	int* GetImpulse();
	float& GetCycle();
	float& GetAnimtime();
	bool IsScoped();

	__forceinline Vector WorldSpaceCenter()
	{
		Vector Max = this->GetCollideable()->OBBMaxs() + this->GetOrigin();
		Vector Min = this->GetCollideable()->OBBMaxs() + this->GetOrigin();

		Vector Size = Max - Min;

		Size /= 2;

		Size += Min;

		return Size;
	}

	size_t getintermediatedatasize()
	{
		const auto datamap = GetPredDescMap();

		size_t intermediate_size = 4;

		if (datamap->packed_size > 4)
			intermediate_size = datamap->packed_size;

		return intermediate_size;
	}

	bool SetupBones(matrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	float& GetSimulationTime();
	float CameFromDormantTime();
	float& GetSpawnTime()
	{
		return *(float*)((DWORD)this + 0x103C0);
	}

	__forceinline Vector GetBonePos(matrix* mat, int i)
	{
		if (!mat)
			return Vector(0, 0, 0);

		return Vector(mat[i][0][3], mat[i][1][3], mat[i][2][3]);
	}
	__forceinline VarMapping_t* GetVarMap()
	{
		return reinterpret_cast<VarMapping_t*>((DWORD)this + 0x24);
	}

	Vector GetNetworkOrigin();

	bool HasGunGameImmunity();
	float& GetMaxSpeed();
	bool& IsDucking();
	bool& IsWalking();
	int& GetMoveState();
	float& GetThirdpersonRecoil();
	int GetChokedPackets();
	DWORD GetLeaderHandle(); // for chicken
	void SetModelIndex(int index);
	int* GetWeaponsHandles(); //array of int size 48
	int* GetWearables();
	int& index();
	unsigned& GetModelIndex();
	bool IsVisible(Vector pos);
	bool IsVisible(IBasePlayer* to);
	int& ragdoll();
	float GetStamina();
};

class IBaseViewModel : public IBasePlayer {
public:
	float& GetAnimtime();
	float& GetCycle();
	CBaseHandle GetViewmodelWeapon();
	int GetViewModelIndex();
	float& GetCurrentCycle();
	float& GetModelAnimTime();
	int& GetCurrentSequence();
	int& GetAnimationParity();
	int& GetOwner();
};
class ClassIdManager
{
public:
	ClassIdManager();
private:
	int GetClassID(const char * classname);
public:
	int CAI_BaseNPC = -1;
	int CAK47 = -1;
	int CBaseAnimating = -1;
	int CBaseAnimatingOverlay = -1;
	int CBaseAttributableItem = -1;
	int CBaseButton = -1;
	int CBaseCombatCharacter = -1;
	int CBaseCombatWeapon = -1;
	int CBaseCSGrenade = -1;
	int CBaseCSGrenadeProjectile = -1;
	int CBaseDoor = -1;
	int IBasePlayer = -1;
	int CBaseFlex = -1;
	int CBaseGrenade = -1;
	int CBaseParticleEntity = -1;
	int CBasePlayer = -1;
	int CBasePropDoor = -1;
	int CBaseTeamObjectiveResource = -1;
	int CBaseTempEntity = -1;
	int CBaseToggle = -1;
	int CBaseTrigger = -1;
	int CBaseViewModel = -1;
	int CBaseVPhysicsTrigger = -1;
	int CBaseWeaponWorldModel = -1;
	int CBeam = -1;
	int CBeamSpotlight = -1;
	int CBoneFollower = -1;
	int CBRC4Target = -1;
	int CBreachCharge = -1;
	int CBreachChargeProjectile = -1;
	int CBreakableProp = -1;
	int CBreakableSurface = -1;
	int CBumpMineProjectile = -1;
	int CC4 = -1;
	int CCascadeLight = -1;
	int CChicken = -1;
	int CColorCorrection = -1;
	int CColorCorrectionVolume = -1;
	int CCSGameRulesProxy = -1;
	int CCSPlayer = -1;
	int CCSPlayerResource = -1;
	int CCSRagdoll = -1;
	int CCSTeam = -1;
	int CDangerZone = -1;
	int CDangerZoneController = -1;
	int CDEagle = -1;
	int CDecoyGrenade = -1;
	int CDecoyProjectile = -1;
	int CDrone = -1;
	int CDronegun = -1;
	int CDynamicLight = -1;
	int CDynamicProp = -1;
	int CEconEntity = -1;
	int CEconWearable = -1;
	int CEmbers = -1;
	int CEntityDissolve = -1;
	int CEntityFlame = -1;
	int CEntityFreezing = -1;
	int CEntityParticleTrail = -1;
	int CEnvAmbientLight = -1;
	int CEnvDetailController = -1;
	int CEnvDOFController = -1;
	int CEnvGasCanister = -1;
	int CEnvParticleScript = -1;
	int CEnvProjectedTexture = -1;
	int CEnvQuadraticBeam = -1;
	int CEnvScreenEffect = -1;
	int CEnvScreenOverlay = -1;
	int CEnvTonemapController = -1;
	int CEnvWind = -1;
	int CFEPlayerDecal = -1;
	int CFireCrackerBlast = -1;
	int CFireSmoke = -1;
	int CFireTrail = -1;
	int CFish = -1;
	int CFists = -1;
	int CFlashbang = -1;
	int CFogController = -1;
	int CFootstepControl = -1;
	int CFunc_Dust = -1;
	int CFunc_LOD = -1;
	int CFuncAreaPortalWindow = -1;
	int CFuncBrush = -1;
	int CFuncConveyor = -1;
	int CFuncLadder = -1;
	int CFuncMonitor = -1;
	int CFuncMoveLinear = -1;
	int CFuncOccluder = -1;
	int CFuncReflectiveGlass = -1;
	int CFuncRotating = -1;
	int CFuncSmokeVolume = -1;
	int CFuncTrackTrain = -1;
	int CGameRulesProxy = -1;
	int CGrassBurn = -1;
	int CHandleTest = -1;
	int CHEGrenade = -1;
	int CHostage = -1;
	int CHostageCarriableProp = -1;
	int CIncendiaryGrenade = -1;
	int CInferno = -1;
	int CInfoLadderDismount = -1;
	int CInfoMapRegion = -1;
	int CInfoOverlayAccessor = -1;
	int CItem_Healthshot = -1;
	int CItemCash = -1;
	int CItemDogtags = -1;
	int CKnife = -1;
	int CKnifeGG = -1;
	int CLightGlow = -1;
	int CMaterialModifyControl = -1;
	int CMelee = -1;
	int CMolotovGrenade = -1;
	int CMolotovProjectile = -1;
	int CMovieDisplay = -1;
	int CParadropChopper = -1;
	int CParticleFire = -1;
	int CParticlePerformanceMonitor = -1;
	int CParticleSystem = -1;
	int CPhysBox = -1;
	int CPhysBoxMultiplayer = -1;
	int CPhysicsProp = -1;
	int CPhysicsPropMultiplayer = -1;
	int CPhysMagnet = -1;
	int CPhysPropAmmoBox = -1;
	int CPhysPropLootCrate = -1;
	int CPhysPropRadarJammer = -1;
	int CPhysPropWeaponUpgrade = -1;
	int CPlantedC4 = -1;
	int CPlasma = -1;
	int CPlayerResource = -1;
	int CPointCamera = -1;
	int CPointCommentaryNode = -1;
	int CPointWorldText = -1;
	int CPoseController = -1;
	int CPostProcessController = -1;
	int CPrecipitation = -1;
	int CPrecipitationBlocker = -1;
	int CPredictedViewModel = -1;
	int CProp_Hallucination = -1;
	int CPropCounter = -1;
	int CPropDoorRotating = -1;
	int CPropJeep = -1;
	int CPropVehicleDriveable = -1;
	int CRagdollManager = -1;
	int CRagdollProp = -1;
	int CRagdollPropAttached = -1;
	int CRopeKeyframe = -1;
	int CSCAR17 = -1;
	int CSceneEntity = -1;
	int CSensorGrenade = -1;
	int CSensorGrenadeProjectile = -1;
	int CShadowControl = -1;
	int CSlideshowDisplay = -1;
	int CSmokeGrenade = -1;
	int CSmokeGrenadeProjectile = -1;
	int CSmokeStack = -1;
	int CSnowball = -1;
	int CSnowballPile = -1;
	int CSnowballProjectile = -1;
	int CSpatialEntity = -1;
	int CSpotlightEnd = -1;
	int CSprite = -1;
	int CSpriteOriented = -1;
	int CSpriteTrail = -1;
	int CStatueProp = -1;
	int CSteamJet = -1;
	int CSun = -1;
	int CSunlightShadowControl = -1;
	int CSurvivalSpawnChopper = -1;
	int CTablet = -1;
	int CTeam = -1;
	int CTeamplayRoundBasedRulesProxy = -1;
	int CTEArmorRicochet = -1;
	int CTEBaseBeam = -1;
	int CTEBeamEntPoint = -1;
	int CTEBeamEnts = -1;
	int CTEBeamFollow = -1;
	int CTEBeamLaser = -1;
	int CTEBeamPoints = -1;
	int CTEBeamRing = -1;
	int CTEBeamRingPoint = -1;
	int CTEBeamSpline = -1;
	int CTEBloodSprite = -1;
	int CTEBloodStream = -1;
	int CTEBreakModel = -1;
	int CTEBSPDecal = -1;
	int CTEBubbles = -1;
	int CTEBubbleTrail = -1;
	int CTEClientProjectile = -1;
	int CTEDecal = -1;
	int CTEDust = -1;
	int CTEDynamicLight = -1;
	int CTEEffectDispatch = -1;
	int CTEEnergySplash = -1;
	int CTEExplosion = -1;
	int CTEFireBullets = -1;
	int CTEFizz = -1;
	int CTEFootprintDecal = -1;
	int CTEFoundryHelpers = -1;
	int CTEGaussExplosion = -1;
	int CTEGlowSprite = -1;
	int CTEImpact = -1;
	int CTEKillPlayerAttachments = -1;
	int CTELargeFunnel = -1;
	int CTEMetalSparks = -1;
	int CTEMuzzleFlash = -1;
	int CTEParticleSystem = -1;
	int CTEPhysicsProp = -1;
	int CTEPlantBomb = -1;
	int CTEPlayerAnimEvent = -1;
	int CTEPlayerDecal = -1;
	int CTEProjectedDecal = -1;
	int CTERadioIcon = -1;
	int CTEShatterSurface = -1;
	int CTEShowLine = -1;
	int CTesla = -1;
	int CTESmoke = -1;
	int CTESparks = -1;
	int CTESprite = -1;
	int CTESpriteSpray = -1;
	int CTest_ProxyToggle_Networkable = -1;
	int CTestTraceline = -1;
	int CTEWorldDecal = -1;
	int CTriggerPlayerMovement = -1;
	int CTriggerSoundOperator = -1;
	int CVGuiScreen = -1;
	int CVoteController = -1;
	int CWaterBullet = -1;
	int CWaterLODControl = -1;
	int CWeaponAug = -1;
	int CWeaponAWP = -1;
	int CWeaponBaseItem = -1;
	int CWeaponBizon = -1;
	int CWeaponCSBase = -1;
	int CWeaponCSBaseGun = -1;
	int CWeaponCycler = -1;
	int CWeaponElite = -1;
	int CWeaponFamas = -1;
	int CWeaponFiveSeven = -1;
	int CWeaponG3SG1 = -1;
	int CWeaponGalil = -1;
	int CWeaponGalilAR = -1;
	int CWeaponGlock = -1;
	int CWeaponHKP2000 = -1;
	int CWeaponM249 = -1;
	int CWeaponM3 = -1;
	int CWeaponM4A1 = -1;
	int CWeaponMAC10 = -1;
	int CWeaponMag7 = -1;
	int CWeaponMP5Navy = -1;
	int CWeaponMP7 = -1;
	int CWeaponMP9 = -1;
	int CWeaponNegev = -1;
	int CWeaponNOVA = -1;
	int CWeaponP228 = -1;
	int CWeaponP250 = -1;
	int CWeaponP90 = -1;
	int CWeaponSawedoff = -1;
	int CWeaponSCAR20 = -1;
	int CWeaponScout = -1;
	int CWeaponSG550 = -1;
	int CWeaponSG552 = -1;
	int CWeaponSG556 = -1;
	int CWeaponSSG08 = -1;
	int CWeaponTaser = -1;
	int CWeaponTec9 = -1;
	int CWeaponTMP = -1;
	int CWeaponUMP45 = -1;
	int CWeaponUSP = -1;
	int CWeaponXM1014 = -1;
	int CWorld = -1;
	int CWorldVguiText = -1;
	int DustTrail = -1;
	int MovieExplosion = -1;
	int ParticleSmokeGrenade = -1;
	int RocketTrail = -1;
	int SmokeTrail = -1;
	int SporeExplosion = -1;
	int SporeTrail = -1;
};

extern ClassIdManager * ClassId;

enum weapons
{
	weapon_deagle = 1,
	weapon_elite = 2,
	weapon_fiveseven = 3,
	weapon_glock = 4,
	weapon_ak47 = 7,
	weapon_aug = 8,
	weapon_awp = 9,
	weapon_famas = 10,
	weapon_g3sg1 = 11,
	weapon_galilar = 13,
	weapon_m249 = 14,
	weapon_m4a1 = 16,
	weapon_mac10 = 17,
	weapon_p90 = 19,
	weapon_ump = 24,
	weapon_mp5sd = 23,
	weapon_xm1014 = 25,
	weapon_bizon = 26,
	weapon_mag7 = 27,
	weapon_negev = 28,
	weapon_sawedoff = 29,
	weapon_tec9 = 30,
	weapon_taser = 31,
	weapon_hkp2000 = 32,
	weapon_mp7 = 33,
	weapon_mp9 = 34,
	weapon_nova = 35,
	weapon_p250 = 36,
	weapon_scar20 = 38,
	weapon_sg556 = 39,
	weapon_ssg08 = 40,
	weapon_knife = 42,
	weapon_flashbang = 43,
	weapon_hegrenade = 44,
	weapon_smokegrenade = 45,
	weapon_molotov = 46,
	weapon_decoy = 47,
	weapon_incgrenade = 48,
	weapon_c4 = 49,
	weapon_knife_t = 59,
	weapon_m4a1_silencer = 60,
	weapon_usp_silencer = 61,
	weapon_cz75a = 63,
	weapon_revolver = 64,
	weapon_bayonet = 500,
	weapon_knife_flip = 505,
	weapon_knife_gut = 506,
	weapon_knife_karambit = 507,
	weapon_knife_m9_bayonet = 508,
	weapon_knife_tactical = 509,
	weapon_knife_falchion = 512,
	weapon_knife_survival_bowie = 514,
	weapon_knife_butterfly = 515,
	weapon_knife_push = 516
};

enum
{
	deagle = 1,
	p2000 = 32,
	fiveseven = 3,
	glock = 4,
	dualelites = 2,
	ak = 7,
	aug = 8,
	awp = 9,
	famas = 10,
	g3sg1 = 11,
	galil = 13,
	p250 = 36,
	tec9 = 30,
	m249 = 14,
	m4 = 16,
	mac10 = 17,
	p90 = 19,
	ump45 = 24,
	ppbizon = 26,
	negev = 28,
	mp7 = 33,
	mp9 = 34,
	scar30 = 38,
	sg553 = 39,
	ssg08 = 40,
	goldknife = 41,
	ctknife = 42,
	tknife = 59,
	flashgren = 43,
	hegren = 44,
	smoke = 45,
	molotov = 46,
	incendiary = 48,
	decoygren = 47,
	bomb = 49,
	nova = 35,
	xm1014 = 25,
	sawedoff = 29,
	mag7 = 27,
	zeus = 31
};
/*
enum eWeaponType1 {
	WEAPON_DEAGLE = 1,
	WEAPON_DUALBERETTA = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALIL = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_ZEUSX27 = 31,
	WEAPON_USP = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG553 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE_GG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INC = 48,
	WEAPON_C4 = 49,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1S = 60,
	WEAPON_USPS = 61,
	WEAPON_CZ75 = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};
*/

extern unsigned int FindInDataMap(datamap_t* pMap, const char* name);
class IBaseCombatWeapon : public IBasePlayer
{
public:
	bool CanFire();
	int Owner();
	bool InReload();
	float& NextSecondaryAttack();
	int GetItemDefinitionIndex();
	float NextPrimaryAttack();
	float GetLastShotTime();
	int GetAmmo(bool second);
	int32_t GetZoomLevel();

	bool m_bReloadVisuallyComplete();
	float &GetAccuracyPenalty();
	float &GetRecoilIndex();
	float LastShotTime();
	bool StartedArming();
	float GetPostponeFireReadyTime();
	void SetPostPoneTime(float asdasdasd);
	bool IsCanScope()
	{
		if (!this)
			return false;

		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_AWP || WeaponId == WEAPON_SCAR20 || WeaponId == WEAPON_G3SG1 || WeaponId == WEAPON_SSG08 || WeaponId == WEAPON_SG556 || WeaponId == WEAPON_AUG;
	}

	bool IsAuto()
	{
		if (this == nullptr)
			return false;
		return this->GetItemDefinitionIndex() == WEAPON_SCAR20 || this->GetItemDefinitionIndex() == WEAPON_G3SG1;
	}

	bool IsAWP()
	{
		if (this == nullptr)
			return false;
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_AWP;
	}

	bool IsSMG()
	{
		if (this == nullptr)
			return false;
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_BIZON || WeaponId == WEAPON_MAC10 || WeaponId == WEAPON_MP5_SD || WeaponId == WEAPON_MP7 || WeaponId == WEAPON_MP9 || WeaponId == WEAPON_P90 || WeaponId == WEAPON_UMP45;
	}

	float GetInaccuracy()
	{
		if (!this)
			return 0.f;

		typedef float(__thiscall* OriginalFn)(void*);
		return getvfunc< OriginalFn >(this, 483)(this);
	}

	float GetSpread()
	{
		if (!this)
			return 0.f;

		typedef float(__thiscall* OriginalFn)(void*);
		return getvfunc< OriginalFn >(this, 453)(this);
	}

	int GetMaxTickbaseShift()
	{
		if (!DTable())
			return 14;

		auto idx = GetItemDefinitionIndex();
		auto max_tickbase_shift = 0;

		switch (idx)
		{
		case WEAPON_M249:
		case WEAPON_MAC10:
		case WEAPON_P90:
		case WEAPON_MP5_SD:
		case WEAPON_NEGEV:
		case WEAPON_MP9:
			max_tickbase_shift = 5;
			break;
		case WEAPON_ELITE:
		case WEAPON_UMP45:
		case WEAPON_BIZON:
		case WEAPON_TEC9:
		case WEAPON_MP7:
			max_tickbase_shift = 6;
			break;
		case WEAPON_AK47:
		case WEAPON_AUG:
		case WEAPON_FAMAS:
		case WEAPON_GALILAR:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_CZ75A:
			max_tickbase_shift = 7;
			break;
		case WEAPON_FIVESEVEN:
		case WEAPON_GLOCK:
		case WEAPON_P250:
		case WEAPON_SG556:
			max_tickbase_shift = 8;
			break;
		case WEAPON_HKP2000:
		case WEAPON_USP_SILENCER:
			max_tickbase_shift = 9;
			break;
		case WEAPON_DEAGLE:
			max_tickbase_shift = 13;
			break;
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
			max_tickbase_shift = 14;
			break;
		}

		//if (csgo->game_rules->IsValveDS())
			//max_tickbase_shift = min(max_tickbase_shift, 6);

		return max_tickbase_shift;
	}
	CCSWeaponInfo* GetCSWpnData();
	int32_t WeaponMode();
	void UpdateAccuracyPenalty()
	{
		if (!this) return;

		typedef void(__thiscall* oUpdateAccuracyPenalty)(PVOID);
		return getvfunc<oUpdateAccuracyPenalty>(this, 484)(this);
	}

	float hitchance()
	{
		float hitchance = 101.f;
		if (!this) return 0.f;

		float inaccuracy = GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;
		return hitchance;
	}

	std::string GetGunName()
	{
		if (!this)
			return crypt_str("???");
		switch (this->GetItemDefinitionIndex())
		{
		case WEAPON_AK47:
			return crypt_str("AK47");
			break;
		case WEAPON_AUG:
			return crypt_str("AUG");
			break;
		case weapon_revolver:
			return crypt_str("Revolver");
			break;
		case WEAPON_AWP:
			return crypt_str("AWP");
			break;
		case WEAPON_BIZON:
			return crypt_str("Bizon");
			break;
		case WEAPON_C4:
			return crypt_str("Bomb");
			break;
		case WEAPON_CZ75A:
			return crypt_str("CZ75");
			break;
		case WEAPON_DEAGLE:
			return crypt_str("Deagle");
			break;
		case WEAPON_DECOY:
			return crypt_str("Decoy");
			break;
		case WEAPON_ELITE:
			return crypt_str("Dual Berettas");
			break;
		case WEAPON_FAMAS:
			return crypt_str("Famas");
			break;
		case WEAPON_FIVESEVEN:
			return crypt_str("Five-SeveN");
			break;
		case WEAPON_FLASHBANG:
			return crypt_str("Flashbang");
			break;
		case WEAPON_G3SG1:
			return crypt_str("G3SG1");
			break;
		case WEAPON_GALILAR:
			return crypt_str("Galil");
			break;
		case WEAPON_GLOCK:
			return crypt_str("Glock-18");
			break;
		case WEAPON_INCGRENADE:
			return crypt_str("Incendiary");
			break;
		case WEAPON_MOLOTOV:
			return crypt_str("Molotov");
			break;
		case WEAPON_SSG08:
			return crypt_str("SSG-08");
			break;
		case WEAPON_HEGRENADE:
			return crypt_str("HE Grenade");
			break;
		case WEAPON_M249:
			return crypt_str("M249");
			break;
		case WEAPON_M4A1:
			return crypt_str("M4A1");
			break;
		case WEAPON_MAC10:
			return crypt_str("MAC10");
			break;
		case WEAPON_MAG7:
			return crypt_str("MAG7");
			break;
		case WEAPON_MP7:
			return crypt_str("MP7");
			break;
		case WEAPON_MP9:
			return crypt_str("MP9");
			break;
		case WEAPON_NOVA:
			return crypt_str("Nova");
			break;
		case WEAPON_NEGEV:
			return crypt_str("Negev");
			break;
		case WEAPON_P250:
			return crypt_str("P250");
			break;
		case WEAPON_P90:
			return crypt_str("P90");
			break;
		case WEAPON_SAWEDOFF:
			return crypt_str("Sawed-Off");
			break;
		case WEAPON_SCAR20:
			return crypt_str("SCAR20");
			break;
		case WEAPON_SMOKEGRENADE:
			return crypt_str("Smoke");
			break;
		case WEAPON_SG556:
			return crypt_str("SG553");
			break;
		case WEAPON_TEC9:
			return crypt_str("Tec-9");
			break;
		case WEAPON_HKP2000:
			return crypt_str("P2000");
			break;
		case WEAPON_USP_SILENCER:
			return crypt_str("USP-S");
			break;
		case WEAPON_UMP45:
			return crypt_str("UMP-45");
			break;
		case WEAPON_XM1014:
			return crypt_str("XM1014");
			break;
		case WEAPON_TASER:
			return crypt_str("Taser");
			break;
		case WEAPON_M4A1_SILENCER:
			return crypt_str("M4A1-S");
			break;
		case 57:
			return crypt_str("Healthshot");
			break;
		case WEAPON_MP5_SD:
			return crypt_str("MP5");
			break;
		}
		if (IsKnife())
			return crypt_str("Knife");
		return crypt_str("?");
	}
	bool IsZeus()
	{
		if (!this)
			return false;

		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_TASER;
	}
	bool IsBomb()
	{
		int iWeaponID = this->GetItemDefinitionIndex();

		return iWeaponID == bomb;
	}
	bool IsNade();

	bool IsMiscWeapon()
	{
		if (!this)
			return false;
		int iWeaponID = this->GetItemDefinitionIndex();
		return (iWeaponID == 57 || iWeaponID == tknife || iWeaponID == ctknife
			|| iWeaponID == goldknife || iWeaponID == bomb
			|| iWeaponID == hegren || iWeaponID == decoygren
			|| iWeaponID == flashgren || iWeaponID == molotov
			|| iWeaponID == smoke || iWeaponID == incendiary || iWeaponID == 59 || iWeaponID == 57
			|| iWeaponID == 500 || iWeaponID == 505 || iWeaponID == 506
			|| iWeaponID == 507 || iWeaponID == 508 || iWeaponID == 509
			|| iWeaponID == 515);
	}
	bool IsKnife();
	bool isPistol()
	{
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_DEAGLE || WeaponId == WEAPON_ELITE || WeaponId == WEAPON_FIVESEVEN || WeaponId == WEAPON_P250 ||
			WeaponId == WEAPON_GLOCK || WeaponId == WEAPON_USP_SILENCER || WeaponId == WEAPON_CZ75A || WeaponId == WEAPON_HKP2000 ||
			WeaponId == WEAPON_TEC9 || WeaponId == weapon_revolver;
	}
	bool IsHeavyPistol() {
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_DEAGLE || WeaponId == weapon_revolver;
	}

	bool isShotgun()
	{
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_XM1014 || WeaponId == WEAPON_NOVA || WeaponId == WEAPON_SAWEDOFF || WeaponId == WEAPON_MAG7;
	}
	bool isRifle()
	{
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_AK47 || WeaponId == WEAPON_M4A1 || WeaponId == WEAPON_M4A1_SILENCER || WeaponId == WEAPON_GALILAR || WeaponId == WEAPON_FAMAS ||
			WeaponId == WEAPON_AUG || WeaponId == WEAPON_SG556;
	}
	bool isSniper()
	{
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_AWP || WeaponId == WEAPON_SCAR20 || WeaponId == WEAPON_G3SG1 || WeaponId == WEAPON_SSG08;
	}
	bool DTable() {
		int WeaponId = this->GetItemDefinitionIndex();
		return IsGun() && !isShotgun() && WeaponId != WEAPON_SSG08 && WeaponId != WEAPON_AWP;
	}
	bool isAutoSniper()
	{
		int WeaponId = this->GetItemDefinitionIndex();

		return WeaponId == WEAPON_SCAR20 || WeaponId == WEAPON_G3SG1;
	}
	bool IsGun();

	int& GetWeaponWorldModel();
};

class IBaseAttributableItem : public IBaseCombatWeapon
{
public:
	int& GetAccountID();
	short& GetItemDefinitionIndex();
	int& GetItemIDHigh();
	int& GetEntityQuality();
	char* GetCustomName();
	unsigned& GetFallbackPaintKit();
	unsigned& GetFallbackSeed();
	float& GetFallbackWear();
	unsigned& GetFallBackStatTrak();
};

class CBaseCSGrenade : IBaseCombatWeapon
{
public:
	float GetThrowTime();
};
class bf_read {
public:
	const char* m_pDebugName;
	bool m_bOverflow;
	int m_nDataBits;
	unsigned int m_nDataBytes;
	unsigned int m_nInBufWord;
	int m_nBitsAvail;
	const unsigned int* m_pDataIn;
	const unsigned int* m_pBufferEnd;
	const unsigned int* m_pData;

	bf_read() = default;

	bf_read(const void* pData, int nBytes, int nBits = -1) {
		StartReading(pData, nBytes, 0, nBits);
	}

	void StartReading(const void* pData, int nBytes, int iStartBit, int nBits) {
		// Make sure it's dword aligned and padded.
		m_pData = (uint32_t*)pData;
		m_pDataIn = m_pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}
		m_bOverflow = false;
		m_pBufferEnd = reinterpret_cast<uint32_t const*>(reinterpret_cast<uint8_t const*>(m_pData) + nBytes);
		if (m_pData)
			Seek(iStartBit);
	}

	bool Seek(int nPosition) {
		bool bSucc = true;
		if (nPosition < 0 || nPosition > m_nDataBits) {
			m_bOverflow = true;
			bSucc = false;
			nPosition = m_nDataBits;
		}
		int nHead = m_nDataBytes & 3; // non-multiple-of-4 bytes at head of buffer. We put the "round off"
									  // at the head to make reading and detecting the end efficient.

		int nByteOfs = nPosition / 8;
		if ((m_nDataBytes < 4) || (nHead && (nByteOfs < nHead))) {
			// partial first dword
			uint8_t const* pPartial = (uint8_t const*)m_pData;
			if (m_pData) {
				m_nInBufWord = *(pPartial++);
				if (nHead > 1)
					m_nInBufWord |= (*pPartial++) << 8;
				if (nHead > 2)
					m_nInBufWord |= (*pPartial++) << 16;
			}
			m_pDataIn = (uint32_t const*)pPartial;
			m_nInBufWord >>= (nPosition & 31);
			m_nBitsAvail = (nHead << 3) - (nPosition & 31);
		}
		else {
			int nAdjPosition = nPosition - (nHead << 3);
			m_pDataIn = reinterpret_cast<uint32_t const*>(
				reinterpret_cast<uint8_t const*>(m_pData) + ((nAdjPosition / 32) << 2) + nHead);
			if (m_pData) {
				m_nBitsAvail = 32;
				GrabNextDWord();
			}
			else {
				m_nInBufWord = 0;
				m_nBitsAvail = 1;
			}
			m_nInBufWord >>= (nAdjPosition & 31);
			m_nBitsAvail = min(m_nBitsAvail, 32 - (nAdjPosition & 31)); // in case grabnextdword overflowed
		}
		return bSucc;
	}

	FORCEINLINE void GrabNextDWord(bool bOverFlowImmediately = false) {
		if (m_pDataIn == m_pBufferEnd) {
			m_nBitsAvail = 1; // so that next read will run out of words
			m_nInBufWord = 0;
			m_pDataIn++; // so seek count increments like old
			if (bOverFlowImmediately)
				m_bOverflow = true;
		}
		else if (m_pDataIn > m_pBufferEnd) {
			m_bOverflow = true;
			m_nInBufWord = 0;
		}
		else {
			m_nInBufWord = DWORD(*(m_pDataIn++));
		}
	}
};
class bf_write {
public:
	unsigned char* m_pData;
	int m_nDataBytes;
	int m_nDataBits;
	int m_iCurBit;
	bool m_bOverflow;
	bool m_bAssertOnOverflow;
	const char* m_pDebugName;

	void StartWriting(void* pData, int nBytes, int iStartBit = 0, int nBits = -1) {
		// Make sure it's dword aligned and padded.
		// The writing code will overrun the end of the buffer if it isn't dword aligned, so truncate to force alignment
		nBytes &= ~3;

		m_pData = (unsigned char*)pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}

		m_iCurBit = iStartBit;
		m_bOverflow = false;
	}

	bf_write() {
		m_pData = NULL;
		m_nDataBytes = 0;
		m_nDataBits = -1; // set to -1 so we generate overflow on any operation
		m_iCurBit = 0;
		m_bOverflow = false;
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
	}

	// nMaxBits can be used as the number of bits in the buffer.
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
		StartWriting(pData, nBytes, 0, nBits);
	}

	bf_write(const char* pDebugName, void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = pDebugName;
		StartWriting(pData, nBytes, 0, nBits);
	}
};
class CLC_Move{
private:
	char __PAD0[0x8]; // 0x0 two vtables
public:
	int m_nBackupCommands; // 0x8
	int m_nNewCommands;    // 0xC
	std::string* m_data;   // 0x10 std::string
	bf_read m_DataIn;      // 0x14
	bf_write m_DataOut;    // 0x38
};                       // size: 0x50

class INetChannel;
class INetMessage
{
public:
	virtual					~INetMessage() { }
	virtual void			SetNetChannel(void* pNetChannel) = 0;
	virtual void			SetReliable(bool bState) = 0;
	virtual bool			Process() = 0;
	virtual	bool			ReadFromBuffer(bf_read& buffer) = 0;
	virtual	bool			WriteToBuffer(bf_write& buffer) = 0;
	virtual bool			IsReliable() const = 0;
	virtual int				GetType() const = 0;
	virtual int				GetGroup() const = 0;
	virtual const char* GetName() const = 0;
	virtual void* GetNetChannel(void) const = 0;
	virtual const char* ToString() const = 0;
};

class INetChannel
{
public:
	char pad_0x0000[0x18]; //0x0000
	__int32 iOutSequenceNr; //0x0018 
	__int32 iInSequenceNr; //0x001C 
	__int32 iOutSequenceNrAck; //0x0020 
	__int32 iOutReliableState; //0x0024 
	__int32 iInReliableState; //0x0028 
	__int32 iChokedPackets; //0x002C

	void Transmit(bool onlyreliable)
	{
		using Fn = bool(__thiscall*)(void*, bool);
		getvfunc<Fn>(this, 49)(this, onlyreliable);
	}

	void SetChoked()
	{
		typedef void(__thiscall* oSetChocked)(INetChannel*);
		getvfunc<oSetChocked>(this, 45)(this);
	}

	void SendDatagram()
	{
		using Fn = int(__thiscall*)(void*, void*);
		getvfunc<Fn>(this, 46)(this, 0);
	}

	void SetTimeOut(float seconds)
	{
		using Fn = void(__thiscall*)(void*, float);
		return getvfunc<Fn>(this, 4)(this, seconds);
	}

	int RequestFile(const char* filename)
	{
		using Fn = int(__thiscall*)(void*, const char*);
		return getvfunc<Fn>(this, 62)(this, filename);
	}
};

template < typename T >
class CNetMessagePB : public INetMessage, public T {};
using CCLCMsg_Move_t = CNetMessagePB< CLC_Move >;

class CFogController : public IBasePlayer {
public:

	int& enable();
	float& start();
	float& end();
	float& density();
	int& colorPrimary();
};