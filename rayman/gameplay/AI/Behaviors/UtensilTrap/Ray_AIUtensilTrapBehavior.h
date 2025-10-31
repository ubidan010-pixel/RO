#ifndef _ITF_RAY_UTENSILTRAPBEHAVIOR_H_
#define _ITF_RAY_UTENSILTRAPBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF {

class AIAction;
class AIAction_Template;
class PolylineComponent;
class Ray_AIUtensilTrapBehavior_Template;
class Ray_SubAnchorComponent;

//------------------------------------------------------------------------------
/// Utensils that fly and stick into the environment to become platforms.
class Ray_AIUtensilTrapBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIUtensilTrapBehavior, AIBehavior,3620588178)
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIUtensilTrapBehavior();
    virtual ~Ray_AIUtensilTrapBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onResourceReady();
    virtual void onActivate();
    virtual void update(f32 _dt);
    virtual void onEvent(Event* _event);
    virtual void updateAnimInputs();
    virtual void onActionFinished();

private:
    enum State_t
    {
        State_NA,
        State_Trap,
        State_Launch,
        State_Fly,
        State_Stick,
        State_Platform,
    };

    ITF_INLINE const Ray_AIUtensilTrapBehavior_Template* getTemplate() const;
    
    void changeState(State_t _newState);
    void updateState(f32 _dt);
    bbool rayCastPhantoms();
    Vec2d getRealStickPos() const;

    State_t m_state;
    bbool m_firstFrame;
    f32 m_rayLength;
    Vec2d m_noseColPos;
    AIAction* m_trapAction;
    AIAction* m_launchAction;
    AIAction* m_flyAction;
    AIAction* m_stickAction;
    AIAction* m_platformAction;
    PolylineComponent* m_polylineComponent;
    Ray_SubAnchorComponent* m_subAnchorComponent;
    ActorRef m_activator;
};

//------------------------------------------------------------------------------
class Ray_AIUtensilTrapBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIUtensilTrapBehavior_Template, TemplateAIBehavior,1432217607);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIUtensilTrapBehavior);

public:
    Ray_AIUtensilTrapBehavior_Template();
    ~Ray_AIUtensilTrapBehavior_Template();
    
    ITF_INLINE f32 getStickOffset() const { return m_stickOffset; }
    ITF_INLINE f32 getSpeed() const { return m_speed; }
    ITF_INLINE AIAction_Template* getTrapAction() const { return m_trapAction; }
    ITF_INLINE AIAction_Template* getLaunchAction() const { return m_launchAction; }
    ITF_INLINE AIAction_Template* getFlyAction() const { return m_flyAction; }
    ITF_INLINE AIAction_Template* getStickAction() const { return m_stickAction; }
    ITF_INLINE AIAction_Template* getPlatformAction() const { return m_platformAction; }
    ITF_INLINE bbool getAssignRewardToActivator() const { return m_assignRewardToActivator; }

private:
    f32 m_stickOffset;
    f32 m_speed;
    AIAction_Template* m_trapAction;
    AIAction_Template* m_launchAction;
    AIAction_Template* m_flyAction;
    AIAction_Template* m_stickAction;
    AIAction_Template* m_platformAction;
    bbool m_assignRewardToActivator;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIUtensilTrapBehavior_Template*
Ray_AIUtensilTrapBehavior::getTemplate() const
{
    return static_cast<const Ray_AIUtensilTrapBehavior_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_Ray_AIUtensilTrapBehavior_H_
