#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_WM_FOOTPATHCOMPONENT_H_
#include "rayman/gameplay/Components/Worldmap/Ray_WM_FootpathComponent.h"
#endif //_ITF_RAY_WM_FOOTPATHCOMPONENT_H_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_SpotComponent_Template)

    BEGIN_SERIALIZATION(Ray_WM_SpotComponent_Template)
        SERIALIZE_MEMBER("closedAnim",m_closedAnim);
        SERIALIZE_MEMBER("newAnim",m_newAnim);
        SERIALIZE_MEMBER("cannotEnterAnim",m_cannotEnterAnim);
        SERIALIZE_MEMBER("openAnim",m_openAnim);
        SERIALIZE_MEMBER("completedAnim",m_completedAnim);
        SERIALIZE_MEMBER("textNameID",m_textNameID);
        SERIALIZE_MEMBER("isMid",m_isMid);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_SpotComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_SpotComponent)
        SERIALIZE_MEMBER("tag", m_tag);
        SERIALIZE_MEMBER("isStandSwim", m_isStandSwim);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_WM_SpotComponent)
#ifndef ITF_FINAL
    VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "tag", m_tag != StringID::Invalid, "must be set to work properly");
#endif // ITF_FINAL
    VALIDATE_COMPONENT_PARAM("closedAnim", getTemplate()->getClosedAnim() || getTemplate()->isMid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("newAnim", getTemplate()->getNewAnim() || getTemplate()->isMid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("cannotEnterAnim", getTemplate()->getCannotEnterAnim() || getTemplate()->isMid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("openAnim", getTemplate()->getOpenAnim() || getTemplate()->isMid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("completedAnim", getTemplate()->getCompletedAnim() || getTemplate()->isMid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("", m_animatedComponent || getTemplate()->isMid(), "AnimatedComponent mandatory");
    VALIDATE_COMPONENT_PARAM("", m_footpathComponent, "Ray_WM_FootpathComponent mandatory");
    END_VALIDATE_COMPONENT()

    Ray_WM_SpotComponent::Ray_WM_SpotComponent()
        : m_tag(StringID::Invalid)
        , m_activateCount(0)
        , m_state(SPOT_STATE_CLOSED)
        , m_isConnected(bfalse)
        , m_isPlayerPresent(bfalse)
        , m_animatedComponent(NULL)
        , m_footpathComponent(NULL)
        , m_textBoxesComponent(NULL)
        , m_electoons(0)
        , m_hasCup(bfalse)
		, m_isFrescoLevel(bfalse)
		, m_hasNoRelics(bfalse)
		, m_hasAllRelics(bfalse)
        , m_hasPrize(bfalse)
        , m_is3Slots(bfalse)
        , m_isNoSlot(bfalse)
        , m_isStandSwim(bfalse)
    {
    }

    Ray_WM_SpotComponent::~Ray_WM_SpotComponent()
    {
    }


    void Ray_WM_SpotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_textBoxesComponent = m_actor->GetComponent<UITextBoxesComponent>();
        
        // mandatory, validated
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        if (m_animatedComponent)
        {
            m_animatedComponent->setUpdateAnimInput(this);
        }

        // mandatory, validated
        m_footpathComponent = m_actor->GetComponent<Ray_WM_FootpathComponent>();

        // "fake" spot tag
        if (getTemplate()->isMid())
        {
            m_tag = StringID(m_actor->getUserFriendly());
        }

        // register to events
        // Setup worldmap info
        ObjectPath path;
        SceneObjectPathUtils::getAbsolutePathFromObject(m_actor, path);
        RAY_GAMEMANAGER->setWMSpotObjectPath(m_tag, path);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventWMOpenPaths,2918906348), this);
    }

    void Ray_WM_SpotComponent::onSceneActive()
    {
        Super::onSceneActive();

        // force open paths
        if (m_state == SPOT_STATE_COMPLETED)
        {
            if (getTemplate()->isMid())
            {
                // NB[LaurentCou]: do nothing, mid nodes chain open when reached
                // by their predecessor footpath
            }
            else if (!RAY_GAMEMANAGER->isJustCompletedSpot(m_tag))
            {
                // force open all footpath
                m_footpathComponent->forceOpen();

                // send event to children (fluid falls)
                EventGeneric eventGeneric;
                eventGeneric.setId(ITF_GET_STRINGID_CRC(ForceOpen,410328037));
                LinkComponent * linkComponent = m_actor->GetComponent<LinkComponent>();
                if (linkComponent)
                {
                    AIUtils::LinkIterator iterator(linkComponent);
                    while (Actor* child = iterator.getNextRelativeActorTagFilter(
                        ITF_GET_STRINGID_CRC(roots,2814133504), btrue)) // ignore roots: RO-15317
                    { 
                        if (!child->GetComponent<TrajectoryNodeComponent>())
                        {
                            child->onEvent(&eventGeneric);
                        }
                    }
                }
            }
            else if (RAY_GAMEMANAGER->isJustCompletedSpotBWorldOnly(m_tag))
            {
                // force open all footpaths to non-B-worlds
                m_footpathComponent->forceOpenNonBWorlds();
            }
            // else -> will be open (no forcing) with game manager sequence
        }
        else
        {
            // no open, hide what needs to be hidden
            m_footpathComponent->initNoOpen();
        }
    }

    void Ray_WM_SpotComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
    }

    void Ray_WM_SpotComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        // "time to open paths" says the game manager sequence
        if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventWMOpenPaths,2918906348)))
        {
            if (getTemplate()->isMid())
            {
                // NB[LaurentCou]: do nothing, mid nodes chain open when reached
                // by their predecessor footpath
            }
            else if (RAY_GAMEMANAGER->isJustCompletedSpot(m_tag))
            {
                if (!RAY_GAMEMANAGER->isJustCompletedSpotBWorldOnly(m_tag))
                {
                    // open all footpath
                    m_footpathComponent->open();

                    // send event to children (fluid falls)
                    EventGeneric eventGeneric;
                    eventGeneric.setId(ITF_GET_STRINGID_CRC(Open,812432275));
                    LinkComponent * linkComponent = m_actor->GetComponent<LinkComponent>();
                    if (linkComponent)
                    {
                        AIUtils::LinkIterator iterator(linkComponent);
                        while (Actor* child = iterator.getNextRelativeActorTagFilter(
                            ITF_GET_STRINGID_CRC(roots,2814133504), btrue)) // ignore roots: RO-15317
                        { 
                            if (!child->GetComponent<TrajectoryNodeComponent>())
                            {
                                child->onEvent(&eventGeneric);
                            }
                        }
                    }
                }
                else
                {
                    // open all footpaths to B-worlds
                    m_footpathComponent->openBWorlds();
                }
            }
        }
    }

    void Ray_WM_SpotComponent::Draw()
    {
        Super::Draw();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_WM_SpotComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const
    {
        const Vec3d position3d = m_actor->getPos();
        Vec3d position2D;
        GFX_ADAPTER->compute3DTo2D(position3d, position2D);

        switch (m_state)
        {
        case SPOT_STATE_CLOSED:
            DebugDraw::text(position2D.m_x, position2D.m_y, Color::red(), "state : CLOSED");
            break;
        case SPOT_STATE_NEW:
            DebugDraw::text(position2D.m_x, position2D.m_y, Color::blue(), "state : NEW");
            break;
        case SPOT_STATE_OPEN:
            DebugDraw::text(position2D.m_x, position2D.m_y, Color::green(), "state : OPEN");
            break;
        case SPOT_STATE_COMPLETED:
            DebugDraw::text(position2D.m_x, position2D.m_y, Color::white(), "state : COMPLETED");
            break;
        case SPOT_STATE_CANNOT_ENTER:
            DebugDraw::text(position2D.m_x, position2D.m_y, Color::violet(), "state : CANNOT ENTER\n Electoons : %d / %d\n  Teeth : %d / %d",
                RAY_GAMEMANAGER->getElectoonsCount(), RAY_GAMEMANAGER->getMinElectoons(m_tag), RAY_GAMEMANAGER->getTotalTakenToothCount(), RAY_GAMEMANAGER->getMinTeeth(m_tag) );
            break;
        }
    }
#endif

    void Ray_WM_SpotComponent::updateAnimInput()
    {
        ITF_ASSERT(m_animatedComponent);
        if (!m_animatedComponent)
        {
            return;
        }
        
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(IsPlayerPresent,1207454410), m_isPlayerPresent);

        StringID type = RAY_GAMEMANAGER->getMapConfigType(m_tag);
        bbool isComplete = bfalse;
        if ( RAY_GAMEMANAGER->getLevelData(m_tag) )
            m_electoons = Ray_AIUtils::computeElectoonMask(type, RAY_GAMEMANAGER->getLevelData(m_tag), NULL, bfalse, isComplete);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon1,1434682974), m_electoons & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon2,925843018), (m_electoons >> 1) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon3,900890873), (m_electoons >> 2) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon4,4023379268), (m_electoons >> 3) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon5,4245588725), (m_electoons >> 4) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon6,3475466807), (m_electoons >> 5) & 1);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(hasCup,2194220942), RAY_GAMEMANAGER->isChallengeWon(m_tag,CHALLENGE_CUP));
		
		m_isFrescoLevel = bfalse;
		m_hasNoRelics = bfalse;
		m_hasAllRelics = bfalse;
		u32 levelMaxRelics = RAY_GAMEMANAGER->getMaxRelics(m_tag);
		if(levelMaxRelics)
		{
			m_isFrescoLevel = btrue;
			u32 takenRelicCount = RAY_GAMEMANAGER->getTakenRelicCount(m_tag);
			if(takenRelicCount >= levelMaxRelics)
			{
				m_hasAllRelics  = btrue;
			}
			else
			{
				m_hasNoRelics  = btrue;
			} 
		}

		m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(isFrescoLevel,1870229400), m_isFrescoLevel);
		m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(hasNoRelics,4057855036), m_hasNoRelics);
		m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(hasAllRelics,3151812660), m_hasAllRelics);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(hasPrize,2525309951), RAY_GAMEMANAGER->isChallengeWon(m_tag,CHALLENGE_LUMATTACK3));

        const Ray_GameManagerConfig_Template::MissionConfig *config = RAY_GAMEMANAGER->findMissionConfigByType(type, btrue);
        m_is3Slots = bfalse;
        m_isNoSlot = bfalse;
        if ( config->m_medalSlots.size() == 3 )
            m_is3Slots = btrue;
        else if ( config->m_medalSlots.size() == 0 )
            m_isNoSlot = btrue;
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(is3Slots,3580378956),m_is3Slots);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(isNoSlot,153266289),m_isNoSlot);

        
        // has Warning
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(hasWarning,4090459567),RAY_GAMEMANAGER->getWMSpotHasWarning(m_tag));
    }

    void Ray_WM_SpotComponent::onEnter(bbool _changeMusic /*= bfalse*/)
    {
        ITF_ASSERT(!m_isPlayerPresent);
        m_isPlayerPresent = btrue;

        // When we came from the home map, we have to update the Rich Presence 
        GAMEMANAGER->updateRichPresenceForActivePlayers(PRESENCE_GAMESCREEN_INWORLDMAP);

        m_footpathComponent->onEnter();
    }
    
    void Ray_WM_SpotComponent::onExit()
    {
        ITF_ASSERT(m_isPlayerPresent);
        m_isPlayerPresent = bfalse;

        m_footpathComponent->onExit();
    }

    void Ray_WM_SpotComponent::onConnection()
    {
        if (getTemplate()->isMid())
        {
            changeState(SPOT_STATE_COMPLETED);
        }
    }

    void Ray_WM_SpotComponent::changeState( ESpot_State _newState )
    {
        switch (_newState)
        {
        case SPOT_STATE_CLOSED:
            // play closed animation
            if (m_animatedComponent)
            {
                m_animatedComponent->setAnim(getTemplate()->getClosedAnim());
            }
        	break;
        case SPOT_STATE_NEW:
            // play new animation
            if (m_animatedComponent)
            {
                m_animatedComponent->setAnim(getTemplate()->getNewAnim());
            }
            if ( m_state == SPOT_STATE_CANNOT_ENTER )
            {
                RAY_GAMEMANAGER->setWMSpotHasWarning(getTag(),btrue);
            }
            break;
        case SPOT_STATE_OPEN:
            // play open animation
            if (m_animatedComponent)
            {
                m_animatedComponent->setAnim(getTemplate()->getOpenAnim());
            }
            if ( m_state == SPOT_STATE_CANNOT_ENTER )
            {
                RAY_GAMEMANAGER->setWMSpotHasWarning(getTag(),btrue);
            }
            break;
        case SPOT_STATE_CANNOT_ENTER:
            // play cannot enter animation
            if (m_animatedComponent)
            {
                m_animatedComponent->setAnim(getTemplate()->getCannotEnterAnim());
            }
            break;
        case SPOT_STATE_COMPLETED:
            // play completed animation
            if (m_animatedComponent)
            {
                m_animatedComponent->setAnim(getTemplate()->getCompletedAnim());
            }
            break;
        default:
            ITF_ASSERT(0); // Should never be here
            break;
        }

        m_state = _newState;
        saveState(_newState);

        // info to write depends on its state, so refresh data if needed
        writeInfo();
    }

    bbool Ray_WM_SpotComponent::isMid() const
    {
        return getTemplate()->isMid();
    }

    void Ray_WM_SpotComponent::writeInfo()
    {
        if ( m_textBoxesComponent == NULL )
            return;

        // Level Name
        m_textBoxesComponent->setLineId(getTemplate()->getTextNameID(),RAY_GAMEMANAGER->getTitleId(m_tag));
    }

    bbool Ray_WM_SpotComponent::isAccessible()
    {
        return (RAY_GAMEMANAGER->getWMSpotState(m_tag) != SPOT_STATE_CLOSED);
    }

    void Ray_WM_SpotComponent::saveState(ESpot_State _state)
    {
        // Save state into specific level persistent game data
        RAY_GAMEMANAGER->setWMSpotState(m_tag,_state);
    }
};
