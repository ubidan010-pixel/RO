#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DANCESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceStoneComponent.h"
#endif //_ITF_RAY_DANCESTONECOMPONENT_H_

#ifndef _ITF_RAY_DANCEEYECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceEyeComponent.h"
#endif //_ITF_RAY_DANCEEYECOMPONENT_H_

#ifndef _ITF_RAY_DANCEDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceDoorComponent.h"
#endif //_ITF_RAY_DANCEDOORCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define STATE_FEEDBACK_DISABLED         0
#define STATE_FEEDBACK_GREEN            1
#define STATE_FEEDBACK_RED              2

namespace ITF
{
    const StringID input_MoveY = ITF_GET_STRINGID_CRC(MOVEY,951160567);

    IMPLEMENT_OBJECT_RTTI(Ray_DanceStoneComponent)

    BEGIN_VALIDATE_COMPONENT(Ray_DanceStoneComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent,       "Ray_DanceStoneComponent requires a LinkComponent");
        VALIDATE_COMPONENT_PARAM("", m_animLightComponent,  "AnimLightComponent requires a LinkComponent");
    END_VALIDATE_COMPONENT()

    BEGIN_SERIALIZATION_CHILD(Ray_DanceStoneComponent)
    END_SERIALIZATION()

    // ****************************************************************************

    Ray_DanceStoneComponent::Ray_DanceStoneComponent()
    : m_linkComponent(NULL)
    , m_animLightComponent(NULL)
    , m_fxControllerComponent(NULL)
    , m_validTemposInARow(0)
    , m_validTemposPrev(0)
    , m_playerIsSnap(bfalse)
    , m_stoneUnlockedByPlayer(bfalse)
    , m_stoneUnlockedByNPC(bfalse)
    , m_animUnlockIsPlayed(bfalse)
    , m_animOpenDoorIsPlayed(bfalse)
    , m_doorIsOpen(bfalse)
    , m_danceIsStarted(bfalse)
    , m_isPerfect(btrue)
    , m_warmupIsStarted(bfalse)
    , m_handleFxRed(U32_INVALID)
    , m_handleFxGreen(U32_INVALID)
    , m_stateFeedBack(STATE_FEEDBACK_DISABLED)
    {
        // none
    }

    // ****************************************************************************

    Ray_DanceStoneComponent::~Ray_DanceStoneComponent()
    {
        GAMEMANAGER->removeInputListener(this);
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        m_dancer = ITF_INVALID_OBJREF;
        m_validTemposInARow = 0;
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_upTempo.m_min = getTemplate()->getUpTempo().getMin();
        m_upTempo.m_max = getTemplate()->getUpTempo().getMax();
        m_downTempo.m_min = getTemplate()->getDownTempo().getMin();
        m_downTempo.m_max = getTemplate()->getDownTempo().getMax();

         // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventCanDanceOnStone,1732983345), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899), this);

        // Get Components
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT_MSG(m_linkComponent, "Ray_DanceStoneComponent requires an LinkComponent.");
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        // Get Actor (Eye) linked
        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& childList  = m_linkComponent->getChildren();
            ITF_WARNING(m_actor, childList.size() != 0, "This dance stone is not link with a Oukoum door Eye !");
            ITF_WARNING(m_actor, childList.size() <= 1, "This dance stone is link with too many actor !");
            
            for (LinkComponent::ChildrenList::const_iterator it = childList.begin(); it != childList.end(); ++it)
            {
                const ObjectPath& objPath = it->getPath();
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objPath);

                if (pickable)
                {
                    m_eye = pickable->getRef();
                    break;
                }
            }
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::onResourceReady( )
    {

        Actor* actor = m_eye.getActor();
        Ray_DanceEyeComponent* danceEyeComponent = actor ? actor->GetComponent<Ray_DanceEyeComponent>() : NULL;

        if ( danceEyeComponent )
        {
            danceEyeComponent->setStone(m_actor->getRef());
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTrigger(trigger);
        }
        else if ( Ray_EventCanDanceOnStone* canDance = _event->DynamicCast<Ray_EventCanDanceOnStone>(ITF_GET_STRINGID_CRC(Ray_EventCanDanceOnStone,1732983345)) )
        {
            processCanDance(canDance);
        }
        else if ( Ray_EventQueryEmptyStone* queryEmpty = _event->DynamicCast<Ray_EventQueryEmptyStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899)) )
        {
            processQueryEmpty(queryEmpty);
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::processTrigger( EventTrigger* _trigger )
    {
        ObjectRef activator = _trigger->getActivator();

        if (_trigger->getActivated())
        {
            if (m_dancer == ITF_INVALID_OBJREF)
            {
                m_dancer = activator;
                m_validTemposInARow = 0;
            }
            else
            {
                if ( !GAMEMANAGER->isPlayerActor(m_dancer) &&
                     GAMEMANAGER->isPlayerActor(activator) )
                {
                    // Player replaces AI
                    m_dancer = activator;
                    m_validTemposInARow = 0;

                    // reset
                    m_stoneUnlockedByNPC    = bfalse;
                    m_stoneUnlockedByPlayer = bfalse;
                    m_animUnlockIsPlayed    = bfalse;

                }
            }
        }
        else
        {
            if (activator == m_dancer)
            {
                m_dancer = ITF_INVALID_OBJREF;
                m_validTemposInARow = 0;
            }
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::processCanDance( Ray_EventCanDanceOnStone* _canDance )
    {
        bbool canDance = !m_doorIsOpen && ( m_dancer==ITF_INVALID_OBJREF || m_dancer==_canDance->getSender() );

        if ( canDance )
        {
            if (m_warmupIsStarted || m_danceIsStarted)
            {
                _canDance->setCanDance(btrue);
                _canDance->setMustWaitForSartDance(bfalse);
            }
            else
            {
                _canDance->setCanDance(btrue);
                _canDance->setMustWaitForSartDance(btrue);
            }
        }
        else
        {
            if ( m_doorIsOpen )
            {
                _canDance->setOpened(btrue);
            }
            else
            {
                _canDance->setOccupied(btrue);
            }
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty )
    {
        Actor* actor = m_eye.getActor();
        Ray_DanceEyeComponent* danceEyeComponent = actor ? actor->GetComponent<Ray_DanceEyeComponent>() : NULL;

        if ( danceEyeComponent )
        {
            Actor* doorActor = danceEyeComponent->getDoor().getActor();

            if ( doorActor )
            {
                doorActor->onEvent(_queryEmpty);
            }
        }
    }

    // ****************************************************************************

    u32 Ray_DanceStoneComponent::startFX(StringID _fxName)
    {
        u32 handle = U32_INVALID;
        if (m_fxControllerComponent)
        {
            if (_fxName.isValid())
            {
                handle = m_fxControllerComponent->playFX(_fxName);
                //m_fxControllerComponent->setFXPos(m_handleFX,m_actor->getPos()); 
            }
        }

        return handle;
    }

    //*****************************************************************************

    void Ray_DanceStoneComponent::stopFX(u32 _handleFx)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFX(_handleFx);
        }
    }

    //*****************************************************************************

    void Ray_DanceStoneComponent::spawnReward(Actor* _sender, ObjectRef _receiver, Vec3d _offset /*= Vec3d::Zero*/, u32 _count /*= 1 */ )
    {
        Ray_EventSpawnRewardLum rewardLum;

        rewardLum.clear();
        rewardLum.setup(_sender, _offset);
        rewardLum.setNumRewards(_count);
        rewardLum.setRewardReceiver(_receiver);
        rewardLum.setEjectionDir(Vec2d::Up);

        GAMEMANAGER->onEvent(&rewardLum);   

    }
    // ****************************************************************************

    void Ray_DanceStoneComponent::Update( f32 _dt )
    {
        Super::Update(_dt);


        // Udpate door state
        Actor* actor = m_eye.getActor();
        Ray_DanceEyeComponent* danceEyeComponent = actor ? actor->GetComponent<Ray_DanceEyeComponent>() : NULL;
        Actor* doorActor = danceEyeComponent ? danceEyeComponent->getDoor().getActor() : NULL;
        Ray_DanceDoorComponent* doorComponent = doorActor ? doorActor->GetComponent<Ray_DanceDoorComponent>() : NULL;
     
        m_doorIsOpen = doorComponent && doorComponent->isOpen();
        m_danceIsStarted = doorComponent && doorComponent->danceIsStarted();
        m_warmupIsStarted = doorComponent && doorComponent->warmupIsStarted();

        m_validTemposPrev = m_validTemposInARow;

        if (m_dancer != ITF_INVALID_OBJREF)
        {
            f32 metronome = METRONOME_MANAGER->getBarPercentage(getTemplate()->getSyncOffset(), getTemplate()->getSyncRatio());

            if ( GAMEMANAGER->isPlayerActor(m_dancer) )
            {
                if (!m_doorIsOpen && m_danceIsStarted )
                {
                    m_upTempo.update(metronome, m_validTemposInARow);
                    m_downTempo.update(metronome, m_validTemposInARow);
                }


                // Spawn lums
                if (!m_doorIsOpen && m_danceIsStarted && m_validTemposInARow > m_validTemposPrev)
                {
                    u32 numLums = getTemplate()->getNumLumNormalReward();
                    if (m_validTemposInARow >= getTemplate()->getNumTemposToUnlock())
                    {
                        numLums = getTemplate()->getNumLumPowerReward();
                    }

                    spawnReward(m_actor, m_dancer, Vec3d::Zero, numLums);
                }

                //// Check perfect mode
                //if (!m_doorIsOpen && m_danceIsStarted && m_validTemposInARow < m_validTemposPrev)
                //{
                //    m_isPerfect = bfalse;
                //}


                // dancer is a player: validate on input
                // -> done by listener
            }
            else
            {
                m_upTempo.update(metronome, m_validTemposInARow);
                m_downTempo.update(metronome, m_validTemposInARow);

                // dancer is an AI: auto-validate
                if (m_upTempo.m_open)
                {
                    m_upTempo.m_validated = btrue;
                }
                if (m_downTempo.m_open)
                {
                    m_downTempo.m_validated = btrue;
                }
            }

            updateFeedback();
        }

        // Update Eye
        if (danceEyeComponent && !m_stoneUnlockedByPlayer)
        {
            bbool occupiedByNPC     = (m_dancer != ITF_INVALID_OBJREF) && !GAMEMANAGER->isPlayerActor(m_dancer);
            bbool occupiedByPlayer  = (m_dancer != ITF_INVALID_OBJREF) && GAMEMANAGER->isPlayerActor(m_dancer);
            bbool unlock = m_validTemposInARow >= getTemplate()->getNumTemposToUnlock();

            if (GAMEMANAGER->isPlayerActor(m_dancer))
            {
                m_stoneUnlockedByPlayer = unlock;
                m_stoneUnlockedByNPC = bfalse;
            }
            else
            {
                m_stoneUnlockedByPlayer = bfalse;
                m_stoneUnlockedByNPC = unlock;
            }
            
            danceEyeComponent->updateStone(m_actor->getRef(), m_actor->getPos(), occupiedByNPC, occupiedByPlayer, unlock);
        }


        // Snap player on stone
        if (!m_playerIsSnap)
        {
            if (!m_doorIsOpen)
            {

                if (Actor* closestActor = AIUtils::getClosestLivePlayer(m_actor))
                {
                    if (m_dancer == closestActor->getRef())
                    {
                  
                        f32 distX = f32_Abs(closestActor->get2DPos().m_x - m_actor->get2DPos().m_x);
                        f32 distY = f32_Abs(closestActor->get2DPos().m_y - m_actor->get2DPos().m_y);

                        if (distX <= getTemplate()->getDistXSnap() && distY <= getTemplate()->getDistYSnap())
                        {
                            Ray_EventSnapDance eventSnapDance; 
                            eventSnapDance.setSender(m_actor->getRef());
                            eventSnapDance.setSnap(btrue);
                            closestActor->onEvent(&eventSnapDance); 
                            m_playerIsSnap = btrue;

                            closestActor->setFlipped(bfalse);

                        }
                    }
                }
            }
        }
        


    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::updateFeedback()
    {

        // Anim
        const StringID offToOn = ITF_GET_STRINGID_CRC(Off_to_On,1956078406);
        const StringID onToOff = ITF_GET_STRINGID_CRC(On_to_Off,3896579405);
        const StringID animGreen = ITF_GET_STRINGID_CRC(Stand_Light_Green,433633643);
        const StringID animRed = ITF_GET_STRINGID_CRC(Stand_Light_Red,1702580486);
         
        // FX
        const StringID fxRed = ITF_GET_STRINGID_CRC(OukloumDoor_SteleFX1,1961751638);
        const StringID fxGreen = ITF_GET_STRINGID_CRC(OukloumDoor_SteleFX2,2274892341);


        if (m_animLightComponent)
        {

            if (m_doorIsOpen)
            {
                if (!m_animOpenDoorIsPlayed)
                {
                    m_animLightComponent->setAnim(onToOff);
                    stopFX(m_handleFxRed);
                    stopFX(m_handleFxGreen);
                    m_animOpenDoorIsPlayed = btrue;
                }
            }
            else if (m_danceIsStarted)
            {
                if (m_validTemposInARow == 0 && m_stateFeedBack != STATE_FEEDBACK_RED)
                {
                    m_animLightComponent->setAnim(animRed);

                    if (m_handleFxGreen != U32_INVALID)
                    {
                        stopFX(m_handleFxGreen);
                        m_handleFxGreen = U32_INVALID;
                    }

                    m_handleFxRed = startFX(fxRed);

                    m_stateFeedBack = STATE_FEEDBACK_RED;

                }
                else if (m_validTemposInARow > 0 && m_stateFeedBack != STATE_FEEDBACK_GREEN)
                {
                    m_animLightComponent->setAnim(animGreen);

                    if (m_handleFxRed != U32_INVALID)
                    {
                        stopFX(m_handleFxRed);
                        m_handleFxRed = U32_INVALID;
                    }

                    m_handleFxGreen = startFX(fxGreen);

                    m_stateFeedBack = STATE_FEEDBACK_GREEN;

                }
            }
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::validatePlayer( f32 axis, const StringID& action )
    {
        Player* player = GAMEMANAGER->getPlayerFromActor(m_dancer);
        if (player && player->getActive())
        {
            bbool inputUp   = axis > 0.6f;
            bbool inputDown = axis < -0.6f;

            if (m_upTempo.m_open)
            {
                if (!m_upTempo.m_validated)
                {
                    if (inputUp)
                    {
                        m_upTempo.m_validated = btrue;
                    }
                }
            }
            else if (m_downTempo.m_open)
            {
                if (!m_downTempo.m_validated)
                {
                    if (inputDown)
                    {
                        m_downTempo.m_validated = btrue;
                    }
                }
            }
        }
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::Tempo::update( f32 _metronome, u32& _validTemposInARow )
    {
        bbool open = m_min < _metronome && _metronome < m_max;

        if (m_open && !open)
        {

            // just closed
            if (m_validated)
            {
                _validTemposInARow++;
            }
            else
            {
                _validTemposInARow = 0;
            }
            m_validated = bfalse;
        }

        m_open = open;


    }

    // ****************************************************************************
    void Ray_DanceStoneComponent::Tempo::validate( bbool _condition )
    {
    }

    // ****************************************************************************

    Actor* Ray_DanceStoneComponent::findActorByName( const String8& _name )
    {
        Actor* pActor = GetActor();
        Scene* scene = pActor ? pActor->getScene() : NULL;
        return scene ? scene->getActorFromUserFriendly(_name) : NULL;
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::Receive( u32 player, f32 axis, const StringID& action )
    {
        if (m_dancer != ITF_INVALID_OBJREF && GAMEMANAGER->isPlayerActor(m_dancer) )
        {
            Player* curPlayer = GAMEMANAGER->getPlayerFromActor(m_dancer);
            if (curPlayer && curPlayer->getIndex() == player)
            {
                if (action == input_MoveY)
                {
                    validatePlayer(axis, action);
                }
            }
        }        
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        GAMEMANAGER->addGamePlayInputListener(this);
    }

    // ****************************************************************************

    void Ray_DanceStoneComponent::onBecomeInactive()
    {
        Super::onBecomeActive();
        GAMEMANAGER->removeInputListener(this);
    }

    // ****************************************************************************

#ifdef ITF_SUPPORT_EDITOR
    void Ray_DanceStoneComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
            return;

        DebugDraw::text("real: %.2f", METRONOME_MANAGER->getBarPercentage());

        {
            f32 metronome = METRONOME_MANAGER->getBarPercentage(getTemplate()->getSyncOffset(), getTemplate()->getSyncRatio());
            const Color& color = m_upTempo.m_open ? (m_upTempo.m_validated ? Color::green() : Color::red()) :
                                 m_downTempo.m_open ? (m_downTempo.m_validated ? Color::green() : Color::red()) :
                                 Color::white();
            if (m_upTempo.m_open)
            {
                DebugDraw::text(m_actor->get2DPos() + Vec2d(0.0f,1.0f), 0.0f, color, "UP | ratio: %.2f", metronome);
            }
            if (m_downTempo.m_open)
            {
                DebugDraw::text(m_actor->get2DPos() - Vec2d(0.0f,1.0f), 0.0f, color, "DOWN | ratio: %.2f", metronome);
            }


        }

        DebugDraw::text(m_actor->get2DPos() + Vec2d(-1.0f,1.8f), 0.0f, Color::white(), "validTempo : %d", m_validTemposInARow);
    }
#endif

     // ****************************************************************************
    // Template
    // ****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_DanceStoneComponent_Template)
    BEGIN_SERIALIZATION(Ray_DanceStoneComponent_Template)
        SERIALIZE_MEMBER("dancerShouldFlip", m_dancerShouldFlip);
        SERIALIZE_OBJECT("upTempo", m_upTempo);
        SERIALIZE_OBJECT("downTempo", m_downTempo);
        SERIALIZE_MEMBER("syncRatio", m_syncRatio);
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
        SERIALIZE_MEMBER("numTemposToUnlock", m_numTemposToUnlock);

        SERIALIZE_MEMBER("distXSnap", m_distXSnap);
        SERIALIZE_MEMBER("distYSnap", m_distYSnap);

        SERIALIZE_MEMBER("numLumNormalReward", m_numLumNormalReward);
        SERIALIZE_MEMBER("numLumPowerReward", m_numLumPowerReward);

    END_SERIALIZATION()

    // ****************************************************************************

    Ray_DanceStoneComponent_Template::Ray_DanceStoneComponent_Template()
    : m_numTemposToUnlock(6)
    , m_upTempo(0.f,0.2f)
    , m_downTempo(0.5,0.7f)
    , m_syncRatio(1.f)
    , m_syncOffset(0.f)
    , m_dancerShouldFlip(bfalse)
    , m_distXSnap(0.1f)
    , m_distYSnap(0.4f)
    , m_numLumNormalReward(1)
    , m_numLumPowerReward(1)
    {
        //none
    }

    // ****************************************************************************


    BEGIN_SERIALIZATION_SUBCLASS(Ray_DanceStoneComponent_Template, Tempo)
        SERIALIZE_MEMBER("min", m_min);
        SERIALIZE_MEMBER("max", m_max);
    END_SERIALIZATION()
};