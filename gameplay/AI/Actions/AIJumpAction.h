#ifndef _ITF_AIJUMPACTION_H_
#define _ITF_AIJUMPACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIJumpAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIJumpAction, AIAction,4165498331);
        DECLARE_VALIDATE_ACTION()

    public:

        AIJumpAction();
        virtual ~AIJumpAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );

    private:
        const class AIJumpAction_Template * getTemplate() const;

        class StickToPolylinePhysComponent* m_physComponent;


        bbool                               m_justStarted;
    };

    class AIJumpAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIJumpAction_Template,AIAction_Template,1081270361);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIJumpAction);

    public:

        AIJumpAction_Template();
        ~AIJumpAction_Template(){}

        ITF_INLINE f32 getJumpForce() const { return m_jumpForce; }
        ITF_INLINE f32 getMinXSpeed() const { return m_minXSpeed; }
        ITF_INLINE f32 getMaxXSpeed() const { return m_maxXSpeed; }
        ITF_INLINE f32 getMinXForce() const { return m_minXForce; }
        ITF_INLINE f32 getMaxXForce() const { return m_maxXForce; }

    private:
        f32                                 m_jumpForce;
        f32                                 m_minXSpeed;
        f32                                 m_maxXSpeed;
        f32                                 m_minXForce;
        f32                                 m_maxXForce;
    };

    ITF_INLINE const class AIJumpAction_Template * AIJumpAction::getTemplate() const
    {
        return static_cast<const class AIJumpAction_Template *>(m_template);
    }
}

#endif // _ITF_AIJUMPACTION_H_