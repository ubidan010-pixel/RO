#ifndef _ITF_AIPLAYACTIONSBEHAVIOR_H_
#define _ITF_AIPLAYACTIONSBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    /* Uber-simple behavior: play its action one by one,
       then sends an OnBehaviorFinished event to its component... up to him to stop us.
    */
    class AIPlayActionsBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPlayActionsBehavior, AIBehavior,1704906529);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        AIPlayActionsBehavior();
        virtual ~AIPlayActionsBehavior();

        virtual void        onActivate();
        virtual void        onActionFinished();
        virtual void        onEvent(Event * _event);
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:

        u32                 m_currentActionIndex;

    private:
        ITF_INLINE const class AIPlayActionsBehavior_Template * getTemplate() const;
    };

    class AIPlayActionsBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPlayActionsBehavior_Template,TemplateAIBehavior,3298039445);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(AIPlayActionsBehavior);

    public:

        AIPlayActionsBehavior_Template();
        ~AIPlayActionsBehavior_Template();

        typedef ITF_VECTOR <AIAction_Template*>  ActionsArray;
        ITF_INLINE const ActionsArray & getPlayActions() const { return m_playActions; }
    private:
        ActionsArray m_playActions;
    };

    ITF_INLINE const class AIPlayActionsBehavior_Template * AIPlayActionsBehavior::getTemplate() const
    {
        return static_cast<const class AIPlayActionsBehavior_Template *>(m_template);
    }

}

#endif // _ITF_AIPLAYACTIONSBEHAVIOR_H_