#include "precompiled_engine.h"

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifndef _ITF_PLAYANIM_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayAnim_evt.h"
#endif //_ITF_PLAYANIM_EVT_H_

#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_PLAYSPAWN_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySpawn_evt.h"
#endif //_ITF_PLAYSPAWN_EVT_H_

#ifndef _ITF_PLAYLOOP_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayLoop_evt.h"
#endif //_ITF_PLAYLOOP_EVT_H_

#ifndef _ITF_PLAYSKIP_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySkip_evt.h"
#endif //_ITF_PLAYSKIP_EVT_H_

#ifndef _ITF_LABEL_EVT_H_
#include "engine/sequenceplayer/seq_events/Label_evt.h"
#endif //_ITF_LABEL_EVT_H_

#ifndef _ITF_REDIRECTPATCHBANK_EVT_H_
#include "engine/sequenceplayer/seq_events/RedirectPatchBank_evt.h"
#endif //_ITF_REDIRECTPATCHBANK_EVT_H_

#ifndef _ITF_STARTCAPTURE_EVT_H_
#include "engine/sequenceplayer/seq_events/StartCapture_evt.h"
#endif //_ITF_STARTCAPTURE_EVT_H_

#ifndef _ITF_EXECUTELUA_EVT_H_
#include "engine/sequenceplayer/seq_events/ExecuteLUA_evt.h"
#endif //_ITF_EXECUTELUA_EVT_H_

#ifndef _ITF_PLAYGAMEPLAYEVENT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayGameplay_evt.h"
#endif //_ITF_PLAYGAMEPLAYEVENT_EVT_H_

#ifndef _ITF_PLAYWAIT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayWait_evt.h"
#endif //_ITF_PLAYWAIT_EVT_H_

#ifndef _ITF_PLAYSOUND_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySound_evt.h"
#endif //_ITF_PLAYSOUND_EVT_H_

#ifndef _ITF_PLAYTEXT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayText_evt.h"
#endif //_ITF_PLAYTEXT_EVT_H_

#ifndef _ITF_PLAYTEXTBANNER_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTextBanner_evt.h"
#endif //_ITF_PLAYTEXTBANNER_EVT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#include "engine/actors/components/ParticleGeneratorComponent.h"
#endif //_ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_TRAJECTORY_H_
#include "engine/animation/trajectory.h"
#endif //_ITF_TRAJECTORY_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_


#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"


////////////////////////////////////
///////// Exepctions

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSPAWNCOMPONENT_H_
#include "gameplay/Components/Common/ActorSpawnComponent.h"
#endif //_ITF_ACTORSPAWNCOMPONENT_H_


#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_


#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_VIDEOCAPTURECOMMAND_H_
#include "engine/misc/videocaptureCommand.h"
#endif //_ITF_VIDEOCAPTURECOMMAND_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_MATHF32_H_
#include "core/math/Mathf32.h"
#endif //_ITF_MATHF32_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_
#include <algorithm>
namespace ITF
{
    // -> Need changing flag
IMPLEMENT_OBJECT_RTTI(SequenceEvent)

IMPLEMENT_OBJECT_RTTI(SequenceEvent_Template)
BEGIN_SERIALIZATION(SequenceEvent_Template)

    SERIALIZE_MEMBER("StartFrame",m_startFrame);
    SERIALIZE_MEMBER("Offset",m_offset);
    SERIALIZE_MEMBER("Duration",m_duration);
    SERIALIZE_MEMBER("TrackLine",m_trackline);
    SERIALIZE_MEMBER("Channel",m_channel);

    BEGIN_CONDITION_BLOCK(ESerialize_Editor)
        SERIALIZE_MEMBER("Selected",m_selected);
        SERIALIZE_MEMBER("DisabledForTesting",m_disableForTesting);
    END_CONDITION_BLOCK()

    SERIALIZE_FUNCTION(computeChannelId, ESerialize_Data_Load);
END_SERIALIZATION()


IMPLEMENT_OBJECT_RTTI(SequenceEventWithActor_Template)
BEGIN_SERIALIZATION_CHILD(SequenceEventWithActor_Template)
#ifdef ITF_SUPPORT_COOKING
    SERIALIZE_FUNCTION(computeObjectID, ESerialize_Data_Save);
#endif //ITF_SUPPORT_COOKING

    SERIALIZE_MEMBER("TargetFriendlyName",m_objectPath);
    SERIALIZE_MEMBER("DisableComponentsActor",m_disableComponentsActor);
    SERIALIZE_MEMBER("TargetID",m_objectID);

#ifdef ITF_SUPPORT_COOKING
    SERIALIZE_FUNCTION(computeObjectID, ESerialize_Data_Load);
#endif //ITF_SUPPORT_COOKING

END_SERIALIZATION()

#ifdef ITF_SUPPORT_BLOBEVENT
#define createBLOB(_name) _name::createFromBlob
#else
#define createBLOB(_name)  NULL
#endif

SequenceEvtMapping SqEvtMapping[] = {
    {SequenceEvent_Template::event_PlayAnim,            "PlayAnim",          PlayAnim_evt::create,         createBLOB(PlayAnim_evtTemplate) },
    {SequenceEvent_Template::event_PlayTrajectory,      "PlayTrajectory",    PlayTrajectory_evt::create,   createBLOB(PlayTrajectory_evtTemplate) },
    {SequenceEvent_Template::event_PlaySpawn,           "PlaySpawn",         PlaySpawn_evt::create,        createBLOB(PlaySpawn_evtTemplate) },
	{SequenceEvent_Template::event_PlayLoop,            "PlayLoop",          PlayLoop_evt::create,         createBLOB(PlayLoop_evtTemplate) },
	{SequenceEvent_Template::event_PlaySkip,            "PlaySkip",          PlaySkip_evt::create,         createBLOB(PlaySkip_evtTemplate) },
    {SequenceEvent_Template::event_StartCapture,        "StartCapture",      StartCapture_evt::create,     createBLOB(StartCapture_evtTemplate) },
    {SequenceEvent_Template::event_ExecuteLUA,          "ExecuteLUA",        ExecuteLUA_evt::create,       createBLOB(ExecuteLUA_evtTemplate)},
    {SequenceEvent_Template::event_GameplayEvent,       "GameplayEvent",     PlayGameplay_evt::create,     createBLOB(PlayGameplay_evtTemplate) },
    {SequenceEvent_Template::event_PlayWait,            "PlayWait",          PlayWait_evt::create,         createBLOB(PlayWait_evtTemplate) },
    {SequenceEvent_Template::event_PlaySound,           "PlaySound",         PlaySound_evt::create,        createBLOB(PlaySound_evtTemplate) },
    {SequenceEvent_Template::event_PlayText,            "PlayText",          PlayText_evt::create,         createBLOB(PlayText_evtTemplate) },
    {SequenceEvent_Template::event_RedirectPatchBank,   "RedirectPatchBank", RedirectPatchBank_evt::create,createBLOB(RedirectPatchBank_evtTemplate) },
    {SequenceEvent_Template::event_Label,               "Label",             Label_evt::create,            createBLOB(Label_evtTemplate) },
    {SequenceEvent_Template::event_PlayTextBanner,      "PlayTextBanner",    PlayTextBanner_evt::create,   createBLOB(PlayTextBanner_evtTemplate) },
};

#define MAPPED_SEQ_EVT_COUNT (sizeof(SqEvtMapping) / sizeof(SqEvtMapping[0]))

IMPLEMENT_OBJECT_RTTI(SequencePlayerComponent)

BEGIN_SERIALIZATION_CHILD(SequencePlayerComponent)
    SERIALIZE_MEMBER("bankState", m_bankState);
END_SERIALIZATION()

#define MIN_FRAME        -100000
#define MIN_FRAME_INIT   (2 * MIN_FRAME)

SequenceEvent::SequenceEvent(SequencePlayerComponent* _ownerSequence) :
m_ownerSequence(_ownerSequence)
{
    initDefaultValues();
}

SequenceEvent::SequenceEvent() :
m_ownerSequence(NULL)
{
    initDefaultValues();
}

void SequenceEvent::initDefaultValues()
{
    m_active            = bfalse;
    m_enableByChannel   = btrue;

    m_template          = NULL;
}

/*
void SequenceEvent::processDataChange( Blob* _blob )
{
    bbool disableForTesting = getTemplate()->isDisableForTesting();

    getTemplate()->fillCommonFromBlob( _blob );

    if(disableForTesting != getTemplate()->isDisableForTesting())
    {
        // enable -> disable
        if(getTemplate()->isDisableForTesting())
        {
            onBecomeInactive();
        }
        // enable -> disable
        else
        {
            onBecomeActive();
        }
    }

    if (m_ownerSequence)
    {
        m_ownerSequence->setEventFrame( getTemplate()->getStartFrame()+ getTemplate()->getOffset(),
                                        getTemplate()->getStopFrame() + getTemplate()->getOffset());
    }
}
*/

void SequenceEvent::onBecomeActive  (    )
{
    setActive(btrue);
}

void SequenceEvent::onBecomeInactive(    )
{
    setActive(bfalse);
}


bbool SequenceEvent::isReload = false;

SequenceEvent::~SequenceEvent()
{
	ITF_ASSERT_CRASH(!isReload, "Deleting template inside soft reload");
}


IMPLEMENT_OBJECT_RTTI(SequencePlayerComponent_Template)
BEGIN_SERIALIZATION(SequencePlayerComponent_Template)

#ifdef ITF_SUPPORT_COOKING
    BEGIN_CONDITION_BLOCK(ESerialize_DataBin)
        SERIALIZE_FUNCTION(preSerializeSave, ESerialize_Data_Save);
    END_CONDITION_BLOCK()
#endif //ITF_SUPPORT_COOKING

    SERIALIZE_CONTAINER_WITH_FACTORY("events",m_events,ACTORSMANAGER->getSequenceEventFactory());
    SERIALIZE_MEMBER("unskippableDurationFirstTime", m_unskippableDurationFirstTime);
    SERIALIZE_MEMBER("deactivatedPlayers", m_deactivatedPlayers);
	SERIALIZE_MEMBER("reactivateAlive", m_reactivateAlive);
    SERIALIZE_MEMBER("reactivatePlayers", m_reactivatePlayers);
    SERIALIZE_MEMBER("saveInitialView", m_saveInitialView);

    // ITF_FINAL
    SERIALIZE_CONTAINER_OBJECT("trackList", m_trackList);

    SERIALIZE_FUNCTION(updateHierarchy, ESerialize_Data_Load);
END_SERIALIZATION()


u32 SequenceEvent_Template::s_count = 0;

SequenceEvent_Template::SequenceEvent_Template(SequencePlayerComponent_Template* _ownerSequence)
: m_ownerSequence(_ownerSequence)
{
    initDefaultValues();
}

SequenceEvent_Template::SequenceEvent_Template()
: m_ownerSequence(NULL)
{
    initDefaultValues();
}


void SequenceEvent_Template::initDefaultValues()
{
    m_startFrame        = 0;
    m_offset            = 0;
    m_duration          = 1;
    m_trackline         = 1;
    m_channel           = "";
    m_disableForTesting = bfalse;    // enable/disable by sequence editor test, is not serialized
    m_selected          = -1;
    m_UID               = ((0xffff & u32(uPtr(this))) << 16) + (s_count++ & 0xffff);
}

void SequenceEvent_Template::serializeCommonToBlob (Blob* _blob)
{
    // type
    _blob->pushInt32(getType());

    // objectRef
    _blob->pushUInt32(getUID());


    // event data
    _blob->pushInt32(m_startFrame);
    _blob->pushInt32(m_offset);
    _blob->pushInt32(m_duration);
    _blob->pushInt32(m_trackline);
    _blob->pushString(m_channel);

    _blob->pushBool(m_disableForTesting);
    _blob->pushInt32(m_selected);
}


void SequenceEvent_Template::fillCommonFromBlob( Blob* _blob )
{
    i32 startFrame = _blob->extractInt32();
    setStartFrame(startFrame);

    i32 offset = _blob->extractInt32();
    setOffset(offset);

    i32 duration = _blob->extractInt32();
    setDuration(duration);

    i32 trackLine = _blob->extractInt32();
    setTrackLine(trackLine);

    String channel_str;
    _blob->extractString(channel_str);
    m_channel = String8(channel_str.cStr());

    m_disableForTesting = _blob->extractBool();

    m_selected = _blob->extractInt32();

}

ITF_INLINE  bbool SequenceEvent_Template::isDisableForTesting() const
{
    // because tracks can be disabled is Disable for testing must check parent tracks
#ifdef ITF_WINDOWS
    if (m_disableForTesting)
        return btrue;

    if (!m_ownerSequence)
        return bfalse;

    u32 currentTrackLine = m_trackline;
    ITF_VECTOR<SequenceTrackInfo_Template> &trackList =  m_ownerSequence->getTrackList();
    u32 maxLevels = 10;
    while (currentTrackLine != U32_INVALID && currentTrackLine < trackList.size() && --maxLevels)
    {
        if (!trackList[currentTrackLine].isEnable())
            return btrue;

        currentTrackLine = trackList[currentTrackLine].getParentGroup();
    }
    ITF_ASSERT_MSG(maxLevels, "preventing infinite loop");
    return bfalse;

#else //ITF_WINDOWS
    return m_disableForTesting;
#endif
}

IMPLEMENT_OBJECT_RTTI(SequenceEventWithActor)

SequenceEventWithActor::SequenceEventWithActor(SequencePlayerComponent* _ownerSequence)
: m_actorPlayerIndex(U32_INVALID)
{

}

SequenceEventWithActor::SequenceEventWithActor()
: m_actorPlayerIndex(U32_INVALID)
{
}

bbool SequenceEventWithActor::linkBindedActor( bbool _forceLink )
{
    // actor already control
    Actor* actor = m_controlledActor.getActor();


    if(actor && !_forceLink)
        return btrue;

    m_controlledActor.invalidate();
    if(!m_ownerSequence)
        return bfalse;

    actor = m_ownerSequence->getActorFromObjectPath(getTemplate()->getObjectPath(), getTemplate()->getObjectID());
    if(!actor)
        return bfalse;

    m_controlledActor = actor->getRef();

    // check if actor has same name as player
    SequencePlayerComponent::actorIsSequencePlayer(getTemplate()->getObjectID(), m_actorPlayerIndex);

    return btrue;
}


void  SequenceEventWithActor::onBecomeActive()
{
    Super::onBecomeActive();
    linkBindedActor();
    // prefetch will
    // 1 -> link binded actor
    // 2 -> load resources if necessary
    // prefetchResources();
}

void SequenceEventWithActor::prefetchResources(i32 _pos)
{
    linkBindedActor();
    Actor* pActor = getBindedActor();
    if (pActor && pActor != m_ownerSequence->GetActor())
    {
        pActor->loadResources();
        SequencePlayerComponent *comp = pActor->GetComponent<SequencePlayerComponent>();
        if (comp)
            comp->prefetchResources(_pos-getTemplate()->getStartFrame());
    }
}

Actor* SequenceEventWithActor::getBindedActor()
{
    return m_controlledActor.getActor();
}

SequenceEventWithActor_Template::SequenceEventWithActor_Template(SequencePlayerComponent_Template* _ownerSequence) :
m_disableComponentsActor(btrue)
{
}

SequenceEventWithActor_Template::SequenceEventWithActor_Template() :
m_disableComponentsActor(btrue)
{
}


void SequenceEventWithActor_Template::computeObjectID()
{
    ITF_STDSTRING   friendlyStr;
    m_objectPath.toString(friendlyStr);

    m_objectID = friendlyStr;
}

void  SequenceEventWithActor_Template::serializeFriendlyToBlob  (Blob* _blob)
{
    ITF_STDSTRING objStr;
    m_objectPath.toString( objStr );

    _blob->pushString8(objStr.c_str());
    _blob->pushBool(m_disableComponentsActor);
}

void  SequenceEventWithActor_Template::fillFriendlyFromBlob     (Blob* _blob)
{
    String8  targetFriendlyName_str;
    _blob->extractString8(targetFriendlyName_str);
    m_objectPath.fromString(targetFriendlyName_str.cStr());

    m_disableComponentsActor = _blob->extractBool();

    computeObjectID();
}



//////////////////////////////////////////////////////////////////////////
const i32 SequencePlayerComponent::s_resourcePrefetch = 90; // prefetch frames

SequencePlayerComponent::SequencePlayerComponent()
: Super()
{
    m_curFrame_rel = 0;
    m_curTime_rel = 0.f;
    m_sequenceSpeedFactor = 1.f;
    m_bUseAnchor = bfalse;
    setState(State_Stopped);
    m_bCameraControlAllowed = btrue;
    m_originalCamMode = CAMERA->getMode();
    m_shortCutKey = '0';
    m_firstEventFrame = MIN_FRAME_INIT;
    m_lastEventFrame = MIN_FRAME_INIT;
    m_frameStartSaved = 0;
    m_editionMode = bfalse;
    m_skipSequenceEvent = U32_INVALID;
    m_deleting = bfalse;
    m_unskippableFramesFirstView = 0;
    m_deactivatedPlayers = bfalse;
    m_deactivatedPlayersDone = bfalse;
    m_externInactiveChannels = NULL;
    m_bankState = U32_INVALID;
    m_inputListenerRegistered = bfalse;
    m_menuValidPressed = bfalse;
    m_menuBackPressed = bfalse;
    m_skipHoldTimer = 0.f;
    m_skipRequiredDuration = SKIP_HOLD_DURATION;
    m_isSkipButtonHeld = bfalse;
    m_sequenceView = NULL;
}


void SequencePlayerComponent::refreshLimitFrameSequence()
{
    m_firstEventFrame   = MIN_FRAME_INIT;
    m_lastEventFrame    = MIN_FRAME_INIT;
    for (u32 i  = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent_Template *evt = (SequenceEvent_Template *)m_eventsList[i]->getTemplate();
        setEventFrame(evt->getStartFrame() + evt->getOffset(), evt->getStopFrame() + evt->getOffset());
    }
}

SequencePlayerComponent::~SequencePlayerComponent()
{
    m_deleting = btrue;

    stop();
    u32 eventCount = m_eventsList.size();
    for (u32 i=0; i<eventCount; i++)
    {
        SF_DEL(m_eventsList[i]);
    }
    m_eventsList.clear();


    ITF_ASSERT(m_spawnActors.size() == 0);  // actors spawned
    m_controlledActors.clear();
    m_templateClientHandler.freeUsedTemplates();

    resetSkipSequence();
}

void SequencePlayerComponent::saveCameraMode()
{
    if (CAMERA->getMode() != Camera::Camera_Slave)
    {
        m_originalCamMode = CAMERA->getMode();
    }
}

void SequencePlayerComponent::restoreCameraMode()
{
    if (m_originalCamMode == Camera::Camera_Slave)
        m_originalCamMode = Camera::Camera_InGame;
    {
        CAMERA->setMode(m_originalCamMode);
        //CAMERA->setMainCamComponent(m_previousMainCamera, 1, 1);
    }
}


void SequencePlayerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    init( getTemplate() );

    m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    refreshLimitFrameSequence();
    refreshEnableEventsByChannel();

    // set Always Active Actor
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceControl,1151213732),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventEndWait,4251945611),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSkipSequence,4247999814),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceSetPlayerPos,3409872873),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventVideoCapture,1922224351),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorSaveZ,2573269496),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceSpeedChange,3037544556),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActivatePlayers,3249842910),this);

    // Register event to be able to go back to main menu through sequence event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGoToMainMenu,1087093414),this);

    m_editionMode = bfalse;

    ObjectPath objPath(SequencePlayerComponent::getMyselfKey());
    forceLinkActorByObjectPath(objPath, GetActor()->getRef());

    allActorsSpawn();
    addTextBannerSounds();

    ResourceGroup *rs =  m_actor->getResourceGroup();
    // we also Add the resources of prefected initialData;
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent               * evt          = m_eventsList[i];
        const SequenceEvent_Template* evtTemplate  = evt->getTemplate();

        i32 startFrame = evtTemplate->getStartFrame() + evtTemplate->getOffset();

        // We load all event that start before s_resourcePrefetch time
        if (startFrame <= s_resourcePrefetch)
        {
            SequenceEventWithActor* evtAct = evt->DynamicCast<SequenceEventWithActor>(ITF_GET_STRINGID_CRC(SequenceEventWithActor,2066607700));
            if (!evtAct)
                continue;

            evtAct->linkBindedActor();
            Actor *act = evtAct->getBindedActor();
            // Add other actor's resources
            // But do not ADD resources from other sequence to prevent cycling !!
            if (act && !act->GetComponent<SequencePlayerComponent>())
                rs->addResource(act->getResourceGroup());
        }
    }
}

bbool SequencePlayerComponent::checkIfCanInterrupt(EContextIcon& _skipContextIcon)
{
    //Check skip events
    if (m_skipSequenceEvent == U32_INVALID ||
        m_skipSequenceEvent >= m_eventsList.size())
        return bfalse;

    SequenceEvent *evt = getEventAtIndex(m_skipSequenceEvent);
    if (!evt || evt->getTemplate()->getType() != SequenceEvent_Template::event_PlaySkip)
        return bfalse;

    const PlaySkip_evtTemplate* skipTemplate = evt->getTemplate()->DynamicCast<PlaySkip_evtTemplate>(ITF_GET_STRINGID_CRC(PlaySkip_evtTemplate,173814531));
    if (skipTemplate)
    {
        _skipContextIcon = skipTemplate->getSkipContextIcon();
    }

    //Check it can interrupt due to already seen
    //NB: "Continue" skips are placed on loops and can *always* be interrupted (RO-11058)
    if (_skipContextIcon != ContextIcon_Continue)
    {
        GameInterface::CanInterruptCutSceneCallback_Fn canInterruptCallback = GAMEINTERFACE->getCanInterruptCutSceneCallback();
        bbool canInterrupt = btrue;
        if (canInterruptCallback )
        {
            canInterrupt = canInterruptCallback(m_actor);
        }
        return canInterrupt;
    }

    return btrue;
}

void SequencePlayerComponent::processSkipSequence()
{
	if (m_skipSequenceEvent == U32_INVALID ||
		m_skipSequenceEvent >= m_eventsList.size())
		return;

	SequenceEvent *evt = getEventAtIndex(m_skipSequenceEvent);
	if (!evt || evt->getTemplate()->getType() != SequenceEvent_Template::event_PlaySkip)
		return;

	PlaySkip_evtTemplate *evtTemplate = (PlaySkip_evtTemplate *)evt->getTemplate();
	i32 endFrame = evtTemplate->getStopFrame();
	setCurrentFrame(endFrame, bfalse);
    m_skipSequenceEvent = U32_INVALID;
}

void SequencePlayerComponent::initSkipSequence(EContextIcon _contextIcon)
{
    if (!m_inputListenerRegistered)
    {
        m_inputListenerRegistered = btrue;
        m_menuValidPressed = bfalse;
        m_menuBackPressed = bfalse;
        GAMEMANAGER->setInputModeForMenu(btrue);
        GAMEMANAGER->addMenuInputListener(this);
        CONTEXTICONSMANAGER->show(ContextIcon_Invalid, _contextIcon);
    }
}void SequencePlayerComponent::resetSkipSequence()
{
    if (m_inputListenerRegistered)
    {
        m_inputListenerRegistered = bfalse;
        m_menuValidPressed = bfalse;
        m_menuBackPressed = bfalse;
        m_skipHoldTimer = 0.f;
        m_isSkipButtonHeld = bfalse;
        GAMEMANAGER->setInputModeForMenu(bfalse);
        GAMEMANAGER->removeInputListener(this);
        CONTEXTICONSMANAGER->hide();
    }
}void SequencePlayerComponent::onEvent(Event * _evt)
{
    bbool canJump = bfalse;
    if (EventSequenceControl * onPlay = _evt->DynamicCast<EventSequenceControl>(ITF_GET_STRINGID_CRC(EventSequenceControl,1151213732)))
    {
        switch (onPlay->getState())
        {
        case State_Stopped :
            stop();
            break;
        case State_Paused:
            pause();
            break;
        case State_Playing:
            canJump = (getState() != State_Playing || onPlay->isForceLabel()) && !onPlay->getLabel().isEmpty();
            start();
            if (canJump)
                gotoLabel(onPlay->getLabel());
            break;
        default:
            ITF_ASSERT_MSG(0, "unhandled case");
            break;
        }
    }
    else if (EventSequenceActorSaveZ * onPlay = _evt->DynamicCast<EventSequenceActorSaveZ>(ITF_GET_STRINGID_CRC(EventSequenceActorSaveZ,2573269496)))
    {
        for (u32 i=0; i<m_controlledActors.size(); i++)
        {
            Actor *pControlActor = static_cast<Actor *>(GETOBJECT(m_controlledActors[i].m_actor));
            if (pControlActor)
            {
                m_controlledActors[i].m_z = pControlActor->getPos().m_z;
            }
        }

        u32 count = CINEMATICMANAGER->getPlayersInfoCount();
        for (u32 i=0; i<count; i++)
        {
            SequencePlayers &seqPlayer  = CINEMATICMANAGER->getPlayersInfoAtIndex(i);
            Player * player             = GAMEMANAGER->getPlayer(i);

            if (!seqPlayer.m_isActive)
                continue;

            Actor * playerAct           = player->getActor();
            seqPlayer.m_pos.m_z         = playerAct->getPos().m_z;
        }
    }
    else if (EventSequenceSpeedChange * setSpeed = _evt->DynamicCast<EventSequenceSpeedChange>(ITF_GET_STRINGID_CRC(EventSequenceSpeedChange,3037544556)))
    {
        m_sequenceSpeedFactor = setSpeed->getSpeed();
    }
    else if (EventSequenceActivatePlayers * activatePlayers = _evt->DynamicCast<EventSequenceActivatePlayers>(ITF_GET_STRINGID_CRC(EventSequenceActivatePlayers,3249842910)))
    {
        ITF_ASSERT(m_deactivatedPlayers == activatePlayers->getActivate());
        if (m_deactivatedPlayers == activatePlayers->getActivate())
        {
            processPlayersStatus(btrue);
            m_deactivatedPlayers = !m_deactivatedPlayers;
            processPlayersStatus(bfalse);
        }
    }
    else if (EventSequenceSetPlayerPos * setPlayerPos = _evt->DynamicCast<EventSequenceSetPlayerPos>(ITF_GET_STRINGID_CRC(EventSequenceSetPlayerPos,3409872873)))
    {
        const String8 & actorName   = setPlayerPos->getActor();
        ObjectPath      actorObjectPath;
        actorObjectPath.fromString(actorName.cStr());
        Actor           * act       = getActorFromObjectPath(actorObjectPath, actorName.cStr());
        Player          * player    = GAMEMANAGER->getPlayer(setPlayerPos->getPlayerId());
        if (act && player)
        {
            Actor * playerAct = (Actor *)player->getActorForMode(setPlayerPos->getPlayerMode()).getObject();
            if (playerAct)
            {
                Vec3d   dummyPos = act->getPos();
                if (setPlayerPos->getUseBaseAdjust())
                {
                    AnimLightComponent *anm = playerAct->GetComponent<AnimLightComponent>();
                    if (anm)
                    {
                        Vec2d offset = anm->getOffset();
                        dummyPos -= offset.to3d();
                    }
                }
                playerAct->setPos(dummyPos);
            }
        }
    }
    else if (EventEndWait * onEndWait = _evt->DynamicCast<EventEndWait>(ITF_GET_STRINGID_CRC(EventEndWait,4251945611)))
    {
        ITF_ASSERT(getState() == State_Paused);
        start();
        unpause();
        allowCameraControl(btrue);
    }
    else if (EventTrigger * evt = _evt->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (evt->getActivated())
            start();
        else
            pause();
    } else if (EventSkipSequence * skipSequence = _evt->DynamicCast<EventSkipSequence>(ITF_GET_STRINGID_CRC(EventSkipSequence,4247999814)))
	{
		processSkipSequence();
    }
    else if (EventGoToMainMenu * goToMainMenu = _evt->DynamicCast<EventGoToMainMenu>(ITF_GET_STRINGID_CRC(EventGoToMainMenu,1087093414)))
    {
        GAMEMANAGER->goToTheMainMenu();
    }
#if defined(ITF_SUPPORT_VIDEOCAPTURE)
    else if (EventVideoCapture * capture = _evt->DynamicCast<EventVideoCapture>(ITF_GET_STRINGID_CRC(EventVideoCapture,1922224351)))
    {
        if (capture->isStart() && getState() == State_Playing)
        {
            VideoCaptureCommand::startStepByStep();
        } else
        {
            VideoCaptureCommand::stop();
        }
    }
#endif
}

void SequencePlayerComponent::getTrajectoryList(ObjectRefList& _list) const
{
    for (u32 i = 0; i < m_eventsList.size(); i++)
    {
        if (m_eventsList[i]->getTemplate()->getType() == SequenceEvent_Template::event_PlayTrajectory)
        {
            PlayTrajectory_evt* evt = (PlayTrajectory_evt*)m_eventsList[i];
            _list.push_back(evt->getTrajectory()->getRef());
        }
    }
}

SequenceEvent* SequencePlayerComponent::getEventWithUID(u32 _uid, u32 *index)
{
    for(u32 _index = 0; _index < m_eventsList.size(); _index++)
    {
        SequenceEvent* evt = m_eventsList[_index];
        u32 uid = evt->getTemplate()->getUID();
        if(uid == _uid)
        {
            if (index)
                *index = _index;
            return evt;
        }
    }
    return NULL;
}



void SequencePlayerComponent::prefetchResources(i32 _pos)
{
    _pos += m_curFrame_rel;
    i32 prefetchPos = _pos/* + s_resourcePrefetch*/;
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent               * evt          = m_eventsList[i];
        const SequenceEvent_Template* evtTemplate  = evt->getTemplate();

        i32 startFrame = evtTemplate->getStartFrame() + evtTemplate->getOffset();
        i32 stopFrame  = evtTemplate->getStopFrame()  + evtTemplate->getOffset();

        // We load all event that start before s_resourcePrefetch time
        if (evtTemplate->getStartFrame() > prefetchPos)
            break;

        if (prefetchPos >= startFrame       &&
            _pos         < stopFrame        &&
            evt->isEnableByChannel()        &&
            !evtTemplate->isDisableForTesting())
        {
            evt->init( m_actor->getWorldInitialPos());
            evt->prefetchResources(_pos);
        }
    }
}

void SequencePlayerComponent::fillActiveEventsTable()
{
    Vec3d position;
    if (m_bUseAnchor)
        position = m_anchorPos;
    else
    {
        //position = m_actor->getPos();
        position = m_actor->getWorldInitialPos();
    }


    m_actorPlayAnimControlled.clear();
    m_activeEvents.clear();


    i32 prefetchPos = m_curFrame_rel + s_resourcePrefetch;
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent               * evt          = m_eventsList[i];
        const SequenceEvent_Template* evtTemplate  = evt->getTemplate();

        i32 startFrame = evtTemplate->getStartFrame() + evtTemplate->getOffset();
        i32 stopFrame  = evtTemplate->getStopFrame()  + evtTemplate->getOffset();

        bbool evtIsActive = bfalse;
        if(evt->isEnableByChannel() && !evtTemplate->isDisableForTesting() && m_curFrame_rel < stopFrame)
        {
            // resource pre-fetch
            if (prefetchPos >= startFrame)
            {
                evt->prefetchResources(prefetchPos);
            }

            if (m_curFrame_rel  >= startFrame)
            {
                // not yet in list
                if (m_activeEvents.find(evt) == -1)
                {
                    // add and initialize event
                    if (!evt->getActive() || evt->isBindedActorOk())
                    {
                        m_activeEvents.push_back(evt);
                        evt->init(position);

                        // activating event
                        if(!evt->getActive()){
                            evt->onBecomeActive();
                        }

                    }
                }
                evtIsActive = btrue;
            }
        }

        if (!evtIsActive)
        {
            // disabling event
            if(evt->getActive())
            {
                evt->onBecomeInactive();
            }
        }
    }

    for(u32 i = 0; i < m_activeEvents.size(); i++)
    {
              SequenceEvent           * evt           = m_activeEvents[i];
        const SequenceEvent_Template  * evtTemplate   = evt->getTemplate();

        if (evtTemplate->getType() == SequenceEvent_Template::event_PlayAnim && evt->isEnableByChannel() && !evtTemplate->isDisableForTesting())
        {
            PlayAnim_evt* evtAnim = (PlayAnim_evt*)evt;
            evtAnim->resetSubAnimsToPlay();
        }
    }
}


void SequencePlayerComponent::Draw()
{
}

void SequencePlayerComponent::updateSkipInput( f32 _deltaTime )
{
    if (m_inputListenerRegistered && !GAMEMANAGER->isInPause())
    {
        u32 mainPlayerIndex = GAMEMANAGER->getMainIndexPlayer();
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, mainPlayerIndex, buttons, JOY_MAX_BUT);

        bbool isSkipPressed = INPUT_ADAPTER->IsActionPressed(mainPlayerIndex, ZInputManager::Action_Back);

        if (isSkipPressed)
        {
            if (!m_isSkipButtonHeld)
            {
                m_isSkipButtonHeld = btrue;
                m_skipHoldTimer = 0.f;
            }
            else
            {
                m_skipHoldTimer += _deltaTime;
                f32 progress = Clamp(getSkipHoldProgress() * 100.f, 0.f, 100.f);
                CONTEXTICONSMANAGER->setSkipProgressPercent(progress);
                if (m_skipHoldTimer >= m_skipRequiredDuration)
                {
                    processSkipSequence();
                    resetSkipSequence();
                }
            }
        }
        else
        {
            if (m_skipHoldTimer > 0.f)
            {
                m_skipHoldTimer -= _deltaTime * 3.f;
                m_skipHoldTimer = std::max(m_skipHoldTimer, 0.f);
                f32 progress = Clamp(getSkipHoldProgress() * 100.f, 0.f, 100.f);
                CONTEXTICONSMANAGER->setSkipProgressPercent(progress);
            }

            if (m_isSkipButtonHeld)
            {
                m_isSkipButtonHeld = bfalse;
            }
        }
    }
}

void SequencePlayerComponent::Update( f32 _deltaTime )
{
#ifndef ITF_FINAL
    if (INPUT_ADAPTER->isKeyPressed(KEY_LALT) && INPUT_ADAPTER->isKeyPressed(m_shortCutKey))
    {
        if (getState() != State_Playing)
            setState(State_Playing);
    }
#endif
    // parent class update
    Super::Update(_deltaTime);

#if defined(ITF_SUPPORT_VIDEOCAPTURE)
    if (getState() != State_Playing)
        VideoCaptureCommand::stop();
#endif

    updateSkipInput(_deltaTime);

    if(m_pause)
        return;

    switch (getState())
    {
    case State_Stopped :
        m_curFrame_rel = 0;
        m_curTime_rel  = 0.f;
        return;

    case State_Playing:
        {
            m_curTime_rel  += _deltaTime * m_sequenceSpeedFactor;
            int nextFrame   = (i32)timeToFrame(m_curTime_rel,ANIMATION_FRAMERATE);
            if (nextFrame == m_curFrame_rel)
            {
                updatePlaying();
            } else
            {
                int step = nextFrame >= m_curFrame_rel ? 1 : -1;
                int actualFrame;
                do
                {
                    actualFrame = m_curFrame_rel += step;
                    updatePlaying();
                } while (m_curFrame_rel != nextFrame && getState() == State_Playing && actualFrame == m_curFrame_rel);
            }
        }
        break;

    case State_Paused:
        return; // if you change this return for a break, you will bother edition: actors position will be forced in pause mode so they won't be moved by the editor
        break;
    default:
        ITF_ASSERT(0);  // unhandled
        break;
    }
}

void SequencePlayerComponent::updatePlaying()
{
    if (m_curFrame_rel > m_unskippableFramesFirstView)
    {
        notifyAlreadySeen(); //This is called every frame, however performance should not be a problem
    }

    if (m_curFrame_rel >= m_lastEventFrame)
    {
        stop();
        return;
    }

    if (m_curFrame_rel < m_firstEventFrame)
        return;

    // fill active event list
    fillActiveEventsTable();

    // update active events
    if (m_activeEvents.size())
    {
        for (u32 i = 0; i < m_activeEvents.size(); i++) {
            m_activeEvents[i]->forceCurrentFrame(m_curFrame_rel);
        }
    }

    activate();


    for(u32 i = 0; i < m_actorPlayAnimControlled.size(); i++)
    {
        PlayAnim_evt::commitSubAnimsToPlay(m_actorPlayAnimControlled[i], m_bankState);
    }

    // sequence can be stopped in forceCurrentFrame; make sure we don't re-show
    // the skip context icon when this happens (RO-11158)
    if (m_playState == State_Playing)
    {
        EContextIcon skipContextIcon = ContextIcon_Invalid;
        if (checkIfCanInterrupt(skipContextIcon))
        {
            initSkipSequence(skipContextIcon);
        }
    }

}

void    SequencePlayerComponent::addActorPlayAnimControlled(ActorRef _actor)
{
    if(m_actorPlayAnimControlled.find(_actor) == -1)
    {
        m_actorPlayAnimControlled.push_back(_actor);
    }
}

void    SequencePlayerComponent::activate()
{
    // avoid disable by the cells system
    AABB aabb(Vec2d(CAMERA->getX() - 20.f, CAMERA->getY() - 20.f),
              Vec2d(CAMERA->getX() + 20.f, CAMERA->getY() + 20.f));
    m_actor->changeRelativeAABBFromAbsolute(aabb);
}


void    SequencePlayerComponent::restart()
{
    updateAnchorPosition();
    m_sequenceSpeedFactor = 1.f;
}

void    SequencePlayerComponent::updateAnchorPosition()
{
    // this can happen if we call updateAnchorPosition from the editor
    if (!m_actor)
        return;

    Vec3d position;
    if (m_bUseAnchor)
        position = m_anchorPos;
    else
    {
        position = m_actor->getWorldInitialPos();
    }

    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent* evt = m_eventsList[i];
        evt->init(position);
    }
}

void    SequencePlayerComponent::start()
{
    ITF_ASSERT(m_actor);
    Scene *pScene = m_actor->getScene();
    if (pScene && pScene->getAlwaysActiveActors().find(m_actor) == -1)
        pScene->setAlwaysActiveActor(m_actor);


    if (getTemplate()->saveInitialView())
    {
        if (!m_sequenceView)
        {
            m_sequenceView = View::createView("sequence", bfalse, btrue);
            m_sequenceView->setPrefetchDataOnly(btrue);
        }
        m_sequenceView->setActive(btrue);
        m_sequenceView->setTargetAABB(View::getMainView()->getScreenProjAABB());
    }

    activate();

    saveCameraMode();
    processPlayersStatus( bfalse );

    switch (getState())
    {
    case State_Stopped :
        {
            m_curFrame_rel = 0;
            m_curTime_rel = 0.f;
            AABB aabb(Vec2d(m_actor->getPos().m_x - 5.f, m_actor->getPos().m_y - 5.f),
                      Vec2d(m_actor->getPos().m_x + 5.f, m_actor->getPos().m_y + 5.f));
            m_actor->changeRelativeAABBFromAbsolute(aabb);

            restart();
            refreshEnableEventsByChannel();
            processPlayerBankChange();
        }
        break;

    case State_Paused:
        break;

    case State_Playing:
        restart();
        break;

    default:
        ITF_ASSERT_MSG(0, "unhandled case");
        break;
    }

    setState(State_Playing);
}

void    SequencePlayerComponent::pause()
{
    setState(State_Paused);
}
///////////////////////////////////////////////////////////////////////////////////////////
void    SequencePlayerComponent::notifyAlreadySeen()
{
    GameInterface::CutSceneAlreadySeenCallback_Fn alreadySeenCallback = GAMEINTERFACE->getCutSceneAlreadySeenCallback();
    if (alreadySeenCallback)
    {
        alreadySeenCallback(m_actor);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void    SequencePlayerComponent::stop()
{
    if (m_sequenceView)
    {
        View::deleteView(m_sequenceView);
        m_sequenceView = NULL;
    }

    if (getState() == State_Stopped)
        return;

    if (CAMERA->getMode() == Camera::Camera_Slave)
        restoreCameraMode();

    if (m_playState==State_Playing || m_playState==State_Paused)
    {
        //notify end
        notifyAlreadySeen();
    }
    setState(State_Stopped);

    // remove this assert because it call only save
    if(m_actor)
    {
        AABB aabb(Vec2d(m_actor->getPos().m_x - 5.f, m_actor->getPos().m_y - 5.f), Vec2d(m_actor->getPos().m_x + 5.f, m_actor->getPos().m_y + 5.f));
        m_actor->changeRelativeAABBFromAbsolute(aabb);
    }

    for (u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent* evt = m_eventsList[i];
        if(evt->isEnableByChannel() && !evt->getTemplate()->isDisableForTesting())
        {
            evt->onStopSequence();

            if (evt->getActive())
            {
                evt->onBecomeInactive();
                ITF_ASSERT(!evt->getActive());
            }
        }
    }

    if(m_actor)
    {
        Scene *pScene = m_actor->getScene();
        if (pScene && pScene->getAlwaysActiveActors().find(m_actor) != -1)
            pScene->setAlwaysActiveActor(m_actor, bfalse);
    }

    processPlayersStatus( btrue );
    m_sequenceSpeedFactor = 1.f;

    resetSkipSequence();
}

bbool SequencePlayerComponent::skipSequenceRequested(EContextIcon _skipContextIcon)
{
    EContextIconType iconType = CONTEXTICONSMANAGER->getType(_skipContextIcon);
    switch (iconType)
    {
    case ContextIconType_Select: return m_menuValidPressed; break;
    case ContextIconType_Back: return m_menuBackPressed; break;
    default:
        break;
    }

    ITF_ASSERT_MSG(bfalse, "Unhandled icon type: %d", iconType);
    return bfalse;
}

void   SequencePlayerComponent::setState(State _state)
{
    m_playState = _state;
}

void    SequencePlayerComponent::setCurrentFrame( i32 _curFrame, bbool _pause, bbool _stopSound)
{
    if (getState() == State_Stopped)
    {
        saveCameraMode();
        processPlayersStatus( bfalse );
    }

    if (_pause)
        setState(State_Paused);

    m_curFrame_rel = _curFrame;
    m_curTime_rel = frameToTime((f32)m_curFrame_rel, ANIMATION_FRAMERATE);

    m_activeEvents.clear();
    fillActiveEventsTable();

    u32 evtCount = m_activeEvents.size();
    for (u32 i = 0; i < evtCount; i++)
    {
        SequenceEvent* e = m_activeEvents[i];

        // When setting a special frame, we cut the sound
        if (_stopSound && (e->getTemplate()->getType() == SequenceEvent_Template::event_PlaySound))
            ((PlaySound_evt *)e)->forceStop();
        else
            e->forceCurrentFrame(_curFrame);
    }

	// Always do the complete on set frame
	completeSetFrame(_curFrame);

    for(u32 i = 0; i < m_actorPlayAnimControlled.size(); i++)
    {
        PlayAnim_evt::commitSubAnimsToPlay(m_actorPlayAnimControlled[i], m_bankState);
    }
}

void SequencePlayerComponent::resetCamera()
{
    u32 evtCount = m_activeEvents.size();
    for (u32 i = 0; i < evtCount; i++)
    {
        SequenceEvent* e = m_activeEvents[i];
        const SequenceEventWithActor_Template* evt = e->getTemplate()->DynamicCast<SequenceEventWithActor_Template>(ITF_GET_STRINGID_CRC(SequenceEventWithActor_Template,292335554));
        if (!evt || strcmp(evt->getObjectPath().getStrId(), SequencePlayerComponent::getCameraKey()))
            continue;
        e->onBecomeInactive();
    }
}


void SequencePlayerComponent::completeSetFrame(int _curFrame)
{
    u32 evtCount = m_activeEvents.size();
    const ITF_VECTOR<ActorEventDefinitionZones> & evtZone = getTemplate()->getActorEventZoneList();
    if (evtZone.size())
    {
        ITF_VECTOR<StringID> actorTrajList;
        ITF_VECTOR<StringID> actorAnimList;
        actorTrajList.resize(evtZone.size());
        actorAnimList.resize(evtZone.size());

        ITF_VECTOR<StringID>::iterator actorTrajIter = actorTrajList.begin();
        ITF_VECTOR<StringID>::iterator actorAnimIter = actorAnimList.begin();
        ITF_VECTOR<ActorEventDefinitionZones>::const_iterator evtZoneIter = evtZone.begin();
        for (; evtZoneIter != evtZone.end(); evtZoneIter++, actorTrajIter++, actorAnimIter++)
        {
            *actorTrajIter = (*evtZoneIter).getActorFriendly();
            *actorAnimIter = *actorTrajIter;
        }

        evtCount = m_activeEvents.size();
        for (u32 i = 0; i < evtCount; i++)
        {
            SequenceEventWithActor* e = m_activeEvents[i]->DynamicCast<SequenceEventWithActor>(ITF_GET_STRINGID_CRC(SequenceEventWithActor,2066607700));
            if (!e)
                continue;

            const SequenceEventWithActor_Template* evt = e->getTemplate();
            switch (evt->getType())
            {
            case SequenceEventWithActor_Template::event_PlayTrajectory :
                {
                    ITF_VECTOR<StringID>::iterator iter = std::find(actorTrajList.begin(), actorTrajList.end(), evt->getObjectID());
                    if (iter != actorTrajList.end())
                        actorTrajList.erase(iter);
                }
                break;
            case SequenceEventWithActor_Template::event_PlayAnim :
                {
                    ITF_VECTOR<StringID>::iterator iter = std::find(actorAnimList.begin(), actorAnimList.end(), evt->getObjectID());
                    if (iter != actorAnimList.end())
                        actorAnimList.erase(iter);
                }
                break;
            default:
                break;
            }
        }

        {
            // do not process players
            u32 nbPlayers = GAMEMANAGER->getMaxPlayerCount();
            String8 playerName;
            StringID name;
            ITF_VECTOR<StringID>::iterator iter;
            for (u32 i=0; i<nbPlayers; i++)
            {
                for (u32 mod = 0; mod < 2; mod++)
                {
                    Player  * p     = GAMEMANAGER->getPlayer(i);
                    Actor   * act   = (Actor *)p->getActorForMode(mod).getObject();
                    if (act)
                    {
                        name = act->getUserFriendly();
                        iter = std::find(actorTrajList.begin(), actorTrajList.end(), name);
                        if (iter != actorTrajList.end())
                            actorTrajList.erase(iter);
                        iter = std::find(actorAnimList.begin(), actorAnimList.end(), name);
                        if (iter != actorAnimList.end())
                            actorAnimList.erase(iter);
                    }
                    playerName.setTextFormat("player%i", mod*100 + i);
                    name = playerName;
                    iter = std::find(actorTrajList.begin(), actorTrajList.end(), name);
                    if (iter != actorTrajList.end())
                        actorTrajList.erase(iter);
                    iter = std::find(actorAnimList.begin(), actorAnimList.end(), name);
                    if (iter != actorAnimList.end())
                        actorAnimList.erase(iter);
                }
            }
        }

        i32 eventIndex, eventFrame;
        evtZoneIter = evtZone.begin();
        for (actorTrajIter = actorTrajList.begin(); actorTrajIter != actorTrajList.end(); actorTrajIter++)
        {
            for (; evtZoneIter != evtZone.end() && (*evtZoneIter).getActorFriendly() != *actorTrajIter; evtZoneIter++);
            if (evtZoneIter == evtZone.end())
                break;
            if ((*evtZoneIter).getTrajectoryFrame(_curFrame, m_eventsList, &eventFrame, &eventIndex))
            {
                m_eventsList[eventIndex]->forceCurrentFrame(eventFrame);
            }
        }

        // here we process need to reset sub-anims for this actors
        evtZoneIter = evtZone.begin();
        for (actorAnimIter = actorAnimList.begin(); actorAnimIter != actorAnimList.end(); actorAnimIter++)
        {
            for (; evtZoneIter != evtZone.end() && (*evtZoneIter).getActorFriendly() != *actorAnimIter; evtZoneIter++);
            if (evtZoneIter == evtZone.end())
                break;
            if ((*evtZoneIter).getAnimationFrame(_curFrame, m_eventsList, &eventFrame, &eventIndex))
            {
                ITF_ASSERT(m_eventsList[eventIndex]->getTemplate()->getType() == SequenceEvent_Template::event_PlayAnim );
                ((PlayAnim_evt *)m_eventsList[eventIndex])->resetSubAnimsToPlay();
            }
        }

        evtZoneIter = evtZone.begin();
        for (actorAnimIter = actorAnimList.begin(); actorAnimIter != actorAnimList.end(); actorAnimIter++)
        {
            for (; evtZoneIter != evtZone.end() && (*evtZoneIter).getActorFriendly() != *actorAnimIter; evtZoneIter++);
            if (evtZoneIter == evtZone.end())
                break;
            if ((*evtZoneIter).getAnimationFrame(_curFrame, m_eventsList, &eventFrame, &eventIndex))
                m_eventsList[eventIndex]->forceCurrentFrame(eventFrame);
        }

    }
}

void SequencePlayerComponent::controlActor(ObjectRef _or, bbool _disableComponentsActor)
{
    if (!m_actor || _or == m_actor->getRef())
        return;

    ITF_VECTOR<SequenceActor>::iterator it = m_controlledActors.begin();
    ITF_VECTOR<SequenceActor>::iterator end = m_controlledActors.end();

    for (; it != end; ++it)
    {
        SequenceActor& data = *it;
        if (data.m_actor == _or)
        {
            data.m_refCount++;
            return;
        }
    }
    m_controlledActors.push_back(SequenceActor(_or));
    Actor *pControlActor = static_cast<Actor *>(GETOBJECT(_or));

    if (!pControlActor)
        return;

    SequenceActor& data             = m_controlledActors[m_controlledActors.size()-1];
    Scene* pScene = pControlActor->getScene();

    data.m_z        = pControlActor->getPos().m_z;
    data.m_scale    = pControlActor->getScale();
    if (pScene && !pScene->isAlwaysActiveActor(pControlActor))
    {
        pScene->setAlwaysActiveActor(pControlActor);
        data.m_removeFromAlwaysActive = btrue;
    }

    if (_disableComponentsActor)
    {
        EventSequenceActorActivate sequenceEventActivate(btrue);
        pControlActor->onEvent(&sequenceEventActivate);
    }

    GetActor()->getBinding()->bindChild(_or);
}

bbool SequencePlayerComponent::releaseActor(ObjectRef _or, bbool _disableComponentsActor)
{
    if (!m_actor || _or == m_actor->getRef())
        return bfalse;

    ITF_VECTOR<SequenceActor>::iterator it = m_controlledActors.begin();
    ITF_VECTOR<SequenceActor>::iterator end = m_controlledActors.end();

    for (; it != end; ++it)
    {
        SequenceActor &data = (*it);
        if (data.m_actor == _or)
        {
            ITF_ASSERT(data.m_refCount > 0);
            data.m_refCount--;

            // release actor
            if (!data.m_refCount)
            {
                Actor* pReleasedActor = (Actor*)GETOBJECT(_or);
                if (!pReleasedActor)
                    return bfalse;

                GetActor()->getBinding()->unbindChild(_or);

                // when sequence is deleting no need to restore z (prevent crash)
                if(!m_deleting && pReleasedActor->isZForced())
                {
                    Vec3d pos     = pReleasedActor->getPos();
                    if (f32_Abs(pos.m_z - data.m_z) > MTH_EPSILON)
                    {
                        pos.m_z   = data.m_z;
                        pReleasedActor->setPos(pos);
                    }
                    if (!pReleasedActor->getScale().IsEqual(data.m_scale, MTH_EPSILON))
                    {
                        pReleasedActor->setScale(data.m_scale);
                    }
                }

                if (data.m_removeFromAlwaysActive)
                {
                    Scene* pScene = pReleasedActor->getScene();
                    if (pScene)
                    {
                        bbool isAlwaysActive = pScene->isAlwaysActiveActor(pReleasedActor);
                        ITF_ASSERT_MSG(isAlwaysActive, "We are removing an actor that is not always active");
                        if (isAlwaysActive)
                            pScene->setAlwaysActiveActor(pReleasedActor, bfalse);
                    }
                }


                if(_disableComponentsActor)
                {
                    EventSequenceActorActivate sequenceEventActivate(bfalse);
                    pReleasedActor->onEvent(&sequenceEventActivate);
                }

                m_controlledActors.erase(it);
                return btrue;
            }
            return bfalse;
        }
    }
    return bfalse;
}

void SequencePlayerComponent::setEventFrame(i32 _start, i32 _stop)
{
    ITF_ASSERT(_start >= MIN_FRAME);
    ITF_ASSERT(_stop  >= MIN_FRAME);
    ITF_ASSERT(_start <= _stop);

    if (_start < m_firstEventFrame || m_firstEventFrame < MIN_FRAME)
        m_firstEventFrame = _start;

    if (_stop > m_lastEventFrame)
        m_lastEventFrame = _stop;
}


void SequencePlayerComponent::refreshEnableEventsByChannel()
{
    CINEMATICMANAGER->recomputeChannels();
    const ITF_VECTOR<StringID> & inactiveChannel        = !m_externInactiveChannels ? CINEMATICMANAGER->getInactiveChannels() : *m_externInactiveChannels;
    ITF_VECTOR<StringID>::const_iterator beginChannel   = inactiveChannel.begin();
    ITF_VECTOR<StringID>::const_iterator endChannel     = inactiveChannel.end();
    ITF_VECTOR<StringID>::const_iterator currentChannel;

    for (u32 idxEvent = 0; idxEvent < m_eventsList.size(); idxEvent++)
    {
        SequenceEvent                 * evt           = m_eventsList[idxEvent];
        const SequenceEvent_Template  * evtTemplate   = evt->getTemplate();
        StringID                        channelId     = evtTemplate->getChannel();

        bbool isEnable = btrue;
        if(channelId.isValid())
        {
            for(currentChannel = beginChannel; currentChannel != endChannel; currentChannel++)
            {
                if (*currentChannel == channelId)
                {
                    isEnable = bfalse;
                    break;
                }
            }
        }

        // disable the event if your channel is in the list
        evt->setEnableByChannel(isEnable);
    }
}

Actor* SequencePlayerComponent::getForcedActor( const StringID & _friendlyID )
{
    FriendlyActorMapping::iterator linkActRef =  m_forceLinkFriendlyActor.find(_friendlyID);
    if (linkActRef != m_forceLinkFriendlyActor.end())
    {
        Actor* actor = ((ActorRef)linkActRef->second).getActor();
        if(actor)
            return actor;
    }
    return NULL;
}

bbool SequencePlayerComponent::removeForcedActor( const ObjectPath& _friendlyName )
{
    ITF_STDSTRING str;
    _friendlyName.toString(str);

    return removeForcedActor( StringID(str) );
}

bbool SequencePlayerComponent::removeForcedActor( const StringID& _friendlyID )
{
    FriendlyActorMapping::iterator linkActRef =  m_forceLinkFriendlyActor.find(_friendlyID);
    if (linkActRef != m_forceLinkFriendlyActor.end())
    {
        m_forceLinkFriendlyActor.erase(linkActRef);
        return btrue;
    }
    return bfalse;
}


Actor* SequencePlayerComponent::getActorFromObjectPath(const ObjectPath& _friendlyName, const StringID& _friendlyID )
{
    // get actor from dictionary
    Actor* actor = NULL;

    actor = getForcedActor( _friendlyID );
    if (actor)
        return actor;

    // check actor in scene
    Pickable *pick = SceneObjectPathUtils::getObjectFromRelativePath(GetActor(), _friendlyName);
    if (pick)
    {
        if (pick->getObjectType() == BaseObject::eActor)
            return (Actor *)pick;
        else
            return NULL;
    }
    return NULL;
}


void  SequencePlayerComponent::forceLinkActorByObjectID(const StringID & _friendlyID, const ObjectRef& _ref )
{
    if(!_friendlyID.isValid() ||  !_ref.isValid())
        return;

    m_forceLinkFriendlyActor[_friendlyID] = _ref;
}

void SequencePlayerComponent::forceLinkActorByObjectPath(const ObjectPath& _friendlyName, const ObjectRef& _ref )
{
    if (!_friendlyName.isValid())
        return;

    ITF_STDSTRING str;
    _friendlyName.toString(str);

    forceLinkActorByObjectID(StringID(str), _ref );
}


Actor* SequencePlayerComponent::getPlayerFromID( StringID _playerID )
{
    for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
    {
        Actor* player = GAMEMANAGER->getActivePlayer(i);
        if (player && StringID(player->getUserFriendly()) == _playerID )
        {
            return player;
        }
    }
    return NULL;
}


void SequencePlayerComponent::refreshActorControlled(bbool _forceLink)
{
    u32 eventSize = m_eventsList.size();
    for(u32 i = 0; i < eventSize; i++)
    {
        SequenceEvent* evt = m_eventsList[i];

        if (evt && CINEMATICMANAGER->isActiveChannel(evt->getTemplate()->getChannelID()))
        {

            SequenceEventWithActor* evtWithActor = evt->DynamicCast<SequenceEventWithActor>(ITF_GET_STRINGID_CRC(SequenceEventWithActor,2066607700));
            if(evtWithActor)
                evtWithActor->linkBindedActor(_forceLink);
        }
    }
}

void SequencePlayerComponent::allActorsSpawn()
{
#ifdef ITF_SUPPORT_EDITOR
    ITF_VECTOR<ObjectPath> spawnedActors;
#endif //ITF_SUPPORT_EDITOR

    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent* evt = m_eventsList[i];
        if (evt->getTemplate()->getType() == SequenceEvent_Template::event_PlaySpawn)
        {
            ((PlaySpawn_evt*)evt)->spawnActor();
#ifdef ITF_SUPPORT_EDITOR
            const ObjectPath & objPath = ((PlaySpawn_evt*)evt)->getTemplate()->getObjectPath();
            ITF_VECTOR<ObjectPath>::iterator iter = std::find(spawnedActors.begin(), spawnedActors.end(), objPath);
            if (iter == spawnedActors.end())
            {
                spawnedActors.push_back(objPath);
            } else
            {
                ITF_STDSTRING   friendlyStr;
                objPath.toString(friendlyStr);
                ITF_WARNING(m_actor, bfalse, "Sequence has multiple spawn actor named %s", friendlyStr.c_str());
            }
#endif
        }
    }
    refreshActorControlled(btrue);
}

void SequencePlayerComponent::allPlayersSpawn(bbool _forceReload)
{
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent* evt = m_eventsList[i];
        if (evt->getTemplate()->getType() == SequenceEvent_Template::event_PlaySpawn)
        {
            u32 playerIndex;
            if (actorIsSequencePlayer(((PlaySpawn_evtTemplate *)evt->getTemplate())->getObjectID(), playerIndex))
            {
                PlaySpawn_evt* evtSpawn = ((PlaySpawn_evt*)evt);
                if (_forceReload)
                    deleteSpawnActor(evtSpawn->getBindedActor());
                evtSpawn->spawnActor();
            }
        }
    }
    refreshActorControlled(btrue);
}


void SequencePlayerComponent::allActorsResourceLoad()
{
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent* evt = m_eventsList[i];
        if (evt->getTemplate()->getType() == SequenceEvent_Template::event_PlaySpawn)
        {
            ((PlaySpawn_evt*)evt)->loadActorResources();
        }
    }
}


void SequencePlayerComponent::allActorsUpdatePhysicalReady()
{
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent * evt = m_eventsList[i];
        SequenceEventWithActor* evtWithActor = evt->DynamicCast<SequenceEventWithActor>(ITF_GET_STRINGID_CRC(SequenceEventWithActor,2066607700));
        if(evtWithActor)
        {
            Actor *act = evtWithActor->getBindedActor();
            if (act)
                act->updatePhysicalReady();
        }
    }
}

void SequencePlayerComponent::init( const SequencePlayerComponent_Template * _template )
{
    m_template = _template;

    f32 unskippableDurationFirstTime = getTemplate()->getUnskippableDurationFirstTime();
    if (unskippableDurationFirstTime<0)
    {
        unskippableDurationFirstTime = GAMEINTERFACE->getCutSceneDefaultUnskippableDurationFirstTime();
    }

    m_unskippableFramesFirstView = (i32)(unskippableDurationFirstTime/LOGICDT);


    u32 eventCount = _template->getEventsCount();
    m_eventsList.reserve(eventCount);

    for (u32 iEvt=0; iEvt<eventCount; iEvt++)
    {
        SequenceEvent_Template * evtTemplate = _template->getEventAtIndex(iEvt);
        SequenceEvent_Template::EvtType type = evtTemplate->getType();
        for (u32 i=0; i<MAPPED_SEQ_EVT_COUNT; i++)
        {
            if (SqEvtMapping[i].m_id == type)
            {
                SequenceEvent * evt =  SqEvtMapping[i].m_instanceFactory(this, evtTemplate);
                m_eventsList.push_back(evt);
                break;
            }
        }
    }

    m_deactivatedPlayers = _template->deactivatedPlayers();
}

bbool SequencePlayerComponent::gotoLabel( const String8 & _label )
{
    u32 eventCount = m_eventsList.size();

    for (u32 iEvt=0; iEvt<eventCount; iEvt++)
    {
        const SequenceEvent_Template * evtTemplate = m_eventsList[iEvt]->getTemplate();
        if (evtTemplate->getType() == SequenceEvent_Template::event_Label)
        {
            Label_evtTemplate *lblEvt = (Label_evtTemplate *)evtTemplate;
            if (lblEvt->getLabel() == _label)
            {
                setCurrentFrame(lblEvt->getStartFrame(), bfalse);
                return btrue;
            }
        }
    }
    return bfalse;
}

void SequencePlayerComponent::processPlayersStatus( bbool _activate )
{
    if (!m_deactivatedPlayers)
        return;

    if (!_activate)
    {
        if (!m_deactivatedPlayersDone)
        {
            CINEMATICMANAGER->disableSequencePlayers();
            m_deactivatedPlayersDone = btrue;
        }
    } else
    {
        if (m_deactivatedPlayersDone)
        {
            CINEMATICMANAGER->enableSequencePlayers(getTemplate()->reactivateAlive(), getTemplate()->reactivatePlayers());
            m_deactivatedPlayersDone = bfalse;
        }
    }
}

bbool SequencePlayerComponent::actorHasAnim(const StringID & _friendlyID)
{
    u32 evtCount =  getTemplate()->getEventsCount();
    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_PlayAnim)
            continue;

        PlayAnim_evtTemplate* evtAnim = (PlayAnim_evtTemplate*)evt;
        //if actor's animation == actor controlled
        if(_friendlyID == evtAnim->getObjectID() )
        {
            return btrue;
        }
    }
    return bfalse;
}

bbool SequencePlayerComponent::actorHasSound(const StringID & _friendlyID)
{
    u32 evtCount =  getTemplate()->getEventsCount();
    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_PlaySound)
            continue;

        PlaySound_evtTemplate* evtSnd = (PlaySound_evtTemplate*)evt;
        if(_friendlyID == evtSnd->getObjectID())
        {
            return btrue;
        }
    }
    return bfalse;
}

bbool SequencePlayerComponent::actorHasText(const StringID & _friendlyID)
{
    u32 evtCount =  getTemplate()->getEventsCount();
    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_PlayText)
            continue;

        PlayText_evtTemplate* evtText = (PlayText_evtTemplate*)evt;
        if(_friendlyID == evtText->getObjectID())
        {
            return btrue;
        }
    }
    return bfalse;
}


bbool SequencePlayerComponent::actorHasRedirectPatchBank(const StringID & _friendlyID, SequenceEvent_Template *& _evt)
{
    u32 evtCount =  getTemplate()->getEventsCount();
    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_RedirectPatchBank)
            continue;

        RedirectPatchBank_evtTemplate* evtText = (RedirectPatchBank_evtTemplate*)evt;
        if(_friendlyID == evtText->getObjectID())
        {
            _evt = evtText;
            return btrue;
        }
    }
    return bfalse;
}

u32 SequencePlayerComponent::fillAnimInfoMarkers(const StringID & _friendlyID, ITF_VECTOR <AnimInfoMarkers> & vecAnim, ITF_VECTOR <StringID> & vecAnimExisting)
{
    u32 evtCount =  getTemplate()->getEventsCount();
    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_PlayAnim)
            continue;

        PlayAnim_evtTemplate* evtAnim = (PlayAnim_evtTemplate*)evt;
        //if actor's animation == actor controlled
        if(_friendlyID == evtAnim->getObjectID())
        {
            bbool found = bfalse;
            String8     animStr = evtAnim->getAnim();
            String      animName = evtAnim->getAnimName();
            StringID    animID = animName;
            for (u32 j = 0; j < vecAnimExisting.size(); j++)
            {
                if (vecAnimExisting[j] == animID)
                {
                    found = btrue;
                    break;
                }
            }
            if (!found)
            {
                AnimInfoMarkers newAnim;
                newAnim.m_path = animStr;
                newAnim.m_name = animName;
                newAnim.m_beginMarker = evtAnim->getBeginMarker();
                newAnim.m_endMarker = evtAnim->getEndMarker();
                vecAnim.push_back(newAnim);
                vecAnimExisting.push_back(animID);
            }
        }
    }
    return vecAnim.size();
}


bbool SequencePlayerComponent::addAnims(Actor * _pActor, const StringID & _friendlyID)
{
    AnimLightComponent * comp = _pActor->GetComponent<AnimLightComponent>();
    if (!comp)
        return 0;

    SubAnimSet* subanimSet = comp->getSubAnimSet();
    ITF_VECTOR <StringID>           vecAnimExisting;
    // find and add subAnim used by this actor
    u32 subCount = subanimSet->getSubAnimCount();
    vecAnimExisting.reserve(subCount);
    for (u32 i=0; i<subCount; i++)
    {
        SubAnim * subAnim = subanimSet->getSubAnim(i);
        if (subAnim)
            vecAnimExisting.push_back(subAnim->getFriendlyName());
    }

    ITF_VECTOR <AnimInfoMarkers> vecAnim;
    u32 animCount = fillAnimInfoMarkers(_friendlyID, vecAnim, vecAnimExisting);

    for (u32 i = 0; i < animCount; ++i)
    {
        comp->addAnimByPath(vecAnim[i].m_path, vecAnim[i].m_name, vecAnim[i].m_beginMarker, vecAnim[i].m_endMarker);
    }

    return btrue;
}

bbool SequencePlayerComponent::addSounds(Actor * _pActor, const StringID & _friendlyID)
{
    SoundComponent * comp = _pActor->GetComponent<SoundComponent>();
    if (!comp)
        return bfalse;

    u32                                     evtCount    = getTemplate()->getEventsCount();
    SafeArray<SoundDescriptor_Template *>   sndList;
    String soundPath;

    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_PlaySound)
            continue;

        PlaySound_evtTemplate* evtSound = (PlaySound_evtTemplate*)evt;
        //if actor's animation == actor controlled

        if(_friendlyID == evtSound->getObjectID())
        {
            evtSound->getSoundPath().getString(soundPath);

            SoundDescriptor_Template *tmplt = newAlloc(mId_Editor, SoundDescriptor_Template());
#if defined(ITF_WII) || defined(ITF_PS3) || defined(ITF_WINDOWS)
            // Only search _50 sound when we are on 50Hz AND the sound is streamed
            if ((GFX_ADAPTER->getMonitorRefreshRate() == 50 || APPLOOPFPS == 50) && evtSound->getIsStream())
            {
                String newPath  = FilePath::getDirectory(soundPath);
                       newPath += FilePath::getFilenameWithoutExtension(soundPath);
                       newPath += "_50fps.";
                       newPath += FilePath::getExtension(soundPath);

                tmplt->addFile(newPath);
            } else
#endif
                tmplt->addFile(evtSound->getSoundPath());
            tmplt->setName(FilePath::getFilenameWithoutExtension(soundPath));
            tmplt->setWwiseEventGUID(evtSound->getWwiseEventGuid());
            tmplt->setVolume(Volume(evtSound->getVolume(), btrue));
            tmplt->setCategory(StringID(evtSound->getCategory().cStr()));
            tmplt->setIsStream(evtSound->getIsStream());
            tmplt->setFullParams(evtSound->getParams());
            tmplt->onTemplateLoaded();
            tmplt->addToResourceGroup(_pActor->getResourceGroup());
            sndList.push_back(tmplt);
        }
    }
    comp->setSoundList(sndList);
    return btrue;
}

bbool SequencePlayerComponent::addTextSounds(Actor * _pActor, const StringID & _friendlyID)
{
    UITextBox      * text = _pActor->GetComponent<UITextBox>();
    if (!text)
        return bfalse;

    u32                     evtCount = getTemplate()->getEventsCount();
    ResourceGroup           *rs      = m_actor->getResourceGroup();

    for (u32 i = 0; i < evtCount; ++i)
    {
        SequenceEvent_Template* evt = getTemplate()->getEventAtIndex(i);
        ITF_ASSERT(evt);
        if(!evt || evt->getType() != SequenceEvent_Template::event_PlayText)
            continue;

        PlayText_evtTemplate* evtText = (PlayText_evtTemplate*)evt;
        //if actor's animation == actor controlled
        if(_friendlyID == evtText->getObjectID())
        {
            const SimpleTextComponent & textComp = evtText->getParams();
            Path audioPath = LOCALISATIONMANAGER->getAudioPath(textComp.getLineId());

            if (!audioPath.isEmpty())
            {
#ifdef  ITF_SUPPORT_WWISE
                ResourceID mpR = rs->addResource(Resource::ResourceType_MusicPart, audioPath);
                const StringID wwiseEventGuid = AUDIO_ADAPTER->getLocalizationWWiseGUIdFromLocId(textComp.getLineId().value);
                MusicPart* musicPart = (MusicPart*)mpR.getResource();
                if (musicPart)
                    musicPart->setWwiseEventGuid(wwiseEventGuid);
#else
                rs->addResource(Resource::ResourceType_MusicPart, audioPath);
#endif
            }
        }
    }
    return btrue;
}

bbool SequencePlayerComponent::addTextBannerSounds()
{
    ResourceGroup *rs =  m_actor->getResourceGroup();
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent               * evt          = m_eventsList[i];
        const SequenceEvent_Template* evtTemplate  = evt->getTemplate();

        if (evtTemplate->getType() == SequenceEvent_Template::event_PlayTextBanner)
        {
			for (u8 i = 0; i < LINEID_MAX_ITEM; i++)
			{
				if (((PlayTextBanner_evtTemplate *)evtTemplate)->getLineId(i) != 0)
				{
					const Path audioPath = LOCALISATIONMANAGER->getAudioPath(((PlayTextBanner_evtTemplate *)evtTemplate)->getLineId(i));
					if (!audioPath.isEmpty())
                    {

#ifdef ITF_WII
                        ResourceID mpR = rs->addResource(Resource::ResourceType_MusicPart, audioPath);
                        MusicPart * musicPart = (MusicPart *)mpR.getResource();
                        if (musicPart)
                            musicPart->forcePrefecth(((PlayTextBanner_evtTemplate *)evtTemplate)->getForcePrefetch());
#else
#ifdef  ITF_SUPPORT_WWISE
                        ResourceID mpR = rs->addResource(Resource::ResourceType_MusicPart, audioPath);
                        const StringID wwiseEventGuid = AUDIO_ADAPTER->getLocalizationWWiseGUIdFromLocId(((PlayTextBanner_evtTemplate*)evtTemplate)->getLineId().value);
                        MusicPart* musicPart = (MusicPart*)mpR.getResource();
                        if (musicPart)
                            musicPart->setWwiseEventGuid(wwiseEventGuid);
#else
                        rs->addResource(Resource::ResourceType_MusicPart, audioPath);
#endif
#endif

                    }
				}
			}
        }
    }
    return true;

}


const ITF_VECTOR<BankChange_Template> * SequencePlayerComponent::getPlayerBankChange(const StringID & _friendlyId)
{
    u32             playerIndex;
    if (!actorIsSequencePlayer(_friendlyId, playerIndex ))
        return NULL;

    // if we enter, player AND actor are OK
    Player* player = GAMEMANAGER->getPlayer(playerIndex);
    Actor * actor  = (Actor *)player->getActor();

    if ( !actor )
        return NULL;

    if (CINEMATICMANAGER->getPlayersInfoCount() == 0)
    {
        if (!player->getActive())
            return NULL;
    } else
    {
        SequencePlayers &seqPlayer  = CINEMATICMANAGER->getPlayersInfoAtIndex(playerIndex);
        if (!seqPlayer.m_isActive)
            return NULL;
    }

    const Actor_Template                *actTemplate    = NULL;
    const AnimLightComponent_Template   *animLightComp  = NULL;

    actTemplate = actor->getTemplate();
    if ( actTemplate != NULL )
        animLightComp = actTemplate->GetComponent<AnimLightComponent_Template>();

    ITF_ASSERT(animLightComp);
    if (!animLightComp)
        return NULL;

    return &animLightComp->getSubAnimTemplate()->getBankChangeList();
}

bbool SequencePlayerComponent::applyBankChangeOnActor(const StringID & _friendlyID, Actor *_actor, const ITF_VECTOR<BankChange_Template> *_bankChange)
{
    m_templateClientHandler.addUsedTemplate(_actor->getLua().getStringID());
    const Actor_Template *spawnActTemplate  = TEMPLATEDATABASE->getTemplate<Actor_Template>(&m_templateClientHandler, _actor->getLua());
    Actor_Template * overrideTemplate       = newAlloc(mId_Gameplay, Actor_Template());
    BinaryClone((Actor_Template *)spawnActTemplate, overrideTemplate);

    const AnimLightComponent_Template   *spawnAnimLightComp  = overrideTemplate->GetComponent<AnimLightComponent_Template>();
    ITF_VECTOR<BankChange_Template> &  spawnBankChange       = (ITF_VECTOR<BankChange_Template> &)spawnAnimLightComp->getSubAnimTemplate()->getBankChangeList();
    spawnBankChange.resize(_bankChange->size());
    ITF_VECTOR<BankChange_Template>::const_iterator bankIter = _bankChange->begin();
    ITF_VECTOR<BankChange_Template>::iterator spawnBankIter  = spawnBankChange.begin();

    ITF_VECTOR<String8> &  skipFilesChange                   = (ITF_VECTOR<String8> &)spawnAnimLightComp->getSubAnimTemplate()->getSkipFiles();
    PathString8_t bankName;

    for (; bankIter != _bankChange->end(); ++bankIter, ++spawnBankIter)
    {
        BinaryClone((BankChange_Template *)&(*bankIter), &(*spawnBankIter));

        BankChange_Template *spawnBank = &(*spawnBankIter);
        // skip unused files
        if (spawnBank->getState() != U32_INVALID && spawnBank->getState() != m_bankState)
        {
            spawnBank->getBankPath().getString(bankName);
            skipFilesChange.push_back(bankName);
        }
    }

    ITF_VECTOR <AnimInfoMarkers> vecAnim;
    ITF_VECTOR <StringID>        vecAnimExisting;

    u32 animCount = fillAnimInfoMarkers(_friendlyID, vecAnim, vecAnimExisting);
    ITF_VECTOR<SubAnim_Template> & subAnimToAdd = (ITF_VECTOR<SubAnim_Template> &)spawnAnimLightComp->getSubAnimTemplate()->getSubAnimList();
    subAnimToAdd.resize(animCount);
    ITF_VECTOR <AnimInfoMarkers>::const_iterator vecIter = vecAnim.begin();
    ITF_VECTOR<SubAnim_Template>::iterator subIter = subAnimToAdd.begin();
    for (; subIter != subAnimToAdd.end(); ++vecIter, ++subIter )
    {
        SubAnim_Template & subAnimT     = *subIter;
        const AnimInfoMarkers & marker  = *vecIter;

        subAnimT.setAnimPath( marker.m_path );
        subAnimT.setFriendlyName( marker.m_name );
        subAnimT.setIsLooped( btrue );

        subAnimT.setStart( marker.m_beginMarker * ANIMATION_FRAMERATE );
        subAnimT.setStop( marker.m_endMarker * ANIMATION_FRAMERATE);
    }


    overrideTemplate->onTemplateLoaded();
    ((AnimLightComponent_Template *)spawnAnimLightComp)->postSerialize();

    _actor->clearOverrideTemplate();
    _actor->setOverrideTemplate(overrideTemplate, btrue);

    return btrue;
}

void SequencePlayerComponent::processPlayerBankChange()
{
    for(u32 i = 0; i < m_eventsList.size(); i++)
    {
        SequenceEvent               * evt          = m_eventsList[i];

        SequenceEventWithActor* evtAct = evt->DynamicCast<SequenceEventWithActor>(ITF_GET_STRINGID_CRC(SequenceEventWithActor,2066607700));
        if (!evtAct)
            continue;

        const SequenceEventWithActor_Template* evtTemplate  = evtAct->getTemplate();
        const ITF_VECTOR<BankChange_Template> * bankChange = getPlayerBankChange(evtTemplate->getObjectID());
        if (!bankChange)
            continue;

        Actor * actor = evtAct->getBindedActor();
        if (!actor)
            continue;

        ActorHotReload actorHot;
        actor->fillHotReloadDataAndHotReload(actorHot, bfalse, btrue);
        actor->clear();
        applyBankChangeOnActor(evtTemplate->getObjectID(), actor, bankChange);
        actor->hotReload(actorHot);
    }
}




// Spawn will load logical data of actor but not start resources !!
bbool SequencePlayerComponent::spawnActor(const ObjectPath & _friendlyName, const StringID & _friendlyID, const String8 & _fileName, const Vec3d &_offset, bbool _flip)
{
    if(GetActor() == NULL)
        return bfalse;

    Scene* pScene = GetActor()->getScene();

    if(pScene == NULL)
        return bfalse;

    // actor spawned ?
    if (getForcedActor( _friendlyID ))
        return btrue;

    const Vec3d&    posSequence = GetActor()->getPos();
    Vec3d           position    = posSequence + _offset;

    String8 fileName_trim = _fileName;
            fileName_trim = fileName_trim.trim();
    Actor * newActor = NULL;

    // spawn actor form animation
    //  or
    // spawn actor is a copy/save in sequence
    if(fileName_trim.getLen() == 0)
    {
        String  templateToUse;

        if (actorHasText(_friendlyID))
        {
            templateToUse = GETPATH_ALIAS("tpl_ActorInSequenceText_Spawn");
            newActor = ACTORSMANAGER->spawnActor(position,pScene,templateToUse,btrue);
            addTextSounds(newActor, _friendlyID);
        } else
        {
            u32     mask = (actorHasAnim(_friendlyID) ? 0x1 : 0) | (actorHasSound(_friendlyID) ? 0x2 : 0);
            switch (mask)
            {
            case 0x1:
                templateToUse = GETPATH_ALIAS("tpl_ActorInSequenceAnim_Spawn");
                break;
            case 0x2:
                templateToUse = GETPATH_ALIAS("tpl_ActorInSequenceSound_Spawn");
                break;
            case 0x3:
                templateToUse = GETPATH_ALIAS("tpl_ActorInSequence_Spawn");
                break;
            default:
                return bfalse;
            }

            if (mask & 0x1)
            {
                const ITF_VECTOR<BankChange_Template> * bankChange = NULL;
                SequenceEvent_Template * evt = NULL;
                if (actorHasRedirectPatchBank(_friendlyID, evt))
                {
                    RedirectPatchBank_evtTemplate *redirectEvtTemplate = (RedirectPatchBank_evtTemplate*)evt;
                    bankChange = &redirectEvtTemplate->getBankChangeList();
                }

                if (bankChange && bankChange->size())
                {
                    // bank change is present on actor -> use it on spawner
                    newActor = ACTORSMANAGER->spawnActor(position,pScene,templateToUse,bfalse);
                    applyBankChangeOnActor(_friendlyID, newActor, bankChange);
                    newActor->onLoaded();
                }
            }

            if (!newActor)
            {
                newActor = ACTORSMANAGER->spawnActor(position,pScene,templateToUse,btrue);

                if (mask & 0x1)
                    addAnims(newActor, _friendlyID);
            }

            if (mask & 0x2)
                addSounds(newActor, _friendlyID);
        }
    }
    // spawn actor with act file (instance)
    else if(fileName_trim.getLen() != 0)
    {
        const String absPathFileName(_fileName.cStr());
        newActor = ACTORSMANAGER->spawnActor(position,pScene,absPathFileName,btrue);
    }

    if(newActor)
    {
        getSpawnActors()->push_back(newActor->getRef());

        forceLinkActorByObjectID(_friendlyID, newActor->getRef() );

        ITF_STDSTRING friendlyStr;
        _friendlyName.toString( friendlyStr );

#ifdef ITF_SUPPORT_EDITOR
        Scene* pScene = newActor->getScene();
        if(pScene && (pScene->getPickableFromUserFriendly(friendlyStr.c_str()) == NULL))
            newActor->setUserFriendly(friendlyStr.c_str());
        else
#endif // ITF_SUPPORT_EDITOR
            newActor->generateNewUserFriendly(friendlyStr.c_str());

        newActor->setSerializable(bfalse);
        newActor->setAllowedCell(bfalse);
        newActor->setFlipped(_flip);

        // by default actor is spawned disable
        newActor->disable();
    }

    return newActor != NULL;
}

void SequencePlayerComponent::deleteSpawnActor(Actor *pActor)
{
    if (!pActor)
        return;

    ObjectRefList  * actorList = getSpawnActors();

    i32 index = actorList->find(pActor->getRef());
    if (index >= 0)
    {
        ObjectPath objPath;
        SceneObjectPathUtils::getRelativePathFromObject(GetActor(), pActor, objPath);
        removeForcedActor(objPath);
        actorList->eraseNoOrder(index);

        Scene* scene = pActor->getScene();
        if (scene)
        {
            scene->unregisterPickable(pActor);
            //scene->processUnregisterPending();
        }
    } else
    {
        ITF_ASSERT_MSG(0, "bad link -> maybe 2 sequences use same names");
    }
}

void SequencePlayerComponent::onBecomeActive()
{
    refreshActorControlled(btrue);
}

void SequencePlayerComponent::onBecomeInactive()
{
    stop();
}

void SequencePlayerComponent::Receive(u32 _deviceID, f32 _axis, const StringID& _action)
{
    ITF_ASSERT(m_inputListenerRegistered);
    if (!m_inputListenerRegistered)
    {
        return;
    }

    if (GAMEMANAGER->getMainIndexPlayer() != _deviceID || GAMEMANAGER->isInPause())
    {
        return;
    }

    if (_action==ITF_GET_STRINGID_CRC(MENU_VALID,1084313942))
    {
        m_menuValidPressed = btrue;
    }
    else if (_action==ITF_GET_STRINGID_CRC(MENU_BACK,2477582220))
    {
        m_menuBackPressed = btrue;
    }
}


//-------------------------------------------------------------------------------------

int compareForSortSequence(const void *s1, const void *s2)
{
    SequenceEvent_Template *e1 = *(SequenceEvent_Template **)s1;
    SequenceEvent_Template *e2 = *(SequenceEvent_Template **)s2;

    i32 delta = e1->getStartFrame() - e2->getStartFrame();
    if (delta != 0)
    {
        if (delta < 0)
            return -1;
        else
            return 1;
    }
    return  e1->getTrackLine() - e2->getTrackLine();
}

void SequencePlayerComponent_Template::sortSequence()
{
    if(m_events.size() > 1)
        qsort(&m_events[0], m_events.size(), sizeof(SequenceEvent_Template*), compareForSortSequence);
}

SequenceEvent_Template* SequencePlayerComponent_Template::createEventFromBlob( Blob* _blob )
{

    String EventTypeName;
    _blob->extractString(EventTypeName);

    // createEvent : objectRef is not exist
    _blob->extractUint32();

    for(u32 iEvt = 0; iEvt < MAPPED_SEQ_EVT_COUNT; iEvt++)
    {
        if (EventTypeName == SqEvtMapping[iEvt].m_name)
        {
            SequenceEvent_Template* newEvt = SqEvtMapping[iEvt].m_templateFactory(_blob, this);
            ITF_ASSERT(newEvt);
            if(newEvt)
            {
                m_events.push_back(newEvt);
                return newEvt;
            }
        }
    }
    return NULL;
}

bbool SequencePlayerComponent_Template::removeEvent(u32 _uid)
{
    for(u32 _index = 0; _index < m_events.size(); _index++)
    {
        SequenceEvent_Template* evt = m_events[_index];
        u32 uid = evt->getUID();
        if(uid == _uid)
        {
            // remove event
            delete m_events[_index];
            m_events.eraseKeepOrder(_index);
            return btrue;
        }
    }

    return bfalse;
}

void SequencePlayerComponent_Template::serializeToBlob (Blob* _blob, const String & _path)
{
    if(_path.getLen() != 0)
    {
        String luaPath = FilePath::getFilename(_path);

        // send sequence name
        _blob->pushString(luaPath);

        // send actor name
        _blob->pushString(luaPath);

        // deactivate players
        _blob->pushBool(m_deactivatedPlayers);

        // force reactivate alive
        _blob->pushBool(m_reactivateAlive);

        // force reactivate - tout court
        _blob->pushBool(m_reactivatePlayers);

        // force reactivate - tout court
        _blob->pushBool(m_saveInitialView);

        //serialize all events
        u32 evtCount = m_events.size();
        _blob->pushUInt32(evtCount);
        for (u32 i = 0; i < evtCount; i++)
        {
            m_events[i]->serializeToBlob(_blob);
        }
        serializeTrackListToBlob(_blob);
    }
    else
    {
        // send sequence name
        _blob->pushString("error: do a refresh list");

        //serialize all events
        _blob->pushUInt32(0);

        // serialize all tracks
        _blob->pushUInt32(0);
    }
}

void SequencePlayerComponent_Template::serializeTrackListToBlob (Blob* _blob)
{
    // serialize all tracks
    u32 trackCount = m_trackList.size();
    _blob->pushUInt32(trackCount);
    for (u32 i = 0; i < trackCount; i++)
    {
        m_trackList[i].serializeToBlob(_blob);
    }
}


void SequencePlayerComponent_Template::checkIndex( u32 index )
{
    if (index >= m_trackList.size())
    {
        m_trackList.resize(index+1);
    }
}

bbool SequencePlayerComponent_Template::insertTrack( u32 index )
{
    if (index >= m_trackList.size()  )
    {
        // inserting a track after the track list is useless
        return bfalse;
    }

    // inserting a track is just
    u32 evtCount =  m_events.size();
    for (u32 i=0; i<evtCount; i++)
    {
        SequenceEvent_Template *evt = m_events[i];
        if (evt->getTrackLine() >= index)
        {
            evt->setTrackLine(evt->getTrackLine() + 1);
        }
    }

    // add empty line on track list
    for (u32 i=index+1; i<m_trackList.size(); i++)
    {
        if (m_trackList[i].getParentGroup() != U32_INVALID && m_trackList[i].getParentGroup() >= index)
        {
            m_trackList[i].setParentGroup(m_trackList[i].getParentGroup() + 1);
        }
    }
    SequenceTrackInfo_Template trkInfo;
    m_trackList.insert(m_trackList.begin() + index, trkInfo);
    return btrue;
}

bbool SequencePlayerComponent_Template::removeTrack( u32 index )
{
    if (index >= m_trackList.size()  )
    {
        // inserting a track after the track list is useless
        return bfalse;
    }

    u32 evtCount = m_events.size();
    for (u32 i=0; i<evtCount; )
    {
        SequenceEvent_Template *evt = m_events[i];
        u32 evtTrack = evt->getTrackLine();
        if (evtTrack == index)
        {
            removeEvent(evt->getUID());
            evtCount--;
        } else
        {
            if (evt->getTrackLine() > index)
            {
                evt->setTrackLine( evt->getTrackLine()-1);
            }
            i++;
        }
    }

    for (u32 i=index+1; i<m_trackList.size(); i++)
    {
        if (m_trackList[i].getParentGroup() != U32_INVALID && m_trackList[i].getParentGroup() > index)
        {
            m_trackList[i].setParentGroup(m_trackList[i].getParentGroup() - 1);
        }
    }
    m_trackList.erase(m_trackList.begin() + index);
    return btrue;
}


bbool SequencePlayerComponent_Template::isChild(u32 _index, u32 _parent, bbool _skipLoopIssues)
{
    if (_index == U32_INVALID || _parent == U32_INVALID)
        return bfalse;

    if (_index >= m_trackList.size() || _parent >= m_trackList.size())
        return bfalse;

    u32 parent      = m_trackList[_index].getParentGroup();
    u32 maxLevels   = 10;
    while (parent != U32_INVALID && --maxLevels)
    {
        if (parent == _parent)
            return btrue;

        if (parent == m_trackList[parent].getParentGroup() && _skipLoopIssues)
            return bfalse;

        parent = m_trackList[parent].getParentGroup();
    }
    ITF_ASSERT_MSG(maxLevels, "preventing infinite loop");
    return bfalse;
}


u32 SequencePlayerComponent_Template::moveTrack( u32 index, u32 newIndex, u32 mode, u32 newParent )
{
    checkIndex( index );
    checkIndex( newIndex );

    switch (mode)
    {
    case 2:
        newIndex++;
        if (m_trackList[newIndex-1].isGroup())
        {
            u32 groupIndex = newIndex-1;
            while (isChild(newIndex, groupIndex, btrue))
                newIndex++;
        }
        checkIndex( newIndex );
    case 1:
        insertTrack(newIndex);
        if (newIndex != newParent)
        {
            if (newParent != newIndex)
                m_trackList[newIndex].setParentGroup(newParent);
            else
                m_trackList[newIndex].setParentGroup(U32_INVALID);
        }
        if (index >= newIndex) // insertion will put index at next pos
            index++;
        break;
    default:
        break;
    }

    u32 evtCount = m_events.size();
    for (u32 k=0; k<evtCount; k++)
    {
        SequenceEvent_Template *evt = m_events[k];
        if (evt->getTrackLine() == index)
        {
            evt->setTrackLine(newIndex);
        } else if (evt->getTrackLine() == newIndex)
        {
            evt->setTrackLine(index);
        }
    }
    u32 indexParent     = m_trackList[index].getParentGroup();
    u32 newIndexParent  = m_trackList[newIndex].getParentGroup();

    SequenceTrackInfo_Template swap = m_trackList[index];
    m_trackList[index] = m_trackList[newIndex];
    m_trackList[newIndex] = swap;

    // conserve parent groups
    m_trackList[index].setParentGroup(indexParent);
    m_trackList[newIndex].setParentGroup(newIndexParent);

    if (mode != 0)
    {
        removeTrack(index);
        if (newIndex > index)
            newIndex--;
    }

    return newIndex;
}

u32 SequencePlayerComponent_Template::moveGroup( u32 index, u32 newIndex, u32 mode, u32 newParent, u32 _nbChildren )
{
    if (newParent == index)
        newParent = U32_INVALID;

    u32 beginChild  = index + 1;
    u32 endChild    = beginChild;
    SafeArray<int> directChildren;
    while (_nbChildren-- && endChild < m_trackList.size() && isChild(endChild, index, bfalse))
    {
        u32 myParent = m_trackList[endChild].getParentGroup();
        if (myParent == index)
        {
            directChildren.push_back(1);
        }
        else
        {
            directChildren.push_back(0);
            ITF_ASSERT(myParent >= beginChild);
            if (beginChild <= myParent && myParent < endChild )
            {
                directChildren[myParent - beginChild]++;
            }
        }
        endChild++;
    }

    // 3 possibilities
    // error one -> newIndex inside children
    if (beginChild <= newIndex  && newIndex < endChild)
        return U32_INVALID;

    u32 childrenParent = moveTrack(index, newIndex, mode, newParent);
    u32 insertPos = childrenParent;

    // newIndex before children (and before index)
    u32 childPos = 0;
    if (beginChild > newIndex)
    {
        for (u32 i=beginChild; i<endChild; i++, childPos++)
        {
            if (!directChildren[childPos])
                continue;
            if (m_trackList[i].isGroup())
            {
                insertPos = moveGroup(i, insertPos, 2, childrenParent, directChildren[childPos] - 1);
            } else
            {
                insertPos = moveTrack(i, insertPos, 2, childrenParent);
            }
        }
    } else
        // newIndex after children
    {
        for (u32 i=beginChild; i<endChild; i++, childPos++)
        {
            if (!directChildren[childPos])
                continue;
            if (m_trackList[beginChild-1].isGroup())
            {
                moveGroup(beginChild-1, insertPos, 2, childrenParent, directChildren[childPos] - 1);
                childrenParent -= directChildren[childPos];
            } else
            {
                moveTrack(beginChild-1, insertPos, 2, childrenParent--);
            }
        }
    }
    return insertPos;
}



void SequencePlayerComponent_Template::computeActorEventZoneList()
{
    m_actorEventZoneList.clear();

    u32 evtCount = m_events.size();
    ITF_VECTOR<StringID> actorList;

    for (u32 evtIdx = 0; evtIdx < evtCount; ++evtIdx)
    {
        SequenceEventWithActor_Template* evt = m_events[evtIdx]->DynamicCast<SequenceEventWithActor_Template>(ITF_GET_STRINGID_CRC(SequenceEventWithActor_Template,292335554));
        if (!evt || !strcmp(evt->getObjectPath().getStrId(), SequencePlayerComponent::getCameraKey()))
            continue;

        const StringID & friendly = evt->getObjectID();
        if (std::find(actorList.begin(), actorList.end(), friendly) == actorList.end())
        {
            actorList.push_back(friendly);
        }
    }

    ITF_VECTOR<StringID>::iterator actorIter = actorList.begin();
    m_actorEventZoneList.resize(actorList.size());
    for (u32 i=0; actorIter != actorList.end(); ++actorIter, ++i)
    {
        ActorEventDefinitionZones * pNewDef = &m_actorEventZoneList[i];
        pNewDef->fillZones(*actorIter, m_events);
    }
}

#ifdef ITF_SUPPORT_COOKING
void SequencePlayerComponent_Template::preSerializeSave()
{
    if (!LOGICDATABASE || !LOGICDATABASE->isBinarisationEnable())
        return;

    // we are saving a console version, need to remove non used events
    for (u32 i  = 0; i < m_events.size();)
    {
        SequenceEvent_Template* evt = m_events[i];
        if (evt->isDisableForTesting())
        {
            m_events.eraseNoOrder(i);
            SF_DEL(evt);
        }
        else
            i++;
    }
}
#endif //ITF_SUPPORT_COOKING


void SequencePlayerComponent_Template::updateHierarchy()
{
    sortSequence();
    for (u32 i  = 0; i < m_events.size(); i++)
    {
        SequenceEvent_Template* evt = m_events[i];
        evt->setOwnerSequence(this);
    }
	computeActorEventZoneList();
}

SequenceEvent_Template* SequencePlayerComponent_Template::getEventTemplateWithUID(u32 _uid, u32 *index)
{
    for(u32 _index = 0; _index < m_events.size(); _index++)
    {
        SequenceEvent_Template* evt = m_events[_index];
        u32 uid = evt->getUID();
        if(uid == _uid)
        {
            if (index)
                *index = _index;
            return evt;
        }
    }
    return NULL;
}

SequencePlayerComponent_Template::~SequencePlayerComponent_Template()
{
    u32 eventCount = m_events.size();
    for (u32 i=0; i<eventCount; i++)
    {
        SF_DEL(m_events[i]);
    }
    m_events.clear();
}


ITF::bbool SequencePlayerComponent::actorIsSequencePlayer( StringID _friendly, u32 & _playerIndex )
{
    _playerIndex  = U32_INVALID;
    for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
    {
        Player* player = GAMEMANAGER->getPlayer(i);
        if (!player)
            continue;

        if (CINEMATICMANAGER->getPlayerChannelID(i) == _friendly)
        {
            _playerIndex = i;
            return btrue;
        }
    }
    return bfalse;
}

StringID SequencePlayerComponent::s_sequenceTextMenu("SequenceText");
String8 SequencePlayerComponent::s_sequenceTextField("text");
const f32 SequencePlayerComponent::SKIP_HOLD_DURATION = 1.5f;

#ifdef ITF_SUPPORT_EDITOR
void SequencePlayerComponent::onEditorMove(bbool _modifyInitialPos)
{
    if (getState() != State_Stopped)
    {
        Vec3d myPos     = m_actor->getPos();
        Vec2d myScale   = m_actor->getScale();
        f32   myAngle   = m_actor->getAngle();

        setCurrentFrame(getCurrentFrame(), btrue);

        m_actor->setPos(myPos);
        m_actor->setScale(myScale);
        m_actor->setAngle(myAngle);
    }
}

#endif // ITF_SUPPORT_EDITOR

BEGIN_SERIALIZATION(SequenceTrackInfo_Template)

SERIALIZE_MEMBER("isEnable", m_isEnable);
SERIALIZE_MEMBER("selected", m_selected);
SERIALIZE_MEMBER("isGroup", m_isGroup);
SERIALIZE_MEMBER("parentGroup", m_parentGroup);
SERIALIZE_MEMBER("name", m_name);
SERIALIZE_MEMBER("fold", m_fold);

END_SERIALIZATION()


void SequenceTrackInfo_Template::serializeToBlob( Blob* _blob )
{
    _blob->pushBool(m_isEnable);
    _blob->pushBool(m_selected);
    _blob->pushBool(m_isGroup);
    _blob->pushUInt32(m_parentGroup);
    _blob->pushString8(m_name);
    _blob->pushInt32(m_startFrame);
    _blob->pushInt32(m_duration);
    _blob->pushBool(m_fold);
}

void SequenceTrackInfo_Template::fillFromBlob( Blob* _blob )
{
    m_isEnable      = _blob->extractBool();
    m_selected      = _blob->extractBool();
    m_isGroup       = _blob->extractBool();
    m_parentGroup   = _blob->extractUint32();
    _blob->extractString8(m_name);
    m_startFrame    = _blob->extractInt32();
    m_duration      = _blob->extractInt32();
    m_fold          = _blob->extractBool();
}


void ActorEventDefinitionZones::fillBeginEnd( i32 _begin, i32 _end, i32 _evtIdx, SafeArray<BeginEndFrame> & _list )
{
    u32 listSize = _list.size();
    u32 insertPos = 0;
    for (; insertPos<listSize && _list[insertPos].m_startFrame < _begin; insertPos++);

    _list.insert(BeginEndFrame(_begin, _end, _evtIdx), insertPos);

    // now merge
    bbool needRecompute;
    do
    {
        needRecompute = bfalse;
        listSize = _list.size();
        for (u32 i=0; i<listSize-1; i++)
        {
            BeginEndFrame & be1 = _list[i];
            BeginEndFrame & be2 = _list[i+1];
            if (be1.m_endFrame > be2.m_startFrame)
            {
                if (be1.m_endFrame < be2.m_endFrame)
                {
                    be1.m_endFrame      = be2.m_endFrame;
                    be1.m_eventIndex    = be2.m_eventIndex;
                }

                _list.eraseKeepOrder(i+1);
                needRecompute = btrue;
                break;
            }
        }
    } while (needRecompute);
}

void ActorEventDefinitionZones::fillZones( StringID & _actorFriendly, SafeArray<SequenceEvent_Template*> & _eventList )
{
    u32 evtCount = _eventList.size();
    m_actor = _actorFriendly;
    for (u32 evtIdx = 0; evtIdx < evtCount; ++evtIdx)
    {
        SequenceEventWithActor_Template* evt = _eventList[evtIdx]->DynamicCast<SequenceEventWithActor_Template>(ITF_GET_STRINGID_CRC(SequenceEventWithActor_Template,292335554));
        if (!evt)
            continue;

        if (evt->getObjectID() != _actorFriendly)
             continue;

        switch (evt->getType())
        {
        case SequenceEvent_Template::event_PlayAnim:
            fillBeginEnd(evt->getStartFrame() + evt->getOffset(),
                         evt->getStopFrame() + evt->getOffset(),
                         evtIdx, m_animationList);
            break;
        case SequenceEvent_Template::event_PlayTrajectory:
            fillBeginEnd(evt->getStartFrame() + evt->getOffset(),
                         evt->getStopFrame() + evt->getOffset(),
                         evtIdx, m_trajectoryList);
            break;
        default:
            break;
        }
    }
}

bbool ActorEventDefinitionZones::getFrame( i32 _currentFrame, const SafeArray<SequenceEvent*> & _eventList, i32 *_usedFrame, i32 *_eventIdx, const SafeArray<BeginEndFrame> & _list ) const
{
    u32 listSize = _list.size();

    if (listSize == 0)
        return bfalse;

    u32 insertPos = 0;
    for (; insertPos<listSize && _list[insertPos].m_startFrame < _currentFrame; insertPos++);

    // before begin, use current pos ...
    if (insertPos == 0)
        return bfalse;

    BeginEndFrame previousBE = _list[insertPos-1];

    SequenceEvent* evt = _eventList[previousBE.m_eventIndex];
    if (!evt->isEnableByChannel() || evt->getTemplate()->isDisableForTesting())
        return bfalse;

    if (_currentFrame < previousBE.m_endFrame)
    {
        return bfalse;
    }

    *_usedFrame = previousBE.m_endFrame;
    *_eventIdx = previousBE.m_eventIndex;
    return btrue;
}



} // namespace ITF

