#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PUSHEDAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_PushedAIComponent.h"
#endif //_ITF_RAY_PUSHEDAICOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

    namespace ITF
    {
    IMPLEMENT_OBJECT_RTTI(Ray_PushedAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_PushedAIComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_PushedAIComponent::Ray_PushedAIComponent()
    : m_physComponent(NULL)
    , m_fxController(NULL)
    , m_soundComponent(NULL)
    , m_forcesThisFrame(Vec2d::Zero)
    , m_isBlocked(bfalse)
    , m_handleSoundSlide(U32_INVALID)
    {
            // none
    }

    //*****************************************************************************

    Ray_PushedAIComponent::~Ray_PushedAIComponent()
    {
            //none
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Components
        m_physComponent  = m_actor->GetComponent<StickToPolylinePhysComponent>();
        m_fxController   = m_actor->GetComponent<FXControllerComponent>();
        m_soundComponent = m_actor->GetComponent<SoundComponent>();


        ITF_ASSERT(m_physComponent);

        // Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPushForce,405779985),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryFaction,3678638658), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404),this);

        
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::Update( f32 _dt )
    {
        if ( !m_physComponent )
        {
            return;
        }

        f32 maxSpeed = getTemplate()->getMaxSpeed();
        Vec2d vecCurrentSpeed = m_physComponent->getSpeed();
        f32 currentSpeed = vecCurrentSpeed.norm();
        if (currentSpeed > maxSpeed )
        {
            vecCurrentSpeed.normalize();
            vecCurrentSpeed *= maxSpeed;
            m_physComponent->setSpeed(vecCurrentSpeed);
        }
            
        // Fx Sound
        if (m_fxController)
        {
            ObjectRef stickedPolyline = m_physComponent->getStickedPolylineRef();
            if (stickedPolyline.isValid())
            {
                if (m_handleSoundSlide == U32_INVALID && currentSpeed > 0.0f)
                {
                    m_handleSoundSlide = m_fxController->playFX(ITF_GET_STRINGID_CRC(Slide, 4289431478));
                }
                else if (m_handleSoundSlide != U32_INVALID &&  currentSpeed == 0.0f)
                {
                    m_fxController->stopFXFromHandle(m_handleSoundSlide);
                    m_handleSoundSlide = U32_INVALID;
                }
            }
            else
            {
                if (m_handleSoundSlide != U32_INVALID)
                {
                    m_fxController->stopFXFromHandle(m_handleSoundSlide);
                    m_handleSoundSlide = U32_INVALID;
                }
            }
 
        }
        

        if (m_soundComponent)
        {
            m_soundComponent->setInput(ITF_GET_STRINGID_CRC(Speed, 4218325830), currentSpeed);
        }

        // If obj is stuck add an inverse force
        if (m_isBlocked && !m_lastSpeed.IsEqual(Vec2d::Zero))
        {
            m_forcesThisFrame += (-m_lastSpeed * getTemplate()->getBounceMultiplier()) / _dt;
            m_isBlocked = bfalse;
        }
        m_lastSpeed = m_physComponent->getSpeed();

        if ( m_forcesThisFrame != Vec2d::Zero )
        {

            Vec2d force = m_forcesThisFrame * getTemplate()->getForceMultiplier();
                
            // Clamp speed X/Y separate
            force.m_x *= 1.0f - Clamp(vecCurrentSpeed.m_x / maxSpeed, 0.f, 1.f);
            force.m_y *= 1.0f - Clamp(vecCurrentSpeed.m_y / maxSpeed, 0.f, 1.f);
           
            // Apply force
            m_physComponent->addForce(force);
            m_forcesThisFrame = Vec2d::Zero;
        }
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if ( EventPushForce* evtOnPush = _event->DynamicCast<EventPushForce>(ITF_GET_STRINGID_CRC(EventPushForce,405779985)) )
        {
            processPush(evtOnPush);
        }
        else if ( PunchStim* evtOnHit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processHit(evtOnHit);
        }
        else if ( EventStickOnPolyline* evtOnStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            processStickOnPolyline(evtOnStick);
        }
        else if ( EventBlockedByPolyline * evtblockedOnPoly = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            processBlockedByPolyline(evtblockedOnPoly);
        }
        else if (EventQueryFaction * onQueryFaction = _event->DynamicCast<EventQueryFaction>(ITF_GET_STRINGID_CRC(EventQueryFaction,3678638658)))
        {
            onQueryFaction->setFaction(getTemplate()->getFaction());
        }
        else if (EventQueryCanStickOrCollide * onQueryCanStick = _event->DynamicCast<EventQueryCanStickOrCollide>(ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404)))
        {

            const ActorRef actorSender = onQueryCanStick->getSender();

            //bbool canStick = btrue;

            //for(u32 i = 0; i < m_actorSticked.size(); i++)
            //{
            //    ActorSticked& actorSticked = m_actorSticked[i];

            //    if (actorSender == actorSticked.m_actor)
            //    {
            //        u32 currentFrame = CURRENTFRAME;

            //        if (currentFrame - actorSticked.m_frame < 10.0f)
            //        {
            //            canStick = bfalse;
            //        }
            //    }
            //}


            //onQueryCanStick->setCanStick(canStick);
            //onQueryCanStick->setCanCollide(canStick);

        }

        

    }

    //*****************************************************************************

    bbool Ray_PushedAIComponent::canReceiveHit( PunchStim* _hit )
    {
        if ( _hit->getSender() == m_actor->getRef() )
        {
            return bfalse;
        }
        Faction faction = AIUtils::getFaction(m_actor);
        if (!AIUtils::isEnemyHit(_hit, faction) )
        {
            return bfalse;
        }

        return btrue;
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::addForce( Vec2d _force )
    {
        m_forcesThisFrame += _force;
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::processPush( EventPushForce* _push )
    {
        addForce(_push->getForce() * 0.1f); // TODO : expose
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::processHit( PunchStim* _hit )
    {
        if (!canReceiveHit(_hit))
        {
            return;
        }

        // Paf FX
        if (m_fxController)
        {
            const StringID feedbackAction = _hit->getActionFromHit();
            u32 fxHandle = m_fxController->playFeedback(_hit->getSender(), feedbackAction, m_actor->getRef());
            m_fxController->setFXPosFromHandle(fxHandle, _hit->getFxPos());
        }

        EReceivedHitType hitType = _hit->getReceivedHitType();
        u32 hitLevel = _hit->getLevel();
        Vec2d hitDir;
        f32 power = 0.0f;

        if ( hitType == RECEIVEDHITTYPE_UPPUNCH ||
             hitType == RECEIVEDHITTYPE_EJECTY)
        {
            hitDir = Vec2d::Up;
        }
        else
        {
            hitDir = _hit->getDirection();
            hitDir.normalize();
        }

        switch(hitLevel)
        {
        case 0:
        case 1:
            power = getTemplate()->getWeakHitForce();
            break;
        case 2:
            power = getTemplate()->getStrongHitForce();
            break;
        }

        Vec2d force = hitDir * power;

        if (m_isBlocked)
        {
            force = -force * getTemplate()->getBounceMultiplier();
            m_isBlocked = bfalse;
        }

        addForce(force);
    }

    //*****************************************************************************

    void Ray_PushedAIComponent::processStickOnPolyline( EventStickOnPolyline* _stickEvent )
    {
        const ActorRef actorSender = _stickEvent->getActor();

        if ( _stickEvent->isSticked() )
        {
            bbool canAddForce = btrue;

            for(u32 i = 0; i < m_actorSticked.size(); i++)
            {
                ActorSticked& actorSticked = m_actorSticked[i];

                if (actorSender == actorSticked.m_actor)
                {
                    u32 currentFrame = CURRENTFRAME;

                    if (currentFrame - actorSticked.m_frame < 60.0f)
                    {
                        canAddForce = bfalse;
                    }
                }
            }

            if (canAddForce)
            {
                Vec2d force = _stickEvent->getSpeed() * getTemplate()->getLandForceMultiplier();
                force.m_x *= getTemplate()->getLandXForceMultiplier();
                addForce(force);
            }
        }
        else
        {
            bbool found = bfalse;
            for(u32 i = 0; i < m_actorSticked.size(); i++)
            {
                ActorSticked& actorSticked = m_actorSticked[i];

                if (actorSender == actorSticked.m_actor)
                {
                    found = btrue;
                    actorSticked.m_frame = CURRENTFRAME;                  
                }
            }

            if (!found)
            {
                ActorSticked newActor;
                newActor.m_actor = actorSender;
                newActor.m_frame = CURRENTFRAME;

                m_actorSticked.push_back(newActor);
            }



        }
    }

    //*****************************************************************************
      
    void Ray_PushedAIComponent::processBlockedByPolyline( EventBlockedByPolyline * _evtBlockedOnPoly )
    {
        m_isBlocked = btrue;  
    }

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_PushedAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_physComponent)
        {
            DebugDraw::text("Speed : %f / %f", m_physComponent->getSpeed().m_x, m_physComponent->getSpeed().m_y);
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_PushedAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_PushedAIComponent_Template)
        SERIALIZE_MEMBER("faction",                 m_faction);
        SERIALIZE_MEMBER("forceMultiplier", m_forceMultiplier);
        SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
        SERIALIZE_MEMBER("bounceMultiplier", m_bounceMultiplier);
        SERIALIZE_MEMBER("weakHitForce", m_weakHitForce);
        SERIALIZE_MEMBER("strongHitForce", m_strongHitForce);
        SERIALIZE_MEMBER("landForceMultiplier",     m_landForceMultiplier);
        SERIALIZE_MEMBER("landXForceMultiplier",    m_landXForceMultiplier);
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_PushedAIComponent_Template::Ray_PushedAIComponent_Template()
    : m_faction(RAY_FACTION_PROJECTILE)
    , m_forceMultiplier(1.f)
    , m_maxSpeed(3.f)
    , m_bounceMultiplier(0.20f)
    , m_weakHitForce(800.0f)
    , m_strongHitForce(800.0f)
    , m_landForceMultiplier(25.0f)
    , m_landXForceMultiplier(5.0f)
        {
            // none
        }

    //*****************************************************************************

    Ray_PushedAIComponent_Template::~Ray_PushedAIComponent_Template()
    {
            // none
    }

    //*****************************************************************************



}
