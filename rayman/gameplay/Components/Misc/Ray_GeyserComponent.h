#ifndef _ITF_RAY_GEYSERCOMPONENT_H_
#define _ITF_RAY_GEYSERCOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{

class AnimatedComponent;
class AnimLightComponent;
class EventTrigger;
class Ray_GeyserComponent_Template;
class Ray_TriggerBounceComponent;

//------------------------------------------------------------------------------
// Launch children in a sequence after warning
class Ray_GeyserComponent : public Ray_ChildLaunchComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_GeyserComponent, Ray_ChildLaunchComponent,722708767)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_GeyserComponent();
    virtual ~Ray_GeyserComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onCheckpointLoaded();
    virtual void Update(f32 _dt);
    virtual void updateAnimInput();
    virtual void onEvent(Event* _event);

private:
    enum State_t {State_NA,State_Idle,State_Warn,State_Launch,State_Done};

    ITF_INLINE const Ray_GeyserComponent_Template* getTemplate() const;

    void processTrigger(EventTrigger* trigger);
    void setState(State_t _state);

    State_t m_state;
    f32 m_warnTimer;
    f32 m_launchTimer;
    f32 m_nextLaunchTime;
    SpawneeGenerator m_warnFxGenerator;
    SpawneeGenerator m_launchFxGenerator;
    AnimatedComponent* m_animatedComponent;
};

//------------------------------------------------------------------------------
class Ray_GeyserComponent_Template : public Ray_ChildLaunchComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_GeyserComponent_Template,Ray_ChildLaunchComponent_Template,2330498979);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_GeyserComponent);

public:
    Ray_GeyserComponent_Template();
    virtual ~Ray_GeyserComponent_Template();

    ITF_INLINE f32 getWarnDuration() const { return m_warnDuration; }
    ITF_INLINE f32 getLaunchDuration() const { return m_launchDuration; }
    ITF_INLINE const StringID& getLaunchCameraShake() const { return m_launchCameraShake; }
    ITF_INLINE const Path& getWarnFx() const { return m_warnFx; }
    ITF_INLINE const Vec3d& getWarnFxOffset() const { return m_warnFxOffset; }
    ITF_INLINE const Path& getLaunchFx() const { return m_launchFx; }
    ITF_INLINE const Vec3d& getLaunchFxOffset() const { return m_launchFxOffset; }
    ITF_INLINE const StringID& getIdleAnim() const { return m_idleAnim; }
    ITF_INLINE const StringID& getWarnAnim() const { return m_warnAnim; }
    ITF_INLINE const StringID& getLaunchAnim() const { return m_launchAnim; }
    ITF_INLINE const StringID& getDoneAnim() const { return m_doneAnim; }

private:
    f32 m_warnDuration;
    f32 m_launchDuration;
    StringID m_launchCameraShake;
    Path m_warnFx;
    Vec3d m_warnFxOffset;
    Path m_launchFx;
    Vec3d m_launchFxOffset;
    StringID m_idleAnim;
    StringID m_warnAnim;
    StringID m_launchAnim;
    StringID m_doneAnim;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_GeyserComponent_Template*
Ray_GeyserComponent::getTemplate() const
{
    return static_cast<const Ray_GeyserComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_GEYSERCOMPONENT_H_
