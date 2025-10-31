#ifndef _ITF_SEQUENCEEDITORMESSAGES_H_
#define _ITF_SEQUENCEEDITORMESSAGES_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

namespace ITF
{

class SequencePlayerComponent;
class AnimatedComponent;
class Actor;
class NETPeer;
class Blob;
class SequenceEditorPlugin;
class Scene;

class SequenceEditorMessages
{
public:
    SequenceEditorMessages();

    // Receive message
    static const char*      undoCmdName()                   {return  "undo";}
    static const char*      redoCmdName()                   {return  "redo";}
    static const char*      exitCmdName()                   {return  "exit";}

    static const char*      updateEventIsDoneCmdName()      {return  "updateEventIsDone";}
    static const char*      addEventIsDoneCmdName()         {return  "addEventIsDone";}
    static const char*      removeEventIsDoneCmdName()      {return  "removeEventIsDone";} 

    static const char*      updateTrackIsDoneCmdName()      {return  "updateTrackIsDone";}
    static const char*      addTrackIsDoneCmdName()         {return  "addTrackIsDone";}
    static const char*      removeTrackIsDoneCmdName()      {return  "removeTrackIsDone";} 


    // Send message
    static const char*      getSessionInfoCmdName()         {return  "getSessionInfo";}
    static const char*      actorSelectedCmdName()          {return  "actorSelected";}
    static const char*      getSequenceCmdName()            {return  "getSequence";}

    static const char*      addNewSequenceCmdName()         {return  "addNewSequence";}
    static const char*      sequenceSelectedCmdName()       {return  "sequenceSelected";}
    static const char*      sequenceTrackListCmdName()      {return  "sequenceTrackList";}
    static const char*      playSequenceCmdName()           {return  "playSequence";}
    static const char*      stopSequenceCmdName()           {return  "stopSequence";}
    static const char*      sequenceStateCmdName()          {return  "sequenceState";}
    static const char*      setCurrentFrameCmdName()        {return  "setCurrentFrame";}
    static const char*      setInactivateChannels()         {return  "setInactivateChannels";}

    static const char*      getInfoListAnimationsByFriendlyCmdName()    {return  "getInfoListAnimationsByFriendly";}
    static const char*      getInfoListSubAnimsByFriendlyCmdName()      {return  "getInfoListSubAnimsByFriendly";}
    static const char*      getInfoListSoundByFriendlyCmdName()         {return  "getInfoListSoundByFriendly";}

    static const char*      getListActorCmdName()           {return  "getListActor";}
    static const char*      getListPlayersCmdName()         {return  "getListPlayers";}
    static const char*      getListSequenceCmdName()        {return  "getListSequence";}

    static const char*      getInfoListAnimationsCmdName()  {return  "infoListAnimations";}
    static const char*      getInfoListSubAnimsCmdName()    {return  "infoListSubAnims";}
    static const char*      getInfoListSoundCmdName()       {return  "infoListSound";}

    static const char*      setPreviewPlayAnimCmdName()     {return  "previewPlayAnim";}
    static const char*      stopPreviewPlayAnimCmdName()    {return  "stopPreviewPlayAnim";}

    static const char*      sendUnselectCmdName()           {return  "unselect";}
    static const char*      sendSelectedEventsCmdName()     {return  "selectedEvents";}

    static const char*      sendSaveWorldCmdName()          {return  "saveWorld";}
    static const char*      sendSaveOneSequenceCmdName()    {return  "saveOneSequence";}
    static const char*      sendSaveAllSequencesCmdName()   {return  "saveAllSequences";}

    static const char*      addEventCmdName()               {return  "addEvent";}
    static const char*      updateEventCmdName()            {return  "updateEvent";}
    static const char*      removeEventCmdName()            {return  "removeEvent";}

    static const char*      addTrackCmdName()               {return  "addTrack";}
    static const char*      updateTrackCmdName()            {return  "updateTrack";}
    static const char*      removeTrackCmdName()            {return  "removeTrack";}
    static const char*      groupTracksCmdName()            {return  "groupTracks";}
    static const char*      ungroupTracksCmdName()          {return  "ungroupTracks";}

    static const char*      splitEventCmdName()             {return  "splitEvent";}
    static const char*      mergeEventCmdName()             {return  "mergeEvent";}

    static const char*      setRecordModeStateCmdName()     {return  "setRecordModeState";}
    static const char*      getRecordModeStateCmdName()     {return  "getRecordModeState";}

    static const char*      setDefaultSplineModeCmdName()   {return  "setDefaultSplineMode";}
    static const char*      getDefaultSplineModeCmdName()   {return  "getDefaultSplineMode";}

    static const char*      setBezierLinkedCmdName()   {return  "setBezierLinked";}
    static const char*      getBezierLinkedCmdName()   {return  "getBezierLinked";}

    static const char*      setActicePlayerListCmdName()    {return  "setActivePlayerList";}
    static const char*      getActicePlayerListCmdName()    {return  "getActivePlayerList";}

    static const char*      setDisablePlayersCmdName()          {return  "setDisablePlayers";}
    static const char*      getDisablePlayersCmdName()          {return  "getDisablePlayers";}
    static const char*      setReactivatePlayersAliveCmdName()  {return  "setReactivatePlayersAlive";}
    static const char*      getReactivatePlayersAliveCmdName()  {return  "getReactivatePlayersAlive";}
    static const char*      setReactivatePlayersCmdName()       {return  "setReactivatePlayers";}
    static const char*      getReactivatePlayersCmdName()       {return  "getReactivatePlayers";}
    static const char*      setSaveInitialViewCmdName()         {return  "setSaveInitialView";}
    static const char*      getSaveInitialViewCmdName()         {return  "getSaveInitialView";}

    static const char*      setCameraModeStateCmdName()     {return  "setCameraModeState";}
    static const char*      getCameraModeStateCmdName()     {return  "getCameraModeState";}

    static const char*      setCameraOnActorCmdName()       {return  "setCameraOnActor";}

    static const char*      getGameplayEventsListCmdName()  {return  "getGameplayEvents";}
  
    static  const char*     getTimerSynchroCmdName()        {return  "timerSynchro";}

    void                    sendSequenceList ( bbool _forUpdateOnly = bfalse);
    void                    sendSequence( Actor* actSequence );
    void                    sendSequenceTrackList( Actor* actSequence );

    void                    sendTracks(Actor* actSequence);

    void                    setOwner(SequenceEditorPlugin* _plug)    {m_owner = _plug;}
    void                    receive(Blob& _blob, const NETPeer* _peer, u32 _senderID);
    void                    saveSequences();

    void                    sendRecordModeState                   (               );
    void                    sendCameraModeState                   (               );
    void                    sendActivePlayerList                  (               );
    void                    sendDefaultSplineMode                 (               );
    void                    sendBezierLinked                      (               );
    void                    sendListPlayers                    (               );

    bbool                   playerListSendOk() const { return m_listPlayersSendOk; }

private:
    void updateLUA(Blob& _blob);
    void onReceiveSequence(Blob& _blob);
    void onSequencePlayerActorPicked(Actor* _pActor, Blob& _blob);
    Actor* getSequence(Blob& _blob);
    SequenceEditorPlugin* m_owner;
    String m_sequencePlayerTemplateLUA;
    bbool  m_listPlayersSendOk;

    void onGetSessionInfoCmd                   (   Blob& _blob );
    void onPlaySequenceCmd                     (   Blob& _blob );
    void onStopSequenceCmd                     (   Blob& _blob );
    void onAddNewSequenceCmd                   (   Blob& _blob );
    void onSetCurrentFrameCmd                  (   Blob& _blob );
    void onSetRecordModeStateCmd               (   Blob& _blob );
    void onSetActivePlayerListCmd              (   Blob& _blob );
    void onSetDefaultSplineModeCmd             (   Blob& _blob );
    void onSetBezierLinkedCmd                  (   Blob& _blob );
    void onSetInactivateChannels               (   Blob& _blob );

    void onGetListActionAndAnimCmd             (   Blob& _blob ); 
    void onGetInfoListAnimationsByFriendlyCmd  (   Blob& _blob, bbool _subAnims ); 
    void onGetInfoListSoundByFriendlyCmd       (   Blob& _blob ); 

    void onGetListSequenceCmd                  (   Blob& _blob );
    void onGetListActorCmd                     (   Blob& _blob );
    void onGetListPlayersCmd                   (   Blob& _blob );
    void onGetSequenceCmd                      (   Blob& _blob );

    void onUnselectCmd                         (   Blob& _blob );
    void onSelectedEventsCmd                   (   Blob& _blob );

    void onSaveWorldCmd                        (   Blob& _blob );
    void onSaveOneSequenceCmd                  (   Blob& _blob );
    void onSaveAllSequencesCmd                 (   Blob& _blob );

    void onAddEventCmd                         (   Blob& _blob );
    void onRemoveEventCmd                      (   Blob& _blob );
    void onUpdateEventCmd                      (   Blob& _blob );

    void onAddTrackCmd                         (   Blob& _blob );
    void onRemoveTrackCmd                      (   Blob& _blob );
    void onUpdateTrackCmd                      (   Blob& _blob );
    void onGroupTracksCmd                      (   Blob& _blob );
    void onUngroupTracksCmd                    (   Blob& _blob );

    void onSplitEventCmd                       (   Blob& _blob );
    void onMergeEventCmd                       (   Blob& _blob );

    void onGetGameplayEventsListCmdName        (   Blob& _blob );

    void onSetDisablePlayersCmd                (   Blob& _blob );
    void onSetReactivatePlayersAliveCmd        (   Blob& _blob );
    void onSetReactivatePlayersCmd             (   Blob& _blob );
    void onSetSaveInitialViewCmd               (   Blob& _blob );
    void onSetCameraModeStateCmd               (   Blob& _blob );
    void setCameraOnActor                      (   Blob& _blob );

    bbool           m_previousUpdateCmdState;
};

} // namespace ITF


#endif // _ITF_SEQUENCEEDITORMESSAGES_H_
