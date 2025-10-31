
#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#define _ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H


namespace ITF
{

#define ITF_SEQUENCE_LUA_VERSION 1

class Blob;
class SequencePlayerComponent;
class Actor;
class BaseObject;
class PlaySpawn_evt;
class Player;
class SequencePlayerComponent_Template;
class BankChange_Template;

struct SequenceActor
{
    SequenceActor(ObjectRef _or) {m_actor = _or; m_refCount = 1; m_removeFromAlwaysActive = bfalse; m_z = 0.f; }
    ObjectRef m_actor;
    i32       m_refCount;
    bbool     m_removeFromAlwaysActive;
    f32       m_z;
    Vec2d     m_scale;
};

///////////////////////////////////////////////////////////////////////////
//// Sequence Base Event
///////////////////////////////////////////////////////////////////////////

class SequenceEvent : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_RTTI(SequenceEvent, IRTTIObject,672446776);

public:
    SequenceEvent(SequencePlayerComponent* _ownerSequence);
    SequenceEvent();
	virtual ~SequenceEvent();

    virtual     void        init             (   const Vec3d& _pos       )   {ITF_ASSERT(0);} // MUST be implemented in children classes
    virtual     void        forceCurrentFrame(      i32 _frame           )   {ITF_ASSERT(0);} // MUST be implemented in children classes
    virtual     void        onStopSequence   (                           )   {} // NOT USED FOR THE MOMENT

    virtual     void         onBecomeActive  (    );
    virtual     void         onBecomeInactive(    );

    ITF_INLINE  void         setActive            (   bbool _active   )  { m_active = _active;}
    ITF_INLINE  bbool        getActive            (                   )  { return m_active; }

    ITF_INLINE  bbool        isEnableByChannel    (                            ) const { return m_enableByChannel; }
    ITF_INLINE  void         setEnableByChannel   ( bbool _enableByChannel     ) { m_enableByChannel = _enableByChannel; }

    virtual     bbool        isBindedActorOk ()   { return btrue; }

    ITF_INLINE  SequencePlayerComponent*  getOwnerSequence(                    ) const  {return m_ownerSequence;}
    const class SequenceEvent_Template *  getTemplate     (                    ) const { return m_template; }

    virtual void             prefetchResources(i32 _pos)  {}

	static bbool isReload;

protected:
    SequencePlayerComponent *           m_ownerSequence;
    const SequenceEvent_Template *      m_template;

    // only use for runtime
    bbool   m_active;               // event running
    bbool   m_enableByChannel;      // enable/disable by channel

    void    initDefaultValues();
};


class SequenceEvent_Template : public IRTTIObject
{
    DECLARE_SERIALIZE_VIRTUAL();
    DECLARE_OBJECT_CHILD_RTTI(SequenceEvent_Template, IRTTIObject,4216994033);

public:

    enum EvtType
    {
        event_Invalid = 0,
        event_PlayAnim,
        event_PlayTrajectory,
        event_PlaySpawn,
		event_PlayLoop,
        event_StartCapture,
        event_ExecuteLUA,
        event_GameplayEvent,
        event_PlayWait,
        event_PlaySound,
        event_PlaySkip,
        event_Label,
        event_PlayText,
        event_RedirectPatchBank,
        event_PlayTextBanner,
        ENUM_FORCE_SIZE_32(0)
    };


    SequenceEvent_Template(SequencePlayerComponent_Template* _ownerSequence);
    SequenceEvent_Template();

    virtual ~SequenceEvent_Template() {};

    virtual     EvtType getType          (                           ) const { ITF_ASSERT(0); return event_Invalid;} // MUST be implemented in children classes
    ITF_INLINE  u32     getUID           (                           ) const { return m_UID; }

    // serialize for sending event to the editor (SequenceEditor)
    virtual     void        serializeToBlob      ( Blob* _blob )   {ITF_ASSERT(0);} // MUST be implemented in children classes
    void                    serializeCommonToBlob( Blob* _blob );

    // fill with blob (for updating event)
    virtual     void        fillFromBlob      (Blob* _blob)   {ITF_ASSERT(0);} // MUST be implemented in children classes
    void                    fillCommonFromBlob(Blob* _blob);

    ITF_INLINE  i32                                 getStartFrame   (                   ) const  {return m_startFrame;}
    ITF_INLINE  i32                                 getOffset       (                   ) const  {return m_offset;}
    ITF_INLINE  i32                                 getStopFrame    (                   ) const  {return m_startFrame + m_duration;}
    ITF_INLINE  i32                                 getDuration     (                   ) const  {return m_duration;}
    ITF_INLINE  u32                                 getTrackLine    (                   ) const  {return m_trackline;}
    ITF_INLINE  String8                             getChannel      (                   ) const  {return m_channel;}
    ITF_INLINE  StringID                            getChannelID    (                   ) const  {return m_channelId;}
    ITF_INLINE  SequencePlayerComponent_Template*   getOwnerSequence(                   ) const  {return m_ownerSequence;}
    ITF_INLINE  void                                setStartFrame   (   i32 _start      )  {m_startFrame = _start;}
    ITF_INLINE  void                                setOffset       (   i32 _offset     )  {m_offset = _offset;}
    ITF_INLINE  void                                setDuration     (   i32 _duration   )  {m_duration = _duration;}
    ITF_INLINE  void                                setTrackLine    (   u32 _trackline  )  {m_trackline = _trackline;}
    ITF_INLINE  void                                setChannel      ( String8 _channel  )  {m_channel = _channel; computeChannelId(); }
    ITF_INLINE  void                                setOwnerSequence(   SequencePlayerComponent_Template* _seq  )  {m_ownerSequence = _seq;}

    ITF_INLINE  bbool        isSelected           (                            ) const { return m_selected >= 0; }
    ITF_INLINE  int          getSelected          (                            ) const { return m_selected; }
    ITF_INLINE  void         setSelected          ( i32 _selected              ) { m_selected = _selected; }

    ITF_INLINE  bbool        isDisableForTesting  (                            ) const;
    ITF_INLINE  void         setDisableForTesting ( bbool _disableForTesting   ) { m_disableForTesting = _disableForTesting; }

    virtual     SequenceEvent_Template *           split( i32 _frame )   { return NULL; }
    virtual     bbool                              concat( SequenceEvent_Template * _evt)   { return bfalse; }

protected:
    static u32                          s_count;

    void                                computeChannelId() { m_channelId = StringID(m_channel.cStr()); }
    u32                                 m_UID;
    i32                                 m_startFrame;
    i32                                 m_offset;      // start time event: (m_startTime + m_offset)
    i32                                 m_duration;
    u32                                 m_trackline;
    String8                             m_channel;
    StringID                            m_channelId;

    bbool                               m_disableForTesting; // "local" template data
    i32                                 m_selected;

    SequencePlayerComponent_Template *  m_ownerSequence;


    void                                initDefaultValues();
};


///////////////////////////////////////////////////////////////////////////
//// Sequence With Actor Event
///////////////////////////////////////////////////////////////////////////


class SequenceEventWithActor : public SequenceEvent
{
    DECLARE_OBJECT_CHILD_RTTI(SequenceEventWithActor, SequenceEvent,2066607700);

public:
    SequenceEventWithActor(SequencePlayerComponent* _ownerSequence);
    SequenceEventWithActor();
    virtual ~SequenceEventWithActor() {}

    Actor*                  getBindedActor           ( );
    virtual     bbool       linkBindedActor          ( bbool _forceLink = bfalse);

    virtual     bbool       isBindedActorOk ()       { return getBindedActor() != NULL; }
    virtual     void        onBecomeActive  (    );
    virtual     void        prefetchResources(i32 _pos);

    const class SequenceEventWithActor_Template *  getTemplate() const { return (SequenceEventWithActor_Template *)m_template; }

protected:
    u32         m_actorPlayerIndex;

private:
    ActorRef m_controlledActor;
};

class SequenceEventWithActor_Template : public SequenceEvent_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(SequenceEventWithActor_Template, SequenceEvent_Template,292335554);

public:
    SequenceEventWithActor_Template(SequencePlayerComponent_Template* _ownerSequence);
    SequenceEventWithActor_Template();
    virtual ~SequenceEventWithActor_Template() {}

    ITF_INLINE void             setObjectPath            ( const ObjectPath &  _objectPath  ) { m_objectPath = _objectPath; computeObjectID(); }
    ITF_INLINE const ObjectPath&getObjectPath            (                                      ) const { return m_objectPath;}
    ITF_INLINE bbool            getDisableComponentsActor(                                      ) const { return m_disableComponentsActor;}
    ITF_INLINE const StringID & getObjectID              (                                      ) const { return m_objectID;}

    void                    computeObjectID();
    void                    serializeFriendlyToBlob  ( Blob* _blob );
    void                    fillFriendlyFromBlob     ( Blob* _blob );

protected:
    bbool       m_disableComponentsActor;
    ObjectPath  m_objectPath;
    StringID    m_objectID;
};

///////////////////////////////////////////////////////////////////////////
//// Track list info
///////////////////////////////////////////////////////////////////////////
class SequenceTrackInfo_Template
{
public:
    DECLARE_SERIALIZE();

    // serialize for sending event to the editor (SequenceEditor)
    void            serializeToBlob   ( Blob* _blob );
    void            fillFromBlob      ( Blob* _blob );

    SequenceTrackInfo_Template()
    : m_isEnable(btrue)
    , m_selected(bfalse)
    , m_isGroup(bfalse)
    , m_parentGroup(U32_INVALID)
    , m_startFrame(0)
    , m_duration(-1)
    , m_fold(btrue)
    {
    }

    ITF_INLINE bbool       isEnable        ()                      { return m_isEnable; }
    ITF_INLINE void        setEnable       (bbool _enable)         { m_isEnable = _enable; }

    ITF_INLINE bbool       isSelected      ()                      { return m_selected; }
    ITF_INLINE void        setSelected     (bbool _selected)       { m_selected = _selected; }

    ITF_INLINE bbool       isGroup        ()                       { return m_isGroup; }
    ITF_INLINE void        setGroup       (bbool _group)           { m_isGroup = _group; }

    ITF_INLINE bbool       isFold         ()                       { return m_fold; }
    ITF_INLINE void        setFold        (bbool _fold)            { m_fold = _fold; }

    ITF_INLINE i32         getStartFrame  ()                       { return m_startFrame; }
    ITF_INLINE void        setStartFrame  (i32 _startFrame)        { m_startFrame = _startFrame; }

    ITF_INLINE i32         getDuration    ()                       { return m_duration; }
    ITF_INLINE void        setDuration    (i32 _duration)          { m_duration = _duration; }


    ITF_INLINE u32         getParentGroup ()                       { return m_parentGroup; }
    ITF_INLINE void        setParentGroup (u32 _parentGroup)       { m_parentGroup = _parentGroup; }

    ITF_INLINE String8 &   getName        ()                       { return m_name; }
    ITF_INLINE void        setName        (const String8 & _name)  { m_name = _name; }

private :
    bbool   m_isEnable;
    bbool   m_selected;
    bbool   m_isGroup;
    bbool   m_fold;
    i32     m_startFrame;
    i32     m_duration;
    u32     m_parentGroup;
    String8 m_name;

};

struct BeginEndFrame
{
    BeginEndFrame()
    : m_startFrame(-1)
    , m_endFrame(-1)
    , m_eventIndex(0)
    {}

    BeginEndFrame(i32 _startFrame, i32 _endFrame, i32 _eventIndex)
    : m_startFrame(_startFrame)
    , m_endFrame(_endFrame)
    , m_eventIndex(_eventIndex)
    {}

    i32         m_startFrame;
    i32         m_endFrame;
    i32         m_eventIndex;
};


class ActorEventDefinitionZones
{
public:
    ActorEventDefinitionZones() {}

    void                        fillZones(StringID & _actor, SafeArray<SequenceEvent_Template*> & _eventList);
    bbool                       isTrajectoryEmpty() { return m_trajectoryList.size() > 0;  }
    bbool                       getTrajectoryFrame(i32 _currentFrame, const SafeArray<SequenceEvent*> & _eventList, i32 *_usedFrame, i32 *_eventIdx ) const { return getFrame(_currentFrame, _eventList, _usedFrame, _eventIdx, m_trajectoryList); }

    bbool                       isAnimationEmpty() { return m_animationList.size() > 0;  }
    bbool                       getAnimationFrame (i32 _currentFrame, const SafeArray<SequenceEvent*> & _eventList, i32 *_usedFrame, i32 *_eventIdx ) const { return getFrame(_currentFrame, _eventList, _usedFrame, _eventIdx, m_animationList); }

    StringID                    getActorFriendly() const { return m_actor; }

private:
    void                        fillBeginEnd(i32 _begin, i32 _end, i32 evtIdx, SafeArray<BeginEndFrame> & _list);
    bbool                       getFrame(i32 _currentFrame, const SafeArray<SequenceEvent*> & _eventList, i32 *_usedFrame, i32 *_eventIdx, const SafeArray<BeginEndFrame> & _list) const;

    StringID                    m_actor;
    SafeArray<BeginEndFrame>    m_trajectoryList;
    SafeArray<BeginEndFrame>    m_animationList;

};


///////////////////////////////////////////////////////////////////////////
//// Sequence Player Component
///////////////////////////////////////////////////////////////////////////

typedef struct AnimInfoMarkers {
    String      m_path;
    String      m_name;
    i32         m_beginMarker;
    i32         m_endMarker;
} AnimInfoMarkers;

class SequencePlayerComponent : public ActorComponent, public IInputListener
{
    DECLARE_OBJECT_CHILD_RTTI(SequencePlayerComponent, ActorComponent,899993551);

    //              friendly   ref
    typedef ITF_MAP<StringID,ActorRef>   FriendlyActorMapping;

    public:
        DECLARE_SERIALIZE()

        enum State
        {
            State_Stopped = 0,
            State_Playing,
            State_Paused,
            ENUM_FORCE_SIZE_32(1)
        };

        SequencePlayerComponent();
        ~SequencePlayerComponent();

        static const char *         getMyselfKey()  { return "myself"; }
        static const char *         getCameraKey()  { return "camera"; }
        static const char *         getSpawnerKey() { return "spawner"; }
        static StringID             s_sequenceTextMenu;
        static String8              s_sequenceTextField;
        static const f32            SKIP_HOLD_DURATION;

        virtual void                init( const SequencePlayerComponent_Template * _template );
        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return btrue; }    // in order to call spawned actors draw() method
        virtual bbool               needsDraw2D() const { return bfalse; }
        
        virtual void                Update( f32 _deltaTime );
        virtual void                Draw();
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onEvent( Event * _evt);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();

        // input listener
        virtual void                Receive(u32 _deviceID, f32 _axis, const StringID& _action);

        void                        start();
        void                        restart();
        void                        stop();
        void                        pause();

        void                        resetCamera( );
        void                        setCurrentFrame( i32 _curFrame, bbool _pause, bbool _stopSound = btrue);
        ITF_INLINE i32              getCurrentFrame() {return m_curFrame_rel;}
        ITF_INLINE i32              getLastEventFrame() { return m_lastEventFrame; }

        void                        updateAnchorPosition();
        ITF_INLINE void             setAnchorPos(const Vec3d& _pos) {m_anchorPos = _pos; m_bUseAnchor = btrue;}
        ITF_INLINE const Vec3d&     getAnchorPos() const {return m_anchorPos;}

        void                        controlActor(ObjectRef _or, bbool _disableComponentsActor);
        bbool                       releaseActor(ObjectRef _or, bbool _disableComponentsActor); //return true if the actor is totally release

        void                        setState        (   State _state                   );
        ITF_INLINE State            getState( )  {return m_playState;}
        void                        activate();

        void                        allowCameraControl (bbool _allow) {m_bCameraControlAllowed = _allow;}
        bbool                       isCameraControlAllowed() {return m_bCameraControlAllowed;}

        void                        getTrajectoryList(ObjectRefList& _list) const;
        ObjectRefList  *            getSpawnActors() {return &m_spawnActors;}

        void                        setEventFrame(i32 _start, i32 _stop);
        void                        setFrameStartSaved(i32 _frameStartSaved) { m_frameStartSaved = _frameStartSaved; }
        i32                         getFrameStartSaved() { return m_frameStartSaved; }

        SequenceEvent*              getEventWithUID(u32 _objectRef, u32 *index = NULL);
        Actor*                      getActorFromObjectPath( const ObjectPath& _friendlyName, const StringID& _friendlyID  );

        void                        refreshEnableEventsByChannel();
        void                        refreshLimitFrameSequence();

        Actor*                      getForcedActor              (const StringID & _friendlyID);
        bbool                       removeForcedActor           (const ObjectPath& _friendlyName );
        bbool                       removeForcedActor           (const StringID& _friendlyID);
        void                        forceLinkActorByObjectID    (const StringID & _friendlyID, const ObjectRef& _ref );
        void                        forceLinkActorByObjectPath  (const ObjectPath & _friendlyName, const ObjectRef& _ref );
        Actor*                      getPlayerFromID             ( StringID _playerID );

        void                        refreshActorControlled(bbool _forceLink);
        void                        addActorPlayAnimControlled(ActorRef _actor);

        bbool                       isEditionMode() { return m_editionMode; }
        void                        setEditionMode(bbool _editMode) { m_editionMode = _editMode; }

        void                        allActorsSpawn();
        void                        allActorsResourceLoad();
        void                        allActorsUpdatePhysicalReady();
        void                        allPlayersSpawn(bbool _forceReload);

        u32                         getEventsListCount() const {return m_eventsList.size();}
        i32                         getEventIndex(SequenceEvent* _evt) const {return m_eventsList.find(_evt);}
        SequenceEvent*              getEventAtIndex(u32 _index) const {return m_eventsList[_index];}
        SafeArray<SequenceEvent*> & getActiveEvents() { return m_activeEvents; }

        const class SequencePlayerComponent_Template* getTemplate() const { return (SequencePlayerComponent_Template*)m_template; }

		void						setSkipSequenceEvent(SequenceEvent *_evt) { i32 seqIndex = getEventIndex(_evt); if (seqIndex >= -1) setSkipSequenceEvent((u32)seqIndex); }
		void						setSkipSequenceEvent(u32 _eventIndex) { m_skipSequenceEvent = _eventIndex; }
		void						removeSkipSequenceEvent() { m_skipSequenceEvent = U32_INVALID; resetSkipSequence(); }
		void						processSkipSequence();
        void                        initSkipSequence(EContextIcon _contextIcon);
        void                        resetSkipSequence();
        void                        updateSkipInput(f32 _deltaTime);
        f32                         getSkipHoldProgress() const { return m_skipRequiredDuration > 0.f ? (m_skipHoldTimer / m_skipRequiredDuration) : 0.f; }
        bbool                       isSkipInProgress() const { return m_isSkipButtonHeld; }
        void                        completeSetFrame(int _curFrame);
        void                        prefetchResources(i32 _pos);
        bbool                       gotoLabel(const String8 & _label);
        void                        notifyAlreadySeen();

        void                        processPlayersStatus(bbool _activate);
        bbool                       spawnActor(const ObjectPath & _friendlyName, const StringID & _friendlyID, const String8 & _fileName, const Vec3d &_offset, bbool _flip);
        void                        deleteSpawnActor(Actor *pActor);

        static bbool                actorIsSequencePlayer(StringID _friendly, u32 & _playerIndex);
        ITF_INLINE void             setExternInativeChannels( ITF_VECTOR<StringID>* _externInactiveChannels ) { m_externInactiveChannels = _externInactiveChannels; }
        u32                         getBankState() const { return m_bankState; }

    private:
        void                        updatePlaying();
        bbool                       addTextBannerSounds();

        bbool                       actorHasAnim(const StringID & _friendlyID);
        bbool                       addAnims(Actor * _pActor, const StringID & _friendlyID);
        u32                         fillAnimInfoMarkers(const StringID & _friendlyID, ITF_VECTOR <AnimInfoMarkers> & vecAnim, ITF_VECTOR <StringID> & vecAnimExisting);

        const ITF_VECTOR<BankChange_Template> * getPlayerBankChange(const StringID & _friendlyName);
        bbool                                   applyBankChangeOnActor(const StringID & _friendlyName, Actor *_actor, const ITF_VECTOR<BankChange_Template> *_bankChange);
        void                                    processPlayerBankChange();

        bbool                       actorHasSound(const StringID & _friendlyID);
        bbool                       addSounds(Actor * _pActor, const StringID & _friendlyID);

        bbool                       actorHasText(const StringID & _friendlyID);
        bbool                       addTextSounds(Actor * _pActor, const StringID & _friendlyID);

        bbool                       actorHasRedirectPatchBank(const StringID & _friendlyID, SequenceEvent_Template *& _evt);

        void                        saveCameraMode();
        void                        restoreCameraMode();

        bbool                       checkIfCanInterrupt(EContextIcon& _skipContextIcon);
        bbool                       skipSequenceRequested(EContextIcon _skipContextIcon);

        State                       m_playState;
        void                        fillActiveEventsTable();

        ObjectRefList               m_spawnActors;
        SafeArray<SequenceEvent*>   m_eventsList;
        ITF_VECTOR<SequenceActor>   m_controlledActors;
        SafeArray<SequenceEvent*>   m_activeEvents;
        TemplateClientHandler       m_templateClientHandler;
        f32                         m_curTime_rel;      // curent time (relative time)
        i32                         m_curFrame_rel;      // curent time (relative time)
		u32							m_skipSequenceEvent;
        f32                         m_sequenceSpeedFactor;

        Vec3d                       m_anchorPos;
        bbool                       m_bUseAnchor;

        bbool                       m_bCameraControlAllowed;
        Camera::Mode                m_originalCamMode;

        i32                         m_firstEventFrame;
        i32                         m_lastEventFrame;

        char                        m_shortCutKey;
        i32                         m_frameStartSaved;

        ITF_VECTOR<StringID>*       m_externInactiveChannels;

        FriendlyActorMapping        m_forceLinkFriendlyActor;
        ActorRefList                m_actorPlayAnimControlled;
        bbool                       m_editionMode;
        bbool                       m_deleting;
        i32                         m_unskippableFramesFirstView;

        bbool                       m_deactivatedPlayers;
        bbool                       m_deactivatedPlayersDone;

        static const i32            s_resourcePrefetch;
        u32                         m_bankState;

        bbool                       m_inputListenerRegistered;
        bbool                       m_menuValidPressed;
        bbool                       m_menuBackPressed;
        f32                         m_skipHoldTimer;
        f32                         m_skipRequiredDuration;
        bbool                       m_isSkipButtonHeld;
        class View *                m_sequenceView;
};

typedef SequenceEvent* (*createFncDecl)(SequencePlayerComponent*, SequenceEvent_Template*);
typedef SequenceEvent_Template* (*createFromBlobFncDecl)(Blob*, SequencePlayerComponent_Template*);

typedef struct SequenceEvtMapping
{
    SequenceEvent_Template::EvtType m_id;
    const char* m_name;
    createFncDecl         m_instanceFactory;
    createFromBlobFncDecl m_templateFactory;
} SequenceEvtMapping;

//-------------------------------------------------------------------------------------
class SequencePlayerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(SequencePlayerComponent_Template, TemplateActorComponent,2333879612);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(SequencePlayerComponent);

public:

    SequencePlayerComponent_Template()
    : m_unskippableDurationFirstTime(-1.f/*take actual value from game interface*/)
    , m_deactivatedPlayers(bfalse)
    , m_reactivateAlive(btrue)
    , m_reactivatePlayers(btrue)
    , m_saveInitialView(bfalse)
	{}
    ~SequencePlayerComponent_Template();

    void                                    sortSequence();

    u32                                     getEventsCount() const {return m_events.size();}
    SequenceEvent_Template*                 getEventAtIndex(u32 _index) const {return m_events[_index];}

    void                                    addEvent        (   SequenceEvent_Template *_evt ) { m_events.push_back(_evt); sortSequence(); }
    bbool                                   removeEvent     (   u32 _uid );

    SequenceEvent_Template*                 createEventFromBlob( Blob* _blob );
    void                                    serializeToBlob (Blob* _blob, const String & _path);

    void                                    updateHierarchy();
    SequenceEvent_Template*                 getEventTemplateWithUID(u32 _uid, u32 *index = NULL);

    f32                                     getUnskippableDurationFirstTime() const {return m_unskippableDurationFirstTime;}
    bbool                                   deactivatedPlayers() const { return m_deactivatedPlayers; }
    void                                    setDeactivatedPlayers(bbool _deactivate) { m_deactivatedPlayers = _deactivate; }

	bbool                                   reactivateAlive() const { return m_reactivateAlive; }
	void                                    setReactivateAlive(bbool _reactivate) { m_reactivateAlive = _reactivate; }

	bbool                                   reactivatePlayers() const { return m_reactivatePlayers; }
	void                                    setReactivatePlayers(bbool _reactivate) { m_reactivatePlayers = _reactivate; }

    bbool                                   saveInitialView() const { return m_saveInitialView; }
    void                                    setSaveInitialView(bbool _save) { m_saveInitialView = _save; }


    ITF_VECTOR<SequenceTrackInfo_Template> & getTrackList() { return m_trackList; }
    bbool                                   isChild(u32 _index, u32 _parent, bbool _skipLoopIssues);
    void                                    checkIndex(u32 index);
    bbool                                   insertTrack(u32 index);
    bbool                                   removeTrack(u32 index);
    u32                                     moveTrack(u32 index, u32 newIndex, u32 mode, u32 newParent);
    u32                                     moveGroup(u32 index, u32 newIndex, u32 mode, u32 newParent, u32 _nbChildren = U32_INVALID);
    void                                    serializeTrackListToBlob (Blob* _blob);

    void                                          computeActorEventZoneList();
    const ITF_VECTOR<ActorEventDefinitionZones> & getActorEventZoneList() const { return m_actorEventZoneList; }

private:
    f32                                     m_unskippableDurationFirstTime;
    SafeArray<SequenceEvent_Template*>      m_events;
    bbool                                   m_deactivatedPlayers;
    bbool                                   m_reactivateAlive;
	bbool                                   m_reactivatePlayers;
    bbool                                   m_saveInitialView;

    ITF_VECTOR<SequenceTrackInfo_Template>  m_trackList;
    ITF_VECTOR<ActorEventDefinitionZones>   m_actorEventZoneList;

#ifdef ITF_SUPPORT_COOKING
    void                                    preSerializeSave();
#endif //ITF_SUPPORT_COOKING

};

} // namespace ITF

#endif // _ITF_SEQUENCEPLAYERCOMPONENT_H_

