#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DANCEDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceDoorComponent.h"
#endif //_ITF_RAY_DANCEDOORCOMPONENT_H_

#ifndef _ITF_RAY_DANCEEYECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceEyeComponent.h"
#endif //_ITF_RAY_DANCEEYECOMPONENT_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define STATE_CLOSED            0
#define STATE_WARMUP            1
#define STATE_DANCE             2
#define STATE_READY_FOR_OPEN    3
#define STATE_OPEN              4

#define STATE_WARMUP_DISABLED   0
#define STATE_WARMUP_3          1
#define STATE_WARMUP_2          2
#define STATE_WARMUP_1          3
#define STATE_WARMUP_GO         4
 
namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_DanceDoorComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_DanceDoorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_DanceDoorComponent)
        VALIDATE_COMPONENT_PARAM("", m_animLightComponent,    "Ray_DanceDoorComponent requires an AnimLightComponent/AnimatedComponent.");
        VALIDATE_COMPONENT_PARAM("",  m_subAnchorComponent,    "Ray_SubAnchorComponent mandatory");
    END_VALIDATE_COMPONENT()

    // ****************************************************************************

    Ray_DanceDoorComponent::Ray_DanceDoorComponent()
    : m_subAnchorComponent(NULL)
    , m_state(STATE_CLOSED)
    , m_musicPlaying(bfalse)
    , m_time(0.0f)
    , m_isInit(bfalse)
    , m_warmupRef(ITF_INVALID_OBJREF)
    , m_stateWarmup(STATE_WARMUP_DISABLED)
    {
        // none 
    }

    // ****************************************************************************

    Ray_DanceDoorComponent::~Ray_DanceDoorComponent()
    {
        // none
    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        m_state = STATE_CLOSED;
    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get components
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
        ITF_ASSERT_MSG(m_animLightComponent, "The door MUST have an AnimLightComponent");
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_subAnchorComponent = m_actor->GetComponent<Ray_SubAnchorComponent>();

        // Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);


    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    } 

    // ****************************************************************************

    void Ray_DanceDoorComponent::onEvent(Event * _event)
    {
        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTrigger(trigger);
        }
        else if ( Ray_EventQueryEmptyStone* queryEmpty = _event->DynamicCast<Ray_EventQueryEmptyStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899)) )
        {
            processQueryEmpty(queryEmpty);
        }
    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::processTrigger( EventTrigger* _trigger )
    {
        ObjectRef activator = _trigger->getActivator();

        if (_trigger->getActivated())
        {

            Actor* dancer = AIUtils::getActor(activator);
            ITF_ASSERT(dancer);
            if (dancer)
            {

                ObjectRef emptyStone = searchEmptyStone();
                if (emptyStone.isValid())
                {
                    Ray_EventDance eventDance;
                    eventDance.setSender(emptyStone);
                    dancer->onEvent(&eventDance);
                }
            }
        }
    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty )
    {
        ObjectRef emptyStone = searchEmptyStone();
        
        _queryEmpty->setEmptyStone(emptyStone);

        if (!emptyStone.isValid())
        {
            const StringID s_waitZonePos = ITF_GET_STRINGID_CRC(waitZonePos,2036463886);

            Vec3d posZone(m_actor->getPos());
            if (m_subAnchorComponent)
            {
                posZone = m_subAnchorComponent->getPos(s_waitZonePos);
            }

            _queryEmpty->setWaitZone(posZone);
            _queryEmpty->setDoor(m_actor->getRef());
        }

    }

    // ****************************************************************************

    ObjectRef Ray_DanceDoorComponent::searchEmptyStone()
    {
       ObjectRef closestStone = ITF_INVALID_OBJREF;
       f32 distMax = 0.0f;

        for (u32 i = 0; i < m_ArraySlots.size(); i++)
        {
            Slot& curSlot = m_ArraySlots[i];
            Actor* actor = curSlot.m_eye.getActor();

            //if (curSlot.m_isReserved)
            //{
            //    continue;
            //}

            Ray_DanceEyeComponent* danceEyeComponent = actor ? actor->GetComponent<Ray_DanceEyeComponent>() : NULL;

            if ( danceEyeComponent && !danceEyeComponent->isOccupied())
            {                
                Actor* stone = AIUtils::getActor(danceEyeComponent->getStone());
                if (stone)
                {
                    f32 dist = (m_actor->getPos() - stone->getPos()).sqrnorm();

                    //if (dist < distMin)
                    //{
                    //    iMin = i;
                    //    distMin = dist;
                    //    closestStone = danceEyeComponent->getStone();
                    //}

                    if (dist > distMax)
                    {
                        distMax = dist;
                        closestStone = danceEyeComponent->getStone();
                    }

                    
                }

            }
        }

        return closestStone;
    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::init()
    {
        if (m_linkComponent)
        {
            AIUtils::LinkIterator iterator(m_linkComponent);
            while ( Actor* child = iterator.getNextRelativeActor() )
            { 
                m_warmupRef = child->getRef();
                break;                
            }
        }
        ITF_ASSERT(m_warmupRef.isValid());

        m_isInit = btrue;
    }

    // ****************************************************************************

    void Ray_DanceDoorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (!m_isInit)
        {
            init();
        }


        if (m_state == STATE_OPEN || m_ArraySlots.size() == 0)
        {
            return;
        }

        // Check total
        bbool atLeastOneOccupied = bfalse;
        bbool allStoneOccupied = btrue;
        bbool allUnlocked = btrue;
        bbool allReadyForOpen = btrue;

        for (u32 i = 0; i < m_ArraySlots.size(); i++)
        {
            Actor* actor = m_ArraySlots[i].m_eye.getActor();
            Ray_DanceEyeComponent* danceEyeComponent = actor ? actor->GetComponent<Ray_DanceEyeComponent>() : NULL;

            if (danceEyeComponent)
            {
                atLeastOneOccupied |= danceEyeComponent->isOccupiedByNPC();
                allStoneOccupied &= danceEyeComponent->isOccupied();
                allUnlocked &= danceEyeComponent->isUnlock();
                allReadyForOpen &= danceEyeComponent->isReadyForDoorOpen();
            }
        }
 

        switch(m_state)
        {
            case STATE_CLOSED:
            {
                if (allStoneOccupied)
                {
                    m_time = getTemplate()->getWarmupTime();

                    if (!m_musicPlaying)
                    {
                        playAnim(getTemplate()->getClosedWithMusicAnim());
                        METRONOME_MANAGER->setTimeSignature(80, 4, 4, METRONOME_TYPE_DEFAULT);
                        m_musicPlaying = btrue;
                    }

                    m_state = STATE_WARMUP;
                }

                break;
            }

            case STATE_WARMUP:
                {
                    m_time -= _dt;

                    udpateWarmupAct(m_time);

#ifdef ITF_SUPPORT_EDITOR
//DebugDraw::text("Warmup : %f", m_time);
#endif

                    if (m_time <= 0.0f)
                    {
                        m_time = getTemplate()->getDanceTime();
                        m_state = STATE_DANCE;
                    }

                    break;
                }


            case STATE_DANCE:
            {
                m_time -= _dt;

                if (m_time <= 0.0f)
                {
                    m_state = STATE_READY_FOR_OPEN;
                }

                break;
            }

            case STATE_READY_FOR_OPEN:
            {
                if (allReadyForOpen)
                {
                    playAnim(getTemplate()->getOpeningAnim());
                    METRONOME_MANAGER->setTimeSignature(120, 4, 4,METRONOME_TYPE_DEFAULT);
                    m_state = STATE_OPEN;

                    // Unsnap all player
                    ActorList playersList;
                    AIUtils::getPlayers(playersList);

                    for (u32 i = 0; i < playersList.size(); i++)
                    {
                        Actor* player = playersList[i];
                        if (player)
                        {
                            Ray_EventSnapDance eventSnapDance; 
                            eventSnapDance.setSender(m_actor->getRef());
                            eventSnapDance.setSnap(bfalse);
                            player->onEvent(&eventSnapDance); 
                        }
                    }
                }
                break;
            }


            case STATE_OPEN:
                {

                    break;
                }

        }
    }

    // ****************************************************************************


    void Ray_DanceDoorComponent::udpateWarmupAct( f32 _time )
    {
        const StringID anim1 = ITF_GET_STRINGID_CRC(warmup1,457182525);
        const StringID anim2 = ITF_GET_STRINGID_CRC(warmup2,4093211531);
        const StringID anim3 = ITF_GET_STRINGID_CRC(warmup3,3307400930);
        const StringID animGo = ITF_GET_STRINGID_CRC(warmupGo,1219598196);

        if (Actor* warmupAct = m_warmupRef.getActor())
        {
            if (AnimLightComponent* animLightCompo = warmupAct->GetComponent<AnimLightComponent>())
            {
                if (m_stateWarmup != STATE_WARMUP_3 &&_time <= 3.0f && _time > 2.0f)
                {
                    animLightCompo->setAnim(anim3);
                    m_stateWarmup = STATE_WARMUP_3;
                }

                if (m_stateWarmup != STATE_WARMUP_2 &&_time <= 2.0f && _time > 1.0f)
                {
                    animLightCompo->setAnim(anim2);
                    m_stateWarmup = STATE_WARMUP_2;

                }

                if (m_stateWarmup != STATE_WARMUP_1 &&_time <= 1.0f && _time > 0.0f)
                {
                    animLightCompo->setAnim(anim1);
                    m_stateWarmup = STATE_WARMUP_1;

                }

                if (m_stateWarmup != STATE_WARMUP_GO &&_time <= 0.0f)
                {
                    animLightCompo->setAnim(animGo);
                    m_stateWarmup = STATE_WARMUP_GO;

                }

            }
        }

    }

    // ****************************************************************************


    void Ray_DanceDoorComponent::registerEye( ObjectRef _objRef )
    {
         if (_objRef.isValid())
         {
            m_ArraySlots.push_back( Slot(_objRef) );
         }
    }

    // ****************************************************************************

    bbool Ray_DanceDoorComponent::playAnim( const StringID& _anim )
    {
        ITF_WARNING(m_actor, _anim.isValid(), "I wan't play an invalid anim !");

        if (_anim.isValid())
        {
            m_animLightComponent->setAnim(_anim);
            return btrue;
        }

        return bfalse;
    }

    // ****************************************************************************

    bbool Ray_DanceDoorComponent::isReadyForOpen() const
    {
        return (m_state == STATE_READY_FOR_OPEN);
    }

    // ****************************************************************************

    bbool Ray_DanceDoorComponent::isOpen() const
    {
        return (m_state == STATE_OPEN);
    }

    // ****************************************************************************

    bbool Ray_DanceDoorComponent::danceIsStarted() const
    {
        return (m_state == STATE_DANCE);
    }

    // ****************************************************************************

    bbool Ray_DanceDoorComponent::warmupIsStarted() const
    {
        return (m_state == STATE_WARMUP);
    }

    // ****************************************************************************
    // Template
    // ****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_DanceDoorComponent_Template)

    BEGIN_SERIALIZATION(Ray_DanceDoorComponent_Template)
        SERIALIZE_MEMBER("animClosed",            m_animClosed);
        SERIALIZE_MEMBER("animClosedWithMusic",   m_animClosedWithMusic);
        SERIALIZE_MEMBER("animOpening",           m_animOpening);

        SERIALIZE_MEMBER("danceTime",             m_danceTime);
        SERIALIZE_MEMBER("warmupTime",            m_warmupTime);
       
        
    END_SERIALIZATION()

    // ****************************************************************************


    Ray_DanceDoorComponent_Template::Ray_DanceDoorComponent_Template()
    : m_warmupTime(3.0f)
    , m_danceTime(10.0f)
    {
        // none
    }

    // ****************************************************************************

 
};



