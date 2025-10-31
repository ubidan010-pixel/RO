#ifndef _ITF_RAY_HEARTSHIELDCOMPONENT_H_
#define _ITF_RAY_HEARTSHIELDCOMPONENT_H_

#ifndef _ITF_RAY_POWERUPDISPLAY_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_PowerUpDisplay.h"
#endif //_ITF_RAY_POWERUPDISPLAY_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF {

class Ray_HeartShield_Template;

//------------------------------------------------------------------------------
/// Extra HP visual
class Ray_HeartShield : public Ray_PowerUpDisplay
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_HeartShield, Ray_PowerUpDisplay,4209222797)
    DECLARE_VALIDATE_OBJECT()

public:
    Ray_HeartShield();
    virtual ~Ray_HeartShield();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onBecomeActive();
    virtual void onBecomeInactive();
    virtual void update(f32 _dt);
    void reset();
    void show( bbool _enable );

private:
    ITF_INLINE const Ray_HeartShield_Template * getTemplate() const;

    void createHeartActor();
    void destroyHeartActor(bbool _force);
    EPlayerHeartTier getCurrentHeartTier() const; 

    bbool m_initialized;
    bbool m_previousHpAtMax;
    Vec3d m_previousTargetPos;
    Vec3d m_smoothedTargetPosSpeed;

    SpawneeGenerator m_heartGenerator;
    SpawneeGenerator m_goldenHeartGenerator;
    SpawneeGenerator m_diamondHeartGenerator;

    ActorRef m_heartRef;
    EPlayerHeartTier m_currentHeartTier;
    bbool m_waitingForImmunityToSpawnRed; 
};

//------------------------------------------------------------------------------
class Ray_HeartShield_Template : public Ray_PowerUpDisplay_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_HeartShield_Template,Ray_PowerUpDisplay_Template,4133734539);
    DECLARE_SERIALIZE();

public:
    Ray_HeartShield_Template();
    virtual ~Ray_HeartShield_Template();

    ITF_INLINE const Path& getHeartActor() const { return m_heartActor; }
    ITF_INLINE const Path& getGoldenHeartActor() const { return m_goldenHeartActor; }
    ITF_INLINE const Path& getDiamondHeartActor() const { return m_diamondHeartActor; }
    ITF_INLINE const StringID& getHeartDeathBhvName() const { return m_heartDeathBhvName; }
    ITF_INLINE const Vec2d& getPlayerFollowOffset() const { return m_playerFollowOffset; }
    ITF_INLINE f32 getSpeedBlend() const { return m_speedBlend; }
    ITF_INLINE f32 getSpeedMin() const { return m_speedMin; }
    ITF_INLINE f32 getSpeedMax() const { return m_speedMax; }
    ITF_INLINE f32 getBlendAtSpeedMin() const { return m_blendAtSpeedMin; }
    ITF_INLINE f32 getBlendAtSpeedMax() const { return m_blendAtSpeedMax; }

private:
    Path m_heartActor;          
    Path m_goldenHeartActor;     
    Path m_diamondHeartActor;    
    StringID m_heartDeathBhvName;
    Vec2d m_playerFollowOffset;
    f32 m_speedBlend;
    f32 m_speedMin;
    f32 m_speedMax;
    f32 m_blendAtSpeedMin;
    f32 m_blendAtSpeedMax;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_HeartShield_Template*
Ray_HeartShield::getTemplate() const
{
    return static_cast<const Ray_HeartShield_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_HEARTSHIELDCOMPONENT_H_
