#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FRIENDLYAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FriendlyAIComponent.h"
#endif //_ITF_RAY_FRIENDLYAICOMPONENT_H_

#ifndef _ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_GoToTargetBehavior.h"
#endif //_ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundFollowBehavior.h"
#endif //_ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_


#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardDanceBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_FriendlyAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_FriendlyAIComponent)
    END_SERIALIZATION()

    Ray_FriendlyAIComponent::Ray_FriendlyAIComponent()
        : Super()
        , m_waitBehavior(NULL)
        , m_triggerBehavior(NULL)
        , m_exitBehavior(NULL)
        , m_pedestalBehavior(NULL)
        , m_jumpBehavior(NULL)
        , m_gotoBehavior(NULL)
        , m_followBehavior(NULL)
        , m_hasNewOrder(bfalse)
        , m_hasNewGoto(bfalse)
        , m_hasCurrentOrder(bfalse)
        , m_hasCurrentGoto(bfalse)
        , m_currentOrderReached(bfalse)
        , m_startDance(bfalse)
        , m_currentOrderBehavior(NULL)
    {
    }

    Ray_FriendlyAIComponent::~Ray_FriendlyAIComponent()
    {
    }

    void Ray_FriendlyAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_waitBehavior = createAiBehavior(getTemplate()->getWaitBehavior());
        m_triggerBehavior = createAiBehavior(getTemplate()->getTriggerBehavior());
        m_exitBehavior = createAiBehavior(getTemplate()->getExitBehavior());
        m_pedestalBehavior = createAiBehavior(getTemplate()->getPedestalBehavior());
        m_jumpBehavior = createAiBehavior(getTemplate()->getJumpBehavior());
        m_gotoBehavior = createAiBehaviorAndCast<Ray_AIFriendly_GoToTargetBehavior>(getTemplate()->getGotoBehavior());
        //m_gotoBehavior = createAiBehavior(getTemplate()->getGotoBehavior());
        m_followBehavior = createAiBehaviorAndCast<Ray_AIGroundFollowBehavior>(getTemplate()->getFollowBehavior());
        m_danceBehavior = createAiBehaviorAndCast<Ray_AIRedWizardDanceBehavior>(getTemplate()->getDanceBehavior());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

        if ( m_danceBehavior )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventDance,83576490), this);
        }
    }

    void Ray_FriendlyAIComponent::onEvent( Event* _event )
    {
        if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
        {
            if ( m_currentBehavior == m_danceBehavior )
            {
                return;
            }
        }

        Super::onEvent(_event);

        if (m_currentBehavior == m_darktoonedBehavior)
        {
            return;
        }

        if ( Ray_EventAIOrder* _order = _event->DynamicCast<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852)) )
        {
            receiveOrder(_order);
        }
        else if ( Ray_EventDance* danceEvent = _event->DynamicCast<Ray_EventDance>(ITF_GET_STRINGID_CRC(Ray_EventDance,83576490)) )
        {
            receiveDance(danceEvent);
        }
    }

    void Ray_FriendlyAIComponent::Update( f32 _dt )
    {
        if (m_hasNewOrder)
        {
            gotoOrder(&m_currentOrder);
            m_hasNewOrder = bfalse;
        }

        if ( m_hasNewGoto )
        {
            gotoOrder(&m_gotoOrder);
            m_hasNewGoto = bfalse;
        }

        if ( m_startDance )
        {
            setBehavior(m_danceBehavior);
            m_startDance = bfalse;
            m_hasCurrentGoto = bfalse;
            m_hasCurrentOrder = bfalse;
        }

        Super::Update(_dt);
    }

    void Ray_FriendlyAIComponent::receiveOrder( const Ray_EventAIOrder* _order )
    {
        if ( m_hasCurrentGoto &&
             m_gotoOrder.getSender() != _order->getSender() )
        {
            return;
        }

        m_currentOrder = *_order;
        ITF_ASSERT(m_currentOrder.getSender().isValid());
        m_hasNewOrder = btrue;
        m_hasCurrentOrder = btrue;
        m_currentOrderReached = bfalse;
        m_currentOrderBehavior = NULL;
        m_hasCurrentGoto = bfalse;
    }

    void Ray_FriendlyAIComponent::gotoOrder( Ray_EventAIOrder* _order )
    {
        bbool reached = bfalse;

        if ( _order->getTargetReachedRadius() )
        {
            Actor* sender = AIUtils::getActor(_order->getSender());

            if ( sender )
            {
                Vec2d dist = m_actor->get2DPos() - sender->get2DPos();
                f32 distSq = dist.sqrnorm();

                if ( distSq <= _order->getTargetReachedRadius() )
                {
                    reached = btrue;
                }
            }
        }
        else
        {
            reached = btrue;
        }

        if ( reached )
        {
            onFinishedGoto();
        }
        else
        {
            startGoto(_order);
        }
    }

    void Ray_FriendlyAIComponent::receiveDance( Ray_EventDance* _eventDance )
    {
        if ( m_danceBehavior )
        {
            m_startDance = btrue;
            m_danceBehavior->onEvent(_eventDance);
        }
    }

    void Ray_FriendlyAIComponent::startOrder( Ray_EventAIOrder* _order )
    {
        ITF_ASSERT(_order->getSender().isValid());

        if ( _order->getId() == ITF_GET_STRINGID_CRC(Wait, 236836681) )
        {
            startWait(_order);
        }
        else if ( _order->getId() == ITF_GET_STRINGID_CRC(Trigger, 161061850) )
        {
            startTrigger(_order);
        }
        else if ( _order->getId() == ITF_GET_STRINGID_CRC(Exit, 98510012) )
        {
            startExit(_order);
        }
        else if ( _order->getId() == ITF_GET_STRINGID_CRC(Pedestal, 1212106600) )
        {
            startPedestal(_order);
        }
        else if ( _order->getId() == ITF_GET_STRINGID_CRC(Jump, 2374255179) )
        {
            startJump(_order);
        }
        else if ( _order->getId() == ITF_GET_STRINGID_CRC(GoTo, 2547998994) )
        {
            if ( Actor* current = AIUtils::getActor(_order->getSender()) )
            {
                Ray_EventAIQueryNextOrder query;
                current->onEvent(&query);

                if (query.getHasNextOrder())
                {
                    m_gotoOrder = query.getNextOrder();
                    ITF_ASSERT(m_gotoOrder.getSender().isValid());
                    m_hasNewGoto = btrue;
                    m_currentOrderReached = bfalse;
                    m_currentOrderBehavior = NULL;
                    m_hasCurrentGoto = btrue;
                }
            }
        }
    }

    void Ray_FriendlyAIComponent::onFinishedOrder()
    {
        m_hasCurrentOrder = bfalse;
        m_currentOrderReached = bfalse;
        m_currentOrderBehavior = NULL;

        // find next order, otherwise roam
        if ( Actor* current = AIUtils::getActor(m_currentOrder.getSender()) )
        {
            Ray_EventAIQueryNextOrder query;
            current->onEvent(&query);

            if (query.getHasNextOrder())
            {
                m_gotoOrder = query.getNextOrder();
                ITF_ASSERT(m_gotoOrder.getSender().isValid());
                m_hasNewGoto = btrue;
                m_currentOrderReached = bfalse;
                m_currentOrderBehavior = NULL;
                m_hasCurrentGoto = btrue;
            }
            else
            {
                startRoam();
            }
        }
        else
        {
            startRoam();
        }
    }

    void Ray_FriendlyAIComponent::onBehaviorFinished()
    {
        if ( m_currentBehavior == m_followBehavior )
        {
            onFinishedFollow();
        }
        else if ( m_currentBehavior == m_gotoBehavior )
        {
            // order reached, process it
            ITF_ASSERT(m_hasCurrentOrder);
            onFinishedGoto();
        }
        else if (m_hasCurrentOrder)
        {
            if ( m_currentBehavior != m_currentOrderBehavior )
            {
                // we most likely got interrupted

                if ( m_currentOrderReached )
                {
                    // resume current order
                    startOrder(&m_currentOrder);
                    //setBehavior(m_currentOrderBehavior);
                }
                else
                {
                    // go to order
                    gotoOrder(&m_currentOrder);
                }

                Super::onBehaviorFinished();
            }
            else
            {
                // current order finished

                if ( m_currentBehavior == m_waitBehavior )
                {
                    onFinishedWait();
                }
                else if ( m_currentBehavior == m_triggerBehavior )
                {
                    onFinishedTrigger();
                }
                else if ( m_currentBehavior == m_exitBehavior )
                {
                    onFinishedExit();
                }
                else if ( m_currentBehavior == m_pedestalBehavior )
                {
                    onFinishedPedestal();
                }
                else if ( m_currentBehavior == m_jumpBehavior )
                {
                    onFinishedJump();
                }
            }
        }
        else if ( m_hasCurrentGoto )
        {
            gotoOrder(&m_gotoOrder);
        }
        else
        {
            Super::onBehaviorFinished();
        }
    }


    void Ray_FriendlyAIComponent::updateRoam( f32 _dt )
    {
        //Super::updateRoam(_dt);

        if (m_followBehavior)
        {
            ActorList players;
            AIUtils::getPlayers(m_actor->getDepth(), players);

            Actor* closestPlayer = AIUtils::getClosestActor( players,
                m_actor, m_actor->isFlipped(),
                getTemplate()->getFollowDetectDistance() );
            if (closestPlayer)
            {
                startFollow(closestPlayer->getRef());
            }
        }
    }


    void Ray_FriendlyAIComponent::startFollow( ObjectRef _target )
    {
        setStance(Stance_Normal);
        m_followBehavior->setTarget(_target);
        setBehavior(m_followBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedFollow()
    {
        startRoam();
    }


    void Ray_FriendlyAIComponent::startGoto( Event* _event )
    {
        setStance(Stance_Normal);
        m_gotoBehavior->setData(_event);
        m_gotoBehavior->setTryReachPosition(btrue);
        setBehavior(m_gotoBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedGoto()
    {
        m_currentOrderReached = btrue;

        if ( m_hasCurrentOrder )
        {
            startOrder(&m_currentOrder);

            if (m_currentOrderReached)
            {
                m_currentOrderBehavior = m_currentBehavior;
            }
        }
    }



    void Ray_FriendlyAIComponent::startWait( Event* _event )
    {
        setStance(Stance_Normal);
        m_waitBehavior->setData(_event);
        setBehavior(m_waitBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedWait()
    {
        onFinishedOrder();
    }


    void Ray_FriendlyAIComponent::startTrigger( Event* _event )
    {
        setStance(Stance_Normal);
        m_triggerBehavior->setData(_event);
        setBehavior(m_triggerBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedTrigger()
    {
        onFinishedOrder();
    }


    void Ray_FriendlyAIComponent::startExit( Event* _event )
    {
        setStance(Stance_Normal);
        m_exitBehavior->setData(_event);
        setBehavior(m_exitBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedExit()
    {
        m_actor->disable();
    }


    void Ray_FriendlyAIComponent::startPedestal( Event* _event )
    {
        setStance(Stance_Normal);
        m_pedestalBehavior->setData(_event);
        setBehavior(m_pedestalBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedPedestal()
    {
        onFinishedOrder();
    }


    void Ray_FriendlyAIComponent::startJump( Event* _event )
    {
        if (m_currentBehavior == m_jumpBehavior)
        {
            // don't use restart flag or we'll lose the setData
            setNullBehavior();
        }

        setStance(Stance_Normal);
        m_jumpBehavior->setData(_event);
        setBehavior(m_jumpBehavior);
    }

    void Ray_FriendlyAIComponent::onFinishedJump()
    {
        onFinishedOrder();
    }


    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_FriendlyAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_FriendlyAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("waitBehavior",m_waitBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("triggerBehavior",m_triggerBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("exitBehavior",m_exitBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("pedestalBehavior",m_pedestalBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("jumpBehavior",m_jumpBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("gotoBehavior",m_gotoBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("followBehavior",m_followBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("danceBehavior",m_danceBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("followDetectDistance", m_followDetectDistance);
    END_SERIALIZATION()

    Ray_FriendlyAIComponent_Template::Ray_FriendlyAIComponent_Template()
        : Super()
        , m_waitBehavior(NULL)
        , m_triggerBehavior(NULL)
        , m_exitBehavior(NULL)
        , m_pedestalBehavior(NULL)
        , m_jumpBehavior(NULL)
        , m_gotoBehavior(NULL)
        , m_followBehavior(NULL)
        , m_danceBehavior(NULL)
        , m_followDetectDistance(5.f)
    {
    }

    Ray_FriendlyAIComponent_Template::~Ray_FriendlyAIComponent_Template()
    {
        SF_DEL(m_waitBehavior);
        SF_DEL(m_triggerBehavior);
        SF_DEL(m_exitBehavior);
        SF_DEL(m_pedestalBehavior);
        SF_DEL(m_jumpBehavior);
        SF_DEL(m_gotoBehavior);
        SF_DEL(m_followBehavior);
        SF_DEL(m_danceBehavior);
    }
};