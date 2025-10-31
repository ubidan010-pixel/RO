#include "precompiled_gameplay.h"

#ifndef _ITF_AISPAWNACTION_H_
#include "gameplay/ai/Actions/AISpawnAction.h"
#endif //_ITF_AISPAWNACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AISpawnAction_Template)

    BEGIN_SERIALIZATION_CHILD(AISpawnAction_Template)
        SERIALIZE_MEMBER                ( "path",           m_path);
        SERIALIZE_MEMBER                ( "bonePosName",    m_bonePosName);
        SERIALIZE_OBJECT_WITH_FACTORY   ( "onSpawnEvent",   m_onSpawnEvent,     ACTORSMANAGER->getEventFactory() );
        SERIALIZE_MEMBER                ( "nbSpawns",       m_nbSpawns);
        SERIALIZE_MEMBER                ( "angleStart",     m_angleStart);
        SERIALIZE_MEMBER                ( "angleStep",      m_angleStep);
    END_SERIALIZATION()

    AISpawnAction_Template::AISpawnAction_Template()
    : m_onSpawnEvent(NULL)
    , m_nbSpawns(1)
    , m_angleStart(bfalse, 0.0f)
    , m_angleStep(bfalse, 0.0f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(AISpawnAction)

    //-----------------------------------------------------------------------------------------------------
    AISpawnAction::AISpawnAction()
        : Super()
        , m_onSpawnEvent(NULL)
        , m_originalSender()
    {
    }

    //-----------------------------------------------------------------------------------------------------
    AISpawnAction::~AISpawnAction()
    {
        SF_DEL( m_onSpawnEvent );
    }

    //-----------------------------------------------------------------------------------------------------
    void AISpawnAction::onActivate()
    {
        Super::onActivate();

        Vec3d pos = m_actor->getPos();
        if (getTemplate()->getBonePosName().isValid())
        {
            u32 index = m_animComponent->getBoneIndex(getTemplate()->getBonePosName());
            if (index != U32_INVALID)
            {
                m_animComponent->getBonePos(index,pos);
            }
        }

        // check if the spawner is in a sub scene, if yes spawn the actor in the parent scene
        Scene * actorScene = m_actor->getScene();
        SubSceneActor* subSceneActor = actorScene->getSubSceneActorHolder();
        if ( subSceneActor != NULL )
        {
            actorScene = subSceneActor->getScene();
        }

        f32 angle = getTemplate()->getAngleStart().ToRadians();
        for(u32 spawnIndex = 0; spawnIndex < getTemplate()->getNbSpawns(); ++spawnIndex)
        {
            Actor * spawn = m_generator.getSpawnee(actorScene,pos,angle);
            if (spawn)
            {
                spawn->setStartFlipped(m_actor->isFlipped());
                
                // if world doesn't support dynamic loading, do it manually
                if (spawn->getScene()->getWorld()->getIgnoreCells())
                {
                    spawn->loadResources();
                }

                if ( m_onSpawnEvent != NULL )
                {
                    m_onSpawnEvent->setSender( m_actor->getRef() );
                    spawn->onEvent( m_onSpawnEvent );
                }

                if (m_originalSender.isValid())
                {
                    EventSetOriginalSender setOriginalSenderEvt;
                    setOriginalSenderEvt.setSender(m_actor->getRef());
                    setOriginalSenderEvt.set(m_originalSender);
                    spawn->onEvent(&setOriginalSenderEvt);
                }
            }

            angle += getTemplate()->getAngleStep().ToRadians();
        }

        m_aiBehavior->onActionFinished();
    }

    //-----------------------------------------------------------------------------------------------------
    void AISpawnAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        SPAWNER->declareNeedsSpawnee(m_actor, &m_generator, getTemplate()->getPath());

        if ( getTemplate()->getOnSpawnEvent() != NULL )
        {
            m_onSpawnEvent = static_cast<Event*>(getTemplate()->getOnSpawnEvent()->CreateNewObject() );
            BinaryClone(const_cast<Event*>(getTemplate()->getOnSpawnEvent()),m_onSpawnEvent );
        }

        // register events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572),m_aiComponent);
    }

    //-----------------------------------------------------------------------------------------------------
    void AISpawnAction::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventSetOriginalSender* setOriginalSenderEvt = _event->DynamicCast<EventSetOriginalSender>(ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572)))
        {
            m_originalSender = setOriginalSenderEvt->get();
        }
    }

}