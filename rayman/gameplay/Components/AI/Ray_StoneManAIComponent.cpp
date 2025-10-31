#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STONEMANAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_StoneManAIComponent.h"
#endif //_ITF_RAY_STONEMANAICOMPONENT_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{
    //-------------------------------------------------------
    //  Ray_StoneManAIComponent
    //-------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_StoneManAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_StoneManAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_StoneManAIComponent)
    END_VALIDATE_COMPONENT()


    //-------------------------------------------------------
    Ray_StoneManAIComponent::Ray_StoneManAIComponent()
    : m_receiveHitWithBoulderBehavior(NULL)
    , m_closeRangeBackAttackBehavior(NULL)
    , m_boulderList()
    , m_boulderPresence(bfalse)
    {
    }

    //-------------------------------------------------------
    Ray_StoneManAIComponent::~Ray_StoneManAIComponent()
    {
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        //create behaviors from template
        m_receiveHitWithBoulderBehavior = createAiBehaviorAndCast<Ray_AIReceiveHitBehavior>(getTemplate()->getReceiveHitWithBoulderBehavior());
        m_closeRangeAttackWithBoulderBehavior = createAiBehavior(getTemplate()->getCloseRangeAttackWithBoulderBehavior());
        m_closeRangeBackAttackBehavior = createAiBehavior(getTemplate()->getCloseRangeBackAttackBehavior());
        m_closeRangeBackAttackWithBoulderBehavior = createAiBehavior(getTemplate()->getCloseRangeBackAttackWithBoulderBehavior());

        Super::onActorLoaded(_hotReload);

        if( m_physComponent )
        {
            m_physComponent->setStickOnWalls(btrue);
        }

        //get tween component and change smooth factor
        TweenComponent * const tweenComponent = m_actor->GetComponent<TweenComponent>();
        if( tweenComponent )
        {
            tweenComponent->setApplyCoordinates(bfalse);
        }

        //register to events
        ACTOR_REGISTER_EVENT_COMPONENT(GetActor(),ITF_GET_STRINGID_CRC(Ray_EventHunterBulletLaunched,1767723773), this);
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
        onActivate();
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::Update(f32 _dt)
    {
        Super::Update(_dt);

        updateBoulderList();
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if(const Ray_EventHunterBulletLaunched * const boulderLaunchEvent = _event->DynamicCast<Ray_EventHunterBulletLaunched>(ITF_GET_STRINGID_CRC(Ray_EventHunterBulletLaunched,1767723773)))
        {
            ITF_ASSERT(m_boulderList.find(boulderLaunchEvent->getBuller()) == U32_INVALID);
            m_boulderList.push_back(boulderLaunchEvent->getBuller());
        }
    }

    //-------------------------------------------------------
    bbool Ray_StoneManAIComponent::checkHealthDeath()
    {
        if( !isCurrentBehavior(m_receiveHitWithBoulderBehavior) )
        {
            return Super::checkHealthDeath();
        }
        else
        {
            return bfalse;
        }
    }

    //-------------------------------------------------------
    //  Boulders management
    //-------------------------------------------------------

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::updateBoulderList()
    {
        u32 index = 0;
        while(index < m_boulderList.size())
        {
            //erase boulder in following cases :
            bbool eraseBoulder = bfalse;
            BoulderRef & boulderRef = m_boulderList[index];

            if( boulderRef.m_ref.isValid() )
            {
                Actor * const boulder = boulderRef.m_ref.getActor();
                if( boulder )
                {
                    if( boulderRef.m_active && !boulder->isActive() )
                    {
                        //boulder is not active
                        eraseBoulder = btrue;
                        //in this case destroy boulder (garbage)
                        boulder->requestDestruction();
                    }
                    else
                    {
                        boulderRef.m_active = boulder->isActive();
                    }
                }
                else
                {
                    //actor is no more referenced (destroyed by someone else)
                    eraseBoulder = btrue;
                }
            }
            else
            {
                //ref is not valid
                eraseBoulder = btrue;
            }

            //iterate or erase
            if( !eraseBoulder )
            {
                ++index;
            }
            else
            {
                m_boulderList.eraseKeepOrder(index);
            }
        }
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::sendSympathyToBoulders()
    {
        for(u32 index = 0; index < m_boulderList.size(); ++index)
        {
            Actor * const boulder = m_boulderList[index].m_ref.getActor();
            if( boulder )
            {
                Ray_EventDyingStoneMan dyingEvent(GetActor()->getRef());
                boulder->onEvent(&dyingEvent);
            }
        }
        m_boulderList.clear();
    }

    //-------------------------------------------------------
    //  Behaviors management
    //-------------------------------------------------------

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if( isCurrentBehavior(m_receiveHitWithBoulderBehavior) )
        {
            onFinishedReceiveHitWithBoulder();
        }
        else if (isCurrentBehavior(m_closeRangeBackAttackBehavior))
        {
            onFinishedCloseRangeAttack();
        }
        else if( isCurrentBehavior(m_closeRangeAttackWithBoulderBehavior))
        {
            onFinishedCloseRangeAttack();
        }
        else if( isCurrentBehavior(m_closeRangeBackAttackWithBoulderBehavior))
        {
            onFinishedCloseRangeAttack();
        }
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::startReceiveHit(HitStim* _hit)
    {
        //if stone man owns boulder
        if( m_boulderPresence )
        {
            setStance(Stance_Fight);
            if( !isCurrentBehavior(m_receiveHitWithBoulderBehavior) )
            {
                setBehavior(m_receiveHitWithBoulderBehavior);
                m_receiveHitWithBoulderBehavior->setData(_hit);
            }
        }
        else
        {
            Super::startReceiveHit(_hit);
        }
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::onFinishedReceiveHitWithBoulder()
    {
        setBoulderPresence(bfalse);
        onFinishedReceiveHit();
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::onFinishedReceiveHit()
    {
        Super::onFinishedReceiveHit();
        sendSympathyToBoulders();
    }

    //-------------------------------------------------------
    void Ray_StoneManAIComponent::startCloseRangeAttack(const bbool _flip)
    {
        if( m_boulderPresence )
        {
            if( m_closeRangeBackAttackWithBoulderBehavior && _flip )
            {
                setBehavior(m_closeRangeBackAttackWithBoulderBehavior);
            }
            else if( m_closeRangeAttackWithBoulderBehavior )
            {
                setBehavior(m_closeRangeAttackWithBoulderBehavior);
            }
        }
        else
        {
            if( m_closeRangeBackAttackBehavior && _flip )
            {
                setBehavior(m_closeRangeBackAttackBehavior);
            }
            else
            {
                Super::startCloseRangeAttack(_flip);
            }
        }
    }



    //-------------------------------------------------------
    //  Ray_StoneManAIComponent_Template
    //-------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_StoneManAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_StoneManAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitWithBoulderBehaviorNew", m_receiveHitWithBoulderBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closeRangeAttackWithBoulderBehavior", m_closeRangeAttackWithBoulderBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closeRangeBackAttackBehavior", m_closeRangeBackAttackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closeRangeBackAttackWithBoulderBehavior", m_closeRangeBackAttackWithBoulderBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    //-------------------------------------------------------
    Ray_StoneManAIComponent_Template::Ray_StoneManAIComponent_Template()
    : m_receiveHitWithBoulderBehavior(NULL)
    , m_closeRangeAttackWithBoulderBehavior(NULL)
    , m_closeRangeBackAttackBehavior(NULL)
    , m_closeRangeBackAttackWithBoulderBehavior(NULL)
    {
    }

    //-------------------------------------------------------
    Ray_StoneManAIComponent_Template::~Ray_StoneManAIComponent_Template()
    {
        SF_DEL(m_receiveHitWithBoulderBehavior);
        SF_DEL(m_closeRangeAttackWithBoulderBehavior);
        SF_DEL(m_closeRangeBackAttackBehavior);
        SF_DEL(m_closeRangeBackAttackWithBoulderBehavior);
    }
} // namespace ITF
