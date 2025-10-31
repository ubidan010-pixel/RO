#ifndef _ITF_AIBUMPERACTION_H_
#define _ITF_AIBUMPERACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIBumperAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIBumperAction, AIAction,3172780191);
        DECLARE_VALIDATE_ACTION()

    public:

        AIBumperAction();
        virtual ~AIBumperAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );
        virtual void        setData(const Vec2d & _dir) { m_direction = _dir; m_direction.normalize();}

    private:
        const class AIBumperAction_Template * getTemplate() const;

        class StickToPolylinePhysComponent* m_physComponent;


        bbool                               m_justStarted;
        Vec2d                               m_direction;
    };

    class AIBumperAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIBumperAction_Template,AIAction_Template,1033957686);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIBumperAction);

    public:

        AIBumperAction_Template();
        ~AIBumperAction_Template(){}

        ITF_INLINE f32 getJumpForce() const { return m_jumpForce; }

    private:
        f32                                 m_jumpForce;
    };

    ITF_INLINE const class AIBumperAction_Template * AIBumperAction::getTemplate() const
    {
        return static_cast<const class AIBumperAction_Template *>(m_template);
    }
}

#endif // _ITF_AIBUMPERACTION_H_