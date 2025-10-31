

#ifndef _ITF_AIJUMPINDIRACTION_H_
#define _ITF_AIJUMPINDIRACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIJumpInDirAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIJumpInDirAction, AIAction,2174684917);

    public:

        AIJumpInDirAction();
        virtual ~AIJumpInDirAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );

    private:
        class StickToPolylinePhysComponent* m_physComponent;

        bbool                               m_justStarted;
        ITF_INLINE const class AIJumpInDirAction_Template * getTemplate() const;
    };

    class AIJumpInDirAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIJumpInDirAction_Template,AIAction_Template,4120346116);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIJumpInDirAction);

    public:

        AIJumpInDirAction_Template();
        ~AIJumpInDirAction_Template(){}

        ITF_INLINE const Vec2d & getJumpForce() const { return m_jumpForce; }
        ITF_INLINE bbool getFlipDirection() const { return m_flipDirection; }
    private:
        Vec2d                               m_jumpForce;
        bbool                               m_flipDirection;
    };
    const class AIJumpInDirAction_Template * AIJumpInDirAction::getTemplate() const
    {
        return static_cast<const class AIJumpInDirAction_Template *>(m_template);
    }
}

#endif // _ITF_AIJUMPINDIRACTION_H_

