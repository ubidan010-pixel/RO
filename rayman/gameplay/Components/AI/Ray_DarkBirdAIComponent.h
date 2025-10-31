#ifndef _ITF_RAY_DARKBIRDAICOMPONENT_H_
#define _ITF_RAY_DARKBIRDAICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundEnemyAIComponent.h"
#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_

namespace ITF
{

class AIBehavior;
class HitStim;
class Ray_DarkBirdAIComponent_Template;
class Ray_MusicScoreOpenEvent;
class Ray_MusicScoreCloseEvent;
class TemplateAIBehavior;

//------------------------------------------------------------------------------
/// Dark birds: stand on a music score, look at players & bounce them back
class Ray_DarkBirdAIComponent : public Ray_GroundEnemyAIComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DarkBirdAIComponent, Ray_GroundEnemyAIComponent,519987365)
    DECLARE_VALIDATE_COMPONENT()
    DECLARE_SERIALIZE();

public:
    Ray_DarkBirdAIComponent();
    virtual ~Ray_DarkBirdAIComponent();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onBecomeActive();
    virtual void Update( f32 _dt );
    virtual void onEvent(Event* _event);
    virtual void onBehaviorFinished();
    
protected:
    virtual void startSpawn();
    virtual void startRoam( const bbool _restart = bfalse );
    virtual void startDeath();
    virtual void startReceiveHit(HitStim* _hit);
    
    void attach();
    void detach();
    
    void processMusicScoreOpen(const Ray_MusicScoreOpenEvent* _open);
    void processMusicScoreClose(const Ray_MusicScoreCloseEvent* _close);

private:
    ITF_INLINE const Ray_DarkBirdAIComponent_Template * getTemplate() const;

    bbool m_started;
    AIBehavior* m_disappear3dBehavior;
    AIBehavior* m_triggerBounceBehavior;
};

//------------------------------------------------------------------------------
class Ray_DarkBirdAIComponent_Template : public Ray_GroundEnemyAIComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DarkBirdAIComponent_Template, Ray_GroundEnemyAIComponent_Template,634680799);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DarkBirdAIComponent);

public:
    Ray_DarkBirdAIComponent_Template();
    ~Ray_DarkBirdAIComponent_Template();
    
    ITF_INLINE const Vec3d& getAppear3dOffset() const { return m_appear3dOffset; }
    ITF_INLINE ITF::bbool getDetachOnDeath() const { return m_detachOnDeath; }
    ITF_INLINE TemplateAIBehavior* getDisappear3dBehavior() const { return m_disappear3dBehavior; }
    ITF_INLINE TemplateAIBehavior* getTriggerBounceBehavior() const { return m_triggerBounceBehavior; }
    
private:
    Vec3d m_appear3dOffset;
    bbool m_detachOnDeath;
    TemplateAIBehavior* m_disappear3dBehavior;
    TemplateAIBehavior* m_triggerBounceBehavior;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_DarkBirdAIComponent_Template*
Ray_DarkBirdAIComponent::getTemplate() const
{
    return static_cast<const Ray_DarkBirdAIComponent_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_DARKBIRDAICOMPONENT_H_
