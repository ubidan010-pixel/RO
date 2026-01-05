#ifndef _ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_
#define _ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_BUBBLEPRIZE_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_BubblePrize.h"
#endif //_ITF_RAY_BUBBLEPRIZE_H_

namespace ITF
{

class AIBezierAction;
class AIBezierAction_Template;
class HitStim;
class PlayerDetectorComponent;
class Ray_AIBlowOffAction;
class Ray_AIBlowOffAction_Template;
class Ray_AIBubblePrizeBehavior_Template;
class Ray_AIFlyIdleAction;
class Ray_AIFlyIdleAction_Template;
class Ray_AIFollowBezierCurveAction;
class Ray_AIFollowBezierCurveAction_Template;
class Ray_EventQueryChildLaunch;
class Ray_EventChildLaunch;

//------------------------------------------------------------------------------
// The "regular" bubble prize behavior, two variants:
// - w/o custom curve: bezierAction -> blowOffAction
// - with custom curve: bezierAction -> followCurveAction -> flyIdleAction
class Ray_AIBubblePrizeBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBubblePrizeBehavior, AIBehavior,3921876667);
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIBubblePrizeBehavior();
    ~Ray_AIBubblePrizeBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate() {}
    virtual void update(f32 _dt);
    virtual void onEvent(Event * _event);
    virtual void onActionFinished();

private:
    enum State_t
    {
        State_NA,
        State_Bezier,
        State_BlowOff,
        State_FollowCurve,
        State_FlyIdle,
        State_Popped
    };
    
    ITF_INLINE const Ray_AIBubblePrizeBehavior_Template * getTemplate() const;
    
    void processLaunchQuery(Ray_EventQueryChildLaunch* _query);
    void processLaunch(Ray_EventChildLaunch* _launch);
    void processHit(HitStim* _hitStim);

    void updatePlayerDetectorRadius();

    Ray_BubblePrize m_bubblePrize;
    State_t m_state;
    Vec3d m_launchPos;
    ActorRef m_hitPlayerRef;
    AIBezierAction* m_bezierAction;
    Ray_AIBlowOffAction* m_blowOffAction;
    Ray_AIFollowBezierCurveAction* m_followCurveAction;
    Ray_AIFlyIdleAction* m_flyIdleAction;

    u32 m_waterCheckOffset;
    PlayerDetectorComponent* m_playerDetectorComponent;

public:
    bbool isSkullCoin() { return m_bubblePrize.isSkullCoin(); };
};

//------------------------------------------------------------------------------
class Ray_AIBubblePrizeBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBubblePrizeBehavior_Template,TemplateAIBehavior,500512533);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBubblePrizeBehavior);

public:
    Ray_AIBubblePrizeBehavior_Template();
    ~Ray_AIBubblePrizeBehavior_Template();

    ITF_INLINE bbool isCustomCurve() const { return m_isCustomCurve; }
    ITF_INLINE const Ray_BubblePrize_Template& getBubblePrize() const { return m_bubblePrize; }
    ITF_INLINE f32 getBezierStretchStart() const { return m_bezierStretchStart; }
    ITF_INLINE f32 getBezierStretchEnd() const { return m_bezierStretchEnd; }
    ITF_INLINE const AIBezierAction_Template * getBezierAction() const { return m_bezierAction; }
    ITF_INLINE const Ray_AIBlowOffAction_Template* getBlowOffAction() const { return m_blowOffAction; }
    ITF_INLINE const Ray_AIFollowBezierCurveAction_Template* getFollowCurveAction() const { return m_followCurveAction; }
    ITF_INLINE const Ray_AIFlyIdleAction_Template* getFlyIdleAction() const { return m_flyIdleAction; }
    ITF_INLINE f32 getPlayerDetectorMultiplierInWater() const { return m_playerDetectorMultiplierInWater; }

private:
    bbool m_isCustomCurve;
    Ray_BubblePrize_Template m_bubblePrize;
    f32 m_bezierStretchStart;
    f32 m_bezierStretchEnd;
    AIBezierAction_Template* m_bezierAction;
    Ray_AIBlowOffAction_Template* m_blowOffAction;
    Ray_AIFollowBezierCurveAction_Template* m_followCurveAction;
    Ray_AIFlyIdleAction_Template* m_flyIdleAction;
    f32 m_playerDetectorMultiplierInWater;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIBubblePrizeBehavior_Template*
Ray_AIBubblePrizeBehavior::getTemplate() const
{
    return static_cast<const Ray_AIBubblePrizeBehavior_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_
