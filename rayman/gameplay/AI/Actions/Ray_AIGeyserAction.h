#ifndef _ITF_RAY_AIGEYSERACTION_H_
#define _ITF_RAY_AIGEYSERACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_


namespace ITF
{
    class Ray_AiGeyserAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AiGeyserAction, AIAction,1462170500);

    public:

        Ray_AiGeyserAction();
        virtual ~Ray_AiGeyserAction();

        virtual void        update( f32 _delta );
        virtual void        onEvent( Event* _event);
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        onDeactivate();

    protected:
        ITF_INLINE const class Ray_AIGeyserAction_Template * getTemplate() const;
        bbool               m_performHit;
        class Ray_GeyserPlatformAIComponent * m_geyserComponent;
    };

    class Ray_AIGeyserAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGeyserAction_Template,AIAction_Template,1308112739);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AiGeyserAction);

    public:

        Ray_AIGeyserAction_Template();
        ~Ray_AIGeyserAction_Template(){}

    private:
    };

    ITF_INLINE const class Ray_AIGeyserAction_Template * Ray_AiGeyserAction::getTemplate() const
    {
        return static_cast<const class Ray_AIGeyserAction_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGEYSERACTION_H_
