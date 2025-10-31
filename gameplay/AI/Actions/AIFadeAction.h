#ifndef _ITF_AIFADEACTION_H_
#define _ITF_AIFADEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIFadeAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFadeAction, AIAction,745996257);

    public:

        AIFadeAction();
        virtual ~AIFadeAction();

        virtual void        onActivate();
        virtual void        update( f32 _delta );

    private:
        const class AIFadeAction_Template * getTemplate() const;
        f32                 m_timer;
    };

    class AIFadeAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFadeAction_Template,AIAction_Template,649375324);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIFadeAction);

    public:

        AIFadeAction_Template();
        ~AIFadeAction_Template();

        ITF_INLINE ITF::bbool getVisible() const { return m_visible; }
        ITF_INLINE ITF::f32 getFadeDuration() const { return m_fadeDuration; }

    private:
        bbool               m_visible;          // fade in or fade out?
        f32                 m_fadeDuration;
    };

    ITF_INLINE const class AIFadeAction_Template * AIFadeAction::getTemplate() const
    {
        return static_cast<const class AIFadeAction_Template *>(m_template);
    }
}

#endif // _ITF_AIFADEACTION_H_

