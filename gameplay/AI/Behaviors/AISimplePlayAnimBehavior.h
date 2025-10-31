

#ifndef _ITF_AISIMPLEPLAYANIMBEHAVIOR_H_
#define _ITF_AISIMPLEPLAYANIMBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIPlayAnimAction;

    class AISimplePlayAnimBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AISimplePlayAnimBehavior, AIBehavior,4171060905);
        //DECLARE_VALIDATE_BEHAVIOR()

    public:

        AISimplePlayAnimBehavior();
        virtual ~AISimplePlayAnimBehavior();

        virtual void        onActivate();
        virtual void        onActionFinished();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    private:
        ITF_INLINE const class AISimplePlayAnimBehavior_Template * getTemplate() const;

        AIPlayAnimAction*   m_playAnim;
    };

    class AISimplePlayAnimBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AISimplePlayAnimBehavior_Template,TemplateAIBehavior,3789495365);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(AISimplePlayAnimBehavior);

    public:

        AISimplePlayAnimBehavior_Template();
        ~AISimplePlayAnimBehavior_Template();

        ITF_INLINE const class AIPlayAnimAction_Template* getPlayAnim() const { return m_playAnim; }
    private:
        class AIPlayAnimAction_Template*   m_playAnim;
    };

    const class AISimplePlayAnimBehavior_Template * AISimplePlayAnimBehavior::getTemplate() const
    {
        return static_cast<const class AISimplePlayAnimBehavior_Template *>(m_template);
    }

}

#endif // _ITF_AISIMPLEPLAYANIMBEHAVIOR_H_

