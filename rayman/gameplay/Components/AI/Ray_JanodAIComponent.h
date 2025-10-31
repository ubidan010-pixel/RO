#ifndef _ITF_RAY_JANODAICOMPONENT_H__
#define _ITF_RAY_JANODAICOMPONENT_H__

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

namespace ITF
{
    class Ray_AIReceiveHitBehavior;
    class Ray_AIDeathBehavior;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_JanodAIComponent : public Ray_FixedAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_JanodAIComponent, Ray_FixedAIComponent,1405091922);
        DECLARE_SERIALIZE()
    public:
        Ray_JanodAIComponent();
        ~Ray_JanodAIComponent();

        void processHit(PunchStim * _hit);
        void Update( f32 _dt );

        bbool getIfStartsAtApex() const {return m_startsAtApex;}
        f32 getDelayBeforeStarting() const {return m_delayBeforeStarting;}
        bbool getMustWaitForTrigger() const {return m_mustWaitForTrigger;}
        f32 getHowLongWasCrushed() const {return m_howLongWasCrushed;}
        void setIsInBristleMode(bbool _bristling) {m_isInBristleMode = _bristling;}
        bbool canFallWhenSleeping() const {return m_canFallWhenSleeping;}

    protected:
        ITF_INLINE const class Ray_JanodAIComponent_Template * getTemplate() const;
        void onBehaviorFinished();
        void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        void onEvent(Event * _event);
        bbool canCrush(); 
        bbool isSquashAuthorized() const {return btrue;}
        void startDeath();
        void processCrush(EventCrushed *crushed);

        Ray_AIReceiveHitBehavior *m_receiveHitBehavior;
        AIBehavior *m_deathBehavior;
        AIBehavior *m_crushedBehavior;
        bbool m_startsAtApex;
        bbool m_mustWaitForTrigger;
        f32 m_delayBeforeStarting;
        f32 m_howLongWasCrushed;
        bbool m_isInBristleMode;
        bbool m_canFallWhenSleeping;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_JanodAIComponent_Template : public Ray_FixedAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_JanodAIComponent_Template,Ray_FixedAIComponent_Template,4131929867);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_JanodAIComponent);
    public:
        Ray_JanodAIComponent_Template();
        ~Ray_JanodAIComponent_Template();

        const class Ray_AIReceiveHitBehavior_Template* getReceiveHitBehavior() const {return m_receiveHitBehavior;}
        const class TemplateAIBehavior* getDeathBehavior() const {return m_deathBehavior;}
        const class TemplateAIBehavior* getCrushedBehavior() const {return m_crushedBehavior;}
        f32 getSquashPenetrationRadius() const {return m_squashPenetrationRadius;}
        ITF_INLINE bbool getCanRehit() const {return m_canRehit;}

    private:
        class Ray_AIReceiveHitBehavior_Template* m_receiveHitBehavior;
        class TemplateAIBehavior* m_deathBehavior;
        class TemplateAIBehavior* m_crushedBehavior;
        f32 m_squashPenetrationRadius;
        bbool m_canRehit;
    };
    ITF_INLINE const class Ray_JanodAIComponent_Template * Ray_JanodAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_JanodAIComponent_Template *>(m_template);
    }
}

#endif
