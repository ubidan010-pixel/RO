

#ifndef _ITF_AIJUMPTOTARGETACTION_H_
#define _ITF_AIJUMPTOTARGETACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIJumpToTargetAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIJumpToTargetAction, AIAction,1294508773);

    public:

        AIJumpToTargetAction();
        virtual ~AIJumpToTargetAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        update( f32 _delta );

        void                setTargetPos( const Vec3d& _pos ) { m_targetPos = _pos; m_targetActor = ITF_INVALID_OBJREF; }
        void                setTargetActor( ObjectRef _ref ) { m_targetActor = _ref; }

    private:
        ITF_INLINE const class AIJumpToTargetAction_Template * getTemplate() const;

        Vec3d               getTargetPos() const;

        class StickToPolylinePhysComponent* m_physComponent;

        f32                 m_jumpCounter;
        Vec3d               m_targetPos;
        Vec3d               m_startPos;
        ObjectRef           m_targetActor;
        Vec2d               m_prevPos;
    };

    class AIJumpToTargetAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIJumpToTargetAction_Template,AIAction_Template,3165987287);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIJumpToTargetAction);

    public:

        AIJumpToTargetAction_Template();
        ~AIJumpToTargetAction_Template(){}

        f32                     getDuration() const { return m_duration; }
        f32                     getHeight0() const { return m_height0; }
        f32                     getHeight1() const { return m_height1; }

    private:

        f32                     m_duration;
        f32                     m_height0;
        f32                     m_height1;
    };
    const class AIJumpToTargetAction_Template * AIJumpToTargetAction::getTemplate() const
    {
        return static_cast<const class AIJumpToTargetAction_Template *>(m_template);
    }
}

#endif // _ITF_AIJUMPTOTARGETACTION_H_

