#ifndef _ITF_RAY_AICORNBEHAVIOR_H_
#define _ITF_RAY_AICORNBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF {

class AIAction;
class AIAction_Template;
class PhysComponent;
class Ray_AICornFloatAction;
class Ray_AICornFloatAction_Template;
class Ray_AICornBehavior_Template;
class Ray_EventChildLaunch;
class Ray_EventQueryChildLaunch;
class Ray_SubAnchorComponent;

//------------------------------------------------------------------------------
/// Platform corns' behavior
class Ray_AICornBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AICornBehavior, AIBehavior,3251709558);

public:
    DECLARE_VALIDATE_BEHAVIOR()

    Ray_AICornBehavior();
    ~Ray_AICornBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void update(f32 _dt);
    virtual void onEvent(Event* _event);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onActionFinished();

private:
    enum State_t
    {
        State_NA,
        State_Jump,
        State_Burn,
        State_Pop,
        State_Float
    };

    ITF_INLINE const Ray_AICornBehavior_Template * getTemplate() const;

    void processLaunchQuery(Ray_EventQueryChildLaunch* _query);
    void processLaunch(Ray_EventChildLaunch* _launch);
    
    void changeState(State_t _newState);
    void updateState(f32 _dt);

    State_t m_state;
    Vec3d m_launchPos;
    AIAction* m_jumpAction;
    AIAction* m_burnAction;
    AIAction* m_popAction;
    Ray_AICornFloatAction* m_floatAction;
    PhysComponent* m_physComponent;
    Ray_SubAnchorComponent* m_subAnchorComponent;
};

//------------------------------------------------------------------------------
class Ray_AICornBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AICornBehavior_Template,TemplateAIBehavior,1666465776);
    DECLARE_SERIALIZE(); 
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AICornBehavior);

public:

    Ray_AICornBehavior_Template();
    ~Ray_AICornBehavior_Template();

    ITF_INLINE const Angle& getRandomAngle() const { return m_randomAngle; }
    ITF_INLINE const AIAction_Template* getBurnAction() const { return m_burnAction; }
    ITF_INLINE const AIAction_Template* getJumpAction() const { return m_jumpAction; }
    ITF_INLINE const AIAction_Template* getPopAction() const { return m_popAction; }
    ITF_INLINE const Ray_AICornFloatAction_Template* getFloatAction() const { return m_floatAction; }

private:
    Angle m_randomAngle;
    AIAction_Template* m_jumpAction;
    AIAction_Template* m_burnAction;
    AIAction_Template* m_popAction;
    Ray_AICornFloatAction_Template* m_floatAction;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AICornBehavior_Template*
Ray_AICornBehavior::getTemplate() const
{
    return static_cast<const class Ray_AICornBehavior_Template *>(m_template);
}

} // namespace ITF

#endif // _ITF_RAY_AICORNBEHAVIOR_H_
