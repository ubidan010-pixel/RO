#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LastPlayerTriggerComponent.h"
#endif //_ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_LastPlayerTriggerComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_LastPlayerTriggerComponent_Template)
        SERIALIZE_MEMBER("useTimeBeforeActivation", m_useTimeBeforeActivation);
    END_SERIALIZATION()

    Ray_LastPlayerTriggerComponent_Template::Ray_LastPlayerTriggerComponent_Template()
        : Super()
        , m_useTimeBeforeActivation(btrue)
    {

    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_LastPlayerTriggerComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_LastPlayerTriggerComponent)
    SERIALIZE_MEMBER("timeBeforeActivation",m_timeBeforeActivation);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_LastPlayerTriggerComponent)
    END_VALIDATE_COMPONENT()

    Ray_LastPlayerTriggerComponent::Ray_LastPlayerTriggerComponent()
    : m_timer(0.0f)
    , m_eventSent(bfalse)
    , m_timeBeforeActivation(3.0f)
    {
    }

    Ray_LastPlayerTriggerComponent::~Ray_LastPlayerTriggerComponent()
    {
    }


    void Ray_LastPlayerTriggerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_activatedActors.clear();
        m_timer = 0.0f;
        m_eventSent = bfalse;
    }

    void Ray_LastPlayerTriggerComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if (getTemplate()->getUseTimeBeforeActivation())
        {
            m_timer = Max(m_timer - _deltaTime, 0.0f);
        }
    }

    void Ray_LastPlayerTriggerComponent::checkSendEvent( ActorRefList &actorsInsideThisFrame )
    {
        if (m_eventSent)
            return;

        u32 activatedActors = m_activatedActors.size();

        //get activated actors
        for (u32 i = 0 ; i < actorsInsideThisFrame.size(); ++i)
        {
            i32 index = m_activatedActors.find(actorsInsideThisFrame[i]);
            if (index == -1)
            {
                m_activatedActors.push_back(actorsInsideThisFrame[i]);
            }
        }

        //check players
        bbool allPassedThrough = btrue;
        bbool allDead = btrue;
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
            if (!player || !player->getActive())
            {
                continue;
            }

            if (!player->isDead())
            {
                allDead = bfalse;
                if ( m_activatedActors.find(player->getCurrentActor()) == -1 )
                {
                    allPassedThrough = bfalse;
                }
            }
        }

        if (allPassedThrough && !allDead)
        {
            //send event
            m_eventSent = btrue;
            sendEvent(m_actor->getRef(),btrue);
            return;
        }

        if (getTemplate()->getUseTimeBeforeActivation())
        {
            //start timer if necessary
            if (activatedActors == 0 && m_activatedActors.size() > 0)
                m_timer = m_timeBeforeActivation;

            //check timer
            if (m_activatedActors.size() > 0)
            {
                if (m_timer == 0.0f)
                {
                    m_eventSent = btrue;
                    //send event
                    sendEvent(m_actor->getRef(),btrue);
                }
            }
        }
    }

    Event* Ray_LastPlayerTriggerComponent::chooseEvent(ActorRef _activator, bbool _enter)
    {
        return m_onEnterEvent;
    }

};