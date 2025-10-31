#ifndef _ITF_RAY_LIVINGSTONEAICOMPONENT2_H_
#define _ITF_RAY_LIVINGSTONEAICOMPONENT2_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundEnemyAIComponent.h"
#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_


#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif
namespace ITF
{
    class Ray_AIGroundRunAwayBehavior;
    class Ray_AIGroundRoamBehavior;
    class Ray_AIWaterBaseBehavior;
    class Ray_AILivingStoneWindBehavior;

    class Ray_LivingStoneAIComponent2 : public Ray_GroundEnemyAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LivingStoneAIComponent2, Ray_GroundEnemyAIComponent,2988881290)
        DECLARE_SERIALIZE();

    public:
        Ray_LivingStoneAIComponent2();
        virtual ~Ray_LivingStoneAIComponent2();

        virtual void                            Update( f32 _deltaTime );

        virtual void                            receiveHit( HitStim* _hit, bbool _damageDependsOnLevel = btrue, u32 _forcedDamage = 0 );
    protected:
        virtual void                            processInteractionQuery( EventInteractionQuery* interactionQuery );

    private:
        ITF_INLINE const class Ray_LivingStoneAIComponent2_Template * getTemplate() const;

        virtual void                            processHit( PunchStim* _hit );

        void                                    sendHitFromHat();
        ObjectRef                               m_sendHurtFromHatTarget;
        Vec2d                                   m_sendHurtFromHatPos;
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_LivingStoneAIComponent2_Template : public Ray_GroundEnemyAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LivingStoneAIComponent2_Template, Ray_GroundEnemyAIComponent_Template,4086570798);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LivingStoneAIComponent2);

    public:

        Ray_LivingStoneAIComponent2_Template();
        ~Ray_LivingStoneAIComponent2_Template();

        ITF_INLINE bbool                        getSpikyHat() const             {return m_spikyHat;}
        ITF_INLINE f32 getSpikyHatMaxDistance() const { return m_spikyHatMaxDistance; }

    private:
       
        bbool                                   m_spikyHat;
        f32                                     m_spikyHatMaxDistance;
    };

    ITF_INLINE const class Ray_LivingStoneAIComponent2_Template * Ray_LivingStoneAIComponent2::getTemplate() const
    {
        return static_cast<const class Ray_LivingStoneAIComponent2_Template *>(m_template);
    }


}

#endif //_ITF_RAY_LIVINGSTONEAICOMPONENT2_H_

