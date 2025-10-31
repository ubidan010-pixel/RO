#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterAIComponent.h"
#endif //_ITF_RAY_SHOOTERAICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_
#include "rayman\gameplay\Components\Shooter\Ray_ShooterActorParameterComponent.h"
#endif //_ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifdef SHOOTER_PROFILING
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#endif //SHOOTER_PROFILING

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_ShooterAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterAIComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
              SERIALIZE_MEMBER("beforeCamRelativeInitialPos",m_beforeCamRelativeInitialPos);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()
    

    const f32 Ray_ShooterAIComponent::k_SameHitSenderCoolDownDuration = 0.25f;

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterAIComponent::Ray_ShooterAIComponent()
        : m_idleBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_swallowed(bfalse)
        , m_underVacuumInfluence(bfalse)
        , m_animInputUnderVacuumInfluence(bfalse)
        , m_beforeCamRelativeInitialPos( F32_INFINITY, F32_INFINITY, F32_INFINITY )
        , m_tweenComponent( NULL )
        , m_stimComponent( NULL )
        , m_shooterAPComponent( NULL )
        , m_isCameraRelative( bfalse )
        , m_cameraRelativeTested( bfalse )
        , m_lastHitSenderCoolDown( k_SameHitSenderCoolDownDuration )
        , m_curFogColor()
        , m_curFogDepth( 30.f )
        , m_hasEnteredScreen( bfalse )
        , m_useOptim( bfalse )        
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterAIComponent::~Ray_ShooterAIComponent()
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if ( getTemplate()->getDisableOnBecomeInactive() )
        {
            if ( !m_swallowed )
            {
                // destroy the object ?
                if ( m_actor->isSerializable() )
                {
                    m_actor->disable();
                }
                else
                {
                    m_actor->requestDestruction();
                }
            }
        }

        if ( getTemplate()->isCameraRelative() )
        {
            CAMERACONTROLLERMANAGER->unbindObject( m_actor );
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        onDepthChanged( m_actor->getDepth(), m_actor->getDepth() );
#ifdef ITF_WII
        m_useOptim = ( RAY_GAMEMANAGER->getCurrentLevelName() == s_MO_SHOOTER_B ) ? btrue : bfalse;
#endif //ITF_WII
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        if ( m_idleBehavior != NULL )
        {
            setBehavior(m_idleBehavior);
        }

        if ( getTemplate()->isTriggerable() )
        {
            m_actor->disable();
        } 

        applyCamRelativeInitialPos();
    }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        // init position
        applyCamRelativeInitialPos();

        m_tweenComponent = m_actor->GetComponent<TweenComponent>();
        m_stimComponent = m_actor->GetComponent<Ray_StimComponent>();
        m_shooterAPComponent = m_actor->GetComponent<Ray_ShooterActorParameterComponent>();
        

        m_idleBehavior = createAiBehavior(getTemplate()->getIdleBehavior());
        m_receiveHitBehavior = static_cast<Ray_AIReceiveHitBehavior *>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));
        //m_deathBehavior = static_cast<Ray_AIDeathBehavior *>(createAiBehavior(getTemplate()->getDeathBehavior()));
        m_deathBehavior = createAiBehavior(getTemplate()->getDeathBehavior());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(VacuumStim,593568508),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168),this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523),this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099), this );
        

        if ( getTemplate()->isTriggerable() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this );
            m_actor->disable();
        }        

        if ( m_idleBehavior != NULL )
        {
            setBehavior(m_idleBehavior);
        }

        m_cameraRelativeTested = bfalse;

        m_curFogColor.m_r = getTemplate()->getDynamicFogDefaultColor().m_x / 255.0f;
        m_curFogColor.m_g = getTemplate()->getDynamicFogDefaultColor().m_y / 255.0f;
        m_curFogColor.m_b = getTemplate()->getDynamicFogDefaultColor().m_z / 255.0f;
        m_curFogDepth = getTemplate()->getDynamicFogMaxDepth();

        if ( ( getTemplate()->getVisualScaleMultiplier() != 1.0f ) && ( m_animComponent != NULL ) )
        {
            m_animComponent->setScaleMultiplier( getTemplate()->getVisualScaleMultiplier() );
        }

        if ( getTemplate()->useDynamicFlip() )
        {
            if ( getTemplate()->useDynamicFlipStartLeft() )
            {
                 m_actor->setFlipped(btrue);            
            }
        }
        m_prevPosition = m_actor->get2DPos();

#ifdef SHOOTER_PROFILING
        SP_ADD( "ShooterAI", m_actor->getRef() )
#endif //SHOOTER_PROFILING

    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

#ifdef ITF_WII
        if ( getTemplate()->getDisableOnBecomeInactive() && m_useOptim )
        {
            AABB screenAABB = CAMERA->getAABBatZ(m_actor->getDepth());
            if ( !screenAABB.checkOverlap( m_actor->getAABB() ) )
            {
                if ( ( m_hasEnteredScreen && ( m_actor->get2DPos().m_x < screenAABB.getMin().m_x ) ) || ( ( m_deathBehavior != NULL ) && ( m_currentBehavior == m_deathBehavior ) ) || ( ( m_receiveHitBehavior != NULL ) && ( m_currentBehavior == m_receiveHitBehavior ) ) )
                {
                    m_actor->disable();
                    return;
                }
            }
            else
            {
                m_hasEnteredScreen = btrue;
            }
        }
#endif //ITF_WII

        // Check cam relative
        //----------------------
        if ( !m_cameraRelativeTested )
        {
            m_cameraRelativeTested = btrue;
            m_isCameraRelative = getTemplate()->isCameraRelative();

            // had been set by someone else ?
            if ( !m_isCameraRelative )
            {
                m_isCameraRelative = CAMERACONTROLLERMANAGER->isObjectBound( m_actor );
            }
        }

        bbool shouldSendStim = bfalse;
        Vec2d curPos = m_actor->getBoundLocal2DPos();
        
        // remove vacuum procedural anim move vector
        if ( m_shooterAPComponent != NULL )
        {
            m_shooterAPComponent->get2DPosWithoutVacuumInfluence( curPos );
        }

        if (m_currentBehavior == m_idleBehavior)
        {
            shouldSendStim = !m_swallowed && !m_hasBeenVaccumed;

            // fix angle
            //-------------
            if ( getTemplate()->useFixedAngle() )
            {
                m_actor->setBoundWorldAngle( getTemplate()->getFixedAngle().ToRadians() );
            }

            // check flip dynamic
            //-----------------------
            if ( getTemplate()->useDynamicFlip() || getTemplate()->getTakeMoveOrientation() )
            {
                if ( shouldSendStim && !m_prevPosition.IsEqual(Vec2d::Zero) )
                {
                    Vec2d moveDir = curPos - m_prevPosition;
                    // remove the camera, movement
                    if ( m_isCameraRelative )
                    {
                        moveDir = moveDir - CAMERACONTROLLERMANAGER->getBindCameraMoveDelta().truncateTo2D();
                    }

                    if ( !moveDir.IsEqual(Vec2d::Zero) )
                    {
                        if ( getTemplate()->getTakeMoveOrientation() )
                        {
                            f32 newAngle;
                            moveDir = ( moveDir.normalize() ).Rotate( getTemplate()->getTakeMoveOrientationOffset().ToRadians() );
                            if ( getTemplate()->useDynamicFlip() )
                            {
                                bbool flipped; 
                                AIUtils::getAngleAndFlipped( moveDir, newAngle, flipped );
                                m_actor->setFlipped(flipped);
                            }
                            else
                            {
                                newAngle = moveDir.getAngle();
                            }
                            m_actor->setAngle( newAngle );
                        }
                        else if ( getTemplate()->useDynamicFlip() )
                        {
                            f32 dotVal = moveDir.dot( Vec2d::Right );
                            m_actor->setFlipped( ( dotVal <= 0.f ) ? btrue : bfalse );
                        }
                    }
                }            
            }            
        }
        m_prevPosition = curPos;//m_actor->getBoundLocal2DPos();

        if (m_underVacuumInfluence)
        {
            shouldSendStim  = bfalse;
        }

        
        if ( m_stimComponent != NULL )
        {
            m_stimComponent->setDisabled(!shouldSendStim);
        }

        m_animInputUnderVacuumInfluence = m_underVacuumInfluence;
        m_underVacuumInfluence = bfalse;
        m_lastHitSenderCoolDown =  Max( 0.f, m_lastHitSenderCoolDown - _deltaTime );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_receiveHitBehavior)
        {
            if ( !getHealth() )
            {
                disableComponents();
                setBehavior(m_deathBehavior);
            }
            else
                setBehavior(m_idleBehavior);
        }
    }    

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if ( PunchStim* punchStim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            receivePunch(punchStim);
        }
        else if ( Ray_EventShooterEjectActor * ejected = _event->DynamicCast<Ray_EventShooterEjectActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133)))
        {
            m_swallowed = bfalse;
        }
        else if ( VacuumStim * vacuum = _event->DynamicCast<VacuumStim>(ITF_GET_STRINGID_CRC(VacuumStim,593568508)))
        {
            if ( !m_hasBeenVaccumed )
            {
                Ray_EventIsInfluencedByVacuum influenced;
                influenced.setSender( m_actor->getRef() );
                m_actor->onEvent( &influenced );
                m_underVacuumInfluence = influenced.getInfluenced();
            }
        }
        else if ( Ray_EventShooterSwallow * vacuum = _event->DynamicCast<Ray_EventShooterSwallow>(ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168)) )
        {
            m_swallowed = btrue;
            
        }
        else if ( Ray_EventHasBeenVacuumed * vacuum = _event->DynamicCast<Ray_EventHasBeenVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523)) )
        {
            disableComponents();
        }        
        else if ( EventTrigger * trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if ( getTemplate()->isTriggerable() )
            {
                m_actor->enable();
                if ( getTemplate()->isCameraRelative() )
                {
                    CAMERACONTROLLERMANAGER->bindObject( m_actor, ( m_tweenComponent != NULL ) ? btrue : bfalse );
                }
            }
        }       
        else if ( EventInteractionQuery* interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
        {
            processInteractionQuery(interactionQuery);
        }
        else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
        {
            processCrushed(crushed);
        }
        else if ( Ray_EventDynamicFogParamSet* fogParams = _event->DynamicCast<Ray_EventDynamicFogParamSet>(ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099)) )
        {
            m_curFogColor = fogParams->getFogColor();
            m_curFogDepth = fogParams->getFogMaxDepth();
        }        
    }

    void Ray_ShooterAIComponent::processInteractionQuery( EventInteractionQuery* interactionQuery )
    {

        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if ( interactionQuery->getInteraction() == CharacterInteractionType_None 
                && getTemplate()->getCanBeCrushed()
                && m_currentBehavior != m_deathBehavior)
            {
                ActorRef ref = interactionQuery->getSender();
                Actor * actor = ref.getActor();
                if (actor)
                {
                    if (actor->getBase().m_y > m_actor->getPos().m_y)
                        interactionQuery->setInteraction(CharacterInteractionType_Crush);

                }
                m_currentBehavior->onEvent(interactionQuery);
            }
        }
    }

    void Ray_ShooterAIComponent::processCrushed( EventCrushed* crushed )
    {
        if (getTemplate()->getCanBeCrushed())
        {
            PunchStim stim;
            stim.setSender(crushed->getSender());
            stim.setLevel(0);
            stim.setAngle(0.0f);
            stim.setDepth(m_actor->getDepth());
            stim.setDirection(crushed->getDirection());
            stim.setFxPos(crushed->getFxPos());
            m_actor->onEvent(&stim);
        }
    }
    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::receivePunch( PunchStim* _punch )
    {
        if ( m_currentBehavior != NULL )
        {
            EventQueryCanReceiveHit query;
            query.setStim(_punch);
            m_currentBehavior->onEvent(&query);

            if (!query.getCanReceiveHit())
                return;
        }        

        if ( m_receiveHitBehavior && !m_receiveHitBehavior->canReceiveHit(_punch) )
        {
            return;
        }

        if ( ( m_receiveHitBehavior != NULL ) && ( getHealth() > 0 ) )
        {
            if ( ( _punch->getSender() == m_lastHitSender ) && ( m_lastHitSenderCoolDown > 0.f ) )
            {
                return;
            }
            m_lastHitSender = _punch->getSender();
            m_lastHitSenderCoolDown = k_SameHitSenderCoolDownDuration;

            Actor* sender = ActorRef(_punch->getSender()).getActor();
            if (sender)
            {
                EventHitSuccessful hitSuccessful;
                hitSuccessful.setSender(m_actor->getRef());
                sender->onEvent(&hitSuccessful);
            }

            // we are already processing a hit, so we need to reset the behavior
            setBehavior(m_receiveHitBehavior);
            m_receiveHitBehavior->setData(_punch);

            if ( getHealth() <= 0 )
            {
                disableComponents();
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();
        m_animatedComponent->setInput( ITF_GET_STRINGID_CRC(Vacuum,3229720905),m_animInputUnderVacuumInfluence );
        m_animatedComponent->setInput( ITF_GET_STRINGID_CRC(Flipped,3505929688), m_actor->isFlipped() );
        m_animInputUnderVacuumInfluence = bfalse;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::disableComponents()
    {
        EventActivate activated;
        activated.setActivated( bfalse );
        m_actor->onEvent( &activated );
        
        if ( m_tweenComponent != NULL )
        {
            m_tweenComponent->suspendTweening();
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::applyCamRelativeInitialPos()
    {
        if ( getTemplate()->isCameraRelative() && !m_beforeCamRelativeInitialPos.IsEqual( Vec3d( F32_INFINITY, F32_INFINITY, F32_INFINITY ) ) )
        {
            m_actor->setBoundWorldInitialPos( m_beforeCamRelativeInitialPos );
            m_actor->setBoundWorldPos( m_beforeCamRelativeInitialPos );
        }    
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged( _oldZ, _newZ );
        
        if ( ( getHealth() > 0 ) && getTemplate()->useDynamicFog() && ( m_curFogDepth != 0.0f ) )
        {
            f32 distToCamZ = f32_Max( 0.0f, CAMERACONTROLLERMANAGER->getCamPos().m_z - CAMERACONTROLLERMANAGER->getDepth() - _newZ );
           
            m_curFogColor.m_a = f32_Min( distToCamZ, m_curFogDepth ) / m_curFogDepth;
           
            m_animComponent->setColorFog( m_curFogColor );
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    //-------------------------------------------------------------------------------------------------------------<-------------------
    void Ray_ShooterAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        m_beforeCamRelativeInitialPos = m_actor->getWorldInitialPos();
    }
#endif // ITF_SUPPORT_EDITOR

    //--------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idleBehavior", m_idleBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());

        SERIALIZE_MEMBER( "cameraRelative",             m_cameraRelative );
        SERIALIZE_MEMBER( "triggerable",                m_triggerable );
        SERIALIZE_MEMBER( "dynamicFlip",                m_dynamicFlip );
        SERIALIZE_MEMBER( "dynamicFlipStartLeft",       m_dynamicFlipStartLeft );
        SERIALIZE_MEMBER( "useFixedAngle",              m_useFixedAngle );
        SERIALIZE_MEMBER( "fixedAngle",                 m_fixedAngle );
        SERIALIZE_MEMBER( "takeMoveOrientation",        m_takeMoveOrientation );  
        SERIALIZE_MEMBER( "takeMoveOrientationOffset",  m_takeMoveOrientationOffset );
        SERIALIZE_MEMBER( "useDynamicFog",              m_useDynamicFog );  
        SERIALIZE_MEMBER( "dynamicFogDefaultColor",     m_dynamicFogDefaultColor );  
        SERIALIZE_MEMBER( "dynamicFogMaxDepth",         m_dynamicFogMaxDepth );  
        SERIALIZE_MEMBER( "canBeCrushed",               m_canBeCrushed );  
        SERIALIZE_MEMBER( "disableOnBecomeInactive",    m_disableOnBecomeInactive );  
        SERIALIZE_MEMBER( "visualScaleMultiplier",      m_visualScaleMultiplier );
                
    END_SERIALIZATION()

    Ray_ShooterAIComponent_Template::Ray_ShooterAIComponent_Template()
    : m_idleBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_deathBehavior(NULL)
    , m_cameraRelative(bfalse)
    , m_triggerable(bfalse)
    , m_dynamicFlip(bfalse)
    , m_dynamicFlipStartLeft(bfalse)
    , m_useFixedAngle( bfalse )
    , m_takeMoveOrientation( bfalse )
    , m_useDynamicFog( bfalse )
    , m_dynamicFogDefaultColor( Vec3d::Zero )
    , m_dynamicFogMaxDepth( 1.f )
    , m_canBeCrushed(bfalse)
    , m_disableOnBecomeInactive( bfalse )
    , m_visualScaleMultiplier( 1.0f )
    {
        m_fixedAngle.SetRadians( 0.f );
        m_takeMoveOrientationOffset.SetRadians( 0.f );
    }

    Ray_ShooterAIComponent_Template::~Ray_ShooterAIComponent_Template()
    {
        SF_DEL(m_idleBehavior);
        SF_DEL(m_receiveHitBehavior);
        SF_DEL(m_deathBehavior);
    }
}

