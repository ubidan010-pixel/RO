#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackHUDResultsComponent.h"
#endif //_ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TimeAttackHUDResultsComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TimeAttackHUDResultsComponent)

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_TimeAttackHUDResultsComponent)

    VALIDATE_COMPONENT_PARAM("AnimatedComponent", m_animatedComponent != NULL, "AnimatedComponent is mandatory");
    VALIDATE_COMPONENT_PARAM("UITextBoxesComponent", m_textBoxesComponent != NULL, "UITextBoxesComponent is mandatory");

END_VALIDATE_COMPONENT()

Ray_TimeAttackHUDResultsComponent::Ray_TimeAttackHUDResultsComponent()
: m_animatedComponent(NULL)
, m_textBoxesComponent(NULL)
, m_waitInput(bfalse)
, m_phase(Phase_Timer)
, m_electoonTime(0)
, m_cupTime(0)
, m_myTime(0)
, m_receivedPlayNext(bfalse)
{
    m_phaseState.resize(Max_Phase);

    for ( u32 i = 0; i < Max_Phase; i++ )
    {
        m_phaseState[i] = Phase_Appear;
    }
}

Ray_TimeAttackHUDResultsComponent::~Ray_TimeAttackHUDResultsComponent()
{
}

void Ray_TimeAttackHUDResultsComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( RAY_GAMEMANAGER->getTimeAttackActorResults() == m_actor->getRef() )
    {
        RAY_GAMEMANAGER->setTimeAttackActorResults(ObjectRef::InvalidRef);
    }

    if ( m_waitInput )
    {
        GAMEMANAGER->removeInputListener(this);
    }
}

void Ray_TimeAttackHUDResultsComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    m_textBoxesComponent = m_actor->GetComponent<UITextBoxesComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
        m_animatedComponent->setRank2D(UI_TEXTMANAGER->getAnimDepthRank());
    }

    RAY_GAMEMANAGER->setTimeAttackActorResults(m_actor->getRef());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventMedalNonCinematicEnded,694861796),this);
}

void Ray_TimeAttackHUDResultsComponent::onBecomeActive()
{
    StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
    m_electoonTime = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, bfalse);
    m_cupTime = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, btrue);
    m_myTime = static_cast<i32>(RAY_GAMEMANAGER->getTimeAttackTimer());

    if ( m_electoonTime == -1 )
    {
        m_electoonTime = 90;
    }

    if ( m_cupTime == -1 )
    {
        m_cupTime = 60;
    }

    i32 myTimeMin = m_myTime/60;
    i32 myTimeSec = m_myTime - (60*myTimeMin);
    String8 timerStr;

    timerStr.setTextFormat("%i:%02i",myTimeMin,myTimeSec);

    m_textBoxesComponent->setText(getTemplate()->getBoneTimer(),timerStr);
    m_animatedComponent->resetCurTime();
    m_animatedComponent->resetTree();

    m_phaseState[Phase_Timer] = Phase_Appear;
    m_phaseState[Phase_Electoon] = Phase_Hidden;
    m_phaseState[Phase_Cup] = Phase_Hidden;

    m_phase = Phase_Timer;

    RAY_GAMEMANAGER->requestGameplayPause();
    MUSICMANAGER->stop(METRONOME_TYPE_GAMEPLAY,0.25f);
}

void Ray_TimeAttackHUDResultsComponent::onBecomeInactive()
{
    m_phaseState[0] = Phase_Appear;

    for ( u32 i = 1; i < Max_Phase; i++ )
    {
        m_phaseState[i] = Phase_Appear;
    }

    m_receivedPlayNext = bfalse;

    m_textBoxesComponent->setText(getTemplate()->getBoneElectoons(),"");
    m_textBoxesComponent->setText(getTemplate()->getBoneCup(),"");
    m_textBoxesComponent->setText(getTemplate()->getBoneTimer(),"");

    if ( m_waitInput )
    {
        m_waitInput = bfalse;
        GAMEMANAGER->removeInputListener(this);
        if (!UIMenuManager::IsBaseMenuHelper())
            CONTEXTICONSMANAGER->hide();
    }
}

void Ray_TimeAttackHUDResultsComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_receivedPlayNext )
    {
        playNext();
        m_receivedPlayNext = bfalse;
    }
    else if ( m_phase == Phase_Cup )
    {
        updateFinish();
    }

    updatePosition();
}

void Ray_TimeAttackHUDResultsComponent::updateFinish()
{
    if ( !m_waitInput )
    {
        if ( m_animatedComponent->isMainNodeFinished() )
        {
            GAMEMANAGER->addGamePlayInputListener(this);
            if (!UIMenuManager::IsBaseMenuHelper())
                CONTEXTICONSMANAGER->show(ContextIcon_Retry, ContextIcon_Continue);
            RAY_GAMEMANAGER->setCurrentLevelTimeAttackTime(m_myTime);
            m_waitInput = btrue;
        }
    }
}

void Ray_TimeAttackHUDResultsComponent::Receive( u32 _deviceID, f32 _axis, const StringID& _action )
{
    if (GAMEMANAGER->getMainIndexPlayer() != _deviceID || GAMEMANAGER->isInPause())
    {
        return; 
    }

    if ( _action == ITF_GET_STRINGID_CRC(WM_ENTER,2267322818) )
    {
        GAMEMANAGER->removeInputListener(this);
        RAY_GAMEMANAGER->commitAndSaveGameState();
        RAY_GAMEMANAGER->resetToWorldMap();
    }
    else if ( _action == ITF_GET_STRINGID_CRC(WM_BACK,4244387740) )
    {
        GAMEMANAGER->removeInputListener(this);
        RAY_GAMEMANAGER->commitAndSaveGameState();
        RAY_GAMEMANAGER->reloadCurrentMap();
    }
}

void Ray_TimeAttackHUDResultsComponent::playNext()
{
    switch(m_phase)
    {
    case Phase_Timer:
        {
            m_phase++;
            m_phaseState[m_phase] = Phase_Appear;

            i32 totalMin = m_electoonTime/60;
            i32 totalSec = m_electoonTime-(60*totalMin);
            String8 totalTimeStr;

            totalTimeStr.setTextFormat("%i:%02i",totalMin,totalSec);

            m_textBoxesComponent->setText(getTemplate()->getBoneElectoons(),totalTimeStr);
        }
        break;
    case Phase_Electoon:
        {
            if ( m_phaseState[m_phase] == Phase_Appear )
            {
                if ( m_myTime <= m_electoonTime )
                {
                    {
                        Ray_EventShowElectoonMedal showMedalEvent;

                        showMedalEvent.setSender(m_actor->getRef());
                        showMedalEvent.m_forBrokenCage = bfalse;
                        showMedalEvent.m_playVictorySequence = bfalse;
                        showMedalEvent.m_playEndSequence = bfalse;
                        showMedalEvent.m_forLumAndTimeAttacks = btrue;
                        showMedalEvent.m_dontSetLumAttack1 = btrue;
                        showMedalEvent.m_dontSetLumAttack2 = btrue;
                        showMedalEvent.m_currentlyEmphasizedChallenge = CHALLENGE_TIMEATTACK;

                        EVENTMANAGER->broadcastEvent(&showMedalEvent);
                    }

                    m_phaseState[m_phase] = Phase_Win;
                }
                else
                {
                    m_phaseState[m_phase] = Phase_Fail;
                }
            }
            else
            {
                i32 cupMin = m_cupTime/60;
                i32 cupSec = m_cupTime-(60*cupMin);
                String8 cupTimeStr;

                cupTimeStr.setTextFormat("%i:%02i",cupMin,cupSec);

                m_textBoxesComponent->setText(getTemplate()->getBoneCup(),cupTimeStr);

                m_phase++;
                m_phaseState[m_phase] = Phase_Appear;

                // The medal unpauses the game, so I have to put the pause here again
                RAY_GAMEMANAGER->requestGameplayPause();
            }
        }
        break;
    case Phase_Cup:
        {
            if ( m_myTime <= m_cupTime )
            {
                m_phaseState[m_phase] = Phase_Win;
            }
            else
            {
                m_phaseState[m_phase] = Phase_Fail;
            }
        }
        break;
    }
}

void Ray_TimeAttackHUDResultsComponent::updatePosition()
{
    f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
    f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
    Vec2d p(screenWidth*0.5f,screenHeight*0.5f);
    f32 scale = std::min(screenWidth, screenHeight)*getTemplate()->getScale();

    m_actor->set2DPos(p);
    m_actor->setScale(Vec2d(scale,scale));
}

void Ray_TimeAttackHUDResultsComponent::onEvent( Event* _event )
{
    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Phase_Finish,84886780) )
        {
            m_receivedPlayNext = btrue;
        }
        else if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Obj_Finish,1397914016) )
        {
            m_receivedPlayNext = btrue;
        }
    }
    else if ( Ray_EventMedalNonCinematicEnded* animEvent = _event->DynamicCast<Ray_EventMedalNonCinematicEnded>(ITF_GET_STRINGID_CRC(Ray_EventMedalNonCinematicEnded,694861796)) )
    {
        m_receivedPlayNext = btrue;
    }
}

void Ray_TimeAttackHUDResultsComponent::updateAnimInput()
{
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(phase0,1030333466),m_phaseState[0]);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(phase1,1934586577),m_phaseState[1]);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(phase2,2561081217),m_phaseState[2]);
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_TimeAttackHUDResultsComponent_Template)
BEGIN_SERIALIZATION(Ray_TimeAttackHUDResultsComponent_Template)

    SERIALIZE_MEMBER("scale",m_scale);
    SERIALIZE_MEMBER("boneTimer",m_boneTimer);
    SERIALIZE_MEMBER("boneCup",m_boneCup);
    SERIALIZE_MEMBER("boneElectoons",m_boneElectoons);

END_SERIALIZATION()

Ray_TimeAttackHUDResultsComponent_Template::Ray_TimeAttackHUDResultsComponent_Template()
: m_scale(15.f)
{
}

Ray_TimeAttackHUDResultsComponent_Template::~Ray_TimeAttackHUDResultsComponent_Template()
{
}

}
