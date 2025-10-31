#ifndef _ITF_RAY_SUPERPUNCHGAUGE_H_
#define _ITF_RAY_SUPERPUNCHGAUGE_H_

#ifndef _ITF_RAY_POWERUPDISPLAY_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_PowerUpDisplay.h"
#endif //_ITF_RAY_POWERUPDISPLAY_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF {

class FXControllerComponent;
class ITF_ParticleGenerator;
class Ray_EventSuperPunchLaunch;
class Ray_SuperPunchGauge_Template;

//------------------------------------------------------------------------------
/// Infos for one super punch particle
struct Ray_SuperPunchGaugeParticle
{
    Ray_SuperPunchGaugeParticle()
        : m_index(0)
        , m_pos(Vec3d::Zero)
        , m_flipped(bfalse)
        , m_previousTargetPos(Vec3d::Zero)
        , m_smoothedSpeed(Vec3d::Zero)
        , m_alpha(0)
        {}
    ~Ray_SuperPunchGaugeParticle() {}

    u32 m_index; ///< index in generator
    Vec3d m_pos;
    bbool m_flipped;
    Vec3d m_previousTargetPos;
    Vec3d m_smoothedSpeed;    
    u8 m_alpha;
};

//------------------------------------------------------------------------------
typedef ITF_VECTOR<Ray_SuperPunchGaugeParticle> Ray_SuperPunchGaugeParticleList;

//------------------------------------------------------------------------------
/// Super punch gauge: code-driven particles, following the player smoothly.
class Ray_SuperPunchGauge : public Ray_PowerUpDisplay
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SuperPunchGauge, Ray_PowerUpDisplay,3098268127)
    DECLARE_VALIDATE_OBJECT()

public:
    Ray_SuperPunchGauge();
    virtual ~Ray_SuperPunchGauge();

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onEvent(class Event * _event);
    virtual void update(f32 _dt);

private:
    ITF_INLINE const Ray_SuperPunchGauge_Template * getTemplate() const;
    
    void processLaunch(const Ray_EventSuperPunchLaunch* _launch);
    
    ITF_ParticleGenerator* getParticleGenerator();
    
    void cleanup();
    bbool isFxPlaying() const;
    bbool playFx(const StringID& _type, u32 _ammo);
    void stopFx();
    void updateFx(f32 _dt);
    
    bbool m_initialized;
    u32 m_fxHandle;
    Ray_SuperPunchGaugeParticleList m_particleList;
    SpawneeGenerator m_spawneeGenerator;
    FXControllerComponent* m_fxController;
};

//------------------------------------------------------------------------------
class Ray_SuperPunchGauge_Template : public Ray_PowerUpDisplay_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SuperPunchGauge_Template,Ray_PowerUpDisplay_Template,604060895);
    DECLARE_SERIALIZE();

public:
    Ray_SuperPunchGauge_Template();
    virtual ~Ray_SuperPunchGauge_Template();

    ITF_INLINE const StringID& getFxControl() const { return m_fxControl; }
    ITF_INLINE const StringID& getParticleGenerator() const { return m_particleGenerator; }
    ITF_INLINE const Path& getPunchActor() const { return m_punchActor; }
    ITF_INLINE const Vec2d& getPlayerFollowOffset() const { return m_playerFollowOffset; }
    ITF_INLINE const Vec2d& getOthersFollowOffset() const { return m_othersFollowOffset; }
    ITF_INLINE f32 getLaunchDistance() const { return m_launchDistance; }
    ITF_INLINE f32 getLaunchYOffset() const { return m_launchYOffset; }
    ITF_INLINE f32 getReducedLaunchDistance() const { return m_reducedLaunchDistance; }
    ITF_INLINE f32 getReducedLaunchYOffset() const { return m_reducedLaunchYOffset; }
    ITF_INLINE u32 getVisibleAmmo() const { return m_visibleAmmo; }
    ITF_INLINE f32 getSpeedBlend() const { return m_speedBlend; }
    ITF_INLINE f32 getSpeedMin() const { return m_speedMin; }
    ITF_INLINE f32 getSpeedMax() const { return m_speedMax; }
    ITF_INLINE f32 getBlendAtSpeedMin() const { return m_blendAtSpeedMin; }
    ITF_INLINE f32 getBlendAtSpeedMax() const { return m_blendAtSpeedMax; }

private:
    StringID m_fxControl;
    StringID m_particleGenerator;
    Path m_punchActor;
    Vec2d m_playerFollowOffset;
    Vec2d m_othersFollowOffset;
    f32 m_launchDistance;
    f32 m_launchYOffset;
    f32 m_reducedLaunchDistance;
    f32 m_reducedLaunchYOffset;
    u32 m_visibleAmmo;
    f32 m_speedBlend;
    f32 m_speedMin;
    f32 m_speedMax;
    f32 m_blendAtSpeedMin;
    f32 m_blendAtSpeedMax;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_SuperPunchGauge_Template*
Ray_SuperPunchGauge::getTemplate() const
{
    return static_cast<const Ray_SuperPunchGauge_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_SUPERPUNCHGAUGE_H_
