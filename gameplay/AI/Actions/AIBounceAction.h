

#ifndef _ITF_AIBOUNCEACTION_H_
#define _ITF_AIBOUNCEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

    class AIBounceAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIBounceAction, AIAction,2698598524);

    public:

        AIBounceAction();
        virtual ~AIBounceAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );
        virtual void        onEvent(Event * _event);

        Vec2d               bounce( const Vec2d& speed, const Vec2d& _edgeDir );
    private:
        ITF_INLINE const class AIBounceAction_Template * getTemplate() const;

        class StickToPolylinePhysComponent* m_physComponent;

        bbool                               m_justStarted;

        Vec2d                               m_speed;
        Vec2d                               m_previousSpeed;

    };

    class AIBounceAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIBounceAction_Template,AIAction_Template,1670937690);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIBounceAction);

    public:

        AIBounceAction_Template();
        ~AIBounceAction_Template();
        ITF_INLINE f32 getMaxXSpeed() const { return m_maxXSpeed; }
        ITF_INLINE f32 getBounceForce() const { return m_bounceForce; }
        ITF_INLINE f32 getMaxXSpeedThresholdForce() const { return m_maxXSpeedThresholdForce; }
        ITF_INLINE Angle getMinBounceAngle() const { return m_minBounceAngle; }

    private:
        f32                                 m_maxXSpeed;
        f32                                 m_bounceForce;
        f32                                 m_maxXSpeedThresholdForce;
        Angle                               m_minBounceAngle;

    };

    const class AIBounceAction_Template * AIBounceAction::getTemplate() const
    {
        return static_cast<const class AIBounceAction_Template *>(m_template);
    }
}

#endif // _ITF_AIBOUNCEACTION_H_

