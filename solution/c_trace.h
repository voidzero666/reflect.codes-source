
#include "matrix.h"
#include "i_base_player.h"
#include "Vector.h"
#include "RecvProps.h"

enum class TraceType
{
    TRACE_EVERYTHING = 0,
    TRACE_WORLD_ONLY,
    TRACE_ENTITIES_ONLY,
    TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* pEntity, int contentsMask) = 0;
    virtual TraceType GetTraceType() const = 0;
};


//-----------------------------------------------------------------------------
// Classes are expected to inherit these + implement the ShouldHitEntity method
//-----------------------------------------------------------------------------

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        ClientClass* eCC = ((IBasePlayer*)pEntityHandle)->GetClientClass();
        if (eCC && strcmp(ccIgnore, crypt_str("")))
        {
            if (eCC->m_pNetworkName == ccIgnore)
                return false;
        }

        return !(pEntityHandle == pSkip);
    }

    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }

    inline void SetIgnoreClass(char* Class)
    {
        ccIgnore = Class;
    }

    void* pSkip;
    char* ccIgnore = new char[1];
};

class CTraceFilterOneEntity : public ITraceFilter {
public:
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/) {
        return (pEntityHandle == pEntity);
    }

    TraceType GetTraceType() const {
        return TraceType::TRACE_EVERYTHING;
    }

    void* pEntity;
};

class CTraceFilterSkipEntity : public ITraceFilter
{
public:
    CTraceFilterSkipEntity(IHandleEntity* pEntityHandle)
    {
        pSkip = pEntityHandle;
    }

    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == pSkip);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
    void* pSkip;
};

class CTraceFilterEntitiesOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return true;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_ENTITIES_ONLY;
    }
};


//-----------------------------------------------------------------------------
// Classes need not inherit from these
//-----------------------------------------------------------------------------
class CTraceFilterWorldOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return false;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_WORLD_ONLY;
    }
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return false;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
};

class CTraceFilterPlayersOnlySkipOne : public ITraceFilter
{
public:
    CTraceFilterPlayersOnlySkipOne(IBasePlayer* ent)
    {
        e = ent;
    }
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return pEntityHandle != e && ((IBasePlayer*)pEntityHandle)->GetClientClass()->m_ClassID == 40;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_ENTITIES_ONLY;
    }

private:
    IBasePlayer* e;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
    CTraceFilterSkipTwoEntities(IBasePlayer* ent1, IBasePlayer* ent2)
    {
        e1 = ent1;
        e2 = ent2;
    }
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == e1 || pEntityHandle == e2);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }

private:
    IBasePlayer* e1;
    IBasePlayer* e2;
};

class CTraceFilterHitAll : public CTraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return true;
    }
};


enum class DebugTraceCounterBehavior_t
{
    kTRACE_COUNTER_SET = 0,
    kTRACE_COUNTER_INC,
};

//-----------------------------------------------------------------------------
// Enumeration interface for EnumerateLinkEntities
//-----------------------------------------------------------------------------
class IEntityEnumerator
{
public:
    // This gets called with each handle
    virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};


struct BrushSideInfo_t
{
    Vector4D plane;               // The plane of the brush side
    unsigned short bevel;    // Bevel plane?
    unsigned short thin;     // Thin?
};

class CPhysCollide;

struct cplane_t {
    Vector normal;
    float dist;
    uint8_t type;   // for fast side tests
    uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
    uint8_t pad[2];

};

struct vcollide_t
{
    unsigned short solidCount : 15;
    unsigned short isPacked : 1;
    unsigned short descSize;
    // VPhysicsSolids
    CPhysCollide** solids;
    char* pKeyValues;
    void* pUserData;
};

struct cmodel_t
{
    Vector         mins, maxs;
    Vector         origin;        // for sounds or lights
    int            headnode;
    vcollide_t     vcollisionData;
};

struct csurface_t
{
    const char* name;
    short          surfaceProps;
    unsigned short flags;         // BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------
struct Ray_t
{
    VectorAligned  m_Start;  // starting point, centered within the extents
    VectorAligned  m_Delta;  // direction + length of the ray
    VectorAligned  m_StartOffset; // Add this to m_Start to Get the actual ray start
    VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
    const matrix* m_pWorldAxisTransform;
    bool m_IsRay;  // are the extents zero?
    bool m_IsSwept;     // is delta != 0?

    Ray_t() : m_pWorldAxisTransform(NULL) {}

    Ray_t(Vector const& start, Vector const& end) {
        m_Delta = end - start;

        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents.Init();

        m_pWorldAxisTransform = NULL;
        m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        m_StartOffset.Init();
        m_Start = start;
    }

    void Init(Vector const& start, Vector const& end)
    {
        m_Delta = end - start;

        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents.Init();

        m_pWorldAxisTransform = NULL;
        m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        m_StartOffset.Init();
        m_Start = start;
    }

    void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
    {
        m_Delta = end - start;

        m_pWorldAxisTransform = NULL;
        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents = maxs - mins;
        m_Extents *= 0.5f;
        m_IsRay = (m_Extents.LengthSqr() < 1e-6);

        // Offset m_Start to be in the center of the box...
        m_StartOffset = maxs + mins;
        m_StartOffset *= 0.5f;
        m_Start = start + m_StartOffset;
        m_StartOffset *= -1.0f;
    }
    Vector InvDelta() const
    {
        Vector vecInvDelta;
        for (int iAxis = 0; iAxis < 3; ++iAxis) {
            if (m_Delta[iAxis] != 0.0f) {
                vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
            }
            else {
                vecInvDelta[iAxis] = FLT_MAX;
            }
        }
        return vecInvDelta;
    }

private:
};
