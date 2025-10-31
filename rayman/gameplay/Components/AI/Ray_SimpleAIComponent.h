#ifndef _ITF_RAY_SIMPLEAICOMPONENT_H_
#define _ITF_RAY_SIMPLEAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

    class AIBehavior;
    class AIReceiveHitBehavior;

    class Ray_SimpleAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SimpleAIComponent,Ray_AIComponent,2237723057)
        DECLARE_SERIALIZE();

    public:

        Ray_SimpleAIComponent();
        ~Ray_SimpleAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onCheckpointLoaded();
        virtual void        Update( f32 _dt );
        virtual void        onBehaviorFinished();
        virtual void        onEvent(class Event * _event);

    protected:

        ITF_INLINE const class Ray_SimpleAIComponent_Template * getTemplate() const;

        virtual void        updateBehaviors( f32 _dt );
        virtual void        updateDeath( f32 _dt );
        virtual void        processHit( HitStim* _hit );
        virtual void        startDeath();
        virtual void        onReceiveHitBehaviorFinished();


        AIBehavior*             m_behavior;
        AIReceiveHitBehavior*   m_receiveHitBehavior;
        AIBehavior*             m_deathBehavior;

    private:

        void                    postLoadCheckpoint();
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class TemplateAIBehavior;
    class AIReceiveHitBehavior_Template;
    class TemplateAIBehavior;

    class Ray_SimpleAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SimpleAIComponent_Template,Ray_AIComponent_Template,2631517923);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SimpleAIComponent);

    public:

        Ray_SimpleAIComponent_Template();
        ~Ray_SimpleAIComponent_Template();

        ITF_INLINE const TemplateAIBehavior* getBehavior() const                        { return m_behavior; }
        ITF_INLINE const AIReceiveHitBehavior_Template* getReceiveHitBehavior() const   { return m_receiveHitBehavior; }
        ITF_INLINE const TemplateAIBehavior* getDeathBehavior() const                   { return m_deathBehavior; }
        ITF_INLINE const bbool               getAllowMultiHit() const                   { return m_allowMultiHit; }

    private:
        TemplateAIBehavior*              m_behavior;
        AIReceiveHitBehavior_Template*   m_receiveHitBehavior;
        TemplateAIBehavior*              m_deathBehavior;
        bbool                            m_allowMultiHit;


    };

    ITF_INLINE const class Ray_SimpleAIComponent_Template * Ray_SimpleAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_SimpleAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_SIMPLEAICOMPONENT_H_

