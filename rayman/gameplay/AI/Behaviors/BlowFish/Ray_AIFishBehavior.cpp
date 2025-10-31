#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIFISHBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/BlowFish/Ray_AIFishBehavior.h"
#endif //_ITF_RAY_AIFISHBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_


#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIReceiveCameraEjectHitAction.h"
#endif //_ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIFishBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIFishBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("struggle", m_struggle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("release", m_release, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("spikeHit", m_spikeHit, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("releaseSpeed",m_releaseSpeed);
        SERIALIZE_MEMBER("releaseAccel",m_releaseAccel);
        SERIALIZE_MEMBER("rotateOnRelease",m_rotateOnRelease);
        SERIALIZE_MEMBER("rotationSpeed",m_rotationSpeed);
        SERIALIZE_MEMBER("struggleOffsetAmplitude",m_struggleOffsetAmplitude);
        SERIALIZE_MEMBER("struggleOffsetFrequency",m_struggleOffsetFrequency);

    END_SERIALIZATION()

        Ray_AIFishBehavior_Template::Ray_AIFishBehavior_Template()
        : m_struggle(NULL)
        , m_release(NULL)
        , m_spikeHit(NULL)
        , m_releaseSpeed(1.0f)
        , m_releaseAccel(1.0f)
        , m_rotateOnRelease(btrue)
        , m_struggleOffsetFrequency(0.1f)
        , m_struggleOffsetAmplitude(1.0f)
    {

    }

    Ray_AIFishBehavior_Template::~Ray_AIFishBehavior_Template()
    {
        SF_DEL(m_struggle);
        SF_DEL(m_release);
        SF_DEL(m_spikeHit);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIFishBehavior)

        Ray_AIFishBehavior::Ray_AIFishBehavior()
        : m_struggle(NULL)
        , m_release(NULL)
        , m_spikeHit(NULL)
        , m_active(btrue)
        , m_subAnchorComponent(NULL)
        , m_phantomComponent(NULL)
        , m_tweenComponent(NULL)
        , m_releaseSpeed(0.0f)
        , m_releasePos(0.0f,0.0f,0.0f)
        , m_releaseDir(0.0f,0.0f)
        , m_prevPos(0.0f,0.0f)
        , m_timer(0.0f)
        , m_dangerLevel(0)
        , m_hitDir(0.0f,0.0f)
        , m_hitPos(0.0f,0.0f)
    {
    }

    Ray_AIFishBehavior::~Ray_AIFishBehavior()
    {
    }

    void Ray_AIFishBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_struggle = createAiAction(getTemplate()->getStruggle());
        m_release = createAiAction(getTemplate()->getRelease());
        m_spikeHit = createAiActionAndCast<Ray_AIReceiveCameraEjectHitAction>(getTemplate()->getSpikeHit());

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),m_aiComponent);

        m_subAnchorComponent = m_actor->GetComponent<Ray_SubAnchorComponent>();
        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
        m_tweenComponent = m_actor->GetComponent<TweenComponent>();

        m_active = btrue;

        m_releasePos = m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(release,3780715669));
        m_releaseSpeed  = 0.0f;

        startStruggle();
    }

    void Ray_AIFishBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
         if (m_currentAction == m_struggle)
        {
            updateStruggle(_delta);
        }
        else if (m_currentAction == m_release)
        {
            updateRelease(_delta);
        }
    }

    void Ray_AIFishBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        if (EventActivate * onActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            if (onActivate->getActivated() &&  m_currentAction == m_struggle)
            {
                startRelease();
            }
        }
        else if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (onTrigger->getActivated())
            {
                if (m_currentAction != m_release && m_currentAction != m_spikeHit)
                {
                    startRelease();
                }
            }
        }
    }


    void Ray_AIFishBehavior::onActionFinished()
    {
        Super::onActionFinished();
         if (m_currentAction == m_struggle)
        {
            onStruggleFinished();
        }
        else if (m_currentAction == m_release)
        {
            onReleaseFinished();
        }
        else if (m_currentAction == m_spikeHit)
        {
            onSpikeHitFinished();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //  .oooooo.               oooooooooooo  o8o               o8o           oooo                        .o8  
    // d8P'  `Y8b              `888'     `8  `"'               `"'           `888                       "888  
    //888      888 ooo. .oo.    888         oooo  ooo. .oo.   oooo   .oooo.o  888 .oo.    .ooooo.   .oooo888  
    //888      888 `888P"Y88b   888oooo8    `888  `888P"Y88b  `888  d88(  "8  888P"Y88b  d88' `88b d88' `888  
    //888      888  888   888   888    "     888   888   888   888  `"Y88b.   888   888  888ooo888 888   888  
    //`88b    d88'  888   888   888          888   888   888   888  o.  )88b  888   888  888    .o 888   888  
    // `Y8bood8P'  o888o o888o o888o        o888o o888o o888o o888o 8""888P' o888o o888o `Y8bod8P' `Y8bod88P" 
    //////////////////////////////////////////////////////////////////////////

    void Ray_AIFishBehavior::onStruggleFinished()
    {
    }

    void Ray_AIFishBehavior::onReleaseFinished()
    {
    }

    void Ray_AIFishBehavior::onSpikeHitFinished()
    {
        m_aiComponent->receiveHit(ActorRef(),m_dangerLevel,m_hitPos,m_hitPos.to3d(m_actor->getDepth()));
    }


    //////////////////////////////////////////////////////////////////////////
    //ooooo     ooo                  .o8                .             
    //`888'     `8'                 "888              .o8             
    // 888       8  oo.ooooo.   .oooo888   .oooo.   .o888oo  .ooooo.  
    // 888       8   888' `88b d88' `888  `P  )88b    888   d88' `88b 
    // 888       8   888   888 888   888   .oP"888    888   888ooo888 
    // `88.    .8'   888   888 888   888  d8(  888    888 . 888    .o 
    //   `YbodP'     888bod8P' `Y8bod88P" `Y888""8o   "888" `Y8bod8P' 
    //               888                                              
    //              o888o                                             
    //////////////////////////////////////////////////////////////////////////
    //void Ray_AIFishBehavior::updateIdle()
    //{
    //}

    //void Ray_AIFishBehavior::updateAppear(f32 _dt)
    //{
    //}

    void Ray_AIFishBehavior::updateStruggle(f32 _dt)
    {

    }

    void Ray_AIFishBehavior::updateRelease(f32 _dt)
    {
        checkContacts();

        m_releaseSpeed = Min(m_releaseSpeed + _dt * getTemplate()->getReleaseAccel(),getTemplate()->getReleaseSpeed());


        Vec2d newPos = m_actor->get2DPos() + m_releaseDir * m_releaseSpeed * _dt;
        m_prevPos = m_actor->get2DPos();
        m_actor->set2DPos(newPos);
        if (getTemplate()->getRotateOnRelease())
        {
            m_actor->setAngle(m_actor->getAngle() + _dt * getTemplate()->getRotationSpeed().Radians());
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // .oooooo..o     .                          .   
    //d8P'    `Y8   .o8                        .o8   
    //Y88bo.      .o888oo  .oooo.   oooo d8b .o888oo 
    // `"Y8888o.    888   `P  )88b  `888""8P   888   
    //     `"Y88b   888    .oP"888   888       888   
    //oo     .d8P   888 . d8(  888   888       888 . 
    //8""88888P'    "888" `Y888""8o d888b      "888" 
    //////////////////////////////////////////////////////////////////////////

    void Ray_AIFishBehavior::startStruggle()
    {
        m_timer = 0.0f;
        m_releaseDir = m_releasePos.truncateTo2D() - m_actor->get2DPos();
        m_releaseDir.normalize();
        m_prevPos = m_actor->get2DPos();
        //setAction(m_struggle);
    }

    void Ray_AIFishBehavior::startRelease()
    {
        m_tweenComponent->suspendTweening();
        setAction(m_release);
        m_releaseDir = m_releasePos.truncateTo2D() - m_actor->get2DPos();
        m_releaseDir.normalize();
        m_releaseSpeed = 0.0f;
    }

    void Ray_AIFishBehavior::startSpikeHit()
    {
        setAction(m_spikeHit);
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_AIFishBehavior::checkContacts()
    {
        PhysContactsContainer envTouchedContacts;

        PHYSWORLD->checkEncroachment( m_actor->get2DPos(), m_prevPos, m_actor->getAngle(), m_phantomComponent->getShape(),ECOLLISIONFILTER_ENVIRONMENT, m_actor->getDepth(), envTouchedContacts );
        //PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_prevPos,m_actor->getAngle(),
        //    m_phantomComponent->getShape(),m_actor->getDepth(),ECOLLISIONFILTER_ALL,envTouchedContacts);

        u32 dangerLevel = 0;

        i32 contactIndex = -1;
        f32 time = FLT_MAX;
        for ( u32 i = 0; i < envTouchedContacts.size(); i++ )
        {
            const SCollidableContact& contact = envTouchedContacts[i];

            if (  contact.m_collidableUserData == m_actor->getRef() )
            {
                envTouchedContacts.eraseKeepOrder(i);
                i--; 
                continue;
            }

            Actor * actor = ActorRef(contact.m_collidableUserData).getActor();
            if (actor)
            {
                EventQueryIsDead query;
                actor->onEvent(&query);
                if (query.getIsDead())
                {
                    envTouchedContacts.eraseKeepOrder(i);
                    i--;
                    continue;
                }

                Faction faction = AIUtils::getFaction(actor);
                if (faction == FACTION_UNKNOWN || faction == RAY_FACTION_NEUTRAL)
                {
                    envTouchedContacts.eraseKeepOrder(i);
                    i--;
                    continue;
                }
            }

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                PolyLine* poly(NULL);
                const PolyLineEdge* edge(NULL);
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

                if ( poly && ( poly->getOwnerActor() == m_actor ) )
                {
                    envTouchedContacts.eraseKeepOrder(i);
                    i--; 
                    continue;
                }                

                const GameMaterial_Template* envTouchedContactsGmat = World::getGameMaterial(edge->getGameMaterial());
                if (envTouchedContactsGmat)
                {
                    const Ray_GameMaterial_Template * gmat = envTouchedContactsGmat->DynamicCast<Ray_GameMaterial_Template>(ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804));
                    if (gmat)
                    {
                        dangerLevel = Max(gmat->getDangerousLevel(),dangerLevel);
                    }
                }
            }
            if (contact.m_t < time)
            {
                time = contact.m_t;
                contactIndex = i;
            }
        }		

        if ( contactIndex != -1 )
        {   
            if (dangerLevel > 0)
            {
                m_dangerLevel = dangerLevel;
                m_hitDir = -m_releaseDir;
                m_hitPos = envTouchedContacts[0].m_contactPoint;
                startSpikeHit();
            }
            else
            {
                const SCollidableContact& contact = envTouchedContacts[contactIndex];
                Vec2d normal = -contact.m_normal;

                Vec2d speed = m_actor->get2DPos() - m_prevPos;
                speed.normalize();
                if (speed.dot(normal) < 0.0f)
                {
                    f32 angle = (-normal).getAngle();
                    Vec2d dir = speed;
                    dir = dir.Rotate(-angle);
                    dir.m_x = -dir.m_x;
                    dir = dir.Rotate(angle);

                    m_releaseDir = dir;
                }
            }

            return btrue;
        }

        return bfalse;
    }

    void Ray_AIFishBehavior::onActivate()
    {
    }


    void Ray_AIFishBehavior::onDeactivate()
    {
        Super::onDeactivate();
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIFishBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
        Vec2d dir = m_releasePos.truncateTo2D() - m_actor->get2DPos();
        DebugDraw::arrow2D(m_actor->get2DPos(),m_actor->get2DPos() + dir,m_actor->getDepth());
    }

#endif // ITF_SUPPORT_EDITOR
}

