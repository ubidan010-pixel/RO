#ifndef _ITF_AIBEZIERACTION_H_
#define _ITF_AIBEZIERACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

class AIBezierAction_Template;
class PhysComponent;

enum FixedDurationInterpolType
{
    linear  = 0,
    X2      = 1,
    X3      = 2,
    X4      = 3,
    X5      = 4,
    invX2   = 5,
    invX3   = 6,
    invX4   = 7,
    invX5   = 8,
    ENUM_FORCE_SIZE_32( FixedDurationInterpolType )
};

//------------------------------------------------------------------------------
/// Make an actor follow a cubic Bezier curve
class AIBezierAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI(AIBezierAction, AIAction,1315574504);
    DECLARE_VALIDATE_ACTION()

public:
    AIBezierAction();
    virtual ~AIBezierAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void update(f32 _dt);
    virtual void onEvent(Event * _event);
    virtual void updateAnimInputs();
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    void setupBezier(
        const Vec3d& _p0,
        const Vec3d& _p1,
        const Vec3d& _p2,
        const Vec3d& _p3
        );
        
    ITF_INLINE const Vec3d getStartTangent() const { return (m_p1 - m_p0); }
    ITF_INLINE const Vec3d getEndTangent() const { return (m_p3 - m_p2); }
    ITF_INLINE const f32 getCurrentCursor() const { return m_t; }
        
private:
    ITF_INLINE const AIBezierAction_Template* getTemplate() const;

    f32 m_timer;
    f32 m_t;
    Vec3d m_p0;
    Vec3d m_p1;
    Vec3d m_p2;
    Vec3d m_p3;
    bbool m_physWasDisabled;
    PhysComponent* m_physComponent;
};

//------------------------------------------------------------------------------
class AIBezierAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AIBezierAction_Template,AIAction_Template,358550596);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(AIBezierAction);

public:
    AIBezierAction_Template();
    ~AIBezierAction_Template();

    ITF_INLINE bbool getChangeAngle()                   const { return m_changeAngle; }
    ITF_INLINE bbool getUpdatePhysSpeedAtEnd()          const { return m_updatePhysSpeedAtEnd; }
    ITF_INLINE f32 getSpeed()                           const { return m_speed; }
    ITF_INLINE f32 getFixedDuration()                   const { return m_fixedDuration; }
    ITF_INLINE const FixedDurationInterpolType  getFixedDurationInterpolType()  const { return static_cast<FixedDurationInterpolType>( m_fixedDurationInterpolType ); }

private:
    bbool                               m_changeAngle;
    bbool                               m_updatePhysSpeedAtEnd;
    f32                                 m_speed;
    f32                                 m_fixedDuration;
    u32                                 m_fixedDurationInterpolType;
};

//------------------------------------------------------------------------------
ITF_INLINE
const class AIBezierAction_Template*
AIBezierAction::getTemplate() const
{
    return static_cast<const AIBezierAction_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_AIBEZIERACTION_H_

