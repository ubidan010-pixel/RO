#ifndef _ITF_AIBOUNCETOLAYERACTION_H_
#define _ITF_AIBOUNCETOLAYERACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIBounceToLayerAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIBounceToLayerAction, AIAction,3227750740);
        DECLARE_VALIDATE_ACTION()

    public:

        AIBounceToLayerAction();
        virtual ~AIBounceToLayerAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        update( f32 _delta );

        void                setData( f32 _speed, f32 _height, f32 _height2, ObjectRef _target, const Vec2d& _offset );

    private:
        const class AIBounceToLayerAction_Template * getTemplate() const;
        Vec3d getTargetPosition() const;
        void updateSpeed( f32 _dt );
        void updatePosition( f32 _dt );


        StickToPolylinePhysComponent *      m_physComponent;

        ObjectRef   m_target;
        Vec3d       m_startPos;
        f32         m_totalTime;
        f32         m_speed;
        f32         m_timeCounter;
        f32         m_height;
        f32         m_height2;
        Vec2d       m_targetOffset;
        f32         m_gravity;
        Vec2d       m_currentSpeed;
        Vec2d       m_prevPos;
        Vec3d       m_targetPoint;
    };

    class AIBounceToLayerAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIBounceToLayerAction_Template,AIAction_Template,4055762704);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIBounceToLayerAction);

    public:

        AIBounceToLayerAction_Template();
        ~AIBounceToLayerAction_Template(){}

    private:

    };

    ITF_INLINE const class AIBounceToLayerAction_Template * AIBounceToLayerAction::getTemplate() const
    {
        return static_cast<const class AIBounceToLayerAction_Template *>(m_template);
    }
}

#endif // _ITF_AIBOUNCETOLAYERACTION_H_