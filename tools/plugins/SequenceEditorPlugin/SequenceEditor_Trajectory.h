#ifndef _ITF_SEQUENCEEDITORTRAJECTORY_H_
#define _ITF_SEQUENCEEDITORTRAJECTORY_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#include "editor\PickingShape\PickingShapeData_RotationScale.h"
#endif //_ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

namespace ITF
{
class SequenceEditorPlugin;
class SequencePlayerComponent;
class Actor;

class Trajectory;
class Trajectory_Template;
class PlayTrajectory_evt;
class PlayTrajectory_evtTemplate;

class SequenceEvent_Template;

#ifdef ITF_WINDOWS
    #define DEBUG_SEQUENCES
#endif

#ifdef DEBUG_SEQUENCES
    #define SET_TRAJ_STATE(statearg) setState(statearg, __FILE__, __LINE__)
    #define SET_CUR_TRAJ(_index) setCurrentTrajectory(_index, __FILE__, __LINE__)
#else
    #define SET_TRAJ_STATE(statearg) setState(statearg)
    #define SET_CUR_TRAJ(_index) setCurrentTrajectory(_index)
#endif

class SequenceEditorTrajectory
{
public:
    enum state
    {
        Idle,
        AddPoints,
        FollowCam,
        CamSnapShot,
        ActorSnapShot,
        ReplayCam,
        ReplayCam2,
        ENUM_FORCE_SIZE_32(0)
    };

    enum DataModifier
    {
        none = 0,
        translation,
        rotation,
        scale,
        flip,
        usePatches,
        weight,
        globalColor,
        alpha,
    };
    
                    SequenceEditorTrajectory();
    void            update();
    void            onKey     (   i32 _key, InputAdapter::PressStatus _status );
    void            onMouseButton  (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
    void            onMouseWheel   (   i32 _wheelValue, i32 _wheelDelta);
    bbool           onPickShape     (PickingShape* _shape);
    bbool           isPickingAllowed ( );
    void            onObjectChanged(BaseObject* pBO);
    bbool           onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos);
    bbool           onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos );

#ifdef DEBUG_SEQUENCES
    void            setState(state _state, const char* _file, int _line);
    void            setCurrentTrajectory(int _index, const char* _file, int _line);
#else
    void            setState(state _state);
    void            setCurrentTrajectory(int _index);
#endif
    state           getState();

    PlayTrajectory_evt*         getCurrentEvent();
    Trajectory*                 getCurrentTrajectory();

    PlayTrajectory_evtTemplate* getCurrentEventTemplate();
    Trajectory_Template*        getCurrentTrajectoryTemplate();

    void            selectTrajectory(int _index);

    bbool           bCanEnterEditMode ();
    bbool           bCanLeaveEditMode ();
    void            UnselectAll       (bbool _force);
    void            getSelectionStatus (ObjectRef& _selection, bbool& _selectionIsLocked);
    void            setOwner(SequenceEditorPlugin* _plug)    {m_owner = _plug;}
    void            sendCurTrajectory(bbool _undoRedo = btrue, bbool _unselectShape = bfalse);
    void            sendTrajectory(Trajectory* traj, PlayTrajectory_evtTemplate* evtTemplate, bbool _undoRedo = btrue);


    static const int       m_createNewTrajectory = -2;
    static const int       m_editTrajectory = -1;

    void            draw();
    void            drawOneTrajectory(Trajectory* _t);
    void            updateFollowCam();
    void            updateReplay();
    void            onSceneSave();
    void            onStartRotation();
    void            onEndRotation();
    bbool           onEditorRemove  (   const SafeArray<PickingShape*>& _shapesWithSameOwner   );

    void            setShowAllTraj(bbool _showAllTrajectory) { m_bShowAllTraj = _showAllTrajectory; }
    void            clearSelection() { m_curTrajectoryIndex = U32_INVALID; }

    i32             getCurrentSelectedPointIndex();
    void            setCurrentSelectedPointIndex(i32 _index);       

    bbool           m_duplicationJustoccured, m_currentSelectionIsLocked, m_globalMoveInProgress, m_canDrag;
    f32             m_NoPickingTimer, m_LMBclickedTimer;
    PickingInfo2D   m_currentPickingInfo;
    Vec3d           m_deltaCam;
    SequenceEditorPlugin* m_owner;

    f32 m_positionMouseXOld;

    // picking
    virtual PickingShapePluginData* createPickingShapeData(PickingShape *);
    void                            createPickingShapeForTrajPoints (const ObjectRef _actor, const Trajectory_Template *trajTemplate = NULL, bbool _unselectShape = bfalse);
    void                            fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene) const;
    void                            refreshPicking(Trajectory* _t);


    //picking add point
    void                            setAddPointFrame(float _frame) { m_addPointFrame = _frame; }
    float                           getAddPointFrame(            ) { return m_addPointFrame;   }

    //////////////////////////////////////////////////////////////////////////
    ///Internal class for picking shape of polyline
    class PickingShapeData : public PickingShapePluginData
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        ///Construction
        PickingShapeData(PickingShape* _pParentShape, Plugin* _pPlugin);

        //////////////////////////////////////////////////////////////////////////
        ///Draw component
        virtual void        draw();
    };



    bbool   bIsEditingActorTrajectory(const ObjectRef _actor);
    bbool   isSubEditAllowed ();

    bbool fillContextMenu (EditorContextMenu& _menu, ObjectRef _target, bbool _topItems);
    void  onContextMenuItemSelected(ContextMenuItem* /*_item*/, ObjectRef /*_menuTarget*/);

private:
    typedef struct ActorOriginalData
    {
        Vec3d          m_savedPos;
        f32            m_savedRot;
        Vec2d          m_savedScale;
        ObjectRef       m_objRef;
        f32             m_rotAtLastRightClick;
    } ActorOriginalData;

    


    void            updateCamSnapShot();
    void            updateActorSnapShot();
    void            onKeyUpCamSnapShot();
    void            onKeyUpActorSnapShot();
    void            onKeyDownCamSnapShot();
    void            onKeyDownActorSnapShot();
    void            startActorSnapShot();
    void            stopActorSnapShot();
    Actor*          getCurrentTrajectoryActor();
    void            goToNextKey(Trajectory* _pTraj, PlayTrajectory_evt* _evt);
    void            goToPrevKey(Trajectory* _pTraj, PlayTrajectory_evt* _evt);
    void            goToKey(Trajectory* _pTraj, PlayTrajectory_evt* _evt, int _index);
    void            applyRightClick();
    void            reachNearestNextKeyFromCurrentFrame();
    void            reachNearestPrevKeyFromCurrentFrame();
    state           m_trajState;
    state           m_previousState;

    u32             m_curTrajectoryIndex;
    ActorOriginalData m_trajActor;
    bbool           m_bUnselectAllAuthorized;
    
    bbool           m_bShowAllTraj;
    bbool           m_mbLeftIsPressed;


    //picking add point
    PickingShape_Disc*      addPickingPoint(const ObjectRef _curTrajectoryRef, u32 _index, u32 _mode);
    bbool                   computeInternalShape(Trajectory *_traj, Trajectory_Template *_trajTemplate, const Vec3d &_delta);

    f32             m_addPointFrame;
    bbool           m_insertPointOk;    
    u32             m_currentPosCount;
    Vec3d           m_addPointPosition;

};

} // namespace ITF


#endif // _ITF_SEQUENCEEDITORTRAJECTORY_H_
