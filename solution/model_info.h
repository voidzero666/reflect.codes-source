#include "DLL_MAIN.h"

enum Bone : int
{
	BONE_PELVIS = 0,
	LEAN_ROOT,
	CAM_DRIVER,
	BONE_HIP,
	BONE_LOWER_SPINAL_COLUMN,
	BONE_MIDDLE_SPINAL_COLUMN,
	BONE_UPPER_SPINAL_COLUMN,
	BONE_NECK,
	BONE_HEAD,
};

enum Hitboxes : int
{
	HITBOX_HEAD = 0,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_SPINE,
	HITBOX_LEGS,
	HITBOX_ARMS,
};

enum Hitboxes2 : int
{
	VHITBOX_HEAD,
	VHITBOX_NECK,
	VHITBOX_PELVIS,
	VHITBOX_BODY,
	VHITBOX_THORAX,
	VHITBOX_CHEST,
	VHITBOX_UPPER_CHEST,
	VHITBOX_RIGHT_THIGH,
	VHITBOX_LEFT_THIGH,
	VHITBOX_LEFT_CALF,
	VHITBOX_RIGHT_CALF,
	VHITBOX_LEFT_FOOT,
	VHITBOX_RIGHT_FOOT,
	VHITBOX_LEFT_HAND,
	VHITBOX_RIGHT_HAND,
	VHITBOX_LEFT_UPPER_ARM,
	VHITBOX_LEFT_FOREARM,
	VHITBOX_RIGHT_UPPER_ARM,
	VHITBOX_RIGHT_FOREARM,
	VHITBOX_MAX
};


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
struct vcollide_t;
struct model_t;
class Vector;
class CGameTrace;
struct cplane_t;
typedef CGameTrace trace_t;
struct studiohdr_t;
struct virtualmodel_t;
typedef unsigned char BYTE;
struct virtualterrainparams_t;
class CPhysCollide;
class CUtlBuffer;
class IClientRenderable;
class Quaternion;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct mstudiobodyparts_t;
struct mstudiotexture_t;

class RadianEuler
{
public:
	inline RadianEuler(void) { }
	inline RadianEuler(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	inline RadianEuler(Quaternion const &q);	// evil auto type promotion!!!
	inline RadianEuler(Vector const &angles);	// evil auto type promotion!!!

												// Initialization
	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) { x = ix; y = iy; z = iz; }

	//	conversion to Vector
	Vector ToQAngle(void) const;
	bool IsValid() const;
	void Invalidate();

	inline float *Base() { return &x; }
	inline const float *Base() const { return &x; }

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	float x, y, z;
};


class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a float[4]
public:
	inline Quaternion(void) {}
	inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) {}

	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

	float* Base() { return (float*)this; }
	const float* Base() const { return (float*)this; }

	float x, y, z, w;
};


#define DECL_ALIGN(x)			__declspec( align( x ) )
#define ALIGN16 DECL_ALIGN(16)

class ALIGN16 QuaternionAligned : public Quaternion
{
public:
	inline QuaternionAligned(void) {};
	inline QuaternionAligned(float X, float Y, float Z, float W)
	{
		Init(X, Y, Z, W);
	}
public:
	explicit QuaternionAligned(const Quaternion &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
	}

	QuaternionAligned& operator=(const Quaternion &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
		return *this;
	}
};

struct mstudiohitboxset_t
{
	int sznameindex;
	int numhitboxes;
	int hitboxindex;

	inline mstudiobbox_t* pHitbox(int i) const
	{
		return (mstudiobbox_t*)(((BYTE*)this) + hitboxindex) + i;
	}
};

struct mstudiobone_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

											// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix				poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void *pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((BYTE *)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char * const pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
	inline int			GetSurfaceProp(void) const { return surfacepropLookup; }

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate
};

struct mstudiobonecontroller_t
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// BYTE index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};


struct mstudioattachment_t
{
	int					sznameindex;
	inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
	unsigned int		flags;
	int					localbone;
	matrix			local; // attachment point
	int					unused[8];
};
struct studiohdr_t
{
	int    id;					// 0x0000
	int    version;				// 0x0004
	int    checksum;				// 0x0008
	char   name[64];			// 0x000C
	int    length;				// 0x004C
	Vector eye_pos;				// 0x0050
	Vector illum_pos;				// 0x005C
	Vector hull_mins;				// 0x0068
	Vector hull_maxs;             // 0x0074
	Vector view_mins;             // 0x0080
	Vector view_maxs;             // 0x008C
	int    flags;					// 0x0098
	int    num_bones;				// 0x009C
	int    bone_id;				// 0x00A0
	int    num_controllers;		// 0x00A4
	int    controller_id;			// 0x00A8
	int    num_sets;				// 0x00AC
	int    set_id;				// 0x00B0

	__forceinline mstudiobone_t* GetBone(int index) const {
		return (mstudiobone_t*)(((BYTE*)this) + bone_id) + index;
	}

	__forceinline mstudiohitboxset_t* pHitboxSet(int index) const {
		return (mstudiohitboxset_t*)(((BYTE*)this) + set_id) + index;
	}
	inline mstudiobbox_t *pHitbox(int i, int set) const
	{
		mstudiohitboxset_t const *s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->pHitbox(i);
	};

	//int					id;
	//int					version;

	//long				checksum;		// this has to be the same in the phy and vtx files to load!

	//inline const char *	pszName(void) const { return name; }
	//char				name[64];

	//int					length;

	//Vector				eyeposition;	// ideal eye position

	//Vector				illumposition;	// illumination center

	//Vector				hull_min;		// ideal movement hull size
	//Vector				hull_max;

	//Vector				view_bbmin;		// clipping bounding box
	//Vector				view_bbmax;

	//int					flags;

	//int					numbones;			// bones
	//int					boneindex;
	//inline mstudiobone_t *pBone(int i) const {
	//	//Assert(i >= 0 && i < numbones); 
	//	return (mstudiobone_t *)(((BYTE *)this) + boneindex) + i;
	//};
	//int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone
	//int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone

	//int					numbonecontrollers;		// bone controllers
	//int					bonecontrollerindex;
	//inline mstudiobonecontroller_t *pBonecontroller(int i) const {
	//	//Assert(i >= 0 && i < numbonecontrollers);
	//	return (mstudiobonecontroller_t *)(((BYTE *)this) + bonecontrollerindex) + i;
	//};

	//inline mstudioattachment_t	*pLocalAttachment(int i) const { 
	//	//Assert(i >= 0 && i < numlocalattachments); 
	//	return (mstudioattachment_t *)(((BYTE *)this) + localattachmentindex) + i;
	//};


	//int					numhitboxsets;
	//int					hitboxsetindex;

	//// Look up hitbox set by index
	//mstudiohitboxset_t	*pHitboxSet(int i) const
	//{
	//	//Assert(i >= 0 && i < numhitboxsets);
	//	return (mstudiohitboxset_t*)(((BYTE*)this) + hitboxsetindex) + i;
	//};

	//// Calls through to hitbox to determine size of specified set
	//inline mstudiobbox_t *pHitbox(int i, int set) const
	//{
	//	mstudiohitboxset_t const *s = pHitboxSet(set);
	//	if (!s)
	//		return NULL;

	//	return s->pHitbox(i);
	//};

	//// Calls through to set to get hitbox count for set
	//inline int			iHitboxCount(int set) const
	//{
	//	mstudiohitboxset_t const *s = pHitboxSet(set);
	//	if (!s)
	//		return 0;

	//	return s->numhitboxes;
	//};

	//// file local animations? and sequences
	////private:
	//int					numlocalanim;			// animations/poses
	//int					localanimindex;		// animation descriptions
	//inline mstudioanimdesc_t *pLocalAnimdesc(int i) const { return NULL; };

	//int					numlocalseq;				// sequences
	//int					localseqindex;
	//inline mstudioseqdesc_t *pLocalSeqdesc(int i) const { return NULL; };

	////public:
	//bool				SequencesAvailable() const
	//{
	//	return true;
	//}

	//int					GetNumSeq() const;
	//mstudioanimdesc_t	&pAnimdesc(int i) const;
	//mstudioseqdesc_t	&pSeqdesc(int i) const;
	//int					iRelativeAnim(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	//int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

	//																		//private:
	//mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	//mutable int			eventsindexed;
	////public:
	//int					GetSequenceActivity(int iSequence);
	//void				SetSequenceActivity(int iSequence, int iActivity);
	//int					GetActivityListVersion(void);
	//void				SetActivityListVersion(int version) const;
	//int					GetEventListVersion(void);
	//void				SetEventListVersion(int version);

	//// raw textures
	//int					numtextures;
	//int					textureindex;
	//inline mstudiotexture_t *pTexture(int i) const { return NULL; };


	//// raw textures search paths
	//int					numcdtextures;
	//int					cdtextureindex;
	//inline char			*pCdtexture(int i) const { return (((char *)this) + *((int *)(((BYTE *)this) + cdtextureindex) + i)); };

	//// replaceable textures tables
	//int					numskinref;
	//int					numskinfamilies;
	//int					skinindex;
	//inline short		*pSkinref(int i) const { return (short *)(((BYTE *)this) + skinindex) + i; };

	//int					numbodyparts;
	//int					bodypartindex;
	//inline mstudiobodyparts_t	*pBodypart(int i) const { return NULL; };

	//int localattachmentindex;
	//float mass;
	//int contents;
	//int numlocalattachments;
	//int numincludemodels;
	//int includemodelindex;
};


class virtualgroup_t
{
public:
	void *cache;
	const studiohdr_t *GetStudioHdr(void) const
	{
		return (studiohdr_t *)cache;
	}

	CUtlVector< int > boneMap;				// maps global bone to local bone
	CUtlVector< int > masterBone;			// maps local bone to global bone
	CUtlVector< int > masterSeq;			// maps local sequence to master sequence
	CUtlVector< int > masterAnim;			// maps local animation to master animation
	CUtlVector< int > masterAttachment;	// maps local attachment to global
	CUtlVector< int > masterPose;			// maps local pose parameter to global
	CUtlVector< int > masterNode;			// maps local transition nodes to global
};

struct virtualsequence_t
{
	int	flags;
	int activity;
	int group;
	int index;
};

struct virtualgeneric_t
{
	int group;
	int index;
};

struct virtualmodel_t
{
	char pad_mutex[0x8];
	CUtlVector< virtualsequence_t > m_seq;
	CUtlVector< virtualgeneric_t > m_anim;
	CUtlVector< virtualgeneric_t > m_attachment;
	CUtlVector< virtualgeneric_t > m_pose;
	CUtlVector< virtualgroup_t > m_group;
	CUtlVector< virtualgeneric_t > m_node;
	CUtlVector< virtualgeneric_t > m_iklock;
	CUtlVector< unsigned short > m_autoplaySequences;
};



class c_studio_hdr
{
public:
	mutable studiohdr_t* m_pStudioHdr; //0x0000 
	mutable virtualmodel_t* m_pVModel; //0x0004 
	char				pad_unknown[0x4]; //0x0008 
	mutable CUtlVector< const studiohdr_t* > m_pStudioHdrCache; //0x000C 
	mutable int			m_nFrameUnlockCounter; //0x0020 
	int*				m_pFrameUnlockCounter; //0x0024 
	char				pad_mutex[0x8]; //0x0028 
	CUtlVector< int >	m_boneFlags; //0x0030
	CUtlVector< int >	m_boneParent; //0x0050 
	inline int			numbones(void) const { return m_pStudioHdr->num_bones; };
	inline mstudiobone_t *pBone(int i) const { return m_pStudioHdr->GetBone(i); };
	inline virtualmodel_t		*GetVirtualModel(void) const { return m_pVModel; };
	const studiohdr_t *GroupStudioHdr(int i)
	{
		const studiohdr_t *pStudioHdr = m_pStudioHdrCache[i];

		if (!pStudioHdr)
		{
			virtualgroup_t *pGroup = &m_pVModel->m_group[i];
			pStudioHdr = pGroup->GetStudioHdr();
		}

		//Assert(pStudioHdr);
		return pStudioHdr;
	}
}; //Size=0x0054


//class c_studio_hdr
//{
//public:
//	mutable studiohdr_t* m_pStudioHdr; //0x0000 
//	mutable virtualmodel_t* m_pVModel; //0x0004 
//	char				pad_unknown[0x4]; //0x0008 
//	mutable CUtlVector<const studiohdr_t*> m_pStudioHdrCache; //0x000C 
//	mutable int			m_nFrameUnlockCounter; //0x0020 
//	int*				m_pFrameUnlockCounter; //0x0024 
//	char				pad_mutex[0x8]; //0x0028 
//	CUtlVector<int>	bone_flags; //0x0030
//	CUtlVector<int>	m_boneParent; //0x0050
//};
//struct studiohdr_t;

struct studiohwdata_t
{
	int					m_RootLOD;	// calced and clamped, nonzero for lod culling
	int					m_NumLODs;
	void* m_pLODs;
	int					m_NumStudioMeshes;

	inline float LODMetric(float unitSphereSize) const { return (unitSphereSize != 0.0f) ? (100.0f / unitSphereSize) : 0.0f; }
	inline int GetLODForMetric(float lodMetric) const
	{
		//if (!m_NumLODs)
		//	return 0;

		//// shadow lod is specified on the last lod with a negative switch
		//// never consider shadow lod as viable candidate
		//int numLODs = (m_pLODs[m_NumLODs - 1].m_SwitchPoint < 0.0f) ? m_NumLODs - 1 : m_NumLODs;

		//for (int i = m_RootLOD; i < numLODs - 1; i++)
		//{
		//	if (m_pLODs[i + 1].m_SwitchPoint > lodMetric)
		//		return i;
		//}

		//return numLODs - 1;
		return 0;
	}
};
//struct vcollide_t;
//struct virtualmodel_t;
struct vertexFileHeader_t;

namespace OptimizedModel
{
	struct FileHeader_t;
}

class IVModelInfo
{
public:
	inline void* GetModel(int Index) {
		return getvfunc<void*(__thiscall *)(void*, int)>(this, 1)(this, Index);
	}
	int	GetModelIndex(const char *name)
	{
		typedef int(__thiscall* oGetModelName)(PVOID, const char *);
		return getvfunc< oGetModelName >(this, 2)(this, name);
	}
	const char *GetModelName(const model_t *mod)
	{
		typedef const char *(__thiscall* oGetModelName)(PVOID, const model_t*);
		return getvfunc< oGetModelName >(this, 3)(this, mod);
	}
	inline const char* GetModelName(const void* Model) {
		return getvfunc<const char*(__thiscall *)(void*, const void*)>(this, 3)(this, Model);
	}
	studiohdr_t	*GetStudioModel(const model_t *mod)
	{
		typedef studiohdr_t *(__stdcall* oGetStudiomodel)(const model_t*);
		return getvfunc< oGetStudiomodel >(this, 32)(mod);
	}
};

class IVModelInfoClient : public IVModelInfo
{
public:
};


struct virtualterrainparams_t
{
	// UNDONE: Add grouping here, specified in BSP file? (test grouping to see if this is necessary)
	int index;
};


