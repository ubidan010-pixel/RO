#ifndef _ITF_RAY_FRUITTRAPBEHAVIOR_H_
#define _ITF_RAY_FRUITTRAPBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF {

class AIAction;
class AIAction_Template;
class PolylineComponent;
class PunchStim;
class Ray_AIFruitTrapBehavior_Template;

//------------------------------------------------------------------------------
/// Utensils that fly and stick into the environment to become platforms.
class Ray_AIFruitTrapBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIFruitTrapBehavior, AIBehavior,701659010);
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIFruitTrapBehavior();
    virtual ~Ray_AIFruitTrapBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onActivate();
    virtual void update(f32 _dt);
    virtual void onEvent(Event * _event);
    virtual void updateAnimInputs();
    virtual void onActionFinished();

private:
    enum State_t
    {
        State_NA,
        State_Stand,
        State_FruitFall,
    };

    ITF_INLINE const Ray_AIFruitTrapBehavior_Template* getTemplate() const;
    
    bbool isCarrying() const;
    void changeState(State_t _newState);
    void updateState(f32 _dt);
    void processPunch(const PunchStim* _punch);

    State_t m_state;
    f32 m_timer;
    bbool m_crushAttackReceived;
    AIAction* m_standAction;
    AIAction* m_fruitFallAction;
    PolylineComponent* m_polylineComponent;
};

//------------------------------------------------------------------------------
class Ray_AIFruitTrapBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIFruitTrapBehavior_Template, TemplateAIBehavior,3020317045);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIFruitTrapBehavior);

public:
    Ray_AIFruitTrapBehavior_Template();
    ~Ray_AIFruitTrapBehavior_Template();
    
    ITF_INLINE f32 getDelay() const { return m_delay; }
    ITF_INLINE f32 getWeightThreshold() const { return m_weightThreshold; }
    ITF_INLINE AIAction_Template* getStandAction() const { return m_standAction; }
    ITF_INLINE AIAction_Template* getFruitFallAction() const { return m_fruitFallAction; }

private:
    f32 m_delay;
    f32 m_weightThreshold;
    AIAction_Template* m_standAction;
    AIAction_Template* m_fruitFallAction;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIFruitTrapBehavior_Template*
Ray_AIFruitTrapBehavior::getTemplate() const
{
    return static_cast<const Ray_AIFruitTrapBehavior_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_Ray_AIFruitTrapBehavior_H_
