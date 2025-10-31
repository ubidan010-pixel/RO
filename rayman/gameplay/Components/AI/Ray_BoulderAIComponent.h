#ifndef _ITF_RAY_BOULDERAICOMPONENT_H_
#define _ITF_RAY_BOULDERAICOMPONENT_H_

#ifndef _ITF_RAY_AIFRUITCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FRUitAIComponent.h"
#endif //_ITF_RAY_AIFRUITCOMPONENT_H_

namespace ITF
{
    //forward declarations
    class Ray_BoulderAIComponent_Template;
    class Ray_AIBoulderSentBackBehavior;
    class Ray_AIBoulderSentBackBehavior_Template;
    class Ray_StimComponent;


    //------------------------------------------------------------------
    // Ray_BoulderAIComponent : manage boulder AI
    //------------------------------------------------------------------
    class Ray_BoulderAIComponent : public Ray_FruitAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BoulderAIComponent, Ray_FruitAIComponent,2361173509)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BoulderAIComponent();
        virtual ~Ray_BoulderAIComponent();

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onBecomeActive();
        virtual void Update(f32 _dt);
        virtual void onEvent(Event* _event);

    protected:

        //interruptions
        virtual void processHit(PunchStim* _hit);

        //check death conditions
        virtual bbool isInDeathCondition() const;

    private:

        //get template
        ITF_INLINE const Ray_BoulderAIComponent_Template * getTemplate() const;

        //behaviors management
        void startSentBack(const PunchStim * const _hit);
        void updateSentBack();


        //Data
    private:
        Ray_AIBoulderSentBackBehavior * m_sentBackBehavior;
        Ray_StimComponent * m_stimComponent;
        f32 m_timeSinceLastHitFX;
        f32 m_lifeTime;
        f32 m_destructionCountDown;
        bbool m_senderDead;
        bbool m_hitSent;
        bbool m_hitEnvironment;
        bbool m_destroyingHit;
    };




    //------------------------------------------------------------------
    // Ray_BoulderAIComponent_Template
    //------------------------------------------------------------------
    class Ray_BoulderAIComponent_Template : public Ray_FruitAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BoulderAIComponent_Template,Ray_FruitAIComponent_Template,1007983663);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BoulderAIComponent);

    public:

        Ray_BoulderAIComponent_Template();
        virtual ~Ray_BoulderAIComponent_Template();

        ITF_INLINE Ray_AIBoulderSentBackBehavior_Template * getSentBackBehavior() const;
        ITF_INLINE f32 getMaxSentBackDuration() const;
        ITF_INLINE f32 getMaxBouncesCount() const;
        ITF_INLINE f32 getTimeAfterLastBounce() const;
        ITF_INLINE f32 getMaxTimeWithoutBounce() const;
        ITF_INLINE f32 getMaxLifeTimeAfterSenderDeath() const;
        ITF_INLINE f32 getImmediateExplosionOnSenderDeathPeriod() const;
        ITF_INLINE f32 getNoStimPeriod() const;
        ITF_INLINE f32 getSentBackNormalSpeed() const;
        ITF_INLINE f32 getSentBackTornadoSpeed() const;

        //Data
    private:
        Ray_AIBoulderSentBackBehavior_Template * m_sentBackBehavior;
        f32 m_maxSentBackDuration;
        f32 m_maxBouncesCount;
        f32 m_timeAfterLastBounce;
        f32 m_maxTimeWithoutBounce;
        f32 m_maxTimeAfterSenderDeath;
        f32 m_immediateExplosionOnSenderDeathPeriod;
        f32 m_noStimPeriod;
        f32 m_sentbackNormalSpeed;
        f32 m_sentbackTornadoSpeed;
    };




    //------------------------------------------------------------------
    // Ray_BoulderAIComponent inlined functions definition
    //------------------------------------------------------------------

    ITF_INLINE const Ray_BoulderAIComponent_Template* Ray_BoulderAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BoulderAIComponent_Template*>(m_template);
    }



    //------------------------------------------------------------------
    // Ray_BoulderAIComponent_Template inlined functions definition
    //------------------------------------------------------------------

    ITF_INLINE Ray_AIBoulderSentBackBehavior_Template * Ray_BoulderAIComponent_Template::getSentBackBehavior() const
    {
        return m_sentBackBehavior;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getMaxSentBackDuration() const
    {
        return m_maxSentBackDuration;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getMaxBouncesCount() const
    {
        return m_maxBouncesCount;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getTimeAfterLastBounce() const
    {
        return m_timeAfterLastBounce;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getMaxTimeWithoutBounce() const
    {
        return m_maxTimeWithoutBounce;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getMaxLifeTimeAfterSenderDeath() const
    {
        return m_maxTimeAfterSenderDeath;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getImmediateExplosionOnSenderDeathPeriod() const
    {
        return m_immediateExplosionOnSenderDeathPeriod;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getNoStimPeriod() const
    {
        return m_noStimPeriod;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getSentBackNormalSpeed() const
    {
        return m_sentbackNormalSpeed;
    }

    ITF_INLINE f32 Ray_BoulderAIComponent_Template::getSentBackTornadoSpeed() const
    {
        return m_sentbackTornadoSpeed;
    }
}

#endif //_ITF_RAY_BOULDERAICOMPONENT_H_
