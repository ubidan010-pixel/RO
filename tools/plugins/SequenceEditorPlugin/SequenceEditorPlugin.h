#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#define _ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_SEQUENCEEDITORTRAJECTORY_H_
#include "SequenceEditor_Trajectory.h"
#endif //_ITF_SEQUENCEEDITORTRAJECTORY_H_

#ifndef _ITF_SEQUENCEEDITORACTOR_H_
#include "SequenceEditor_Actor.h"
#endif //_ITF_SEQUENCEEDITORACTOR_H_

#ifndef _ITF_SEQUENCEEDITORMESSAGES_H_
#include "SequenceEditor_Messages.h"
#endif //_ITF_SEQUENCEEDITORMESSAGES_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

namespace ITF
{

class Transaction;
class Transaction_Serialize_Receive;
class NETPeer;
class Blob;
class Actor;

typedef void(*funcAssetFactoryCommand)(Blob*,Transaction_Serialize_Receive*);
struct  PickingInfo;
class   NETPeer;
class   SequencePlayerComponent;

#if defined ITF_WINDOWS
    #define DEBUG_SEQUENCES
#endif

#ifdef DEBUG_SEQUENCES
    #define SET_SEQPLUG_STATE(statearg) setState(statearg, __FILE__, __LINE__)
#else
    #define SET_SEQPLUG_STATE(statearg) setState(statearg)
#endif

class SequenceEditorPlugin : public Plugin
{
public:
    enum state
    {
        state_Idle,
        state_EditSequence,
        state_MovePivot,
        ENUM_FORCE_SIZE_32(0)
    };
    
    static                      SequenceEditorPlugin*       m_singleton;
    // constant
    static  const char*         getPluginName()             {return  "SequenceEditor_Plugin";}
    static  const NETPeer*      getNetPeer()                {return m_singleton->m_editorPeer;}
    static  u32                 getSenderID()               {return m_singleton->m_editorSenderID;}

    // constructor - descrictor
    SequenceEditorPlugin();
    virtual ~SequenceEditorPlugin();
    
    
    bbool                       isPickingAllowed    ( );   
    void                        fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const;

    virtual void                onToolConnect(const NETPeer* _peer, u32 _senderID);
    virtual void                onToolDisconnect(const NETPeer* _peer);

    void                        onPostSceneActivation (const Scene * /*_scene*/, bbool /*_activated*/);
    void                        update              ( );
    void                        draw                ();
    void                        receive             ( Blob& _blob, const NETPeer* _peer, u32 _senderID);
    bbool                       onPickShape         ( PickingShape* _shape);
    void                        onKey               ( i32 _key, InputAdapter::PressStatus _status );
    void                        onMouseButton       ( InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
    void                        onMouseWheel        ( i32 _wheelValue, i32 _wheelDelta);
    void                        onObjectCreated     ( BaseObject* _pObj );
    void                        onObjectAdded       ( const Scene* _pScene, BaseObject* _pObj );
    bbool                       onEditorRemove      (   const SafeArray<PickingShape*>& /*_shapesWithSameOwner*/  );
    bbool                       bCanEnterEditMode   ( );
    bbool                       bCanLeaveEditMode   ( );
    void                        UnselectAll         ( bbool _force);
    bbool                       selectEvent         ( u32 _uid, i32 selectFlag);
    void                        processSelectedEvent( );
    void                        getSelectionStatus  ( ObjectRef& _selection, bbool& _selectionIsLocked);
    void                        onObjectChanged     ( BaseObject* pBO);
    bbool                       onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos);
    bbool                       onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos );

    SequenceEditorTrajectory&   getTrajectoryEditor ( ) {return m_trajectoryEditor;}
    SequenceEditorMessages&     getSequenceEditorMessages ( ) {return m_messagesEditor;}

    virtual PickingShapePluginData* createPickingShapeData(PickingShape *);    
    void                        fullClone(Actor_Template *actSrc, Actor_Template *actDst);
    void                        moveAllTrajectories(Actor *act, const Vec3d & _delta);
    void                        updateSequenceActors(SequencePlayerComponent *seqPlayComp);

#ifdef DEBUG_SEQUENCES
    void                        setState(state _state, const char* _file, int _line);
#else
    void                        setState(state _state);
#endif
    state                       getState();

    bbool                               setCurrentSequenceActor (Actor* _act, bbool _allowAdd);
    Actor*                              getCurrentSequenceActor ();
    Actor_Template*                     getCurrentSequenceActorTemplate();
    SequencePlayerComponent*            getCurrentSequenceComponent();
    SequencePlayerComponent_Template*   getCurrentSequenceComponentTemplate();

    void                        onNewSequence(ObjectRef _actor);
    SequenceEditorActor&        getActorEditor() {return  m_actorEditor;}
    void                        onActorPicked   (ObjectRef _actor);
    void                        onActorSubEdit  (ObjectRef _actor);
    virtual void                onPickableRenamed(Pickable* _pickable, const String8& _name);
    void                        onCameraModeChange (int _prevMode, int _newMode);
    void                        onStartRotation();
    void                        onEndRotation();

    void                        hideTrajectories();
    void                        stopAll();
    void                        sendCurrentFrame(ObjectRef _sequenceRef, i32 _frame);
    bbool                       bIsEditingActorTrajectory(const ObjectRef _actor);
    bbool                       isSubEditAllowed ();// inherited from Plugin
    void                        frameStep(i32 _amount);

    void                        goToNextFrame(i32 _amount = 1);
    void                        goToPrevFrame(i32 _amount = 1);
    
    Spline::Interpolation       getDefaultSplineMode() { return m_defaultSplineMode; }
    void                        setDefaultSplineMode(Spline::Interpolation _defaultSplineMode) 
    {   m_defaultSplineMode             = _defaultSplineMode; 
        Spline::m_defaultInterpolation  = _defaultSplineMode; 
    }

    bbool                       isBezierLinked() { return m_bezierLinked; }
    void                        setBezierLinked(bbool _bezierLinked) { m_bezierLinked = _bezierLinked; } 
    

    void                        setAutoKeyMode(bbool _mode) { m_autoKeyMode = _mode;}
    bbool                       getAutoKeyMode() { return m_autoKeyMode; } ;
    bbool                       isAutoKeyAllowed();

    void                        setCameraModeSequence(bbool _mode) { m_cameraModeSequence = _mode; updateGlobalSequenceData(bfalse); }
    bbool                       getCameraModeSequence() { return m_cameraModeSequence; } ;
    void                        forceCurrentFrame(Actor *seqActor, int frame);
    void                        updateGlobalSequenceData(bbool _unpause);

    void                        changeAutoKeyMode() { m_autoKeyMode = !m_autoKeyMode; }
    void                        setNewInitialPosition();

    void                        sendSenquenceState(ObjectRef _sequenceRef, SequencePlayerComponent::State _seqState);
    void                        fillEmptyTrajectories(const Vec3d &_pos);

    bbool                       fillContextMenu (EditorContextMenu& _menu, ObjectRef _target, bbool _topItems);
    void                        onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget);

    ITF_VECTOR<StringID> &      getExternalInactiveChannels() { return m_externInactiveChannels; }
    void                        setExternInactiveChannels(bbool _useExternInactiveChannels) { m_useExternInactiveChannels = _useExternInactiveChannels; }

#ifdef DEBUG_SEQUENCES
    FILE*                       m_log;
#endif


    typedef struct SeqState
    {
        ObjectRef                           m_actorSequenceRef;
        Actor_Template                      *m_actorTemplate;
        Actor_Template                      *m_actorTemplateOverride;
        Actor_Template                      *m_actorTemplateUndoRedo;
        i32                                 m_sequenceFrame;
        SequencePlayerComponent::State      m_oldState;
        bbool                               m_needReload;
        bbool                               m_processUndo;
        bbool                               m_modified;
        String                              m_path;

        SeqState()
        : m_sequenceFrame(0)
        , m_oldState(SequencePlayerComponent::State_Stopped)
        , m_needReload(bfalse)
        , m_processUndo(bfalse)
        , m_modified(bfalse)
        , m_actorTemplate(NULL)
        , m_actorTemplateOverride(NULL)
        , m_actorTemplateUndoRedo(NULL)
        {}

        void clear()
        { 
            SF_DEL(m_actorTemplate);
            SF_DEL(m_actorTemplateOverride);
            SF_DEL(m_actorTemplateUndoRedo);
        }
    } SeqState;

    ITF_VECTOR<SeqState> &      getSequencesState() { return m_sequencesState; }
    bbool                       getSequenceState(ObjectRef _actorSequenceRef, SeqState* & _seqState);
    void                        propagateActorChanges(Actor * _actorSequence, bbool _undoRedo, bbool _noHotReaload = bfalse, bbool _sendTrackList = btrue);
    void                        revertUnpropagateActorChanges(Actor * _actorSequence);
    
    // 
#define COMPUTE_TRACK_LENGHT    0x1
#define FORCE_RECOMPUTE         0x2

#define RECOMPUTE_ALL           (COMPUTE_TRACK_LENGHT | FORCE_RECOMPUTE)

    void                        checkTracks(u32 _mask);
    ObjectRef                   getSelectedSeqActor() { return m_actorEditor.getSeqPlaySelected(); }

private:
    void                        clearSequenceStates();
    int                         m_currentSequenceActorIndex;

    SequenceEditorMessages      m_messagesEditor;
    SequenceEditorActor         m_actorEditor;
    SequenceEditorTrajectory    m_trajectoryEditor;

    ActorRef                    m_setSequencePivotActor;

    Spline::Interpolation       m_defaultSplineMode;
    bbool                       m_bezierLinked;
    bbool                       m_autoKeyMode;
    bbool                       m_cameraModeSequence;
    state                       m_plugState;

    bbool                       m_useExternInactiveChannels;
    ITF_VECTOR<StringID>        m_externInactiveChannels;

    ITF_VECTOR<SeqState>        m_sequencesState;
    TemplateClientHandler       m_templateClientHandler;
};

void SequenceEditorProcessUndoRedoChange(ObjectRef _actorRef);

} // namespace ITF


#endif // _ITF_SequenceEditorPLUGIN_H_
