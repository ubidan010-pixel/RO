#ifndef _ITF_AIFALLNOPHYSACTION_H_
#define _ITF_AIFALLNOPHYSACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

class AIFallNoPhysAction_Template;
class StickToPolylinePhysComponent;

//------------------------------------------------------------------------------
/// Same as AIFallAction, but with a bit of friction and a time out
class AIFallNoPhysAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI(AIFallNoPhysAction, AIAction,1120323652);
    DECLARE_VALIDATE_ACTION()

public:
    AIFallNoPhysAction();
    virtual ~AIFallNoPhysAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/) ;
    virtual void onActivate();
    virtual void update(f32 _dt);

private:
    ITF_INLINE const AIFallNoPhysAction_Template* getTemplate() const;

    f32 m_timer;
    f32 m_speed;
};

//------------------------------------------------------------------------------
class AIFallNoPhysAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AIFallNoPhysAction_Template,AIAction_Template,3237588225);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(AIFallNoPhysAction);

public:
    AIFallNoPhysAction_Template();
    virtual ~AIFallNoPhysAction_Template();

    ITF_INLINE f32 getDuration() const { return m_duration; }
    ITF_INLINE f32 getFakeGravity() const { return m_fakeGravity; }
    ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }

private:
    f32 m_duration;
    f32 m_fakeGravity;
    f32 m_maxSpeed;
};

//------------------------------------------------------------------------------
ITF_INLINE
const AIFallNoPhysAction_Template*
AIFallNoPhysAction::getTemplate() const
{
    return static_cast<const AIFallNoPhysAction_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_AIFALLNOPHYSACTION_H_
