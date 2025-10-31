#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_REWARDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_RewardAIComponent.h"
#endif //_ITF_RAY_REWARDAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_TRAIL3DCOMPONENT_H_
#include "gameplay/components/misc/Trail3DComponent.h"
#endif //_ITF_TRAIL3DCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_RewardAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_RewardAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_RewardAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_playerDetectorComponent,     "Ray_RewardAIComponent requires an PlayerDetectorComponent.");
        VALIDATE_COMPONENT_PARAM("", m_triggerComponent,            "Ray_RewardAIComponent requires an TriggerComponent.");
        VALIDATE_COMPONENT_PARAM("", m_idleBehavior,                "Ray_RewardAIComponent requires an idleBehavior.");
        VALIDATE_COMPONENT_PARAM("", m_pickupBehavior,              "Ray_RewardAIComponent requires an pickupBehavior.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_RewardAIComponent::Ray_RewardAIComponent()
    : m_snapPlayer(NULL)
    , m_idleBehavior(NULL)
    , m_carriedBehavior(NULL)
    , m_pickupBehavior(NULL)
    , m_snapBehavior(NULL)
    , m_fallbackReward(NULL)
    , m_physComponent(NULL)
    , m_playerDetectorComponent(NULL)
    , m_triggerComponent(NULL)
    , m_trailComponent(NULL)
    {
        // none
    }

    //*****************************************************************************

    Ray_RewardAIComponent::~Ray_RewardAIComponent()
    {
        SF_DEL(m_fallbackReward);
    }

    //*****************************************************************************

    void Ray_RewardAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_idleBehavior      = createAiBehavior(getTemplate()->getIdleBehavior());
        m_carriedBehavior   = createAiBehavior(getTemplate()->getCarriedBehavior());
        m_pickupBehavior    = createAiBehavior(getTemplate()->getPickupBehavior());
        m_snapBehavior      = createAiBehavior(getTemplate()->getSnapBehavior());

        Super::onActorLoaded(_hotReload);

        // Components
        m_physComponent             = m_actor->GetComponent<StickToPolylinePhysComponent>();
        m_playerDetectorComponent   = m_actor->GetComponent<PlayerDetectorComponent>();
        m_triggerComponent          = m_actor->GetComponent<TriggerComponent>();
        m_trailComponent            = m_actor->GetComponent<Trail3DComponent>();
        
        // clone fallback reward event
        if (getTemplate()->getFallbackReward())
        {
            m_fallbackReward = getTemplate()->getFallbackReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getFallbackReward()), m_fallbackReward);
        }

        // starts with trail disabled
        if (m_trailComponent)
        {
            EventTrail hideTrail(bfalse);
            m_trailComponent->onEvent(&hideTrail);
        }

        // Register Events
        if (m_carriedBehavior)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHunterBirdBind,135728216), this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHunterBirdUnbind,1029386227), this);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225), this);


        // Init behavior
        setBehavior(m_idleBehavior);

    }

    //*****************************************************************************

    void Ray_RewardAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    //*****************************************************************************

    void Ray_RewardAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        // TODO : rename/convert this Ray_EventHunterBirdBind/Unbind to and generic Ray_EventBind/Unbind
        if (Ray_EventHunterBirdBind* eventBind  = _event->DynamicCast<Ray_EventHunterBirdBind>(ITF_GET_STRINGID_CRC(Ray_EventHunterBirdBind,135728216)))
        {
            onCarried();
        }
        else if(Ray_EventHunterBirdUnbind* eventUnbind = _event->DynamicCast<Ray_EventHunterBirdUnbind>(ITF_GET_STRINGID_CRC(Ray_EventHunterBirdUnbind,1029386227)))
        {
            onDropped();
        }
        else if (EventQueryBlocksHits* blockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
        {
            bbool canBlockHit;
            if (m_currentBehavior == m_carriedBehavior)
            {
                canBlockHit = bfalse;
            }
            else
            {
                canBlockHit = btrue;
            }

            blockHits->setCanBlockHits(canBlockHit);
        }
        else if (EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (isCurrentBehavior(m_idleBehavior))
            {
                snapOrPickup(trigger->getActivator());
            }
            else if (isCurrentBehavior(m_snapBehavior))
            {
                if (m_snapPlayer->getActor() &&
                    (m_snapPlayer->getActor()->getRef() == trigger->getActivator()))
                {
                    pickup(trigger->getActivator());      
                }
            }
        }
        else if (Ray_EventRewardPickedUp* pickUp = _event->DynamicCast<Ray_EventRewardPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225)))
        {
            if (isCurrentBehavior(m_idleBehavior))
            {
                snapOrPickup(pickUp->getActivator());
            }
        }
    }

    //*****************************************************************************
    void Ray_RewardAIComponent::snapOrPickup(ActorRef _playerRef)
    {
        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(_playerRef, bfalse));
        if (!player)
        {
            return;
        }
        
        Actor * actor = AIUtils::getActor( _playerRef );
        if ( actor == NULL )
        {
            return;
        }

        f32 playerSqrDist = ( actor->get2DPos() - m_actor->get2DPos() ).sqrnorm();
        m_snapPlayer = player;

        // heart not needed? snap heart to the closest player in need
        if ( getTemplate()->getAddHp() && m_snapBehavior && 
            ( ( !player->heartNeeded() && findSnapPlayer() ) || ( player->heartNeeded() && ( playerSqrDist > ( getTemplate()->getSnapMinDistance() * getTemplate()->getSnapMinDistance() ) ) ) )
           )
        {
            if (m_physComponent)
            {
                m_physComponent->setDisabled(btrue);
            }
            if (m_trailComponent)
            {
                EventTrail showTrail(btrue);
                m_trailComponent->onEvent(&showTrail);
            }
            setBehavior(m_snapBehavior);
        }
        else
        {
            pickup(_playerRef);
        }
    }
    
    //*****************************************************************************

    Vec2d Ray_RewardAIComponent::getCameraDirection()
    {
        // get camera modifier direction *at the chest position* if set, or (1, 0)
        CameraControllerManager* camControllerMgr = CAMERACONTROLLERMANAGER;
        if (!camControllerMgr)
        {
            return Vec2d::XAxis;
        }
        const ICameraController* camController = camControllerMgr->getMainCameraController();
        if (!camController)
        {
            return Vec2d::XAxis;
        }            
        const CameraModifierComponent* camModifier = camController->getCameraModifier();
        if (!camModifier)
        {
            return Vec2d::XAxis;
        }
        const CamModifierUpdate& updateData = camModifier->getUpdateData();
        Vec2d direction = updateData.getDirection();
        if (direction == Vec2d::Zero)
        {
            return Vec2d::XAxis;
        }
        direction.normalize();
        return direction;
    }
    
    //*****************************************************************************

    bbool Ray_RewardAIComponent::findSnapPlayer()
    {
        if ( ( m_snapPlayer != NULL ) && m_snapPlayer->heartNeeded() )
        {
            return btrue;
        }

        // get furthest player needing a heart
        f32 maxDot = -F32_INFINITY;
        //m_snapPlayer = NULL;
        Ray_Player * player = NULL;

        for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* otherPlayer = static_cast<Ray_Player*>(RAY_GAMEMANAGER->getPlayer(i));
            if (!otherPlayer)
            {
                continue;
            }
                
            // ignore if no heart needed
            if (!otherPlayer->heartNeeded())
            {
                continue;
            }

            // ignore missing player actor
            Actor* otherPlayerActor = otherPlayer->getActor();
            if (!otherPlayerActor)
            {
                continue;
            }
                
            // ignore dead players
            EventQueryIsDead query;
            otherPlayerActor->onEvent(&query);
            if (query.getIsDead())
            {
                continue;
            }

            // distance in camera space = dot product with camera direction
            Vec2d playerToMe = m_actor->get2DPos() - otherPlayerActor->get2DPos();
            f32 dot = getCameraDirection().dot(playerToMe);
            if (dot >= maxDot)
            {
                maxDot = dot;
                player = otherPlayer;
            }
        }

        if ( player != m_snapPlayer )
        {
            m_snapPlayer = player;
        }

        return (m_snapPlayer != NULL);
    }

    //*****************************************************************************

    void Ray_RewardAIComponent::onCarried()
    {
        if (m_physComponent)
        {
            m_physComponent->setDisabled(btrue);
        }
        setBehavior(m_carriedBehavior);        
    }

    //*****************************************************************************

    void Ray_RewardAIComponent::onDropped()
    {
        if (m_physComponent)
        {
            m_physComponent->setDisabled(bfalse);
        }
        setBehavior(m_idleBehavior);
    }


    //*****************************************************************************

    void Ray_RewardAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);
        
        if (isCurrentBehavior(m_snapBehavior))
        {
            // no player in need? back to idle
            if (!findSnapPlayer())
            {
                if (m_physComponent)
                {
                    m_physComponent->setDisabled(bfalse);
                }
                if (m_trailComponent)
                {
                    EventTrail hideTrail(bfalse);
                    m_trailComponent->onEvent(&hideTrail);
                }
                setBehavior(m_idleBehavior);
            }
            // otherwise snap to player
            else if (Actor* snapPlayerActor = m_snapPlayer->getActor())
            {
                Vec3d newPos = m_actor->getPos();
                f32 snapAddFactor = 1.0f;
                if ( getTemplate()->getSnapMinDistance() != 0.0f )
                {
                    snapAddFactor = f32_Min( ( snapPlayerActor->get2DPos() - m_actor->get2DPos() ).norm(), getTemplate()->getSnapMinDistance() ) / getTemplate()->getSnapMinDistance();
                    snapAddFactor = ( 1.0f - snapAddFactor ) * 2.0f;
                }

                AIUtils::michelSmooth(
                    newPos,
                    snapPlayerActor->getPos(),
                    ( getTemplate()->getSnapFactor() + snapAddFactor )
                    );
                m_actor->setPos(newPos);
            }
            // hu? actor's gone?
            else
            {
                ITF_ASSERT_MSG(bfalse, "Can't retrieve snap player actor");
            }
        }
    }

    //*****************************************************************************

    void Ray_RewardAIComponent::pickup(ActorRef _playerRef)
    {
        if (_playerRef.isValid())
        {
            // CASA::CTR::SFS : Add For Arena
            EventPickRewardNotification eventPickRewardNotification(_playerRef);
            EVENTMANAGER->broadcastEvent(&eventPickRewardNotification);
            // CASA>


            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(_playerRef));

            if (player)
            {
                if (getTemplate()->getAddHp())
                {
                    bbool hasDiamondHeart = (player->getHeartTier() == HeartTier_Diamond);
                    bbool hasRedHeartNeedUpgrade = (RAY_GAMEMANAGER->getHealthModifier() == HealthModifier_GoldenHeart && player->getHeartTier() == HeartTier_Red);

                    if ((hasDiamondHeart || (!player->heartNeeded() && !hasRedHeartNeedUpgrade)) && m_fallbackReward)
                    {
                        m_fallbackReward->clear();
                        m_fallbackReward->setup(m_actor);
                        m_fallbackReward->setRewardReceiver(_playerRef);
                        GAMEMANAGER->onEvent(m_fallbackReward);
                    }
                    else
                    {
                        player->addHitPoints(getTemplate()->getAddHp());
                    }
                }

                if (getTemplate()->getAddMaxHp())
                {
                    player->addMaxHitPoints(getTemplate()->getAddMaxHp());
                }
            }
        }

        setBehavior(m_pickupBehavior);

    }


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_RewardAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_RewardAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idleBehavior",       m_idleBehavior,     ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("carriedBehavior",    m_carriedBehavior,  ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("pickupBehavior",     m_pickupBehavior,   ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("snapBehavior",       m_snapBehavior,     ACTORSMANAGER->getAIBehaviorsFactory());

        SERIALIZE_OBJECT_WITH_FACTORY("fallbackReward",     m_fallbackReward,   ACTORSMANAGER->getEventFactory());

        SERIALIZE_MEMBER("addHp",       m_addHp);
        SERIALIZE_MEMBER("addMaxHp",    m_addMaxHp);
        
        SERIALIZE_MEMBER("snapFactor",      m_snapFactor);
        SERIALIZE_MEMBER("snapMinDistance", m_snapMinDistance);
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_RewardAIComponent_Template::Ray_RewardAIComponent_Template()
    : m_idleBehavior(NULL)
    , m_carriedBehavior(NULL)
    , m_pickupBehavior(NULL)
    , m_snapBehavior(NULL)
    , m_fallbackReward(NULL)
    , m_addHp(0)
    , m_addMaxHp(0)
    , m_snapFactor(1.0f)
    , m_snapMinDistance( 3.0f )
    {
        // none
    }

    //*****************************************************************************

    Ray_RewardAIComponent_Template::~Ray_RewardAIComponent_Template()
    {
        SF_DEL(m_fallbackReward);
        SF_DEL(m_idleBehavior);
        SF_DEL(m_carriedBehavior);
        SF_DEL(m_pickupBehavior);
        SF_DEL(m_snapBehavior);
    }

    //*****************************************************************************


}
