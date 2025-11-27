#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIFRUITCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FRUitAIComponent.h"
#endif //_ITF_RAY_AIFRUITCOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRunAwayBehavior.h"
#endif //_ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_AISLEEPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AISleepBehavior.h"
#endif //_ITF_RAY_AISLEEPBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Fruit/Ray_AIFruitRoamingBehavior.h"
#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_FruitAIComponent)
        BEGIN_SERIALIZATION_CHILD(Ray_FruitAIComponent)
        SERIALIZE_MEMBER("firstBounceDirection", m_firstBounceDirection);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_FruitAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_physComponent, "Ray_FruitAIComponent requires a StickToPolylinePhysComponent");
        VALIDATE_COMPONENT_PARAM("", m_phantomComponent, "Ray_FruitAIComponent requires a PhantomComponent");
    END_VALIDATE_COMPONENT()


    //************************************
    // Method:    Ray_FruitAIComponent
    // FullName:  ITF::Ray_FruitAIComponent::Ray_FruitAIComponent
    // Access:    public 
    // Returns:   
    // Qualifier: : Super() , m_roamBehavior(NULL) , m_receiveHitBehavior(NULL) , m_sleepBehavior(NULL) , m_physComponent(NULL) , m_polyLineActive(bfalse)
    //************************************
    Ray_FruitAIComponent::Ray_FruitAIComponent() 
        : Super()
        , m_roamBehavior(NULL)
        , m_floatingBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_sleepBehavior(NULL)
        , m_stuckBehavior(NULL)
        , m_snappedBehavior(NULL)

        , m_physComponent(NULL)
        , m_phantomComponent(NULL)
        , m_polyLineComponent(NULL)

        , m_isStuck(bfalse)
        , m_polyLineActive(bfalse)
        , m_timer(1.0f)
        , m_snapBone("b_snapon")
        , m_snapBoneIndex(U32_INVALID)
        , m_snappedCharacterPreviousLookRight(btrue)
        , m_snappedCharacterLookRight(btrue)
        , m_stuckPolyline(ITF_INVALID_OBJREF)
        , m_stuckActor(ITF_INVALID_OBJREF)
        , m_stuckEdge(U32_INVALID)
        , m_stuckLocalPos(Vec2d::Zero)
        , m_firstBounceDirection(0)
        , m_shooterHitTimer( 0.0f )
        , m_wasTriggered(bfalse)
    {
    }

    //************************************
    // Method:    ~Ray_FruitAIComponent
    // FullName:  ITF::Ray_FruitAIComponent::~Ray_FruitAIComponent
    // Access:    public 
    // Returns:   
    // Qualifier:
    //************************************
    Ray_FruitAIComponent::~Ray_FruitAIComponent()
    {
        clear();
    }

    //************************************
    // Method:    clear
    // FullName:  ITF::Ray_FruitAIComponent::clear
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::clear()
    {
        m_snappedCharacter.invalidate();
        m_snapBoneIndex = U32_INVALID;
        m_actorsOnPolyline.clear();
        m_actorsHangingOnPolyline.clear();
        m_attachedActorsMemory.clear();        
    }

    //************************************
    // Method:    onActorLoaded
    // FullName:  ITF::Ray_FruitAIComponent::onActorLoaded
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_roamBehavior = static_cast<Ray_AIFruitRoamingBehavior *>(createAiBehavior(getTemplate()->getRoamBehavior()));
        m_floatingBehavior = static_cast<Ray_AIWaterBaseBehavior *>(createAiBehavior(getTemplate()->getFloatingBehavior()));
        m_receiveHitBehavior = static_cast<Ray_AIReceiveHitBehavior *>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));
        m_deathBehavior = createAiBehavior(getTemplate()->getDeathBehavior());
        m_sleepBehavior = static_cast<Ray_AISleepBehavior *>(createAiBehavior(getTemplate()->getSleepBehavior()));
        m_stuckBehavior = createAiBehavior(getTemplate()->getStuckBehavior());
        m_snappedBehavior = createAiBehavior(getTemplate()->getSnappedBehavior());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventReleaseChild,251461974), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventUturn,121408696), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601), this);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated
        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();             // mandatory, validated
        m_polyLineComponent = m_actor->GetComponent<PolylineComponent>();             // mandatory, validated

        m_physComponent->setUpdateRotation(bfalse);
        activatePolyline();
        m_wasTriggered = bfalse;
        resetState();
    }

    void Ray_FruitAIComponent::onResourceReady()
    {
        Super::onResourceReady();
        if( m_animatedComponent )
        {
            m_snapBoneIndex = m_animatedComponent->getBoneIndex(m_snapBone);
        }
        if( m_animComponent )
        {
            m_snapBoneIndex = m_animComponent->getBoneIndex(m_snapBone);
        }
    }

    //************************************
    // Method:    onCheckpointLoaded
    // FullName:  ITF::Ray_FruitAIComponent::postLoadCheckpoint
    // Access:    private 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        bbool wasTriggered = m_wasTriggered;         //This is a work around to correct tech design change for triggers and checkpoints
        resetState();
        if (wasTriggered)
        {
            leaveSleep();
        }
        m_wasTriggered = bfalse; //should be useless, be we could change in resetState
    }


    //************************************
    // Method:    Update
    // FullName:  ITF::Ray_FruitAIComponent::Update
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: f32 _dt
    //************************************
    void Ray_FruitAIComponent::Update( f32 _dt )
    {
        m_timer = Max(0.0f,m_timer - _dt);
        m_shooterHitTimer = Max( 0.0f, m_shooterHitTimer - _dt );

        updateMemory(_dt);

        updateCheckDeath();

        if ( m_currentBehavior == m_roamBehavior )
        {
            updateRoam(_dt);
        }
        else if ( m_currentBehavior == m_receiveHitBehavior )
        {
            updateReceiveHit(_dt);
            // nothing to do ATM...
        }
        else if (m_sleepBehavior && m_currentBehavior == m_sleepBehavior)
        {
            // nothing to do ATM...
        }
        else if (m_stuckBehavior && m_currentBehavior == m_stuckBehavior)
        {
            updateStuck(_dt);
        }
        else if (m_currentBehavior == m_snappedBehavior)
        {
            Actor * actor = m_snappedCharacter.getActor();
            if (actor)
            {
                if (actor->isEnabled())
                {
                    EventQuerySnapData querySnapData;
                    actor->onEvent(&querySnapData);
                    Vec3d rootPos = Vec3d::Zero;
                    Vec3d snapPos = Vec3d::Zero;
                    if( m_animatedComponent )
                    {
                        m_animatedComponent->getRoot(rootPos);
                        m_animatedComponent->getBonePos(m_snapBoneIndex,snapPos); 
                    }
                    if( m_animComponent )
                    {
                        rootPos = m_animComponent->getRootDelta().to3d();
                        if( m_snapBoneIndex != U32_INVALID )
                        {
                            m_animComponent->getBonePos(m_snapBoneIndex,snapPos); 
                        }
                    }
                    m_actor->setPos(querySnapData.getPos() + rootPos - snapPos);
                    if (m_snappedCharacterPreviousLookRight !=querySnapData.getLookRight())
                    {
                        m_snappedCharacterLookRight = querySnapData.getLookRight();

                    }
                    m_snappedCharacterPreviousLookRight = querySnapData.getLookRight();
                    m_actor->setAngle(querySnapData.getLookRight()?querySnapData.getAngle():MTH_PI+querySnapData.getAngle());
                }
                else
                {
                    resetState();
                }
            }
            else
            {
                resetState();
            }
        }
        else if ( m_currentBehavior == m_floatingBehavior )
        {
            updateSwim(_dt);
        }
        else if ( m_currentBehavior == m_deathBehavior)
        {
        }
        else
        {
            ITF_ASSERT_MSG( m_currentBehavior, "invalid behavior!");
            //ITF_ASSERT_MSG(0, "Unhandled behavior!");     // removed by slaigle for shooter : some other bhv can be launched on the actor without any ai management.
        }


        Super::Update(_dt);
        processDelayedChangesFromBehaviors();
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_FruitAIComponent::isInDeathCondition() const
    {
        PolyLine* deathPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Death,m_actor->get2DPos());
        if ( deathPoly )
        {
            return btrue;
        }
        if (Ray_AIUtils::getSquashingPolyline(m_actor,m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdge(),m_physComponent->getRadius(),getTemplate()->getSquashPenetrationRadiusPercent() * m_physComponent->getRadius()))
        {
            return btrue;
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_FruitAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if (m_currentBehavior == m_roamBehavior ||
            m_currentBehavior == m_floatingBehavior ||
           (m_stuckBehavior && m_currentBehavior == m_stuckBehavior) )
        {
            if (getTemplate()->getReinitWhenBecomesInactive())
            {
                resetState();
            }
        }

        m_attachedActorsMemory.clear();
    }

    void Ray_FruitAIComponent::resetState()
    {
        m_actor->setPos(m_actor->getWorldInitialPos());
        m_actor->setAngle(m_actor->getWorldInitialRot());
        m_physComponent->unstick();
        if (m_roamBehavior)
        {
            m_roamBehavior->setFirstBounceDirection(m_firstBounceDirection);
        }

        setNullBehavior();  // make sure we restart in case we were already roaming
        if (m_sleepBehavior)
        {
            startSleep();
        }
        else
        {
            startRoam();
        }

    }

    //************************************
    // Method:    updateRoam
    // FullName:  ITF::Ray_FruitAIComponent::updateRoam
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: f32 _dt
    //************************************
    void Ray_FruitAIComponent::updateRoam( f32 _dt )
    {
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());

        if ( waterPoly )
        {
            startFloating(waterPoly->getRef());
            return;
        }
    }


    //************************************
    // Method:    updateReceiveHit
    // FullName:  ITF::Ray_FruitAIComponent::updateReceiveHit
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: f32 _dt
    //************************************
    void Ray_FruitAIComponent::updateReceiveHit( f32 _dt )
    {
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());

        if ( waterPoly )
        {
            startFloating(waterPoly->getRef());
            return;
        }
    }

    void Ray_FruitAIComponent::updateSwim( f32 _dt )
    {
        if ( m_floatingBehavior->getDistanceFromWater() > 0.2f )
        {
            startRoam();
        }
        else
        {
            f32 scaleX = m_actor->getLocalInitialScale().m_x;
            m_actor->setScale((m_actor->getScale()+Vec2d(scaleX,scaleX))*0.5f);
        }
    }

    void Ray_FruitAIComponent::updateCheckDeath()
    {
        if ((m_currentBehavior!=m_deathBehavior) && (m_deathBehavior!=NULL))
        {
            if (isInDeathCondition())
            {
                startDeath();                
            }
        }
    }

    void Ray_FruitAIComponent::startDeath()
    {
        setHealth(0);
        setBehavior(m_deathBehavior);
    }

    void Ray_FruitAIComponent::startSnapped()
    {
        m_physComponent->setDisabled(btrue);
        setBehavior(m_snappedBehavior);
    }

    void Ray_FruitAIComponent::startFloating( ObjectRef _ref )
    {
        if ( m_floatingBehavior )
        {
            m_floatingBehavior->setSwimingPolyline(_ref);
            setBehavior(m_floatingBehavior);
        }
    }

    //************************************
    // Method:    onBehaviorFinished
    // FullName:  ITF::Ray_FruitAIComponent::onBehaviorFinished
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if ( m_currentBehavior == m_receiveHitBehavior )
        {
            onFinishedReceiveHit();
        }
        else if ( m_currentBehavior == m_sleepBehavior )
        {
            startRoam();
        }
        else if (m_currentBehavior == m_snappedBehavior)
        {
            m_snappedCharacter.invalidate();
            resetState();
        }
        else if (m_currentBehavior == m_floatingBehavior)
        {
        }
        else if (m_currentBehavior == m_roamBehavior)
        {
        }
        else if (m_currentBehavior == m_stuckBehavior)
        {
        }
        else if (m_currentBehavior == m_deathBehavior && m_deathBehavior!=NULL)
        {
        }

        ITF_ASSERT_MSG(m_currentBehavior,"Bhavior should never be null");
    }

    //************************************
    // Method:    onFinishedReceiveHit
    // FullName:  ITF::Ray_FruitAIComponent::onFinishedReceiveHit
    // Access:    private 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::onFinishedReceiveHit()
    {
        startRoam();
    }

    //************************************
    // Method:    startRoam
    // FullName:  ITF::Ray_FruitAIComponent::startRoam
    // Access:    private 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::startRoam(bbool _restart)
    {

        setBehavior(m_roamBehavior, _restart);
        activatePolyline();
    }

    //************************************
    // Method:    startReceiveHit
    // FullName:  ITF::Ray_FruitAIComponent::startReceiveHit
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: HitStim * _hit
    //************************************
    void Ray_FruitAIComponent::startReceiveHit( HitStim* _hit )
    {
        setBehavior(m_receiveHitBehavior);
        m_receiveHitBehavior->setData(_hit);
    }

    //************************************
    // Method:    startSleep
    // FullName:  ITF::Ray_FruitAIComponent::startSleep
    // Access:    private 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::startSleep()
    {
        setBehavior(m_sleepBehavior);
        stick();
        deactivatePolyline();
    }

    //************************************
    // @name	ITF::Ray_FruitAIComponent::stick
    // @private 
    // @return	void
    //************************************
    void Ray_FruitAIComponent::stick()
    {
        unstick();

        if (!m_isStuck)
        {
            m_isStuck = btrue;
            m_physComponent->setSpeed(Vec2d::Zero);
            m_physComponent->resetForces();
            m_physComponent->setDisabled(btrue,bfalse, btrue);
        }
    }

    //************************************
    // @name	ITF::Ray_FruitAIComponent::unstick
    // @private 
    // @return	void
    //************************************
    void Ray_FruitAIComponent::unstick()
    {
        if (m_isStuck)
        {
            m_physComponent->setDisabled(bfalse,bfalse, btrue);
            m_isStuck = bfalse;

            if ( m_stuckPolyline.isValid())
            {
                PolyLine* poly;
                const PolyLineEdge* stickedEdge;

                AIUtils::getPolyLine(m_stuckPolyline,m_stuckEdge,poly,stickedEdge);

                if ( poly )
                {
                    if ( poly->getOwnerActor() )
                    {
                        EventStickOnPolyline stickEvent(poly->getRef(),m_actor->getRef(),m_stuckEdge,1.f,
                                                        m_actor->get2DPos(),Vec2d::Zero,bfalse);

                        poly->getOwnerActor()->onEvent(&stickEvent);
                        poly->getOwnerActor()->getBinding()->unbindChild(m_actor->getRef());
                    }
                    else
                    {
                        poly->getBinding()->unbindChild(m_actor->getRef());
                    }
                }
            }
            else if ( m_stuckActor != ITF_INVALID_OBJREF )
            {
                Actor* stuckActor = AIUtils::getActor(m_stuckActor);

                if ( stuckActor )
                {
                    Ray_EventFruitDetach detachEvent(m_actor->getRef());
                    stuckActor->onEvent(&detachEvent);
                    stuckActor->getBinding()->unbindChild(m_actor->getRef());
                }
            }

            m_stuckPolyline.invalidate();
            m_stuckEdge = U32_INVALID;
            m_stuckLocalPos = Vec2d::Zero;
            m_stuckActor.invalidate();
            m_timer = getTemplate()->getNonStickableTime();
        }
    }

    void Ray_FruitAIComponent::leaveSleep()
    {
        if (m_currentBehavior && (m_currentBehavior == m_sleepBehavior))
        {
            unstick();
            if (m_roamBehavior)
            {
                m_roamBehavior->setFirstBounceDirection(m_firstBounceDirection);
            }
            startRoam(bfalse);
        }
        m_wasTriggered = bfalse;
    }
    void Ray_FruitAIComponent::processTrigger(EventTrigger *_trigger)
    {
        m_wasTriggered = btrue;
        leaveSleep();
    }

    //************************************
    // Method:    onEvent
    // FullName:  ITF::Ray_FruitAIComponent::onEvent
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: Event * _event
    //************************************
    void Ray_FruitAIComponent::onEvent( Event* _event )
    {

        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            processTrigger(trigger);
        }
        else if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processHit(hit);
        }
        else if ( Ray_EventQueryCanBeAttacked* query = _event->DynamicCast<Ray_EventQueryCanBeAttacked>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644)) )
        {
            processQueryCanBeAttacked(query);
        }
        else if ( EventQueryBlocksHits* queryBlocksHit = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)) )
        {
            processQueryBlocksHits(queryBlocksHit);
        }
        else if ( Ray_EventQueryCanDarktoonify* queryCanDarktoonify = _event->DynamicCast<Ray_EventQueryCanDarktoonify>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444)) )
        {   
            processQueryCanDarktoonify(queryCanDarktoonify);
        }
        else if (EventStickOnPolyline * onEventStickOnPolyline = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
        {
            processStickOnPolyline(onEventStickOnPolyline);
        }
        else if ( EventHanging* hanging = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
        {
            processNewHanging(hanging);
        }
        else if (EventStickOnPolylineUpdate* onPolylineUpdate = _event->DynamicCast<EventStickOnPolylineUpdate>(ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949)))
        {
            tryStickOnPolyline(onPolylineUpdate->getPolyline(),onPolylineUpdate->getEdge());
        }
        else if (EventQueryCanStickOrCollide* onQueryCanStickOrCollide = _event->DynamicCast<EventQueryCanStickOrCollide>(ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404)))
        {
            if (onQueryCanStickOrCollide->getSender() == m_snappedCharacter)
            {
                onQueryCanStickOrCollide->setCanStick(bfalse);
                onQueryCanStickOrCollide->setCanCollide(bfalse);
            }
        }
        else if (Ray_EventReleaseChild* onReleased = _event->DynamicCast<Ray_EventReleaseChild>(ITF_GET_STRINGID_CRC(Ray_EventReleaseChild,251461974)))
        {
            m_snappedCharacter.invalidate();
            resetState();
        }
        else if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventUturn,121408696)))
        {
            m_snappedCharacterLookRight = !m_snappedCharacterLookRight;
        }
        if ( EventBlockedByPolyline * stickOnPolyEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            if ( !tryStickOnPolyline(stickOnPolyEvent->getPolyline(),stickOnPolyEvent->getEdge()) && m_currentBehavior == m_receiveHitBehavior )
            {
                startRoam();
            }
        }

        Super::onEvent(_event);
    }

    void Ray_FruitAIComponent::processStickOnPolyline( EventStickOnPolyline* _stickOnPolyline )
    {
        if (m_currentBehavior==m_deathBehavior)
        {
            return;
        }
        if (_stickOnPolyline->getActor().isValid())
        {
            if (_stickOnPolyline->isSticked())
            {
                ITF_ASSERT_MSG(m_actorsOnPolyline.find(_stickOnPolyline->getActor()) == -1,"trying to stick an actor that is already stuck");
                m_actorsOnPolyline.push_back(_stickOnPolyline->getActor());
            }
            else
            {
                i32 index = m_actorsOnPolyline.find(_stickOnPolyline->getActor());
                ITF_ASSERT_MSG(index != -1,"unsticking an actor that was never stuck");
                if (index>=0)
                    m_actorsOnPolyline.eraseNoOrder(index);
            }
        }

        if ( _stickOnPolyline->isSticked() )
        {
            removeFromMemory(_stickOnPolyline->getActor());

            if ( getTemplate()->getHangingFruit() && 
                m_polyLineComponent->containsPolyline(_stickOnPolyline->getPolyline()) && 
                m_currentBehavior == m_sleepBehavior)
            {
                m_wasTriggered = bfalse;
                unstick();
                m_sleepBehavior->wakeUp();
                startRoam();
            }
            else
            {
                tryStickOnPolyline(_stickOnPolyline->getPolyline(),_stickOnPolyline->getEdge());
            }

            //SEB : DO NOT ADD THIS FORCE : it adds weird movement when you walk on the fruit
            //m_physComponent->addForce(_stickOnPolyline->getSpeed()/TARGETDT);
        }
        else
        {
            m_attachedActorsMemory.push_back(AttachedActorsMemory(_stickOnPolyline->getActor(),0.5f));
        }
    }

    void Ray_FruitAIComponent::processNewHanging( EventHanging* _hanging )
    {
        if (m_currentBehavior==m_deathBehavior)
        {
            return;
        }
        if ( _hanging->isHanging() )
        {
            ITF_ASSERT_MSG(m_actorsHangingOnPolyline.find(_hanging->getActor()) == -1,"trying to hang an actor that is already hung");
            m_actorsHangingOnPolyline.push_back(_hanging->getActor());

            removeFromMemory(_hanging->getActor());
        }
        else
        {
            i32 index = m_actorsHangingOnPolyline.find(_hanging->getActor());

            ITF_ASSERT_MSG(index != -1,"unhanging an actor that was never hung");

            if (index>=0)
            {
                m_actorsHangingOnPolyline.eraseNoOrder(index);
            }

            m_attachedActorsMemory.push_back(AttachedActorsMemory(_hanging->getActor(),0.5f));
        }
    }

    //************************************
    // Method:    processHit
    // FullName:  ITF::Ray_FruitAIComponent::processHit
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: PunchStim * _hit
    //************************************
    void Ray_FruitAIComponent::processHit( PunchStim* _hit )
    {
        if (m_currentBehavior==m_deathBehavior)
        {
            return;
        }
        Actor* sender = AIUtils::getActor(_hit->getSender());

        if ( _hit->getReceivedHitType()==RECEIVEDHITTYPE_SHOOTER )
        {
            if ( m_shooterHitTimer == 0.0f )
            {
                const Vec2d &hitDir = _hit->getDirection();
                Vec2d hitForce = hitDir*getTemplate()->getShooterHitForce();
                Vec2d speed = m_physComponent->getSpeed();
                f32 dirSpeed = hitDir.dot(speed);
                if (dirSpeed<0)
                {
                    Vec2d dirSpeedVect = hitDir*dirSpeed;
                    Vec2d orthoSpeed = speed-dirSpeedVect;

                    speed = orthoSpeed+ dirSpeedVect*getTemplate()->getShooterHitSpeedMultiplier();
                    m_physComponent->setSpeed(speed);

                }
                m_physComponent->addForce(hitForce);

                m_shooterHitTimer = 0.1f;
                
                // send Fx
                if ( m_fxController != NULL )
                {
                    const StringID feedbackAction = _hit->getActionFromHit();
                    u32 fxHandle = m_fxController->playFeedback(_hit->getSender(), feedbackAction, m_actor->getRef());
                    m_fxController->setFXPosFromHandle(fxHandle, _hit->getFxPos());
                }
            }

            if ( sender )
            {
                EventHitSuccessful hitSuccessful;
                hitSuccessful.setSender(m_actor->getRef());
                hitSuccessful.setHasBeenDamaged( bfalse );
                sender->onEvent(&hitSuccessful);
            }

            return;
        }

        if (!getTemplate()->getCanReceiveHit())
        {
            return;
        }
        if (m_sleepBehavior && m_currentBehavior == m_sleepBehavior)
        {
            m_wasTriggered = bfalse;
            if (getTemplate()->getHangingFruit())
            {
                unstick();
                startRoam(bfalse);
//                m_sleepBehavior->wakeUp();
                return;
            }
            else if(_hit->getReceivedHitType() == RECEIVEDHITTYPE_EARTHQUAKE)
            {
                unstick();
                m_sleepBehavior->wakeUp();
                return;
            }
            else
            {
                return;
            }
        }
        if (m_currentBehavior == m_snappedBehavior)
        {
            Actor * actor = m_snappedCharacter.getActor();
            if (actor)
            {
//                actor->onEvent(_hit);
                return;
            }
        }

        if (m_actorsOnPolyline.find(_hit->getSender())!= -1)
            return;

        if ( getHealth() <= 0 )
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            return;
        }

        // we only accept enemy hits, or donimo hits (from ejected friendlies),
        // but then only if we're not getting ejected ourselves...
        bbool isEnemyHit = AIUtils::isEnemyHit(_hit, getFaction());
        bbool isRehit = _hit->IsClassCRC(ITF_GET_STRINGID_CRC(RehitStim,1008690800));
        bbool isReceivingHit = (m_currentBehavior == m_receiveHitBehavior);
        if ( (isRehit && isReceivingHit) ||
             (!isRehit && !isEnemyHit) )
        {
            return;
        }

        if ( m_timer == 0.0f )
        {
            if ( sender )
            {
                Ray_EventFruitAttachQuery queryEvent(m_actor->getRef(),m_physComponent->getSpeed(),m_actor->get2DPos());
                sender->onEvent(&queryEvent);

                if ( queryEvent.getAttached() && !queryEvent.getUseSnap() )
                {
                    startStuck(sender);
                    return;
                }
            }
        }

        unstick();

        // notify the sender that he touched us
        
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }


        startReceiveHit(_hit);
    }

    //************************************
    // Method:    processQueryCanBeAttacked
    // FullName:  ITF::Ray_FruitAIComponent::processQueryCanBeAttacked
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: Ray_EventQueryCanBeAttacked * query
    //************************************
    void Ray_FruitAIComponent::processQueryCanBeAttacked( Ray_EventQueryCanBeAttacked* query )
    {
        if ( getHealth() <= 0 )
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            query->setCanBeAttacked(bfalse);
            return;
        }

        query->setCanBeAttacked(m_currentBehavior != m_receiveHitBehavior);
    }


    //************************************
    // Method:    processQueryCanDarktoonify
    // FullName:  ITF::Ray_FruitAIComponent::processQueryCanDarktoonify
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: Ray_EventQueryCanDarktoonify * _queryCanDarktoonify
    //************************************
    void Ray_FruitAIComponent::processQueryCanDarktoonify( Ray_EventQueryCanDarktoonify* _queryCanDarktoonify )
    {
        _queryCanDarktoonify->setCanDarktoonify(bfalse);
    }

    //************************************
    // Method:    processQueryBlocksHits
    // FullName:  ITF::Ray_FruitAIComponent::processQueryBlocksHits
    // Access:    private 
    // Returns:   void
    // Qualifier:
    // Parameter: EventQueryBlocksHits * _queryBlocksHit
    //************************************
    void Ray_FruitAIComponent::processQueryBlocksHits( EventQueryBlocksHits* _queryBlocksHit )
    {
        _queryBlocksHit->setCanBlockHits( m_currentBehavior != m_receiveHitBehavior );
    }


    //************************************
    // Method:    updateAnimInput
    // FullName:  ITF::Ray_FruitAIComponent::updateAnimInput
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        if( m_animatedComponent )
        {        
            const StringID s_Speed = ITF_GET_STRINGID_CRC(Speed,4218325830);
            const StringID s_SpeedY = ITF_GET_STRINGID_CRC(SpeedY,461177981);
            const StringID s_SnappedCharacterLookRight = ITF_GET_STRINGID_CRC(SnappedCharacterLookRight,3233915570);
            const Vec2d& speed = m_physComponent->getSpeed();
            Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
            Vec2d gravityDir = gravity;

            gravityDir.normalize();

            //speedX = f32_Abs(gravityDir.getPerpendicular().dot(speed));
            f32 speedY = f32_Abs((-gravityDir).dot(speed));

            m_animatedComponent->setInput(s_SpeedY, speedY);
            m_animatedComponent->setInput(s_Speed, speed.norm());

            if (m_currentBehavior == m_snappedBehavior)
            {
                m_animatedComponent->setInput(s_SnappedCharacterLookRight,static_cast<u32>(m_snappedCharacterLookRight));
            }
        }
    }


    //************************************
    // Method:    activatePolyline
    // FullName:  ITF::Ray_FruitAIComponent::activatePolyline
    // Access:    private 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::activatePolyline()
    {
    }

    //************************************
    // Method:    deactivatePolyline
    // FullName:  ITF::Ray_FruitAIComponent::deactivatePolyline
    // Access:    private 
    // Returns:   void
    // Qualifier:
    //************************************
    void Ray_FruitAIComponent::deactivatePolyline()
    {
    }

    void Ray_FruitAIComponent::startStuck( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex )
    {
        stick();

        if (m_stuckBehavior)
        {
            if ( _poly && _edge )
            {
                m_stuckPolyline = _poly->getRef();
                m_stuckEdge = _edgeIndex;

                f32 edgeAngle = getVec2Angle(_edge->m_normalizedVector);
                Vec2d localPos = m_actor->get2DPos() - _edge->getPos();

                m_stuckLocalPos = localPos.Rotate(-edgeAngle);

                if ( _poly->getOwnerActor() )
                {
                    EventStickOnPolyline stickEvent(_poly->getRef(),m_actor->getRef(),m_stuckEdge,1.f,
                                                    m_actor->get2DPos(),Vec2d::Zero,btrue);
                    _poly->getOwnerActor()->onEvent(&stickEvent);
                    _poly->getOwnerActor()->getBinding()->bindChild(m_actor->getRef());
                }
                else
                {
                    _poly->getBinding()->bindChild(m_actor->getRef());
                }
            }
            else
            {
                m_stuckPolyline.invalidate();
                m_stuckEdge = U32_INVALID;
                m_stuckLocalPos = Vec2d::Zero;
            }

            setBehavior(m_stuckBehavior);
        }
    }

    void Ray_FruitAIComponent::startStuck( Actor* _actor )
    {
        stick();

        if (m_stuckBehavior)
        {
            m_stuckActor = _actor->getRef();
            m_stuckLocalPos = ( m_actor->get2DPos() - _actor->get2DPos() ).Rotate(-_actor->getAngle());
            setBehavior(m_stuckBehavior);
        }
    }

    bbool Ray_FruitAIComponent::processStick( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex )
    {
        if (m_currentBehavior==m_deathBehavior)
        {
            return bfalse;
        }

        if ( m_timer == 0.0f && _edge )
        {
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_edge->getGameMaterial()));

            if ( mat && mat->canFruitStick()  )
            {
                startStuck(_poly,_edge,_edgeIndex);
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_FruitAIComponent::requestDelayedStickToCharacter(const ActorRef &_character)
    {
        m_delayedChangeFromBehavior.m_delayedChange = DelayedChange_StartStuck;
        m_delayedChangeFromBehavior.m_targetActor = _character;
    }

    void Ray_FruitAIComponent::requestDelayedSnapToCharacter(const ActorRef &_character)
    {
        m_delayedChangeFromBehavior.m_delayedChange = DelayedChange_StartSnap;
        m_delayedChangeFromBehavior.m_targetActor = _character;
    }

    void Ray_FruitAIComponent::processDelayedChangesFromBehaviors()
    {
        if (m_currentBehavior==m_deathBehavior)
            return;

        switch(m_delayedChangeFromBehavior.m_delayedChange)
        {
        case DelayedChange_None:
            break;
        case DelayedChange_StartSnap:
            m_snappedCharacter=m_delayedChangeFromBehavior.m_targetActor;
            startSnapped(); 
            break;
        case DelayedChange_StartStuck: 
            {
                m_snappedCharacter=m_delayedChangeFromBehavior.m_targetActor;
                Actor *target = m_delayedChangeFromBehavior.m_targetActor.getActor();
                if (target)
                {
                    startStuck(target);
                }
            } break;

        }

        m_delayedChangeFromBehavior.init();
    }


    bbool Ray_FruitAIComponent::tryStickOnPolyline( ObjectRef _polyline, u32 _edgeIndex )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(_polyline,_edgeIndex,poly,edge);

        if ( poly && edge && poly->getOwnerActor() != m_actor )
        {
            return processStick(poly,edge,_edgeIndex);
        }

        return bfalse;
    }

    void Ray_FruitAIComponent::updateStuck( f32 _dt )
    {
        if ( m_stuckPolyline.isValid() )
        {
            PolyLine* poly;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(m_stuckPolyline,m_stuckEdge,poly,edge);

            if ( poly && edge )
            {
                f32 edgeAngle = getVec2Angle(edge->m_normalizedVector);

                Vec2d worldPos = m_stuckLocalPos.Rotate(edgeAngle) + edge->getPos();

                m_actor->setPos(worldPos.to3d(m_actor->getPos().m_z));
            }
        }
        else if ( m_stuckActor.isValid() )
        {
            Actor* stuckActor = (Actor*)m_stuckActor.getObject();

            if ( stuckActor )
            {
                Vec2d worldPos = ( m_stuckLocalPos.Rotate(stuckActor->getAngle()) ) + stuckActor->get2DPos();

                m_actor->setPos(worldPos.to3d(m_actor->getPos().m_z));
            }
        }
    }

    void Ray_FruitAIComponent::updateMemory( f32 _dt )
    {
        u32 memIndex = 0;

        while ( memIndex < m_attachedActorsMemory.size() )
        {
            AttachedActorsMemory& memory = m_attachedActorsMemory[memIndex];

            memory.m_timer -= _dt;

            if ( memory.m_timer <= 0.f )
            {
                m_attachedActorsMemory.eraseKeepOrder(memIndex);
            }
            else
            {
                memIndex++;
            }
        }
    }

    bbool Ray_FruitAIComponent::isActorInMemory( ObjectRef _ref ) const
    {
        u32 numMemory = m_attachedActorsMemory.size();

        for ( u32 i = 0; i < numMemory; i++ )
        {
            if ( m_attachedActorsMemory[i].m_ref == _ref )
            {
                return btrue;
            }
        }

        return bfalse;
    }

    void Ray_FruitAIComponent::removeFromMemory( ObjectRef _ref )
    {
        u32 numMemory = m_attachedActorsMemory.size();

        for ( u32 i = 0; i < numMemory; i++ )
        {
            if ( m_attachedActorsMemory[i].m_ref == _ref )
            {
                m_attachedActorsMemory.eraseNoOrder(i);
                break;
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_FruitAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_FruitAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("roamBehavior", m_roamBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("floatingBehavior", m_floatingBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("sleepBehavior", m_sleepBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("stuckBehavior", m_stuckBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("snappedBehavior", m_snappedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("isHanging",m_hangingFruit);
        SERIALIZE_MEMBER("nonStickableTime",m_nonStickableTime);
        SERIALIZE_MEMBER("reinitWhenBecomesInactive", m_reinitWhenBecomesInactive);
        SERIALIZE_MEMBER("canReceiveHit", m_canReceiveHit);
        SERIALIZE_MEMBER("shooterHitForce", m_shooterHitForce);
        SERIALIZE_MEMBER("shooterHitSpeedMultiplier", m_shooterHitSpeedMultiplier);
        SERIALIZE_MEMBER("squashPenetrationRadiusPercent", m_squashPenetrationRadiusPercent);
    END_SERIALIZATION()

    Ray_FruitAIComponent_Template::Ray_FruitAIComponent_Template()
    : m_hangingFruit(bfalse)
    , m_nonStickableTime(1.0f)
    , m_roamBehavior(NULL)
    , m_deathBehavior(NULL)
    , m_floatingBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_sleepBehavior(NULL)
    , m_stuckBehavior(NULL)
    , m_snappedBehavior(NULL)
    , m_reinitWhenBecomesInactive(bfalse)
    , m_canReceiveHit(btrue)
    , m_shooterHitForce(600.f)
    , m_shooterHitSpeedMultiplier(0.8f)
    , m_squashPenetrationRadiusPercent(0.8f)
    {
    }

    Ray_FruitAIComponent_Template::~Ray_FruitAIComponent_Template()
    {
        SF_DEL(m_roamBehavior);
        SF_DEL(m_floatingBehavior);
        SF_DEL(m_receiveHitBehavior);
        SF_DEL(m_sleepBehavior);
        SF_DEL(m_stuckBehavior);
        SF_DEL(m_snappedBehavior);
        SF_DEL(m_deathBehavior);
    }

} // namespace ITF

