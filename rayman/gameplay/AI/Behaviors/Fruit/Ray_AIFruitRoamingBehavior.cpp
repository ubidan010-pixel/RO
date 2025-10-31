#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Fruit/Ray_AIFruitRoamingBehavior.h"
#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_AIFRUITCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FRUitAIComponent.h"
#endif //_ITF_RAY_AIFRUITCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_


#define FRUIT_PREJUMP_ANIM "proceduralPreBounce"
#define FRUIT_JUMPING_ANIM "bounce"
#define FRUIT_FALL_ANIM "FALL"
#define FRUIT_APEX_ANIM "apex"

#define FRUIT_BOUNCE_CURSOR_INPUT               "bounce_Cursor"
#define FRUIT_LEFT_BOUNCE_CURSOR_INPUT          "leftBounce_Cursor"
#define FRUIT_LEFT_BOUNCE_WEIGHT_INPUT          "leftBounce_Weight"
#define FRUIT_RIGHT_BOUNCE_CURSOR_INPUT         "rightBounce_Cursor"
#define FRUIT_RIGHT_BOUNCE_WEIGHT_INPUT         "rightBounce_Weight"
#define FRUIT_UPWARD_BOUNCE_WEIGHT_INPUT        "upwardBounce_Weight"
#define FRUIT_UPWARD_BOUNCE_CURSOR_INPUT        "upwardBounce_Cursor"


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIFruitRoamingBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIFruitRoamingBehavior_Template)
        SERIALIZE_MEMBER("apexTypicalHeight",m_apexTypicalHeight);
        SERIALIZE_MEMBER("normalGravityMultiplier", m_normalGravityMultiplier);
        SERIALIZE_MEMBER("startToFloatTime", m_startToFloatTime);
        SERIALIZE_MEMBER("floatConstantForceDuration", m_floatConstantForceDuration);
        SERIALIZE_MEMBER("floatMaxForce", m_floatMaxForce);
        SERIALIZE_MEMBER("speedThatTriggersFloating", m_speedThatTriggersFloating);
        SERIALIZE_MEMBER("floatTimeBeforeFalling", m_floatTimeBeforeFalling);
        SERIALIZE_MEMBER("bounceOnCharacterAreaAngle", m_bounceOnCharacterAreaAngle);
        SERIALIZE_MEMBER("angleToBounceVerticallyOnCharacter", m_angleToBounceVerticallyOnCharacter);
        SERIALIZE_MEMBER("slantingBounceOnCharacterAngle", m_slantingBounceOnCharacterAngle);
        SERIALIZE_MEMBER("bounceXSpeedFactor", m_bounceXSpeedFactor);
        SERIALIZE_MEMBER("bounceYSpeedFactor", m_bounceYSpeedFactor);
        SERIALIZE_MEMBER("speedForMaxSquash", m_speedForMaxSquash);
        SERIALIZE_MEMBER("maxSquashDuration", m_maxSquashDuration);
        SERIALIZE_MEMBER("maxSquashFactor", m_maxSquashFactor);
        SERIALIZE_MEMBER("interFruitForce", m_interFruitForce);
        SERIALIZE_MEMBER("maxBounceSpeed", m_maxBounceSpeed);
        SERIALIZE_MEMBER("canAttach", m_canAttach);
        SERIALIZE_MEMBER("bounceSpeedAlongNormal", m_bounceSpeedAlongNormal);
        SERIALIZE_MEMBER("speedPerturbationWhenTooVertical", m_speedPerturbationWhenTooVertical);
        SERIALIZE_MEMBER("initialSpeed", m_initialSpeed);
        SERIALIZE_MEMBER("canBounceOnHead", m_canBounceOnHead);
		SERIALIZE_MEMBER("minBounceDuration", m_minBounceDuration);
        SERIALIZE_MEMBER("groundToVerticalBlendFactor", m_groundToVerticalBlendFactor);
        SERIALIZE_MEMBER("minSpeedForPerturbation", m_minSpeedForPerturbation);
        SERIALIZE_MEMBER("isStaticOnX", m_isStaticOnX);
        SERIALIZE_MEMBER("speedForMaxBounceDuration", m_speedForMaxBounceDuration);
        SERIALIZE_MEMBER("maxBounceDuration", m_maxBounceDuration);
        SERIALIZE_MEMBER("interreaction_maxBounceSpeedMultiplier", m_interreaction_maxBounceSpeedMultiplier);
        SERIALIZE_MEMBER("interreaction_addVerticalSpeed", m_interreaction_addVerticalSpeed);
        SERIALIZE_MEMBER("massInfluenceOnBounce", m_massInfluenceOnBounce);
        SERIALIZE_MEMBER("fallDeformationReactivity", m_fallDeformationReactivity);
        SERIALIZE_MEMBER("fallDeformation", m_fallDeformation);
        SERIALIZE_MEMBER("speedForFallDeformation", m_speedForFallDeformation);
        SERIALIZE_MEMBER("standAnimationName", m_standAnimationName);
        SERIALIZE_MEMBER("rotationSpeed", m_rotationSpeed);
        SERIALIZE_MEMBER("fxOnBounce", m_fxOnBounce);
        SERIALIZE_MEMBER("fxAfterFirstBounce", m_fxAfterFirstBounce);        
    END_SERIALIZATION()

    Ray_AIFruitRoamingBehavior_Template::Ray_AIFruitRoamingBehavior_Template()
    : m_apexTypicalHeight(1.1f)
    , m_normalGravityMultiplier(1.f)
    , m_startToFloatTime(0.5f)
    , m_floatConstantForceDuration(0.5f)
    , m_floatMaxForce(100.f)
    , m_speedThatTriggersFloating(2.f)
    , m_floatTimeBeforeFalling(0.5f)
    , m_bounceOnCharacterAreaAngle(btrue, 80)
    , m_angleToBounceVerticallyOnCharacter(btrue, 20.f)
    , m_slantingBounceOnCharacterAngle(btrue, 10.f)
    , m_bounceXSpeedFactor(0.5f)
    , m_bounceYSpeedFactor(0.1f)
    , m_speedForMaxSquash(6.f)
    , m_maxSquashDuration(1.f)
    , m_maxSquashFactor(0.5f)
    , m_interFruitForce(50.f)
    , m_maxBounceSpeed(1.f)
    , m_canAttach(btrue)
    , m_bounceSpeedAlongNormal(1.f)
    , m_speedPerturbationWhenTooVertical(0.f)
    , m_canBounceOnHead(btrue)
	, m_minBounceDuration(0.1f)
    , m_groundToVerticalBlendFactor(0)
    , m_minSpeedForPerturbation(0.5f)
    , m_isStaticOnX(bfalse)
    , m_speedForMaxBounceDuration(1.f)
    , m_maxBounceDuration(0.2f)
    , m_interreaction_maxBounceSpeedMultiplier(1.2f)
    , m_interreaction_addVerticalSpeed(2.f)
    , m_massInfluenceOnBounce(0.5f)
    , m_fallDeformationReactivity(0.9f)
    , m_fallDeformation(1.1f)
    , m_speedForFallDeformation(6.f)
    , m_standAnimationName("Idle")
    , m_rotationSpeed(bfalse, 0.0f)
    , m_fxOnBounce(StringID::Invalid)
    , m_fxAfterFirstBounce(StringID::Invalid)
    {

    }

    Ray_AIFruitRoamingBehavior_Template::~Ray_AIFruitRoamingBehavior_Template()
    {        
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIFruitRoamingBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIFruitRoamingBehavior)
    VALIDATE_BEHAVIOR_PARAM("bounceOnCharacterAreaAngle", cosf(getTemplate()->getBounceOnCharacterAreaAngle().ToRadians())>0, "should be less than 90 degrees");
    VALIDATE_BEHAVIOR_PARAM("angleToBounceVerticallyOnCharacter", f32_Cos(getTemplate()->getAngleToBounceVerticallyOnCharacter().ToRadians())>0, "should be (much) less than 90 degrees");
    VALIDATE_BEHAVIOR_PARAM("slantingBounceOnCharacterAngle", m_physComponent->getEdgeOrientationType(getTemplate()->getSlantingBounceOnCharacterAngle().ToVector())==StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND   , "must be small enough to be a ground collision");
    VALIDATE_BEHAVIOR_PARAM("speedForMaxSquash", getTemplate()->getSpeedForMaxSquash()>0, "must be > 0");
    VALIDATE_BEHAVIOR_PARAM("", m_actor->GetComponent<PhantomComponent>()!=NULL, "No Phantom");
    END_VALIDATE_BEHAVIOR()

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AIFruitRoamingBehavior::Ray_AIFruitRoamingBehavior()
    : m_physComponent(NULL)
        , m_previousSpeed(0,0)
        , m_state(State_NA)
        , m_timeInCurrentState(0)
        , m_speedWhenEnteredPreJump(0,0)
        , m_speedWhenEnteredWallBounce(0,0)
        , m_preBounceDuration(1.f)
        , m_timeInProceduralBounce(1.f)
        , m_isInBounceFreeze(bfalse)
        , m_isBouncingTowardsLeft(bfalse)
        , m_isBouncingTowardsRight(bfalse)
        , m_isBouncingUpward(bfalse)
        , m_bouncingTowardsLeft_Weight(0)
        , m_bouncingTowardsRight_Weight(0)
        , m_bouncingUpward_Weight(0)
        , m_bouncingTowardsLeft_Time(0)
        , m_bouncingTowardsRight_Time(0)
        , m_bouncingUpward_Time(0)
        , m_bouncingUpward_Duration(0)
        , m_bouncingTowardsLeft_Duration(0)
        , m_bouncingTowardsRight_Duration(0)
        , m_dontChangeAnimThisFrame(bfalse)
        , m_bounceInitialSpeed(0)
        , m_computedBounceForce(0)
        , m_reinitSpeed(bfalse)
        , m_lastFrameLeftBounce(0)
        , m_firstBounceDirection(0)
        , m_useComplexAnimationSystem(bfalse)
        , m_bouncesCount(0)
        , m_timeSinceLastBounce(0.0f)
        , m_fxHandle(U32_INVALID)
    {
    }

    Ray_AIFruitRoamingBehavior::~Ray_AIFruitRoamingBehavior()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        //detect if we have to use complex or simple animation system
        m_useComplexAnimationSystem = m_animatedComponent != NULL;
        m_currentAnim.invalidate();
        bounceUtility_reset();
        bounceUtility_setHasStateForWallBounce(btrue);
		bounceUtility_onlyKeepNormalPartOfSpeedWhenBouncingOnWall(btrue);
        bounceUtility_setGroundToVerticalBlendFactor(getTemplate()->getGroundToVerticalBlendFactor());
        bounceUtility_registerEvents(m_actor, m_aiComponent);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetDirection,187560938),m_aiComponent);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        ITF_ASSERT(m_physComponent);
        f32 h = getTemplate()->getApexTypicalHeight();
        f32 g = f32_Abs(PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth()).m_y*getTemplate()->getNormalGravityMultiplier());
        f32 DT = LOGICDT;
        //formula : v0� = 2.g.Yapex                  and   v0=(bounceForce-g).dt
        m_computedBounceForce = f32_Sqrt(2.f*g*h)/DT+g;
        //bounceUtility_setBounceForceAmplitude(f32_Sqrt(2.f*g*h)/DT+g);
        m_bounceInitialSpeed = f32_Sqrt(2.f*g*h);
        bounceUtility_setGroundBounceSpeedFactor(Vec2d(getTemplate()->getBounceXSpeedFactor(),getTemplate()->getBounceYSpeedFactor()));
        resetState();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::updateYScale()
    {
        Vec2d targetScale(m_actor->getLocalInitialScale());
        if (m_currentAnim == FRUIT_FALL_ANIM || m_currentAnim== FRUIT_JUMPING_ANIM)
        {
            f32 maxSpeedMultiplier = getTemplate()->getFallDeformation();
            f32 maxSpeed = getTemplate()->getSpeedForFallDeformation();
            f32 speedPercent = f32_Abs(m_physComponent->getSpeed().m_y)/maxSpeed;
            if (speedPercent>1)
                speedPercent = 1;
            speedPercent *= speedPercent;
            f32 multiplier = 1*(1-speedPercent)+maxSpeedMultiplier*speedPercent;
            targetScale.m_x /= multiplier;
            targetScale.m_y *= multiplier;            
        }
        f32 interpolator = getTemplate()->getFallDeformationReactivity();
        Vec2d scale = targetScale*interpolator+m_actor->getScale()*(1-interpolator);
        m_actor->setScale(scale);
    }
    /////////////////////////////////////////////////////////////////////////////////////////// 
    void Ray_AIFruitRoamingBehavior::bounceUtility_bounceOnEdge_applyBouncePhysics(StickToPolylinePhysComponent *_physComponent, const Vec2d &_normalizedEdgeDir)
    {
        Vec2d correctedEdgeDir = bounceUtility_mayBlendEdgeDirWhenGround(_physComponent, _normalizedEdgeDir);
        bounceUtility_setBounceForceAmplitude(f32_Abs(correctedEdgeDir.m_x)*m_computedBounceForce);
        Ray_AIBounceUtility::bounceUtility_bounceOnEdge_applyBouncePhysics(_physComponent,_normalizedEdgeDir);
        Vec2d normal = _normalizedEdgeDir.getPerpendicular();

        Vec2d speed = m_physComponent->getSpeed();

        if (getTemplate()->getSpeedPerturbationWhenTooVertical()!=0)
        {
            static const f32 angleDelta = f32_Cos(MTH_PIBY8);

            if (normal.m_y>angleDelta && f32_Abs(speed.dot(_normalizedEdgeDir))<getTemplate()->getMinSpeedForPerturbation())
            {                
                float angleDeltaSign;
                if (speed.m_x==0)
                {
                    if (m_firstBounceDirection==0)
                    {
                        angleDeltaSign = Seeder::getSharedSeeder().GetFloat( 0.f, 1.f );
                    }
                    else
                    {
                        angleDeltaSign = 0.5f+m_firstBounceDirection;
                        m_firstBounceDirection = 0;
                    }
                    if (angleDeltaSign<0.5f)
                        angleDeltaSign = -1;
                    else 
                        angleDeltaSign = 1;
                }
                else
                {
                    if (speed.m_x<0)
                        angleDeltaSign = -1;
                    else
                        angleDeltaSign = 1;
                }
                speed += _normalizedEdgeDir*(angleDeltaSign*getTemplate()->getSpeedPerturbationWhenTooVertical());
            }
        }

        f32 speedNorm = speed.norm();
        if (speedNorm>getTemplate()->getMaxBounceSpeed())
        {
            speed *= getTemplate()->getMaxBounceSpeed() / speedNorm;
        }

        m_physComponent->setSpeed(speed);
        /*
        Vec2d speed = _physComponent->getSpeed();
        speed.m_x *= m_bounceUtility_groundBounceSpeedFactor.m_x;
        speed.m_y *= m_bounceUtility_groundBounceSpeedFactor.m_y;
        speed += m_bounceInitialSpeed*_normalizedEdgeDir.m_x*_normalizedEdgeDir.getPerpendicular();
        _physComponent->unstick();
        _physComponent->setSpeed(speed);
        */

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::resetState()
    {
        ActorRef invalidRef;

        m_actor->setScale(m_actor->getLocalInitialScale());
        m_previousSpeed = m_physComponent->getSpeed();
        m_previousPos = m_actor->get2DPos();
        m_isInBounceFreeze = bfalse;
        m_isBouncingTowardsRight = bfalse;
        m_isBouncingTowardsLeft = bfalse;
        m_isBouncingUpward = bfalse;
        setRider(invalidRef);
        m_reinitSpeed = btrue;

        setCurrentAnim(getTemplate()->getStandAnimationName());
        setState(State_JumpOrFall);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::setCurrentAnim(const StringID &_newAnim)
    {
        if (!m_dontChangeAnimThisFrame)
        {
            m_currentAnim = _newAnim;
            m_animComponent->setAnim(_newAnim);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::bounceUtility_UpdateJumpOrFall_onIsOnGround(const Vec2d &_normalizedEdge, bbool isFromActor)
    {
        PreJumpParameter preJumpParam(_normalizedEdge, isFromActor);
        setState(State_PreJump, &preJumpParam);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::bounceUtility_UpdateJumpOrFall_onNearTopOfJump()
    {
        //setState(State_FloatingBeforeFalling);
    }

    ////////////////////////////////////////////////////////////////////////////////////////p///
    Vec2d Ray_AIFruitRoamingBehavior::bounceUtility_getBounceForce( Vec2d _edgeDir)
    {
        return bounceUtility_getBounceOnlyForce(_edgeDir, 1.f);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_AIFruitRoamingBehavior::computeBounceWave(f32 _time, f32 _duration)
    {
        f32 lambda = _time/std::max(_duration,0.05f);
        lambda = std::min(lambda,1.f);

        f32 bounceWave = f32_Sin(lambda*MTH_PI);
        bounceWave = 1-bounceWave;
        bounceWave *= bounceWave;
        bounceWave = 1-bounceWave;

        return bounceWave;
    }

    void Ray_AIFruitRoamingBehavior::updateGraphicBounce(f32 _deltaT, 
        bbool &_isBouncingInDirection, 
        f32 &_bouncingInDirection_Time,
        f32 _bouncingInDirection_Duration,
        const StringID &_bounceCursor_InputName,
        const StringID &_bounceWeight_InputName)
    {
        if (_isBouncingInDirection)
        {
            _bouncingInDirection_Time += _deltaT;

            m_animatedComponent->setInput(_bounceCursor_InputName, computeBounceWave(_bouncingInDirection_Time, _bouncingInDirection_Duration));
            if (_bouncingInDirection_Time>_bouncingInDirection_Duration)
            {
                m_animatedComponent->setInput(_bounceWeight_InputName, 0.f);
                _isBouncingInDirection = bfalse;
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIFruitRoamingBehavior::updateProceduralPreBounceAnim(f32 _delta)
    {
        bbool isFinished = bfalse;

        if (m_isInBounceFreeze)
        {
            f32 ratio = 1.0f;
            if( m_preBounceDuration > 0.0f )
            {
                m_timeInProceduralBounce += _delta;
                ratio = m_timeInProceduralBounce / m_preBounceDuration;
            }
            if( ratio >= 1.0f )
            {
                isFinished = btrue;
                m_isInBounceFreeze = bfalse;
            }
        }
        else
        {
            isFinished = btrue;
        }

        if( m_useComplexAnimationSystem )
        {
            updateGraphicBounce(_delta, 
                m_isBouncingTowardsRight, 
                m_bouncingTowardsRight_Time, m_bouncingTowardsRight_Duration,
                FRUIT_RIGHT_BOUNCE_CURSOR_INPUT,FRUIT_RIGHT_BOUNCE_WEIGHT_INPUT);

            updateGraphicBounce(_delta, 
                m_isBouncingTowardsLeft, 
                m_bouncingTowardsLeft_Time, m_bouncingTowardsLeft_Duration,
                FRUIT_LEFT_BOUNCE_CURSOR_INPUT, FRUIT_LEFT_BOUNCE_WEIGHT_INPUT);

            updateGraphicBounce(_delta, 
                m_isBouncingUpward, 
                m_bouncingUpward_Time, m_bouncingUpward_Duration,
                FRUIT_UPWARD_BOUNCE_CURSOR_INPUT, FRUIT_UPWARD_BOUNCE_WEIGHT_INPUT);
        }

        return isFinished;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::updateStickAndSnap()
    {
        ITF_ASSERT_CRASH(m_aiComponent!=NULL, "No AI");

        Ray_FruitAIComponent *fruitComponent = m_aiComponent->DynamicCast<Ray_FruitAIComponent>(ITF_GET_STRINGID_CRC(Ray_FruitAIComponent,1618051749));
        ITF_ASSERT_CRASH(fruitComponent!=NULL, "Not on fruit");

        if (fruitComponent->getCharacterOnWhichSnappedOrStuck().isValid())
            return;
        PhantomComponent *phantomComponent = m_actor->GetComponent<PhantomComponent>();
        ITF_ASSERT_CRASH(phantomComponent!=NULL, "no phantom component");

        PhysContactsContainer contacts;

        Ray_EventFruitAttachQuery queryEvent(m_actor->getRef(),m_physComponent->getSpeed(),m_actor->get2DPos());
        EventInteractionQuery interactionQuery;
        PHYSWORLD->collidePhantoms(
            m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
            phantomComponent->getShape(),
            m_actor->getDepth(),static_cast<ECollisionFilter>(ECOLLISIONFILTER_CHARACTERS|ECOLLISIONFILTER_ENVIRONMENT),
            contacts);

        for ( u32 index = 0; index < contacts.size(); index++ )
        {
            const SCollidableContact& contact = contacts[index];

            if ( contact.m_collidableUserData == m_actor->getRef() )
            {
                continue;
            }

            if (RAY_GAMEMANAGER->isPlayerActor(contact.m_collidableUserData))
            {
                //special handling for player. This is not done here
                continue;
            }

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                if ( fruitComponent->tryStickOnPolyline(contact.m_collidableUserData,contact.m_edgeIndex) )
                {
                    return;
                }
                else
                {
                    continue;
                }
            }


            if ( fruitComponent->isActorInMemory(contact.m_collidableUserData) )
            {
                continue;
            }

            Actor* actor = AIUtils::getActor(contact.m_collidableUserData);

            if ( !actor )
            {
                continue;
            }

            if (getTemplate()->getCanAttach())
            {
                actor->onEvent(&queryEvent);

                if ( queryEvent.getAttached() )
                {
                    if ( queryEvent.getUseSnap() )
                    {
                        fruitComponent->requestDelayedSnapToCharacter(actor->getRef());
                    }
                    else if ( fruitComponent->getUnstickableRemainingTime() == 0.0f )
                    {
                        fruitComponent->requestDelayedStickToCharacter(actor->getRef());
                    }
                    return;
                }
            }

            // only bounce off characters if going down        
            if ( m_physComponent->getSpeed().m_y < 0.0f &&
                !fruitComponent->isActorOnPolyLine(contact.m_collidableUserData) &&    //m_actorsOnPolyline.find(contact.m_collidableUserData)== -1 &&
                !fruitComponent->isActorHangingOnPolyline(contact.m_collidableUserData) && //m_actorsHangingOnPolyline.find(contact.m_collidableUserData)== -1 &&
                contact.m_contactPoint.m_y <= m_actor->getPos().m_y )
            {
                actor->onEvent(&interactionQuery);
                bbool interactionDone = GAMEMANAGER->isPlayerActor(actor->getRef());
                if (interactionQuery.getInteraction() == CharacterInteractionType_Crush)
                {
                    EventCrushed crushed;
                    crushed.setSender(m_actor->getRef());
                    crushed.setDirection(Vec2d(0, -1));
                    crushed.setFxPos( contact.m_contactPoint.to3d(actor->getPos().m_z) );

                    actor->onEvent(&crushed);
                    interactionDone = btrue;
                }
                /*
                if (interactionDone)
                {
                    startRoam(btrue);
                }*/
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::multiplySpeed(f32 _tangentialFactor, f32 _normalFactor, const Vec2d &_edgeDir)
    {
        
        ITF_ASSERT(f32_Abs(_edgeDir.norm()-1)<0.01f);
        const Vec2d speed = m_physComponent->getSpeed();
        Vec2d normal = _edgeDir.getPerpendicular();
        f32 tpart=speed.dot(_edgeDir);
        f32 npart=speed.dot(normal);
        Vec2d newSpeed(tpart*_edgeDir+npart*normal);
        m_physComponent->setSpeed(newSpeed);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::reactToOtherFruits()
    {
        static ActorList actorList;
        AABB searchZone = m_actor->getAABB();
        const Vec2d margin(1,1);
        searchZone.setMin(searchZone.getMin()-margin);
        searchZone.setMax(searchZone.getMax()+margin);


        Vec2d center1 = m_actor->get2DPos();
        f32 radius1 = m_physComponent->getRadius();
        if (radius1==0)
        {
            return;
        }
        actorList.clear();
        AI_MANAGER->getActorsFromLayerWithAIComponent<Ray_FruitAIComponent>(m_actor->getDepth(),searchZone, actorList);
        for (u32 i=0; i<actorList.size(); i++)
        {
            Actor *actor = actorList[i];
            if (actor==m_actor)
            {
                continue;
            }
            PhysComponent *phys2 = actor->GetComponent<PhysComponent>();
            if (!phys2)
            {
                continue;
            }

            const Vec2d &center2 = actor->get2DPos();
            f32 radius2 = phys2->getRadius();
            Vec2d deltaCenter(center1-center2);
            f32 dist = deltaCenter.norm();
            if (dist<=(radius1+radius2))
            {
                Vec2d forceDirection = deltaCenter;
                forceDirection.normalize();
                f32 currentSpeed = getTemplate()->getMaxBounceSpeed();
                const f32 verticalDirectionSin = 0.8f;
                bbool isVerticalBounce = fabsf(forceDirection.m_y)>verticalDirectionSin;
                bbool canAddAntigravity = btrue;
                if (getTemplate()->getIsStaticOnX() && isVerticalBounce)
                {

                    forceDirection.m_x = 0;
                    forceDirection.normalize();
                    currentSpeed = m_bounceInitialSpeed;
                    if (forceDirection.m_y<0)
                    {
                        forceDirection.m_y = 0;
                        canAddAntigravity = bfalse; //This is because we don't add force, but we want to keep falling - otherwise they are stuck
                    }
                }
                else
                {
                    currentSpeed *= getTemplate()->getInterreaction_maxBounceSpeedMultiplier();
                    if (forceDirection.m_y>0)
                        forceDirection.m_y += getTemplate()->getInterreaction_addVerticalSpeed();

                }
                float otherMass = actor->getLocalInitialScale().m_x/m_actor->getLocalInitialScale().m_x;
                float massInfluenceOnBounce = getTemplate()->getMassInfluenceOnBounce();
                otherMass = otherMass*massInfluenceOnBounce+(1-massInfluenceOnBounce)*1;
                //otherMass = otherMass*otherMass;
                forceDirection *= otherMass;
                m_physComponent->setSpeed(forceDirection*currentSpeed);
                if (canAddAntigravity)
                {
                    m_physComponent->addForce(-PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth())*getTemplate()->getNormalGravityMultiplier());
                }
            }

        }
        actorList.clear();

    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::setState(State_t _newState, const void *_params)
    {
        bbool justChangedState=bfalse;

        if (m_state!=_newState)
        {
            m_timeInCurrentState = 0;
            m_state = _newState;
            justChangedState = btrue;
        }
        m_dontChangeAnimThisFrame = bfalse;
        m_physComponent->setSpeedMultiplier(Vec2d::One);
        m_physComponent->setGravityMultiplier(getTemplate()->getNormalGravityMultiplier());
        f32 contactSpeedMultiplier = 0.8f;
        f32 wallContactSpeedMultiplier = 0.f;

        switch(_newState)
        {

        case State_WallBounce:
            {
                ITF_ASSERT_CRASH(_params!=NULL, "no param for State_WallBounce");
                Vec2d &edgeDir = *(Vec2d*)_params;
                Vec2d normal = edgeDir.getPerpendicular();
#ifdef DEVELOPER_SMATHIEU
                LOG("%s entered wallbounce after %d frames", m_actor->getUserFriendly().cStr(), CURRENTFRAME-m_lastFrameLeftBounce);
                LOG(" m_actor->set2DPos(Vec2d(%f,%f));edgeDir.set(%f,%f); normal.set(%f,%f);"
                    "m_physComponent->setSpeed(Vec2d(%.f,%.f));m_previousSpeed.set(%f,%f);"
                    "justChangedState=%d",
                    m_actor->getPos().m_x, m_actor->getPos().m_y,
                    edgeDir.m_x,edgeDir.m_y, normal.m_x,normal.m_y,
                    m_physComponent->getSpeed().m_x, m_physComponent->getSpeed().m_y, m_previousSpeed.m_x, m_previousSpeed.m_y,
                    (i32)justChangedState);
#endif
                if (justChangedState)
                {
                    m_speedWhenEnteredWallBounce = m_physComponent->getSpeed();                
                    m_physComponent->setSpeed(Vec2d(0,0));
                }

                setBounceAnimFromNormalAndPrevSpeed(normal, m_previousSpeed);
                m_dontChangeAnimThisFrame = btrue;
                //multiplySpeed(0.1f,0.1f, edgeDir);
                m_physComponent->setSpeedMultiplier(Vec2d(wallContactSpeedMultiplier,wallContactSpeedMultiplier));

            } break;
        case State_PreJump:
            {
                ITF_ASSERT_CRASH(_params!=NULL, "no param for prejump");
                PreJumpParameter &preJumpParam = *(PreJumpParameter*) _params;
                Vec2d &edgeDir = preJumpParam.m_normalizedEdge;
                Vec2d normal = edgeDir.getPerpendicular();

#ifdef DEVELOPER_SMATHIEU
                LOG("%s entered prejump after %d frames", m_actor->getUserFriendly().cStr(), CURRENTFRAME-m_lastFrameLeftBounce);
                LOG(" m_actor->set2DPos(Vec2d(%f,%f));edgeDir.set(%f,%f); normal.set(%f,%f);"
                    "m_physComponent->setSpeed(Vec2d(%.f,%.f));m_previousSpeed.set(%f,%f);"
                    "justChangedState=%d",
                    m_actor->getPos().m_x, m_actor->getPos().m_y,
                    edgeDir.m_x,edgeDir.m_y, normal.m_x,normal.m_y,
                    m_physComponent->getSpeed().m_x, m_physComponent->getSpeed().m_y, m_previousSpeed.m_x, m_previousSpeed.m_y,
                    (i32)justChangedState);
#endif
                if (justChangedState)
                {
                    m_edgeWhenEnteredPreJump = edgeDir;
                    m_speedWhenEnteredPreJump = m_previousSpeed;//m_physComponent->getSpeed();
                }
                setBounceAnimFromNormalAndPrevSpeed(normal, m_speedWhenEnteredPreJump, !preJumpParam.m_isFromActor);
                //m_physComponent->setSpeed(Vec2d(0,0));//m_physComponent->setSpeedMultiplier(Vec2d::Zero);
                //multiplySpeed(0.1f,0.1f, edgeDir);
                m_physComponent->setSpeedMultiplier(Vec2d(contactSpeedMultiplier,contactSpeedMultiplier));

            } break;
        case State_JumpOrFall:
            {
                StringID jumpingAnim = m_useComplexAnimationSystem ? FRUIT_JUMPING_ANIM : getTemplate()->getStandAnimationName();
                bounceUtility_EnterJumpOrFall(jumpingAnim, m_physComponent, m_animComponent);
            }
            break;

        case State_FloatingBeforeFalling:
            {
                m_physComponent->setGravityMultiplier(0);
                if( m_useComplexAnimationSystem )
                {
                    setCurrentAnim(FRUIT_APEX_ANIM);
                    SubAnim *subAnim = m_animatedComponent->getSubAnim(FRUIT_APEX_ANIM);

                    f32 animDuration;
                    if (subAnim)
                        animDuration =  subAnim->getDuration();
                    else
                        animDuration = 1.f;

                    f32 targetDuration = getTemplate()->getFloatTimeBeforeFalling();
                    m_animatedComponent->setInput("apexPlayRate", animDuration/targetDuration);
                }

                m_apexPoint = m_actor->get2DPos()+getTemplate()->getStartToFloatTime()*m_physComponent->getSpeed();
                
            } break;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::bounceOnPlayers(bbool _checkSpeed)
    {
        if (!getTemplate()->getCanBounceOnHead())
        {
            return;
        }
        static ActorList actors;
        actors.clear();
        PhysContactsContainer contacts;


        PhysSweepInfo sweepInfo;
        PhantomComponent *phantom = m_actor->GetComponent<PhantomComponent>();
        if (!phantom)
            return;
        PhysShape *shape = phantom->getShape();
        if (!shape)
            return;
        PhysCollisionSolver::calculateSweepInfo(m_previousPos, 
            m_actor->get2DPos(), 
            m_actor->getAngle(),
            shape,
            sweepInfo);
        


        AIUtils::getPlayers(m_actor->getDepth(), actors);
        Vec2d speed = m_physComponent->getSpeed();
        f32 collisionTime = FLT_MAX;
        bbool reactOnCollision = btrue;
        if (_checkSpeed && speed.m_y>=0)
        {
            reactOnCollision = bfalse;
        }
        if (getRider().isValid())
        {
            reactOnCollision = bfalse;
        }

        for (u32 actorIndex=0; actorIndex<actors.size(); actorIndex++)
        {
            Actor *actor = actors[actorIndex];

            PhysSweepInfo otherSweepInfo;
            EventQueryPhysShape queryShape;
            actor->onEvent(&queryShape);
            const PhysShape *otherShape = queryShape.getPhysShape();
            if (!otherShape)
            {
                continue;
            }
            PhysCollisionSolver::calculateSweepInfo(queryShape.getPosition(),
                queryShape.getPosition(),
                queryShape.getAngle(),
                queryShape.getPhysShape(),
                otherSweepInfo);


            //
            contacts.clear();
            if (PhysCollisionSolver::collide(otherSweepInfo, sweepInfo, contacts))
            {
                Vec2d averageContact(0,0);
                f32 collisionTime_local = FLT_MAX;
                for (u32 i=0; i<contacts.size(); i++)
                {
                    f32 t = contacts[i].m_t;
                    collisionTime_local = std::min(collisionTime_local, t);
                    averageContact += contacts[i].m_contactPoint;
                }
                averageContact *= 1.f/contacts.size();

#ifdef ITF_SUPPORT_DEBUGFEATURE
                DebugDraw::circle(averageContact,m_actor->getDepth(), 0.2f);
#endif // ITF_SUPPORT_DEBUGFEATURE

                Vec2d toContact = averageContact-m_actor->get2DPos();
                toContact.normalize();
                f32 cosAngleForBouncableArea = f32_Cos(getTemplate()->getBounceOnCharacterAreaAngle().ToRadians());

                if (toContact.m_y>-cosAngleForBouncableArea)
                {
                    continue;
                }
                f32 sinAngleToBounceVerticallyOnCharacter = f32_Sin(getTemplate()->getAngleToBounceVerticallyOnCharacter().ToRadians());
                f32 slantingbounce = getTemplate()->getSlantingBounceOnCharacterAngle().ToRadians();

                //bouncing on player
                if (reactOnCollision)
                {
                    collisionTime = std::min(collisionTime_local, collisionTime);
                    //f32 sideBounceSpeed = 5;

                    if (toContact.m_x<=-sinAngleToBounceVerticallyOnCharacter)
                    {
                        //m_physComponent->setSpeed(Vec2d(sinf(slantingbounce), cosf(slantingbounce))*sideBounceSpeed);

                        f32 s,c;
                        f32_CosSin(slantingbounce, &c, &s);
                        m_lastBlockingEdge.set(c, -s);
                    }
                    else if (toContact.m_x>=sinAngleToBounceVerticallyOnCharacter)
                    {
                        f32 s,c;
                        f32_CosSin(slantingbounce, &c, &s);
                        //m_physComponent->setSpeed(Vec2d(-sinf(slantingbounce), cosf(slantingbounce))*sideBounceSpeed);
                        m_lastBlockingEdge.set(c, s);
                    }
                    else
                    {
                        //m_physComponent->setSpeed(Vec2d(0, 1)*sideBounceSpeed);  
                        m_lastBlockingEdge.set(1,0);
                    }
                }                

            }
        }
        ITF_ASSERT(collisionTime>0 && collisionTime<=1 || collisionTime==FLT_MAX);
        if (collisionTime>=0 && collisionTime<=1)
        {
            collisionTime=0; //temporary
            Vec2d translatedThisFrame = m_actor->get2DPos()-m_previousPos;
            Vec2d correctedTranslation = translatedThisFrame*collisionTime;
            m_actor->set2DPos(m_actor->get2DPos()-translatedThisFrame+correctedTranslation);
        }
        actors.clear();
    }

#ifdef ITF_SUPPORT_EDITOR
/*
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const
    {
        if (m_currentAnim==FRUIT_PREJUMP_ANIM)
        {
            char pBuffer[256];
            sprintf(pBuffer, "procedural %d %d %d", m_isBouncingTowardsLeft,m_isBouncingTowardsRight, m_isBouncingUpward);
            //LOG("%s", pBuffer);
            if (m_isBouncingUpward)
                DebugDraw::circle(m_actor->getPos(), 0.1f, Color::green(), 0.f, pBuffer);
            if (m_isBouncingTowardsLeft)
                DebugDraw::circle(m_actor->getPos(), 0.2f, Color::violet(), 0.f, pBuffer);
            if (m_isBouncingTowardsRight)
                DebugDraw::circle(m_actor->getPos(), 0.2f, Color::blue(), 0.f, pBuffer);
        }
    }
*/
#endif // ITF_SUPPORT_EDITOR

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::setBounceAnimFromNormalAndPrevSpeed(const Vec2d &_normal, const Vec2d &_prevSpeed, bbool _squash)
    {

        f32 squashSpeed = -_prevSpeed.dot(_normal);
//        f32 squashSpeed = _prevSpeed.norm();
        f32 squashImportance = squashSpeed;
        squashImportance = std::max(squashImportance,0.f);
        squashImportance *= 1/getTemplate()->getSpeedForMaxSquash();
        squashImportance = Clamp(squashImportance,0.f,1.f) * getTemplate()->getMaxSquashFactor();

        squashSpeed = -_prevSpeed.dot(_normal);
        f32 clampedToNZSpeed = std::max(squashSpeed,0.001f);
        f32 preBounceDuration = (getTemplate()->getSpeedForMaxBounceDuration()/clampedToNZSpeed)*getTemplate()->getMaxBounceDuration();
        preBounceDuration = std::min(preBounceDuration, getTemplate()->getMaxBounceDuration());

        f32 rightWeight = std::max(_normal.m_x,0.f);
        f32 leftWeight = std::max(-_normal.m_x, 0.f);
		/*
        static f32 lateralDurationFactor = 0.5f;p
        if (rightWeight>0.5f || leftWeight>0.5f)
        {
            preBounceDuration *= lateralDurationFactor;
        }*/

        float timeScale = m_actor->getLocalInitialScale().m_x;
		preBounceDuration = std::max(preBounceDuration, getTemplate()->getMinBounceDuration())*timeScale;
        if (!_squash)
            preBounceDuration = 0;

        if (CURRENTFRAME<(m_lastFrameLeftBounce+5))
        {
            preBounceDuration = 0;
            squashImportance = 0;            
        }

        if (!m_isInBounceFreeze)
        {
            m_timeInProceduralBounce=0;
            m_preBounceDuration = preBounceDuration;
            m_isInBounceFreeze = btrue;
            if( m_useComplexAnimationSystem )
            {
                setCurrentAnim(FRUIT_PREJUMP_ANIM);
            }

            //launch FXs
            launchOnBounceFx();
            if( m_bouncesCount == 0 )
            {
                launchAfterFirstBounceFX();
            }

            ++m_bouncesCount;
            m_timeSinceLastBounce = 0.0f;
        }

        //normal is towards right
        static f32 lateralScale = 1.f;
        if (rightWeight>0.01f && !m_isBouncingTowardsRight && _squash)
        {
            
            m_bouncingTowardsRight_Weight = std::min(rightWeight*squashImportance*lateralScale, 1.f);
            m_bouncingTowardsRight_Time = 0;
            m_bouncingTowardsRight_Duration = preBounceDuration;
            m_isBouncingTowardsRight = btrue;
            if( m_useComplexAnimationSystem )
            {
                m_animatedComponent->setInput(FRUIT_RIGHT_BOUNCE_WEIGHT_INPUT, m_bouncingTowardsRight_Weight);
            }
        }

        //normal is towards left
        if (leftWeight>0.01f && !m_isBouncingTowardsLeft && _squash)
        {
            m_bouncingTowardsLeft_Weight = std::min(leftWeight*squashImportance*lateralScale,1.f);
            m_bouncingTowardsLeft_Time = 0;
            m_bouncingTowardsLeft_Duration = preBounceDuration;
            m_isBouncingTowardsLeft = btrue;
            if( m_useComplexAnimationSystem )
            {
                m_animatedComponent->setInput(FRUIT_LEFT_BOUNCE_WEIGHT_INPUT, m_bouncingTowardsLeft_Weight);
            }
        }

        //normal is upward
        f32 upWeight = std::max(_normal.m_y, 0.f);
        if (upWeight>0 && !m_isBouncingUpward && _squash)
        {
            m_bouncingUpward_Weight = upWeight*squashImportance;
            m_bouncingUpward_Time = 0;
            m_bouncingUpward_Duration = preBounceDuration;
            m_isBouncingUpward = btrue;
            if( m_useComplexAnimationSystem )
            {
                m_animatedComponent->setInput(FRUIT_UPWARD_BOUNCE_WEIGHT_INPUT, m_bouncingUpward_Weight);
            }
        }

        updateProceduralPreBounceAnim(0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::bounceUtility_UpdateBounceOnWall_onBouncingOnWall(const Vec2d &_wallNormal, const Vec2d &_newSpeed)
    {
        Vec2d edgeDir = -_wallNormal.getPerpendicular();
        setBounceAnimFromNormalAndPrevSpeed(_wallNormal, -_newSpeed, !m_bouncingOnWall_isFromCharacter);
        setState(State_WallBounce, &edgeDir);        
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::update( f32 _delta  )
    {
        Super::update(_delta);

        if (m_reinitSpeed)
        {
            m_physComponent->setSpeed(getTemplate()->getInitialSpeed());
            m_reinitSpeed = bfalse;
        }

        updateStickAndSnap();
        updateYScale();

        m_timeInCurrentState += _delta;
        bbool isProceduralBounceFinished = updateProceduralPreBounceAnim(_delta);
        switch(m_state)
        {
        case State_WallBounce:
            {                
                bounceOnPlayers(bfalse); //it is important to process that before anything, as we modify collisions
                reactToOtherFruits();
                if (isProceduralBounceFinished)
                {
#ifdef DEVELOPER_SMATHIEU
                    LOG("- %s Leaving wallbounce", m_actor->getUserFriendly().cStr());
#endif
                    m_lastFrameLeftBounce = CURRENTFRAME;
                    m_physComponent->setSpeed(m_speedWhenEnteredWallBounce/*+m_physComponent->getSpeed()*/);
                    setState(State_JumpOrFall);
                }
            } break;
        case State_PreJump:
            {
                bounceOnPlayers(bfalse); //it is important to process that before anything, as we modify collisions
                reactToOtherFruits();
                if (isProceduralBounceFinished)
                {
#ifdef DEVELOPER_SMATHIEU
                    LOG("- %s Leaving prejump", m_actor->getUserFriendly().cStr());
#endif
                    m_lastFrameLeftBounce = CURRENTFRAME;

                    m_physComponent->setSpeed(Vec2d(m_speedWhenEnteredPreJump.m_x,0.0f));//m_speedWhenEnteredPreJump/*+m_physComponent->getSpeed()*/);
                    bounceUtility_bounceOnEdge_applyBouncePhysics(m_physComponent, m_edgeWhenEnteredPreJump);
                    Vec2d currentSpeed = m_physComponent->getSpeed();
                    currentSpeed.m_y = 0.1f;
                    m_physComponent->setSpeed(currentSpeed);//m_speedWhenEnteredPreJump/*+m_physComponent->getSpeed()*/);
                    m_physComponent->unstick();
                    setState(State_JumpOrFall);
                }
            } break;
        case State_JumpOrFall:
            bounceOnPlayers(btrue); //it is important to process that before anything, as we modify collisions
            if( m_useComplexAnimationSystem )
            {
                bounceUtility_UpdateJumpOrFall(m_physComponent,
                    FRUIT_JUMPING_ANIM,
                    FRUIT_FALL_ANIM,
                    getTemplate()->getSpeedThatTriggersFloating(),
                    m_previousSpeed);
            }
            else
            {
                bounceUtility_UpdateJumpOrFall(m_physComponent,
                    getTemplate()->getStandAnimationName(),
                    getTemplate()->getStandAnimationName(),
                    getTemplate()->getSpeedThatTriggersFloating(),
                    m_previousSpeed);
            }
            reactToOtherFruits();
            break;
        case State_FloatingBeforeFalling:
            {
                bounceOnPlayers(btrue);
                reactToOtherFruits();
                if (!bounceUtility_updateBounceOnWall(m_physComponent))
                {

                    Vec2d tmpEdge;
                    bbool isFromActor = bfalse;

                    if (bounceUtility_isOnGroundEdge(m_physComponent, tmpEdge, isFromActor))
                    {
                        setState(State_JumpOrFall);
                    }
                    else
                    {
                        f32 mass = m_physComponent->getWeight();
                        Vec2d speed = m_physComponent->getSpeed();
                        Vec2d force = bounceUtility_getForceToReach(mass, speed, m_actor->get2DPos(), m_apexPoint, getTemplate()->getFloatConstantForceDuration(), getTemplate()->getFloatMaxForce());
                        force.m_x = 0;
                        if (force.m_y<0)
                            force.m_y *= 0.2f;
                        m_physComponent->addForce(force);

                        if (m_timeInCurrentState>getTemplate()->getFloatTimeBeforeFalling())
                        {
                            setState(State_JumpOrFall);
                        }
                    }                
                }
            } break;
        }

        m_previousSpeed = m_physComponent->getSpeed();
#ifndef ITF_FINAL
        //LOG("speed was : %.1f, %.1f", m_previousSpeed.m_x, m_previousSpeed.m_y);
        //if (getRider().isValid())
        //{
        //    DebugDraw::text(m_actor->getPos(), Color::red(), "has rider");
        //}
#endif // ITF_FINAL

        if( m_useComplexAnimationSystem )
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedY,461177981), f32_Abs(m_physComponent->getSpeed().m_y));
        }

        /*
        if (getTemplate()->getIsStaticOnX())
        {
            Vec2d p = m_actor->getLocal2DPos();
            p.m_x = m_actor->getLocalInitialPos().m_x;
            m_actor->setLocal2DPos(p);           
        }
        */

        m_dontChangeAnimThisFrame = bfalse;
        m_previousPos = m_actor->get2DPos();
        bounceUtility_clearCurrentBounceCollision();

        m_timeSinceLastBounce += _delta;

        updateRotation(_delta);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::onActivate()
    {
        m_previousSpeed = m_physComponent->getSpeed();
        resetState();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::onDeactivate()
    {
        m_physComponent->setGravityMultiplier(1.f);
        m_physComponent->setSpeedMultiplier(Vec2d::One);

        stopAfterFirstBounceFX();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::setRider(const ActorRef &_ref)
    {
        m_rider = _ref;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const ActorRef &Ray_AIFruitRoamingBehavior::getRider()
    {
        if (m_rider.isValid() && !RAY_GAMEMANAGER->isPlayerActor(m_rider))
        {
            m_rider.invalidate();
        }
        return m_rider;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIFruitRoamingBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        bounceUtility_processEvent(_event, m_physComponent);

        if (EventInteractionQuery *interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
        {
            /*
            if (!getRider().isValid())
            {
                ActorRef playerRef(interactionQuery->getSender());
                if (RAY_GAMEMANAGER->isPlayerActor(playerRef))
                {
                    Actor *player = playerRef.getActor();
                    Vec2d playerPos = player->get2DPos();
                    if (playerPos.m_y>= m_actor->get2DPos().m_y)
                    {
                        PhysComponent *physComponent = player->GetComponent<PhysComponent>();
                        if (physComponent)
                        {
                            if (physComponent->getSpeed().m_y<0)
                            {
                                interactionQuery->setInteraction(CharacterInteractionType_Grab);
                                setRider(playerRef);
                            }
                        }
                    }
                }
            }
*/
        }

        if( EventSetDirection * setDirectionEvent = _event->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)) )
        {
            if( m_physComponent )
            {
                m_physComponent->setSpeed(setDirectionEvent->getDirection());
                m_reinitSpeed = bfalse;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_AIFruitRoamingBehavior::updateAnimInputs()
    {
        //static StringID s_bounceDirection = "BounceDirection";
        //m_animatedComponent->setInput(s_bounceDirection,m_bounceDirection);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_AIFruitRoamingBehavior::updateRotation(const f32 _dt)
    {
        const f32 rotationSpeed = getTemplate()->getRotationSpeed().ToRadians();
        if( rotationSpeed != 0.0f )
        {
            f32 moveXSign = 1.0f;
            if( m_physComponent )
            {
                if( m_physComponent->getSpeed().m_x >= 0.0f )
                {
                    moveXSign = -1.0f;
                }
            }
            m_actor->setAngle( m_actor->getAngle() + (_dt * rotationSpeed * moveXSign) );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_AIFruitRoamingBehavior::launchAfterFirstBounceFX()
    {   
        if( m_fxController )
        {
            const StringID & fxAfterFirstBounceName = getTemplate()->getFXAfterFirstBounce();
            if( fxAfterFirstBounceName.isValid() )
            {
                m_fxHandle = m_fxController->playFX(fxAfterFirstBounceName);
            }
        }
    }

    void Ray_AIFruitRoamingBehavior::stopAfterFirstBounceFX()
    {
        if( m_fxController )
        {
            if( m_fxHandle != U32_INVALID )
            {
                if( m_fxController->isPlaying( m_fxHandle ) )
                {
                    m_fxController->stopFX( m_fxHandle );
                }
                m_fxHandle = U32_INVALID;
            }
        }
    }
    
    void Ray_AIFruitRoamingBehavior::launchOnBounceFx()
    {
        if( m_fxController )
        {
            const StringID & fxName = getTemplate()->getFXOnBounce();
            if( fxName.isValid() )
            {
                m_fxController->playFX(fxName);
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIFruitRoamingBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        Vec3d pos = m_actor->getPos();
        DebugDraw::text(m_actor->getPos(), Vec2d(0,0), Color::white(), Color::black(), "anim:%s - bounceDuration=%.3f", m_currentAnim.getDebugString(), m_preBounceDuration);
        f32 y = 0;
        f32 yspacing = 12;
        if (m_isBouncingUpward)
        {
            y-= yspacing;
            DebugDraw::text(m_actor->getPos(), Vec2d(0,y), Color::white(), Color::black(), "bounceUp:%d %%",(int)(m_bouncingUpward_Time/m_bouncingUpward_Duration*100) );
        }
        if (m_isBouncingTowardsLeft)
        {
            y-= yspacing;
            DebugDraw::text(m_actor->getPos(), Vec2d(0,y), Color::white(), Color::black(), "bounceLeft:%d %%",(int)(m_bouncingTowardsLeft_Time/m_bouncingTowardsLeft_Duration*100) );
        }
        if (m_isBouncingTowardsRight)
        {
            y-= yspacing;
            DebugDraw::text(m_actor->getPos(), Vec2d(0,y), Color::white(), Color::black(), "bounceRight:%d %%",(int)(m_bouncingTowardsRight_Time/m_bouncingTowardsRight_Duration*100) );
        }
    }
#endif // ITF_SUPPORT_EDITOR
}

