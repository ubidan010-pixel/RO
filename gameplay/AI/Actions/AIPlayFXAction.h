#ifndef _ITF_AIPLAYFXACTION_H_
#define _ITF_AIPLAYFXACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

    class AIPlayFXAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPlayFXAction, AIAction,1729599829);

    public:

        AIPlayFXAction();
        virtual ~AIPlayFXAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );

    private:
        u32                 m_playIndex;
        ITF_INLINE const class AIPlayFXAction_Template * getTemplate() const;
    };

    class AIPlayFXAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPlayFXAction_Template,AIAction_Template,3661521510);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIPlayFXAction);

    public:

        AIPlayFXAction_Template(){}
        ~AIPlayFXAction_Template(){}
        ITF_INLINE StringID getEffectID() const { return m_effectID; }

    private:
        StringID            m_effectID;
    };
    const class AIPlayFXAction_Template * AIPlayFXAction::getTemplate() const
    {
        return static_cast<const class AIPlayFXAction_Template *>(m_template);
    }
}

#endif // _ITF_AIPLAYFXACTION_H_