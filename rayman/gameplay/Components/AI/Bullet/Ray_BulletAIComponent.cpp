#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BulletAIComponent.h"
#endif //_ITF_RAY_BULLETAICOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTORSPAWNCOMPONENT_H_
#include "gameplay/Components/Common/ActorSpawnComponent.h"
#endif //_ITF_ACTORSPAWNCOMPONENT_H_

#ifndef _ITF_AISPAWNACTION_H_
#include "gameplay/ai/Actions/AISpawnAction.h"
#endif //_ITF_AISPAWNACTION_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BulletAIComponent)
        BEGIN_SERIALIZATION_CHILD(Ray_BulletAIComponent)
    END_SERIALIZATION()
    BEGIN_VALIDATE_COMPONENT(Ray_BulletAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
        VALIDATE_COMPONENT_PARAM("", m_physComponent, "PhysComponent mandatory");
    END_VALIDATE_COMPONENT()

    Ray_BulletAIComponent::Ray_BulletAIComponent()
        : Super()
        , m_launchBehavior(NULL)
        , m_behavior(NULL)
        , m_deathBehavior(NULL)
        , m_deathNoStimBehavior(NULL)
        , m_physComponent(NULL)
        , m_polylineComponent(NULL)
        , m_originalSender()
        , m_direction(Vec2d::Zero)
        , m_timer(0.f)
        , m_noColltimer(0.f)
        , m_isEnvironmentDeath(bfalse)
        , m_nosePolyline("L_Col_Trigger")
        , m_stimPrevPos( Vec2d::Zero )
        , m_hasEnteredScreen( bfalse )
        , m_outOfScreenSafeTimer( 8.0f )
    {
    }

    Ray_BulletAIComponent::~Ray_BulletAIComponent()
    {
    }

    void Ray_BulletAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_launchBehavior = createAiBehavior(getTemplate()->getBehavior());
        m_behavior = createAiBehavior(getTemplate()->getBehavior());
        m_deathBehavior = createAiBehavior(getTemplate()->getDeathBehavior());
        m_deathNoStimBehavior = createAiBehavior(getTemplate()->getDeathNoStimBehavior());

        ITF_ASSERT(m_behavior);

        Super::onActorLoaded(_hotReload);

        m_physComponent = GetActor()->GetComponent<PhysComponent>();            // mandatory, validated
        m_polylineComponent = GetActor()->GetComponent<PolylineComponent>();    // not mandatory

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetDirection,187560938), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPause,829919267), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBulletLaunch,1261820746), this);

        reset();
    }

    void Ray_BulletAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

    }

    void Ray_BulletAIComponent::reset()
    {
        m_timer = getTemplate()->getLifeTime();
        m_noColltimer = getTemplate()->getActivateNoCollDuration();
        m_prevNosePoints.clear();
        if (m_physComponent)    // m_physComponent is mandatory but this is called onActorLoaded, not validated yet
        {
            m_physComponent->setSpeed(Vec2d::Zero);
        }

        m_stimPrevPos = Vec2d::Zero;
    }

    void Ray_BulletAIComponent::die(bbool _useStim /*= btrue*/)
    {
        m_health_current = 0;
        if (m_currentBehavior != m_deathBehavior || m_currentBehavior != m_deathNoStimBehavior)
        {
            m_physComponent->setDisabled(btrue);

            // NB[LaurentCou]: keep the polyline active for spoons when dying
            if (getTemplate()->getDisablePolylineOnDeath())
            {
                m_polylineComponent->setDisabled(btrue);
            }

            if (_useStim)
            {
                setBehavior(m_deathBehavior);
            }
            else
            {
                setBehavior(m_deathNoStimBehavior);
            }
        }

        if ( getTemplate()->isCameraRelative() )
        {
            CAMERACONTROLLERMANAGER->unbindObject( m_actor );
        }

        if ( ActorSpawnComponent * spawnComp = m_actor->GetComponent<ActorSpawnComponent>() )
        {
            spawnComp->destroySpawns();
        }
    }


    void Ray_BulletAIComponent::setDirection( const Vec2d& _direction )
    {
        ITF_ASSERT(m_currentBehavior == m_behavior);
        ITF_ASSERT(m_health_current == getTemplate()->getHealth_default());
        ITF_ASSERT(m_physComponent->getSpeed() == Vec2d::Zero);
        if (m_currentBehavior != m_behavior ||
            m_health_current != getTemplate()->getHealth_default())
        {
            ITF_ASSERT_MSG(0, "this shouldn't happen!");
            //GFX_ADAPTER->drawDBGCircle(m_actor->getPos(), 0.5f, Color::red(), 3.f);
        }

        // HACK: we shouldn't have to reset here, but sometimes
        // the above asserts are raised (current behavior and current health),
        // which means we haven't been properly cleaned...
        reset();



        m_direction = _direction;
        //GFX_ADAPTER->drawArrow(m_actor->getPos(), m_actor->getPos() + m_direction.to3d(), Color::red(), 0.1f, 1.f);

        m_physComponent->setSpeed(m_direction * getTemplate()->getSpeed());

        bbool flipped = m_direction.dot(Vec2d::Right) < 0.0f;
        m_actor->setFlipped(flipped);
        m_actor->setAngle( m_direction.getAngle() + (flipped ? -MTH_PI : 0) );
    }

    void Ray_BulletAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if ( getTemplate()->getDestroyOnExitScreen() )
        {
            checkOutOfScreen( _dt );
        }

        if (m_currentBehavior == m_deathBehavior || m_currentBehavior == m_deathNoStimBehavior)
        {
            return;
        }

        if (getHealth() <= 0)
        {
            die();
            return;
        }

        // Check contact with water
        PolyLine* swimPolyline = AIUtils::getPolylineRegion(m_actor->getDepth(), s_RegionName_Swim, m_actor->get2DPos());
        if (swimPolyline)
        {   
            die();
            return;
        }

        m_timer -= _dt;
        if (m_timer < 0)
        {
            die();
            return;
        }

        updateMovement(_dt);

        m_noColltimer = Max( 0.0f, m_noColltimer -_dt );

        checkNoseCollisions();
    }

    void Ray_BulletAIComponent::updateMovement( f32 _dt )
    {
        if (m_currentBehavior == m_launchBehavior)
        {
            return;    
        }
        
        if (_dt <= 0.0f)
        {
            return;
        }
        
        // maintain target speed + counter gravity
        Vec2d targetSpeed = m_direction * getTemplate()->getSpeed();
        Vec2d flyForce = (targetSpeed - m_physComponent->getSpeed()) / _dt;
        m_physComponent->addForce(flyForce - getCurrentGravity());
    }


    void Ray_BulletAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            processHit(hit);
        }
        else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventBulletLaunch,1261820746)) )
        {
            setBehavior(m_launchBehavior);
        }
        else if ( EventSetDirection* eventSetDirection = _event->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)) )
        {
            // start regular behavior
            setBehavior(m_behavior);
            
            // will spawn smoke/fire FX
            EventUndelaySpawn undelaySpawn;
            m_actor->onEvent(&undelaySpawn);

            setDirection(eventSetDirection->getDirection());

            if ( getTemplate()->isCameraRelative() )
            {
                CAMERACONTROLLERMANAGER->bindObject( m_actor );
            }

            // store original sender (for rewards' Action_KillHunterWithOwnBullet)
            m_originalSender = eventSetDirection->getSender();

            // set original sender in the action that spawns the explosion FX
            EventSetOriginalSender setOriginalSenderEvt;
            setOriginalSenderEvt.setSender(m_actor->getRef());
            setOriginalSenderEvt.set(m_originalSender);
            m_actor->onEvent(&setOriginalSenderEvt);
        }
        else if ( EventPause* eventPause = _event->DynamicCast<EventPause>(ITF_GET_STRINGID_CRC(EventPause,829919267)) )
        {
            if (!eventPause->getPause())
            {
                setDirection(AIUtils::getLookDir(m_actor, m_actor->isFlipped()));
            }
        }
    }

    void Ray_BulletAIComponent::checkNoseCollisions()
    {
        if (Ray_AIUtils::setupNoseShape(
                m_actor,
                m_animComponent,
                m_nosePolyline,
                m_noseShape,
                m_noseShapePoints,
                m_prevNosePoints
            ))
        {
            checkCollisions(&m_noseShape);
        }
    }

    void Ray_BulletAIComponent::checkOutOfScreen( f32 _dt )
    {
        const AABB screenAABB = CAMERACONTROLLERMANAGER->getAABB();
        if ( screenAABB.checkOverlap( m_actor->getAABB() ) )
        {
            m_hasEnteredScreen = btrue;
        }
        else if ( m_hasEnteredScreen )
        {
            directDestroy();
        }
        else 
        {
            m_outOfScreenSafeTimer -= _dt;
            if ( m_outOfScreenSafeTimer <= 0.f )
            {
                directDestroy();
            }
        }
    }

    void Ray_BulletAIComponent::directDestroy()
    {
        // destroy the actor
        if ( getTemplate()->isCameraRelative() )
        {
            CAMERACONTROLLERMANAGER->unbindObject( m_actor );
        }
        if ( ActorSpawnComponent * spawnComp = m_actor->GetComponent<ActorSpawnComponent>() )
        {
            spawnComp->destroySpawns( btrue );
        }
        if ( m_actor->isSerializable() )
        {
            m_actor->disable();
        }
        else
        {
            m_actor->requestDestruction();
        }
    }

    void Ray_BulletAIComponent::processHit( HitStim* _hit )
    {
        if (m_currentBehavior == m_deathBehavior || m_currentBehavior == m_deathNoStimBehavior)
        {
            return;
        }

        // don't get hit by our own stims
        if ( _hit->getSender() == m_actor->getRef() )
        {
            return;
        }
        
        receiveHit( _hit );

        // notify the sender that he touched us
        Actor* sender = static_cast<Actor*>(_hit->getSender().getObject());
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }

        if (m_fxController)
        {
            u32 fxHandle = m_fxController->playFeedback( _hit->getSender(), _hit->getActionFromHit(), m_actor->getRef() );
            m_fxController->setFXPosFromHandle(fxHandle, _hit->getFxPos());
        }

        // check health (seb modif to manage different hp)
        if ( getHealth() <= 0 )
        {
            die(bfalse);
        }
    }

    void Ray_BulletAIComponent::sendDeathStim(Actor* target)
    {
        ITF_ASSERT(target);
        if (!target)
        {
            return;
        }
        
        
        // send punch stim on death
        PunchStim punch;
        punch.setLevel(1);
        punch.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        punch.setDirection(Vec2d::Up);
        punch.setFxPos(m_actor->getPos());
        punch.setDepth(m_actor->getDepth());
        punch.setSender(m_actor->getRef());
        punch.setAngle(m_actor->getAngle());
        punch.setPos(m_actor->getPos().truncateTo2D());
        punch.setPrevPos( m_stimPrevPos.IsEqual( Vec2d::Zero ) ?  m_actor->getPos().truncateTo2D() : m_stimPrevPos );
        punch.setFaction(getFaction());
        punch.setOriginalSender(m_originalSender);
        target->onEvent(&punch);
    }

    void Ray_BulletAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        static StringID s_HasActorsOnPolyline = "HasActorsOnPolyline";
        m_animatedComponent->setInput(s_HasActorsOnPolyline, m_polylineComponent && m_polylineComponent->hasActors());

        static StringID s_IsEnvironmentDeath = "IsEnvironmentDeath";
        m_animatedComponent->setInput(s_IsEnvironmentDeath, m_isEnvironmentDeath);
    }



    void Ray_BulletAIComponent::checkCollisions( const PhysShape* _shape )
    {
        if ( m_noColltimer > 0.0f )
        {
            return;
        }

        if (m_hasBeenVaccumed)
        {
            return;
        }


        PhysContactsContainer contacts;

        if (getTemplate()->getCollideWithEnvironment())
        {
            PHYSWORLD->checkEncroachment(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
                _shape,ECOLLISIONFILTER_ENVIRONMENT, m_actor->getDepth(),contacts);
        }

        if (getTemplate()->getCollideWithPhantoms())
        {
            PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
                _shape,m_actor->getDepth(),ECOLLISIONFILTER_ALL,contacts);
        }

        bbool hitOne = bfalse;

        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            SCollidableContact& contact = contacts[i];
            const ObjectRef& contactObj = contact.m_collidableUserData;

            if ( contactObj == m_actor->getRef() )
            {
                continue;
            }

            BaseObject* obj = contactObj.getObject();
            if (!obj)
            {
                continue;
            }

            if ( PolyLine* polyLine = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
            {
                // die on polylines

                if (polyLine->getOwnerActor() == m_actor)
                {
                    continue;
                }

                // no collision with environment in launch phase (bullets 
                // launchers are placed on friezes)
                if (m_currentBehavior == m_launchBehavior)
                {
                    continue;                
                }

                if (!getTemplate()->getCollideWithSolidEdges())
                {
                    // no collision with non-solid edges
                    const PolyLineEdge& edge = polyLine->getEdgeAt(contact.m_edgeIndex);
                    const GameMaterial_Template* mat = World::getGameMaterial(edge.getGameMaterial());
                    if (mat && !mat->isSolid())
                    {
                        continue;
                    }
                }

                //GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint.to3d(m_actor->getPos().m_z), 0.2f, Color::blue(), 1.f);
                hitOne = btrue;
                m_isEnvironmentDeath = btrue;
                break;
            }
            else if ( Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                // die on phantoms

                if (Ray_AIUtils::isBubblePrize(actor))
                {
                    continue;
                }

                EventQueryIsDead query;
                actor->onEvent(&query);
                if (query.getIsDead())
                {
                    continue;
                }


                //GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint.to3d(m_actor->getPos().m_z), 0.2f, Color::green(), 1.f);

                if (getTemplate()->getSendDeathStim())
                {
                    sendDeathStim(actor);
                }

                hitOne = btrue;
                break;
            }
        }

        if (hitOne)
        {
            die();
        }

        m_stimPrevPos = m_actor->get2DPos();
    }

    void Ray_BulletAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        PhysShapeCircle circle;
        circle.setRadius(m_physComponent->getRadius());
        checkCollisions(&circle);
    }
    
    void Ray_BulletAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if ( getTemplate()->getDestroyOnBecomeInactive() && !m_hasBeenVaccumed )
        {
            directDestroy();
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_BulletAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
            return;
            
        if (m_noseShape.isValid())
        {
            DebugDraw::shape(
                m_actor->get2DPos(), m_actor->getDepth(), m_actor->getAngle(),
                &m_noseShape, Color::red(), 1.f);
        }
    }
#endif // ITF_SUPPORT_EDITOR
    
    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BulletAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_BulletAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("launchBehavior", m_launchBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("genericBehavior", m_behavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathNoStimBehavior", m_deathNoStimBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("speed", m_speed);
        SERIALIZE_MEMBER("lifeTime", m_lifeTime);
        SERIALIZE_MEMBER("collideWithEnvironment", m_collideWithEnvironment);
        SERIALIZE_MEMBER("collideWithPhantoms", m_collideWithPhantoms);
        SERIALIZE_MEMBER("activateNoCollDuration", m_activateNoCollDuration);
        SERIALIZE_MEMBER("sendDeathStim", m_sendDeathStim);
        SERIALIZE_MEMBER("isCameraRelative", m_isCameraRelative);
        SERIALIZE_MEMBER("collideWithSolidEdges", m_collideWithSolidEdges);
        SERIALIZE_MEMBER("disablePolylineOnDeath", m_disablePolylineOnDeath);
        SERIALIZE_MEMBER("destroyOnExitScreen", m_destroyOnExitScreen);
        SERIALIZE_MEMBER("destroyOnExitScreen", m_destroyOnBecomeInactive);
    END_SERIALIZATION()

    Ray_BulletAIComponent_Template::Ray_BulletAIComponent_Template()
        : m_speed(2.f)
        , m_lifeTime(5.f)
        , m_collideWithEnvironment(btrue)
        , m_collideWithPhantoms(btrue)
        , m_launchBehavior(NULL)
        , m_behavior(NULL)
        , m_deathBehavior(NULL)
        , m_deathNoStimBehavior(NULL)
        , m_activateNoCollDuration( 0.0f )
        , m_sendDeathStim(bfalse)
        , m_isCameraRelative( bfalse )
        , m_collideWithSolidEdges(bfalse)
        , m_disablePolylineOnDeath(btrue)
        , m_destroyOnExitScreen( bfalse ) 
        , m_destroyOnBecomeInactive(btrue)
    {
    }

    Ray_BulletAIComponent_Template::~Ray_BulletAIComponent_Template()
    {
        SF_DEL(m_launchBehavior);
        SF_DEL(m_behavior);
        SF_DEL(m_deathBehavior);
        SF_DEL(m_deathNoStimBehavior);
    }
};
