#ifndef _ITF_RAY_BULLETLAUNCHERCOMPONENT_H_
#define _ITF_RAY_BULLETLAUNCHERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

namespace ITF
{

class AnimLightComponent;
class Ray_BulletLauncherComponent_Template;
class Ray_EventBulletLaunch;

//------------------------------------------------------------------------------
class Ray_BulletLauncherComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BulletLauncherComponent, ActorComponent,1330138928)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_BulletLauncherComponent();
    virtual ~Ray_BulletLauncherComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }
    virtual void  onBecomeActive();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update(f32 _dt);
    virtual void onEvent(Event* _event);

private:
    enum State_t {State_NA,State_Stand,State_StandToOpen,State_Fire,State_Open,State_OpenToStand};

    ITF_INLINE const class Ray_BulletLauncherComponent_Template * getTemplate() const;

    void launchBullet(Ray_EventBulletLaunch* _launch);
    void updateLaunch();
    void freeBullet();

    void changeState(State_t _newState);
    void updateState(f32 _dt);

    TimedSpawner        m_timedSpawner;
    TimedSpawnerData    m_timedSpawnerData;         // instance values

    State_t m_state;
    u32 m_launchBoneIndex;
    SpawneeGenerator m_bulletGenerator;
    ActorRef m_bulletRef;
    AnimLightComponent*     m_animLightComponent;
    class TweenComponent *  m_tweenComponent;
};

//------------------------------------------------------------------------------
class Ray_BulletLauncherComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BulletLauncherComponent_Template,TemplateActorComponent,3782398456);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BulletLauncherComponent);

public:
    Ray_BulletLauncherComponent_Template();
    ~Ray_BulletLauncherComponent_Template();

    ITF_INLINE const Path& getBullet() const { return m_bullet; }
    ITF_INLINE const StringID& getLaunchBone() const { return m_launchBone; }
    ITF_INLINE f32 getLaunchZOffset() const { return m_launchZOffset; }
    ITF_INLINE bbool getLaunchInRootScene() const { return m_launchInRootScene; }
    ITF_INLINE const StringID& getStandAnim() const { return m_standAnim; }
    ITF_INLINE const StringID& getStandToOpenAnim() const { return m_standToOpenAnim; }
    ITF_INLINE const StringID& getFireAnim() const { return m_fireAnim; }
    ITF_INLINE const StringID& getOpenAnim() const { return m_openAnim; }
    ITF_INLINE const StringID& getOpenToStandAnim() const { return m_openToStandAnim; }
    ITF_INLINE bbool useSeparateFire() const { return m_useSeparateFire; }
    ITF_INLINE bbool useTimedSpawnerData() const { return m_useTimedSpawnerData; }
    ITF_INLINE bbool launchOnTrigger() const { return m_launchOnTrigger; }

private:
    Path m_bullet;
    StringID m_launchBone;
    f32 m_launchZOffset;
    bbool m_launchInRootScene;
    StringID m_standAnim;
    StringID m_standToOpenAnim;
    StringID m_fireAnim;
    StringID m_openAnim;
    StringID m_openToStandAnim;
    bbool m_useSeparateFire;
    bbool m_useTimedSpawnerData;
    bbool m_launchOnTrigger;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_BulletLauncherComponent_Template*
Ray_BulletLauncherComponent::getTemplate() const
{
    return static_cast<const Ray_BulletLauncherComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_BULLETLAUNCHERCOMPONENT_H_
