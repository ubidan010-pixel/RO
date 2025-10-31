#ifndef _ITF_SIMPLEAICOMPONENT_H_
#define _ITF_SIMPLEAICOMPONENT_H_

#ifndef _ITF_GENERICAICOMPONENT_H_
#include "gameplay/Components/AI/GenericAIComponent.h"
#endif //_ITF_GENERICAICOMPONENT_H_

namespace ITF
{


    class AIBehavior;
    class AIReceiveHitBehavior;

    class SimpleAIComponent : public GenericAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SimpleAIComponent,GenericAIComponent,806807766);
        DECLARE_SERIALIZE();

    public:

        SimpleAIComponent();
        ~SimpleAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onCheckpointLoaded();
        virtual void        Update( f32 _deltaTime );
        virtual void        onBehaviorFinished();
        virtual void        onEvent(class Event * _event);

    protected:

        ITF_INLINE const class SimpleAIComponent_Template * getTemplate() const;

        virtual void            processHit( HitStim* _hit );

        AIReceiveHitBehavior*   m_receiveHitBehavior;
        AIBehavior*             m_deathBehavior;

    private:

        void                    postLoadCheckpoint();
    };



    class SimpleAIComponent_Template : public GenericAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(SimpleAIComponent_Template,GenericAIComponent_Template,688798707);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SimpleAIComponent);

    public:

        SimpleAIComponent_Template();
        ~SimpleAIComponent_Template();

        ITF_INLINE const class AIReceiveHitBehavior_Template* getReceiveHitBehavior() const { return m_receiveHitBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getDeathBehavior() const { return m_deathBehavior; }

    private:
        class AIReceiveHitBehavior_Template*   m_receiveHitBehavior;
        class TemplateAIBehavior*              m_deathBehavior;
    };

    ITF_INLINE const class SimpleAIComponent_Template * SimpleAIComponent::getTemplate() const
    {
        return static_cast<const class SimpleAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_SIMPLEAICOMPONENT_H_

