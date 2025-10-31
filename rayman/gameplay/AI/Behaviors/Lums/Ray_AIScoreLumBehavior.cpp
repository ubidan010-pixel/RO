#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_


#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#define SCORELUM_STAND_ANIM		    "Stand"
#define SCORELUM_APPEAR_ANIM        "Stand"
#define SCORELUM_DISAPPEARS_ANIM    "Explode"

#define SCORELUM_EXPLOSIONTRAIL_BEND_AMPLITUDE  0.1f
#define SCORELUM_EXPLOSIONTRAIL_SPEED           10.f


///////////////////////////////////////////////////////////////////////////////////////////


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIScoreLumBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIScoreLumBehavior_Template)

        SERIALIZE_MEMBER("maxTimeBeforeExplosion_RedMode", m_maxTimeBeforeExplosion_RedMode);
        SERIALIZE_MEMBER("yellowLumValue", m_yellowLumValue);
        SERIALIZE_MEMBER("redLumValue", m_redLumValue);
        SERIALIZE_MEMBER("lumKingValue", m_lumKingValue);
        SERIALIZE_OBJECT_WITH_FACTORY("startKingMusicEvent", m_startKingMusicEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("stopKingMusicEvent", m_stopKingMusicEvent, ACTORSMANAGER->getEventFactory());

    END_SERIALIZATION()

    Ray_AIScoreLumBehavior_Template::Ray_AIScoreLumBehavior_Template()
    : m_maxTimeBeforeExplosion_RedMode(1.f)
    , m_yellowLumValue(1)
    , m_redLumValue(4)
    , m_lumKingValue(5)
    , m_startKingMusicEvent(NULL)
    , m_stopKingMusicEvent(NULL)
    {
    }

    Ray_AIScoreLumBehavior_Template::~Ray_AIScoreLumBehavior_Template()
    {
        SF_DEL(m_startKingMusicEvent);
        SF_DEL(m_stopKingMusicEvent);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIScoreLumBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIScoreLumBehavior)
    END_VALIDATE_BEHAVIOR()

    ObjectRef Ray_AIScoreLumBehavior::m_thisObjectRef;

    static StringID mrkExploding = "MRK_explosion"; //when we flush score
    static StringID mrkExploseEnd = "MRK_Explode_GameplayStop";

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AIScoreLumBehavior::Ray_AIScoreLumBehavior()
        : Super()
        , m_acquiredLumsForSession(0)
        , m_lumScoreForSession(0)
        , m_timeSinceLumReachedScore(MTH_HUGE)
        , m_comboState(ComboState_None)
        , m_timeInCurrentComboState(0)
    {
        for (u32 i=0; i<s_maxPlayers; i++)
        {
            m_lumScoreForSessionPerPlayer[i] = 0;
        }
    }

    Ray_AIScoreLumBehavior::~Ray_AIScoreLumBehavior()
    {
        setComboState(ComboState_None); //this is because  we may have fx running
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), m_aiComponent);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventLumReachesScore,1881707556), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAddLum,4227734836), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryLumStage,4112170611), m_aiComponent);
        m_actor->setAlwaysVisible(btrue);
        resetState();
        m_thisObjectRef = m_actor->getRef();
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),m_aiComponent);
    }

    void Ray_AIScoreLumBehavior::onResourceReady()
    {
        Super::onResourceReady();						
    }

    void Ray_AIScoreLumBehavior::clear() 
    {
    }

    void Ray_AIScoreLumBehavior::resetState()
    {
        m_lumScoreForSession = 0;
        m_acquiredLumsForSession = 0;

        setComboState(ComboState_None);
        flushScorePerPlayer();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::getGlobalColorWeights(f32 &_yellowW, f32 &_orangeW, f32 &_redW) const
    {
        getColorWeightsFromColor(getGlobalLumColor(), _redW, _orangeW, _yellowW);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

        m_timeSinceLumReachedScore += _deltaTime;
        m_timeInCurrentComboState += _deltaTime;

        
        //if we pass beyond combo timeout, reset acquired lums
        switch(m_comboState)
        {
        case ComboState_Combo:
            {
                f32 maxTimeBeforeExplosion = getTemplate()->getMaxTimeBeforeExplosion_RedMode();
                
                if (m_timeInCurrentComboState>maxTimeBeforeExplosion)
                {
                    setComboState(ComboState_None);
                }
            } break;
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (Ray_EventAddLum *addLum = _event->DynamicCast<Ray_EventAddLum>(ITF_GET_STRINGID_CRC(Ray_EventAddLum,4227734836)))
        {
            processAddLum(addLum);
        }
        else if (Ray_EventLumReachesScore *lumReaches = _event->DynamicCast<Ray_EventLumReachesScore>(ITF_GET_STRINGID_CRC(Ray_EventLumReachesScore,1881707556)))
        {
            processLumReachesScore(lumReaches);
        }
        else if (EventRevertToLastCheckpoint *checkpointEvent = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)))
        {
            resetState();
        }
        else if (Ray_EventQueryLumStage * query = _event->DynamicCast<Ray_EventQueryLumStage>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumStage,4112170611)))
        {
            query->setLumStage(u32(getGlobalLumColor()));
            query->setComboState(m_comboState);
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::processAddLum(Ray_EventAddLum *_event)
    {
        //We count acquired lums (not their price) as the ACQUIRED count
        u32 lumCount=1;
        u32 value;
        bbool dontCountForGlobalSession=bfalse;
        if (_event->m_isAccrobatic)
        {
            lumCount = 2;
        }
        value = getLumValue(_event);
        if (m_comboState==ComboState_None)
        {
            m_acquiredLumsForSession = 0;
            m_lumScoreForSession = 0;
            dontCountForGlobalSession = btrue;
        }
        else
        {
            ITF_ASSERT(m_comboState==ComboState_Combo);
            //NB : 2011 03 15 : we dont maintain combo anymore. Only fixed limited time
            //setComboState(ComboState_Combo); //maintain combo
        }
        if (_event->m_isKingLum)
        {
            setComboState(ComboState_Combo);
            lumCount = getTemplate()->getLumKingValue();
            value = 1*lumCount; 
            dontCountForGlobalSession = bfalse;
        }

        if (!dontCountForGlobalSession)
        {
            m_acquiredLumsForSession+=lumCount;
            m_lumScoreForSession += value;
        }
        Player *player = RAY_GAMEMANAGER->getPlayerFromActor(_event->m_activator);
        if (player)
        {
            _event->m_outputScore = value;
            u32 playerIndex = player->getIndex();
            m_lumScoreForSessionPerPlayer[playerIndex] += value;
            //flushScoreForGivenPlayer(playerIndex);
        }


    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::flushScoreForGivenPlayer(u32 _playerIndex)
    {
        u32 &value = m_lumScoreForSessionPerPlayer[_playerIndex];
        Ray_Player *player = (Ray_Player*)RAY_GAMEMANAGER->getPlayer(_playerIndex);
        if (value!=0 && player!=NULL)
        {
            Ray_PlayerHudScoreComponent::addPendingScoreIncrement(player->getIndex(), value);
            //player->addLums(value);
        }
        value = 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::flushScorePerPlayer()
    {
        for (u32 i=0; i<s_maxPlayers; i++)
        {
            flushScoreForGivenPlayer(i);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::processLumReachesScore(Ray_EventLumReachesScore *_event)
    {
        m_timeSinceLumReachedScore = 0;
        ITF_ASSERT_CRASH(_event->m_playerIndex < s_maxPlayers, "incorrect player index");
        u32 &remainingScore = m_lumScoreForSessionPerPlayer[_event->m_playerIndex];
        u32 toAdd = _event->m_valueToAdd;
        if (remainingScore>=toAdd)
        {
            remainingScore -= toAdd;
            Ray_Player *player = (Ray_Player*)RAY_GAMEMANAGER->getPlayer(_event->m_playerIndex);
            if (player)
            {
                Ray_PlayerHudScoreComponent::addPendingScoreIncrement(player->getIndex(), toAdd);
                //player->addLums(toAdd);
                //Ray_PlayerHudScoreComponent::startPulse(_event->m_playerIndex);
            }
        }
        
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_AIScoreLumBehavior::getLumValue(Ray_EventAddLum *_event)
    {
        u32 value=1;

        if (_event->m_isKingLum || _event->m_forceYellow)
        {
            value=getTemplate()->getYellowLumValue();
        }
        else
        {
            switch(getGlobalLumColor())
            {
            case Color_Yellow: value=getTemplate()->getYellowLumValue(); break;
            case Color_Red: value=getTemplate()->getRedLumValue(); break;
            default:ITF_ASSERT(bfalse);break;
            }
        }
        return value;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::getColorWeightsFromColor(Color_t _color, f32 &_redW, f32 &_orangeW, f32 &_yellowW)
    {
        _yellowW = 0;
        _orangeW = 0;
        _redW = 0;

        switch(_color)
        {
        case Color_Red: _redW = 1.f; break;
        case Color_Yellow : _yellowW = 1.f; break;
        default : ITF_ASSERT(bfalse);_yellowW = 1.f; break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::clearRedMode()
    {
        if (m_comboState==ComboState_Combo)
        {
            setComboState(ComboState_None);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIScoreLumBehavior::setComboState(ComboState_t _state)
    {
        m_timeInCurrentComboState = 0;
        if (m_comboState==ComboState_None || _state==ComboState_None)
        {
            m_acquiredLumsForSession = 0;
            m_lumScoreForSession = 0;
        }

        //handle changes
        if (_state!=m_comboState)
        {
            switch(_state)
            {
            case ComboState_Combo:
                {
                    Event *ev = getTemplate()->getStartKingMusicEvent();
                    if (ev)
                    {
                        //EVENTMANAGER->broadcastEvent(ev);
                        MUSICMANAGER->muteAllBut(btrue, METRONOME_TYPE_LUMKING);
                    }
                } break;
                case ComboState_None:
                {
                    Event *ev = getTemplate()->getStopKingMusicEvent();
                    if (ev)
                    {
                        //EVENTMANAGER->broadcastEvent(ev);
                        MUSICMANAGER->muteAllBut(bfalse, METRONOME_TYPE_LUMKING);
                    }
                } break;
            }
        }
        //
        m_comboState = _state;
    }

}
