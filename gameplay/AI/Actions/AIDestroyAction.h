#ifndef _ITF_AIDESTROYACTION_H_
#define _ITF_AIDESTROYACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_


namespace ITF
{
    class AIDestroyAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIDestroyAction, AIAction,909775961);

    public:
        AIDestroyAction() : m_destructionRequested(bfalse), m_safeTimer( 0.0f ){}
        virtual void        onActivate();
        virtual void            update( f32 _dt );
    private:
        bbool   m_destructionRequested;
        f32     m_safeTimer;
        
        ITF_INLINE const class AIDestroyAction_Template * getTemplate() const;
    };

    class AIDestroyAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIDestroyAction_Template,AIAction_Template,1880254611);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIDestroyAction);

    public:

        AIDestroyAction_Template();
        ~AIDestroyAction_Template();

        ITF_INLINE bbool getWaitForFx()     const { return m_waitForFx; }

    private:
        bbool m_waitForFx;
    };

    ITF_INLINE const AIDestroyAction_Template * AIDestroyAction::getTemplate() const
    {
        return static_cast<const class AIDestroyAction_Template *>(m_template);
    }
}

#endif // _ITF_AIDESTROYACTION_H_

