#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LIVINGSTONEAICOMPONENT2_H_
#include "rayman/gameplay/Components/AI/Ray_LivingStoneAIComponent2.h"
#endif //_ITF_RAY_LIVINGSTONEAICOMPONENT2_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRunAwayBehavior.h"
#endif //_ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneWindBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneDrowningBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_LivingStoneAIComponent2)
        BEGIN_SERIALIZATION_CHILD(Ray_LivingStoneAIComponent2)
        END_SERIALIZATION()

        enum LivingStoneBankState
    {
        LivingStoneBankState_Default = 0,
        LivingStoneBankState_HasFruit = 1,
        LivingStoneBankState_IsNaked = 2,
    };

    Ray_LivingStoneAIComponent2::Ray_LivingStoneAIComponent2()
        : Super()
        , m_sendHurtFromHatTarget(ITF_INVALID_OBJREF)
        , m_sendHurtFromHatPos(Vec2d::Zero)
    {
    }


    Ray_LivingStoneAIComponent2::~Ray_LivingStoneAIComponent2()
    {
    }



    void Ray_LivingStoneAIComponent2::processHit( PunchStim* _hit )
    {
        if ( getTemplate()->getSpikyHat() )
        {
            // Check if it's coming from above, if it is we reply with a bounce stim
            Vec2d upVector = Vec2d::Right.Rotate(m_actor->getAngle()+MTH_PIBY2);
            bbool punchDown = _hit->getDirection().dot(upVector) <= -0.5f;

            if ( punchDown )
            {
                // If the direction of the hit is downwards
                Vec2d vecToPos = _hit->getPos() - m_actor->get2DPos();

                if ( vecToPos.dot(upVector) > 0.f )
                {
                    // If the origin of the hit is on top
                    Actor* attacker = AIUtils::getActor(_hit->getSender());

                    if ( attacker )
                    {
                        if ( AIUtils::isEnemy(getFaction(),AIUtils::getFaction(attacker)) )
                        {
                            m_sendHurtFromHatTarget = _hit->getSender();
                            m_sendHurtFromHatPos = _hit->getPos();
                            m_closeRangeAttackVictims.push_back(_hit->getSender());
                        }

                        return;
                    }
                }
            }
        }

        Super::processHit(_hit);
    }

    void Ray_LivingStoneAIComponent2::sendHitFromHat()
    {
        Actor* receiver = AIUtils::getActor(m_sendHurtFromHatTarget);

        if ( receiver )
        {
            PunchStim punchStim;

            punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            punchStim.setIsRadial(bfalse);
            punchStim.setDirection(Vec2d::Up.Rotate(m_actor->getAngle()));
            punchStim.setFxPos(receiver->getPos());
            punchStim.setLevel(0);
            punchStim.setFaction(getFaction());

            punchStim.setSender(m_actor->getRef());
            punchStim.setAngle(m_actor->getAngle());
            punchStim.setPos(m_sendHurtFromHatPos);

            receiver->onEvent(&punchStim);
        }

        m_sendHurtFromHatTarget = ITF_INVALID_OBJREF;
    }

    void Ray_LivingStoneAIComponent2::Update( f32 _dt )
    {
        Super::Update(_dt);

        if ( m_sendHurtFromHatTarget != ITF_INVALID_OBJREF )
        {
            sendHitFromHat();
        }
    }




    void Ray_LivingStoneAIComponent2::processInteractionQuery( EventInteractionQuery* interactionQuery )
    {

        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if (getTemplate()->getSpikyHat())
            {
                if (m_currentBehavior != m_deathBehavior)
                {
                    ActorRef ref = interactionQuery->getSender();
                    Actor * actor = ref.getActor();
                    if (actor)
                    {
                        if (actor->get2DPos().m_y < m_actor->get2DPos().m_y + getTemplate()->getSpikyHatMaxDistance()  )
                        {
                            //Super::processInteractionQuery(interactionQuery);
                            return;
                        }
                        PunchStim punchStim;
                        const SCollidableContact * contact = interactionQuery->getContact();
                        if (contact)
                        {
                            const f32 z = m_actor->getDepth();
                            const Vec3d pos = contact->m_contactPoint.to3d(z);
                            punchStim.setPos(contact->m_contactPoint);
                        }
                        punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
                        punchStim.setIsRadial(bfalse);
                        punchStim.setDirection(Vec2d::Up.Rotate(m_actor->getAngle()));
                        punchStim.setFxPos(actor->getPos());
                        punchStim.setLevel(0);
                        punchStim.setFaction(getFaction());

                        punchStim.setSender(m_actor->getRef());
                        punchStim.setAngle(m_actor->getAngle());
                        actor->onEvent(&punchStim);
                        interactionQuery->setInteraction(CharacterInteractionType_Other);
                    }
                }
                else
                    Super::processInteractionQuery(interactionQuery);
            }
            else
                Super::processInteractionQuery(interactionQuery);
        }
    }


    void Ray_LivingStoneAIComponent2::receiveHit( HitStim* _hit, bbool _damageDependsOnLevel, u32 _forcedDamage)
    {
        PunchStim * stim = _hit->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
        if (stim)
        {
            if (stim->getLevel() == 2)
            {
                Super::receiveHit(_hit,bfalse,getTemplate()->getHealth_default());
                return;
            }
        }
        Super::receiveHit(_hit);

    }


    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_LivingStoneAIComponent2_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_LivingStoneAIComponent2_Template)
    SERIALIZE_MEMBER("spikyHat",m_spikyHat);
    SERIALIZE_MEMBER("spikyHatMaxDistance",m_spikyHatMaxDistance);
    END_SERIALIZATION()

        Ray_LivingStoneAIComponent2_Template::Ray_LivingStoneAIComponent2_Template()
        : m_spikyHat(bfalse)
        , m_spikyHatMaxDistance(2.0f)
    {
    }

    Ray_LivingStoneAIComponent2_Template::~Ray_LivingStoneAIComponent2_Template()
    {
    }

}

