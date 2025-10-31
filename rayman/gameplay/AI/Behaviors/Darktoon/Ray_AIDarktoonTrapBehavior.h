#ifndef _ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_
#define _ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylinePunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

namespace ITF
{

class AIAction;
class Ray_AIDarktoonTrapBehavior_Template;
class Ray_AIPerformHitPolylinePunchAction;
class Ray_AIPerformHitPolylinePunchAction_Template;
class PhysComponent;
class PhysShape;

//------------------------------------------------------------------------------
// Idle & attack behavior for darktoon traps
class Ray_AIDarktoonTrapBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonTrapBehavior, AIBehavior,4205277812);
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIDarktoonTrapBehavior();
    virtual ~Ray_AIDarktoonTrapBehavior();
    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void update(f32 _dt);
    virtual void updateAnimInputs();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onActionFinished();
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    bbool getStartsHidden() const;
    bbool getOpenHole() const;
    bbool returnsHits(u32 hitLevel) const;

private:
    enum State_t {State_NA, State_Idle, State_Attack};

    ITF_INLINE const Ray_AIDarktoonTrapBehavior_Template * getTemplate() const;
    void setState(State_t _state);
    void updateIdleState(f32 _dt);
    void updateAttackState(f32 _dt);

    State_t m_state;
    f32 m_coolDownTimer;
    bbool m_lookOut;
    bbool m_openHole;
    Ray_AIPerformHitPolylinePunchAction* m_idle;
    Ray_AIPerformHitPolylinePunchAction* m_attack;
};

//------------------------------------------------------------------------------
class Ray_AIDarktoonTrapBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonTrapBehavior_Template, TemplateAIBehavior,3979299286);
    DECLARE_SERIALIZE()
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIDarktoonTrapBehavior)

public:
    Ray_AIDarktoonTrapBehavior_Template();
    ~Ray_AIDarktoonTrapBehavior_Template();

    bbool getStartsHidden() const { return m_startsHidden; }
    f32 getHeightFactor() const { return m_heightFactor; }
    f32 getAttackRate() const { return m_attackRate; }
    f32 getCoolDownDuration() const { return m_coolDownDuration; }
    u32 getReturnHitMaxLevel() const { return m_returnHitMaxLevel; }
    const PhysShape* getOuterShape() const { return m_outerShape; }
    const PhysShape* getInnerShape() const { return m_innerShape; }
    const Ray_AIPerformHitPolylinePunchAction_Template* getIdle() const { return m_idle; }
    const Ray_AIPerformHitPolylinePunchAction_Template* getAttack() const { return m_attack; }

private:
    bbool m_startsHidden;
    f32 m_heightFactor;
    f32 m_attackRate;
    f32 m_coolDownDuration;
    u32 m_returnHitMaxLevel;
    PhysShape* m_outerShape;
    PhysShape* m_innerShape;
    Ray_AIPerformHitPolylinePunchAction_Template* m_idle;
    Ray_AIPerformHitPolylinePunchAction_Template* m_attack;
};

//------------------------------------------------------------------------------
ITF_INLINE const Ray_AIDarktoonTrapBehavior_Template*
Ray_AIDarktoonTrapBehavior::getTemplate() const
{
    return static_cast<const class Ray_AIDarktoonTrapBehavior_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_