#ifndef _ITF_RAY_DARKTOONTRAPAICOMPONENT_H_
#define _ITF_RAY_DARKTOONTRAPAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{

class Ray_AIDarktoonTrapBehavior;
class Ray_AIDarktoonTrapBehavior_Template;
class Ray_AIReceiveHitBehavior;
class Ray_AIReceiveHitBehavior_Template;
class Ray_DarktoonTrapAIComponent_Template;

//------------------------------------------------------------------------------
// Global AI for darktoon traps
class Ray_DarktoonTrapAIComponent : public Ray_AIComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DarktoonTrapAIComponent, Ray_AIComponent,4253825461)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_DarktoonTrapAIComponent();
    ~Ray_DarktoonTrapAIComponent();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update(f32 _dt);
    virtual void onEvent(Event* _event);
#ifdef ITF_SUPPORT_EDITOR
    virtual void onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
    virtual void onBehaviorFinished();

private:
    const Ray_DarktoonTrapAIComponent_Template* getTemplate() const;
    virtual void processHit(HitStim* _hit);

    SpawneeGenerator m_holeGenerator;
    ActorRef m_hole;
    Ray_AIDarktoonTrapBehavior* m_trapBehavior;
    Ray_AIReceiveHitBehavior* m_receiveHitBehavior;
    AIBehavior* m_deathBehavior;
};

//------------------------------------------------------------------------------
class Ray_DarktoonTrapAIComponent_Template : public Ray_AIComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DarktoonTrapAIComponent_Template,Ray_AIComponent_Template,3610061445);
    DECLARE_SERIALIZE()
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DarktoonTrapAIComponent);

public:
    Ray_DarktoonTrapAIComponent_Template();
    ~Ray_DarktoonTrapAIComponent_Template();

    const Path& getHolePath() const { return m_holePath; }
    const Vec3d& getHoleOffset() const { return m_holeOffset; }
    const Ray_AIDarktoonTrapBehavior_Template* getTrapBehavior() const { return m_trapBehavior; }
    const Ray_AIReceiveHitBehavior_Template* getReceiveHitBehavior() const { return m_receiveHitBehavior; }
    const TemplateAIBehavior* getDeathBehavior() const { return m_deathBehavior; }

private:
    Path m_holePath;
    Vec3d m_holeOffset;
    Ray_AIDarktoonTrapBehavior_Template* m_trapBehavior;
    Ray_AIReceiveHitBehavior_Template* m_receiveHitBehavior;
    TemplateAIBehavior* m_deathBehavior;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_DarktoonTrapAIComponent_Template*
Ray_DarktoonTrapAIComponent::getTemplate() const
{
    return static_cast<const Ray_DarktoonTrapAIComponent_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_RAY_DARKTOONTRAPAICOMPONENT_H_
