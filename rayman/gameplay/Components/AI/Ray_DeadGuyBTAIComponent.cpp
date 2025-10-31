#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DEADGUYBTAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_DeadGuyBTAIComponent.h"
#endif //_ITF_RAY_DEADGUYBTAICOMPONENT_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_DeadGuyBTAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_DeadGuyBTAIComponent)

END_SERIALIZATION()

#define s_NumTeeth ITF_GET_STRINGID_CRC(NumTeeth,1768789271)
#define s_ReturnTeethFact ITF_GET_STRINGID_CRC(returnTeeth,3227169873)
#define s_InviteSequenceFact ITF_GET_STRINGID_CRC(inviteSequence,1461389631)
#define s_WasHitFact ITF_GET_STRINGID_CRC(wasHit,2162976019)

Ray_DeadGuyBTAIComponent::Ray_DeadGuyBTAIComponent()
: m_numTeeth(0)
, m_returningTeeth(0)
, m_firstUpdate(btrue)
{
}

Ray_DeadGuyBTAIComponent::~Ray_DeadGuyBTAIComponent()
{
}

void Ray_DeadGuyBTAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_numTeeth = RAY_GAMEMANAGER->getReturnedTeethCount();

    m_behaviorTree.getBlackBoard().setFact(s_NumTeeth,m_numTeeth);

    m_toothState.resize(getTemplate()->getMaxTeeth());
    m_toothState.fillZero();

    for ( u32 i = 0; i < m_numTeeth; i++ )
    {
        m_toothState[i] = TOOTHSTATE_SET;
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventReturnTeeth,810560716),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);

    m_firstUpdate = btrue;
}

void Ray_DeadGuyBTAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_firstUpdate )
    {
        if ( m_numTeeth == m_toothState.size() )
        {
            openDoor();
            m_actor->disable();
        }
        
        m_firstUpdate = bfalse;
    }

    if ( m_returningTeeth == 0 && !m_behaviorTree.getBlackBoard().factExists(s_ReturnTeethFact)  )
    {
        checkTriggerSequence();
    }
}

void Ray_DeadGuyBTAIComponent::openDoor()
{
    LinkComponent* link = m_actor->GetComponent<LinkComponent>();

    if ( link )
    {
        EventDisableCollision disableCol(btrue);
        
        link->sendEventToChildren(&disableCol);
    }
}

void Ray_DeadGuyBTAIComponent::checkTriggerSequence()
{
    if ( m_numTeeth >= RAY_GAMEMANAGER->getTotalTakenToothCount() 
#ifdef ITF_SUPPORT_CHEAT
        && !RAY_CHEATMANAGER->getIsCheatTeethSequence()
#endif
        )
    {
        return;
    }

    if ( RAY_GAMEMANAGER->isSequencePlaying() )
    {
        return;
    }

    Player* triggerPlayer = NULL;
    f32 seqDistSq = getTemplate()->getTeethSequenceDistance();
    f32 minDist = 0.f;

    seqDistSq *= seqDistSq;

    for ( u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++ )
    {
        Player* player = GAMEMANAGER->getPlayer(i);

        if ( player->getActive() && !player->isDead() )
        {
            Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

            if ( playerActor )
            {
                f32 distSq = ( playerActor->get2DPos() - m_actor->get2DPos() ).sqrnorm();

                if ( distSq <= seqDistSq )
                {
                    if ( !triggerPlayer || distSq < minDist )
                    {
                        triggerPlayer = player;
                        minDist = distSq;
                    }
                }
            }
        }
    }

    if ( triggerPlayer )
    {
#ifdef ITF_SUPPORT_CHEAT
        if ( RAY_CHEATMANAGER->getIsCheatTeethSequence() )
        {
            if ( m_numTeeth == m_toothState.size() )
            {
                m_numTeeth = 0;
                m_behaviorTree.getBlackBoard().setFact(s_NumTeeth,0);

                for ( u32 i = 0; i < m_toothState.size(); i++ )
                {
                    m_toothState[i] = TOOTHSTATE_NONE;
                }
            }
        }
#endif
        RAY_GAMEMANAGER->triggerReturnTeeth(triggerPlayer,m_actor);
    }
}

void Ray_DeadGuyBTAIComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Throw_Sequence,4059652305) )
        {
            processAnimEventThrowStart();
        }
        else if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Throw_Finished,1868209160) )
        {
            processAnimEventThrowEnd();
        }
    }
    else if (Ray_EventReturnTeeth* returnTeeth = _event->DynamicCast<Ray_EventReturnTeeth>(ITF_GET_STRINGID_CRC(Ray_EventReturnTeeth,810560716)))
    {
        processReturnTeeth(returnTeeth);
    }
    else if (PunchStim* punchEvent = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
    {
        processPunch(punchEvent);
    }
}

void Ray_DeadGuyBTAIComponent::processPunch( PunchStim* _punch )
{
    m_behaviorTree.getBlackBoard().setFact(s_WasHitFact,btrue);
}

void Ray_DeadGuyBTAIComponent::processAnimEventThrowStart()
{
    u32 totalTeeth = m_numTeeth + m_returningTeeth;

    totalTeeth = Min(totalTeeth,m_toothState.size());

    for ( u32 i = m_numTeeth; i < totalTeeth; i++ )
    {
        m_toothState[i] = TOOTHSTATE_FLYING;
    }

    RAY_GAMEMANAGER->returnTeethThrowStart();
}

void Ray_DeadGuyBTAIComponent::processAnimEventThrowEnd()
{
    u32 totalTeeth = m_numTeeth + m_returningTeeth;

    totalTeeth = Min(totalTeeth,m_toothState.size());

    for ( u32 i = m_numTeeth; i < totalTeeth; i++ )
    {
        m_toothState[i] = TOOTHSTATE_SET;
    }

    m_numTeeth = totalTeeth;
    m_behaviorTree.getBlackBoard().setFact(s_NumTeeth,m_numTeeth);
    m_returningTeeth = 0;

    RAY_GAMEMANAGER->returnTeethThrowFinished();

    if ( m_numTeeth == m_toothState.size() )
    {
        m_behaviorTree.getBlackBoard().setFact(s_InviteSequenceFact,btrue);
        openDoor();
    }
}

void Ray_DeadGuyBTAIComponent::processReturnTeeth( class Ray_EventReturnTeeth* _teeth )
{
    if ( _teeth->getStage() == Ray_EventReturnTeeth::STAGE_STOP )
    {
        m_behaviorTree.getBlackBoard().removeFact(s_ReturnTeethFact);
        u32 totalTeeth = m_numTeeth + m_returningTeeth;

        totalTeeth = Min(totalTeeth,m_toothState.size());

        for ( u32 i = m_numTeeth; i < totalTeeth; i++ )
        {
            m_toothState[i] = TOOTHSTATE_SET;
        }

        m_numTeeth = totalTeeth;
        m_behaviorTree.getBlackBoard().setFact(s_NumTeeth,m_numTeeth);
        m_returningTeeth = 0;

        if ( m_numTeeth == m_toothState.size() )
        {
            m_behaviorTree.getBlackBoard().setFact(s_InviteSequenceFact,btrue);
            openDoor();
        }
    }
    else
    {
        m_behaviorTree.getBlackBoard().setFact(s_ReturnTeethFact,btrue);
        
        m_returningTeeth = _teeth->getNumTeeth();

        if ( m_returningTeeth > m_toothState.size() )
        {
            m_returningTeeth = 0;
        }
    }
}

void Ray_DeadGuyBTAIComponent::updateAnimInput()
{
    Super::updateAnimInput();

    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(teeth,2520151479),m_numTeeth);

    u32 numTooth = m_toothState.size();
    char toothStateStr[256];

    for ( u32 i = 0; i < numTooth; i++ )
    {
        SPRINTF_S(toothStateStr,256,"toothState%u",i+1);
        StringID id = toothStateStr;
        m_animatedComponent->setInput(id,m_toothState[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_DeadGuyBTAIComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_DeadGuyBTAIComponent_Template)

    SERIALIZE_MEMBER("maxTeeth",m_maxTeeth);
    SERIALIZE_MEMBER("getTeethSequenceDistance",m_getTeethSequenceDistance);

END_SERIALIZATION()

Ray_DeadGuyBTAIComponent_Template::Ray_DeadGuyBTAIComponent_Template()
: m_maxTeeth(10)
, m_getTeethSequenceDistance(7.f)
{
}

Ray_DeadGuyBTAIComponent_Template::~Ray_DeadGuyBTAIComponent_Template()
{
}

}
