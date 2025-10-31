// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)

#ifndef _ITF_SEQUENCEEDITORTRAJECTORY_H_
#include "SequenceEditor_Trajectory.h"
#endif //_ITF_SEQUENCEEDITORTRAJECTORY_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_TRAJECTORY_H_
#include "engine/animation/trajectory.h"
#endif //_ITF_TRAJECTORY_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDServer.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_
#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#include "tools/plugins/SequenceEditorPlugin/SequenceEditorPlugin.h"
#endif //_ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_


#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#define TYPE_SHAPE_POINT        0
#define TYPE_SHAPE_NORMAL_IN    1
#define TYPE_SHAPE_NORMAL_OUT   2


namespace ITF
{


    union TrajectoryExtraInfo
    {
        explicit TrajectoryExtraInfo(u32 _raw = 0) : m_raw(_raw) {}
        u32 m_raw;
        struct 
        {
            u32 m_index:30;
            u32 m_type:2;
        };
    };

#define TIME_EPSILON (TARGETDT * 0.5f)
#define FRAME_FLOAT_EPSILON (1 * 0.5f)


    SequenceEditorTrajectory::SequenceEditorTrajectory()
    {
        m_duplicationJustoccured = bfalse;
        m_currentSelectionIsLocked = bfalse;
        m_globalMoveInProgress = bfalse;
        m_canDrag = bfalse;
        m_NoPickingTimer = m_LMBclickedTimer = 0.f;
        m_curTrajectoryIndex = U32_INVALID;
        m_bShowAllTraj = bfalse;
        m_trajActor.m_objRef = ITF_INVALID_OBJREF;
        m_bUnselectAllAuthorized = btrue;
        m_owner = NULL;
        m_positionMouseXOld = 0;
        m_mbLeftIsPressed = bfalse;
        m_addPointFrame = 0;
        m_insertPointOk = bfalse;
        m_currentPosCount = 0;

        m_trajState = Idle;
        m_previousState = Idle;
    }

    void SequenceEditorTrajectory::UnselectAll(bbool _force)
    {
        Trajectory* trj = getCurrentTrajectory();
        if(trj)
            trj->setSelectionIsLocked(bfalse);

        if (_force || m_bUnselectAllAuthorized)
        {
            m_bUnselectAllAuthorized = btrue;
            SET_CUR_TRAJ(-1);
            m_currentSelectionIsLocked = bfalse;
            SET_TRAJ_STATE(Idle);
        }
   }



    void SequenceEditorTrajectory::getSelectionStatus (ObjectRef& _selection, bbool& _selectionIsLocked)
    {
        Trajectory * traj = getCurrentTrajectory();
        if (traj)
        {
            _selection = traj->getRef();
        } else
        {
            _selection.invalidate();
        }
        _selectionIsLocked = m_currentSelectionIsLocked;
    }



    bbool SequenceEditorTrajectory::isPickingAllowed()
    {
        //if(!m_owner->isAutoKeyAllowed())
        //    return btrue;

        //switch (getState())
        //{
        //case AddPoints:
        //case FollowCam:
        //case CamSnapShot:
        //case ActorSnapShot:
        //    return bfalse;
        //default:
        //    break;
        //}


        return btrue;
    }

    void SequenceEditorTrajectory::update()
    {
        m_bUnselectAllAuthorized = bfalse;
        Trajectory                * curTraj           = getCurrentTrajectory();
        if (!curTraj)
            return;
        const Trajectory_Template * curTrajTemplate   = curTraj->getTemplate();
        if(!curTrajTemplate)
        {
            if (PLUGINGATEWAY->hasExclusiveFocus(m_owner))
                PLUGINGATEWAY->setExclusiveFocus(NULL);
            return;
        }

        
        PLUGINGATEWAY->setExclusiveFocus(m_owner);

        switch (getState())
        {
        case AddPoints:
            if(!m_insertPointOk)
            {
                if(curTrajTemplate->getPosCount() > 0)
                    EDITOR->showMouseIcon(Editor_IconStock::Icon_Plus);
                else
                    EDITOR->showMouseIcon(Editor_IconStock::Icon_New);
            }
            break;
        case ActorSnapShot:
            // EDITOR->showMouseIcon(Editor::Icon_Plus);
            updateActorSnapShot();
            break;
        case FollowCam:
            updateFollowCam();
            break;
        case CamSnapShot:
            updateCamSnapShot();
            break;
        case ReplayCam:
        case ReplayCam2:
            updateReplay();
            break;
        default:
            break;
        }




        if(curTrajTemplate->getPosCount() != m_currentPosCount)
        {
            refreshPicking(curTraj);
        }
        m_currentPosCount = curTrajTemplate->getPosCount();



        /************************************************************************/
        /*                                                                      */
        /************************************************************************/
        Vec3d v2d;
        v2d.m_x = EDITOR->getMouse2d().m_x;
        v2d.m_y = EDITOR->getMouse2d().m_y;

        f32 tolerance = PickingShape::m_disc_picking_tolerance;
        Vec3d v2dDistanceAddingPoint(tolerance * 2, tolerance * 2, 0);
        m_insertPointOk = bfalse;

        for (u32 i = 0; i < curTrajTemplate->getPosCount(); i++)
        {
            if (i>0)
            {
                if (curTrajTemplate->getPosInterpAt(i) == Spline::interp_linear)
                {
                    //GFX_ADAPTER->drawDBG3DLine(curTraj->getGlobalPosAt(i-1, curTraj->getPos()), curTrajTemplate->getGlobalPosAt(i, curTraj->getPos()), r, g, b, 0, 4.f);
                }
                else
                {
                    f32 prevTime = curTrajTemplate->getPosTimeAt(i-1);
                    f32 currTime = curTrajTemplate->getPosTimeAt(i);
                    // Instead of drawing a single line, draw a line betwean intersections

                    u32     steps           = (u32)((currTime - prevTime));
                    f32     subdivisions    = (currTime - prevTime) / (f32)steps;
                    f32     curSubDiv       = prevTime + subdivisions;

                    Vec3d currentPoint, nextPoint;
                    currentPoint = curTrajTemplate->getGlobalPosAt(i-1, curTraj->getPos());

                    for (u32 j=0; j<steps; j++, curSubDiv += subdivisions)
                    {
                        curTrajTemplate->computeGlobalPosAtTime(curSubDiv, nextPoint, curTraj->getPos());
                        currentPoint = nextPoint;

                        if(getState() == AddPoints)
                        {
                            Vec3d v2dPoint;
                            GFX_ADAPTER->compute3DTo2D(nextPoint, v2dPoint);                                                                
                            f32 delta = tolerance / 2.f;
                            if(v2d.m_x > v2dPoint.m_x - delta && v2d.m_x < v2dPoint.m_x + delta &&
                                v2d.m_y > v2dPoint.m_y - delta && v2d.m_y < v2dPoint.m_y + delta)
                            {
                                f32 x = v2dPoint.m_x - v2d.m_x;
                                if( x < 0) x = -x;

                                f32 y = v2dPoint.m_y - v2d.m_y;
                                if( y < 0) y = -y;

                                if(x < v2dDistanceAddingPoint.m_x || y < v2dDistanceAddingPoint.m_y)
                                {
                                    v2dDistanceAddingPoint.m_x = x;
                                    v2dDistanceAddingPoint.m_y = y;
                                    setAddPointFrame(curSubDiv);
                                    m_insertPointOk = btrue;
                                }
                            }

                        }
                    }
                }
            }
        }

        draw();
    }


    bbool SequenceEditorTrajectory::onEditorRemove  (   const SafeArray<PickingShape*>& _shapesWithSameOwner   )
    {
        bbool bResult = bfalse;

        u32 subAnchorRemovedCount = 0;

        Trajectory_Template*    trajTemplate = getCurrentTrajectoryTemplate();
        if(!trajTemplate)
            return false;

        for(u32 i = 0; i < _shapesWithSameOwner.size(); ++i)
        {
            PickingShape* pShape = _shapesWithSameOwner[i];
            //if(pShape->isSubAnchor())
            {
                const u32 index = TrajectoryExtraInfo(pShape->getData()).m_index - subAnchorRemovedCount; // we have to modify the index, because we are removing one point at a time and the index are not recomputed in extra information

                if (index < trajTemplate->getPosCount())
                {
                    trajTemplate->removePosData(index);

                    subAnchorRemovedCount++;
                    bResult = btrue;
                }
            }
        }

        if(bResult)
            // TODO RELOAD !!
            sendCurTrajectory();

        return bResult;
    }

    void SequenceEditorTrajectory::onStartRotation()
    {
        Actor* pAct = getCurrentTrajectoryActor();
        if (!pAct || getState() != ActorSnapShot)
        {
            applyRightClick();
            return;
        }
        m_trajActor.m_rotAtLastRightClick = pAct->getAngle();
    }

    void SequenceEditorTrajectory::onEndRotation()
    {
        Actor* pAct = getCurrentTrajectoryActor();
        if (!pAct || getState() != ActorSnapShot)
            return;

        if (fabs(m_trajActor.m_rotAtLastRightClick - pAct->getAngle()) < MTH_EPSILON)
            applyRightClick();
    }

    void SequenceEditorTrajectory::onMouseWheel   (   i32 _wheelValue, i32 _wheelDelta)
    {
        if (m_curTrajectoryIndex == U32_INVALID)
            return;

        u32 nbSelection = EDITOR->getSelectionSize();
        if (nbSelection && (INPUT_ADAPTER->isKeyPressed(KEY_RCTRL) || INPUT_ADAPTER->isKeyPressed(KEY_LCTRL)))
        {
            PlayTrajectory_evt*         evt         = getCurrentEvent();
            PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

            ITF_ASSERT(evt && evtTemplate);
            if (!evt || !evtTemplate)
                return;

            Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();
            if (!trajTemplate)
                return;

            bbool somethingDone = bfalse;
            for (u32 i=0; i<nbSelection; i++)
            {
                PickingShape* shape = EDITOR->getSelectionAt(i);
                if (shape && shape->getOwnerPtr() && shape->getOwnerPtr()->getObjectType() == BaseObject::eTrajectory)
                {
                    TrajectoryExtraInfo  trajExtraInfo = (TrajectoryExtraInfo)(shape->getData());
                    const u32 mode     = trajExtraInfo.m_type;
                    const u32 index    = trajExtraInfo.m_index;

                    if (mode == TYPE_SHAPE_POINT)
                    {
                        trajTemplate->setPosTimeAt(trajTemplate->getPosTimeAt(index) + _wheelDelta, index);
                        somethingDone = btrue;
                    }
                }
            }
            if (somethingDone)
                sendCurTrajectory(btrue);
            return;
        }

        if (getState() == CamSnapShot)
        {
            // Only process camera, actor are processed by onObjectChanged
            onKeyDownCamSnapShot();
            return;
        }
    }

    void SequenceEditorTrajectory::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if (_but == InputAdapter::MB_Left && _status == InputAdapter::Pressed)
        {
            m_mbLeftIsPressed = btrue;
            switch (getState())
            {
                case AddPoints:
                if (m_insertPointOk)
                {
                    Trajectory*             traj         = getCurrentTrajectory();
                    Trajectory_Template*    trajTemplate = getCurrentTrajectoryTemplate();
                    if (trajTemplate)
                    {
                        Vec3d pos;
                        trajTemplate->computeGlobalPosAtTime(m_addPointFrame, pos, traj->getPos());
                        trajTemplate->addGlobalPosData(pos, traj->getPos(), m_owner->getDefaultSplineMode(), m_addPointFrame);
                        trajTemplate->sortPosByTime();

                        sendCurTrajectory();
                    }
                }
                else
                {
                    Trajectory*         traj            = getCurrentTrajectory();
                    Trajectory_Template*trajTemplate    = getCurrentTrajectoryTemplate();
                    if (traj && traj->getBindedEvent())
                    {
                        const Actor* binded = traj->getBindedEvent()->getBindedActor();
                        if (binded)
                        {
                            Vec3d pos;
                            f32 time = 0.f;
                            if (trajTemplate->getPosCount() > 0)
                                time = trajTemplate->getPosTimeAt(trajTemplate->getPosCount()-1) + 30.f;

                            // update camera.
                            CAMERA->apply();

                            Vec3d v2d;
                            GFX_ADAPTER->compute3DTo2D(binded->getPos(), v2d);
                            v2d.m_x = EDITOR->getMouse2d().m_x;
                            v2d.m_y = EDITOR->getMouse2d().m_y;
                            GFX_ADAPTER->compute2DTo3D(v2d, pos);
                            pos.m_z = binded->getDepth();
     
                            trajTemplate->addGlobalPosData(pos, traj->getPos(), m_owner->getDefaultSplineMode(), time);
                            f32 lastTime = trajTemplate->getPosTimeAt(trajTemplate->getPosCount()-1);
                            SequenceEvent_Template* evt = trajTemplate->getBindedEventTemplate();
                            if(evt) 
                                evt->setDuration((i32)lastTime);

                            // TODO reload
                            sendCurTrajectory();
                        }
                    }
                }
                break;
            default:
                break;
            }
            
        }
        else if (_but == InputAdapter::MB_Left && _status == InputAdapter::Double_Press)
        {
            m_NoPickingTimer = 0.15f;
            m_LMBclickedTimer = 0.f;
            Trajectory* trj = getCurrentTrajectory();
            if(trj)
                trj->setSelectionIsLocked(btrue);
        }
        //else if ((m_LMBclickedTimer > 0.15f) && m_currentSelectionIsLocked /*&& m_NoPickingTimer <= 0.0f*/ && getCurrentSelectedPointIndex() == -1)
        else if (m_currentSelectionIsLocked && getCurrentSelectedPointIndex() == -1)
        {
            m_currentSelectionIsLocked = false;
        }
        else if (_but == InputAdapter::MB_Right && _status == InputAdapter::Pressed)
        {
            if (getState() != ActorSnapShot) // is case of snapshot, right click is handled by onStartRotation()
                applyRightClick();
        }
        else if (_status == InputAdapter::Released)
        {
            m_LMBclickedTimer = 0.f;
            m_canDrag = bfalse;
            m_mbLeftIsPressed = bfalse;

            if (m_curTrajectoryIndex != U32_INVALID && getState() == CamSnapShot)
            {
                // Only process camera, actor are processed by onObjectChanged
                onKeyDownCamSnapShot();
            }
        }
    }

    void SequenceEditorTrajectory::applyRightClick()
    {
        //if (!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY))
        //    SET_TRAJ_STATE(Idle);

        //m_duplicationJustoccured = bfalse;
        //if (m_currentSelectionIsLocked)
        //    m_currentSelectionIsLocked = bfalse;
        //else
        //{
        //    SET_CUR_TRAJ(-1);
        //}
    }

#ifdef DEBUG_SEQUENCES
    void SequenceEditorTrajectory::setCurrentTrajectory(int _index, const char* _file, int _line)
#else
    void SequenceEditorTrajectory::setCurrentTrajectory(int _index)
#endif
    {
        if (_index == int(m_curTrajectoryIndex))
            return;

        Trajectory *traj = getCurrentTrajectory();
        if (traj)
        {
            traj->setSelectionIsLocked(bfalse);
            traj->setSelected(NULL);
        }
        if (_index >= 0)
        {
            if(_index != int(m_curTrajectoryIndex))
            {
                m_curTrajectoryIndex = _index;
                traj = getCurrentTrajectory();

                if (traj)
                {
                    refreshPicking(traj);
                    traj->setSelectionIsLocked(bfalse);
                    PLUGINGATEWAY->setExclusiveFocus(m_owner);
                    return;
                }
            }
        }
        else
        {
            if (traj)
            {
                const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(traj->getRef());
                if (shapes)
                {
                    EDITOR->deletePickingShapesForObject(traj->getRef());
                }
            }
        }

        m_curTrajectoryIndex = U32_INVALID;
        m_currentSelectionIsLocked = bfalse;
        m_currentPosCount = 0;
        if (PLUGINGATEWAY->hasExclusiveFocus(m_owner))
            PLUGINGATEWAY->setExclusiveFocus(NULL);
        
    }
    
    void SequenceEditorTrajectory::refreshPicking(Trajectory* _t)
    {
        PlayTrajectory_evt* playTrj = _t->getBindedEvent();
        if(!playTrj)
            return;

        Actor* act = playTrj->getBindedActor();
        if(!act && !playTrj->getTemplate()->isBindedToCamera())
            return;

        _t->fillPickingInfo(&m_currentPickingInfo);
        _t->setPickingInfo(&m_currentPickingInfo);
        createPickingShapeForTrajPoints(_t->getRef());
        m_currentPosCount = _t->getTemplate()->getPosCount();

    }

    PlayTrajectory_evt* SequenceEditorTrajectory::getCurrentEvent()
    {
        if (m_curTrajectoryIndex == U32_INVALID)
            return NULL;

        SequencePlayerComponent *seqPlayer = m_owner->getCurrentSequenceComponent();
        if (!seqPlayer || m_curTrajectoryIndex >= (i32)seqPlayer->getEventsListCount() )
        {
            ITF_ASSERT(0);
            m_curTrajectoryIndex = U32_INVALID;
            return NULL;
        }

        SequenceEvent *evt = seqPlayer->getEventAtIndex(m_curTrajectoryIndex);
        if (evt->getTemplate()->getType() != SequenceEvent_Template::event_PlayTrajectory)
        {
            ITF_ASSERT(0);
            m_curTrajectoryIndex = U32_INVALID;
            return NULL;
        }

        return (PlayTrajectory_evt *)evt;
    }

    Trajectory* SequenceEditorTrajectory::getCurrentTrajectory()
    {
        PlayTrajectory_evt *evt = getCurrentEvent();
        if (!evt)
            return NULL;
        return evt->getTrajectory();
    }

    PlayTrajectory_evtTemplate * SequenceEditorTrajectory::getCurrentEventTemplate()
    {
        if (m_curTrajectoryIndex == U32_INVALID)
            return NULL;

        SequencePlayerComponent_Template *seqPlayerTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayerTemplate || m_curTrajectoryIndex >= (i32)seqPlayerTemplate->getEventsCount() )
        {
            ITF_ASSERT(0);
            m_curTrajectoryIndex = U32_INVALID;
            return NULL;
        }

        SequenceEvent_Template *evt = seqPlayerTemplate->getEventAtIndex(m_curTrajectoryIndex);
        if (evt->getType() != SequenceEvent_Template::event_PlayTrajectory)
        {
            ITF_ASSERT(0);
            m_curTrajectoryIndex = U32_INVALID;
            return NULL;
        }
        return (PlayTrajectory_evtTemplate *)evt;
    }

    Trajectory_Template* SequenceEditorTrajectory::getCurrentTrajectoryTemplate()
    {
        PlayTrajectory_evtTemplate *evt = getCurrentEventTemplate();
        if (!evt)
            return NULL;
        return evt->getTrajectoryTemplate();
    }

    void    SequenceEditorTrajectory::drawOneTrajectory(Trajectory* _t)
    {
        if (!_t)
            return;

        const Trajectory_Template *tTemplate = _t->getTemplate();
        if (tTemplate->getPosCount() == 0)
            return;

        float r = 1.f;
        float g = 1.f;
        float b = 1.f;
        bbool isCurrent = bfalse;
        i32 selectedPointIndex = -1;
        if (_t == getCurrentTrajectory())
        {
            isCurrent = btrue;
            const PickingInfo2D* pPickingInfo = _t->getPickingInfo();
            if(pPickingInfo)
            {
                selectedPointIndex = getCurrentSelectedPointIndex();//FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex
            }
            if (_t->isSelectionIsLocked())
            {
                r = 0.5f + ((f32_Cos(ELAPSEDTIME * 2.0f) + 1.f) * 0.25f);
                g = 0.5f + ((f32_Cos(ELAPSEDTIME * 1.5f) + 1.f) * 0.25f);
                b = 0.5f + ((f32_Cos(ELAPSEDTIME * 1.0f) + 1.f) * 0.25f);
            }
            else
            {
                r = 0.5f;
                g = 1.0f;
                b = 0.5f;
            }
        }


        bbool gfxDraw = true;
        for (u32 i = 0; i < tTemplate->getPosCount(); i++) 
        {
            if (tTemplate->getPosInterpAt(i) ==  Spline::interp_bezier || tTemplate->getPosInterpAt(i) ==  Spline::interp_constant ||
                tTemplate->getPosInterpAt(i) ==  Spline::interp_bezier_standard)
            {
                gfxDraw = false;
                break;
            }
        }

        if (gfxDraw) {
            _t->fillSplineInfo();
            Spline_Info *splineInfo = _t->getSplineInfo();
            splineInfo->m_texture   = EDITOR->getIconStock().getTrajectoryTexture();
            splineInfo->m_color     = Color(1.f, r, g, b);

            if (splineInfo->m_texture)
            {
                if (splineInfo->m_spline.GetNumPoints() >= 2)
                    GFX_ADAPTER->addPrimitiveInZList((void*)splineInfo, GFX_ITF_SPLINE, NULL, 0,0,ObjectRef::InvalidRef);
            }
        } else
        {
            for (u32 i = 1; i < tTemplate->getPosCount(); i++)
            {
                if (tTemplate->getPosInterpAt(i-1) == Spline::interp_linear)
                {
                    GFX_ADAPTER->drawDBG3DLine(tTemplate->getGlobalPosAt(i-1, _t->getPos()), tTemplate->getGlobalPosAt(i, _t->getPos()), r, g, b, 0, 4.f);
                }
                else
                {
                    f32 prevTime = tTemplate->getPosTimeAt(i-1);
                    f32 currTime = tTemplate->getPosTimeAt(i);
                    // Instead of drawing a single line, draw a line between intersections

                    u32     steps           = (u32)((currTime - prevTime));
                    f32     subdivisions    = (currTime - prevTime) / (f32)steps;
                    f32     curSubDiv       = prevTime + subdivisions;

                    Vec3d currentPoint, nextPoint;
                    currentPoint = tTemplate->getGlobalPosAt(i-1, _t->getPos());
                    f32 tr[2] = { r, 1.f - r };
                    f32 tg[2] = { g, 1.f - g };
                    f32 tb[2] = { b, 1.f - b };
                    for (u32 j=0; j<steps; j++, curSubDiv += subdivisions)
                    {
                        tTemplate->computeGlobalPosAtTime(curSubDiv, nextPoint, _t->getPos());
                        GFX_ADAPTER->drawDBG3DLine(currentPoint, nextPoint, tr[j%2], tg[j%2], tb[j%2], 0, 4.f);
                        currentPoint = nextPoint;
                    }
                }
            }
        }

        if(getState() == AddPoints)
        {
            if(m_insertPointOk)
            {
                Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_PolyLinePoint;
                u32 iconColor = Color(1.f, 1.f, 1.f, 1.f).getAsU32();
                f32 fIconSize = 24.f; //PickingShape::m_disc_picking_tolerance;                
                Vec3d posAdd;
                tTemplate->computeGlobalPosAtTime(getAddPointFrame(), posAdd, _t->getPos());
                EDITOR->getIconStock().drawIcon(icon, posAdd, fIconSize, 0.0f, bfalse, bfalse, iconColor);
            }
        }
    }
    
    void    SequenceEditorTrajectory::draw( )
    {
        if (CAMERA->getMode() == CAMERA->Camera_InGame)
            return;

        if (m_bShowAllTraj) // do show all trajectories
        {
            SequencePlayerComponent *seqPlayComp = m_owner->getCurrentSequenceComponent();
            if (seqPlayComp)
            {
                ObjectRefList objList;
                seqPlayComp->getTrajectoryList(objList);
                const u32 uCount = objList.size();
                for(u32 i = 0; i < uCount; ++i)
                {
                    const ObjectRef& oRef = objList[i];
                    drawOneTrajectory(static_cast<Trajectory*>(GETOBJECT(oRef)));
                }
            }
        }
        else
        {
            Trajectory *traj = getCurrentTrajectory();
            if (traj)
                drawOneTrajectory(traj);
        }
    }

    bbool SequenceEditorTrajectory::onPickShape(PickingShape* _shape)
    {
        BaseObject* pObj = _shape->getOwnerPtr();

        if (!pObj || pObj->getObjectType() !=  BaseObject::eTrajectory)
        {
            ITF_ASSERT(0);
            return bfalse;
        }

        Trajectory* pp = (Trajectory*)pObj;
        if (!pp)
        {
            SET_CUR_TRAJ(-1);
            return bfalse;
        }
        
        i32                      evtIndex = -1;
        SequencePlayerComponent *seq = m_owner->getCurrentSequenceComponent();
        SequenceEvent           *evt = pp->getBindedEvent();

        if (seq && evt && evt->getTemplate()->getType() == SequenceEvent_Template::event_PlayTrajectory)
        {
            evtIndex = seq->getEventIndex(evt);
        }

        if (evtIndex == -1)
        {
            SET_CUR_TRAJ(-1);
            return bfalse;
        }

        SET_CUR_TRAJ(evtIndex);
        ITF_Memcpy(&m_currentPickingInfo, _shape->getPickingInfoPtr(), sizeof(PickingInfo2D));
        m_canDrag = btrue;
        pp->setSelected(&m_currentPickingInfo);
        return btrue;
    }

    void SequenceEditorTrajectory::sendCurTrajectory(bbool _undoRedo, bbool _unselectShape)
    {
        Trajectory          * traj = getCurrentTrajectory();
        ITF_ASSERT(traj);
        if (!traj)
            return;

		createPickingShapeForTrajPoints(traj->getRef(), getCurrentTrajectoryTemplate(), _unselectShape);
        sendTrajectory(traj, getCurrentEventTemplate(), _undoRedo);
        
        Actor* actSequence = m_owner->getCurrentSequenceActor();
        if (!actSequence)
            return;

        SequencePlayerComponent *seqPlayComp = actSequence->GetComponent<SequencePlayerComponent>();
        if (!seqPlayComp)
            return;

        seqPlayComp->setCurrentFrame(seqPlayComp->getCurrentFrame(), bfalse);
        if (CHEATMANAGER->getPause() && actSequence->isActive())
        {
            actSequence->update(0);
            m_owner->updateSequenceActors(seqPlayComp);
        }
    }

    void SequenceEditorTrajectory::sendTrajectory(Trajectory* traj, PlayTrajectory_evtTemplate* evtTemplate, bbool _undoRedo)
    {
        ITF_ASSERT(traj);
        if (!traj) 
            return;
        
        PlayTrajectory_evt* evt = traj->getBindedEvent();
        ITF_ASSERT(evt);
        if (!evt) 
            return;

        Actor* actSequence = m_owner->getCurrentSequenceActor();
        ITF_ASSERT(actSequence);
        if (!actSequence)
            return;

        // When sending a trajectory also update other actor with same sequence component
        m_owner->propagateActorChanges(actSequence, _undoRedo, btrue);

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);


        // plugin ID
        blob.pushString(SequenceEditorPlugin::getPluginName());  
        // cmd ID
        blob.pushString(SequenceEditorMessages::updateEventCmdName());

        // Sequence ref
        blob.pushUInt32(actSequence->getRef().getValue());

        // event Ref
        blob.pushUInt32(evtTemplate->getUID());

        // serialize event
        evtTemplate->serializeToBlob(&blob);

        PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorTrajectory::onKey(   i32 _key, InputAdapter::PressStatus _status )
    {
        if (!INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            m_duplicationJustoccured = bfalse;

        PlayTrajectory_evt* evt = NULL;
        Trajectory* pTraj = getCurrentTrajectory();
        if (pTraj)
            evt = pTraj->getBindedEvent() ;

        if (_status == InputAdapter::Pressed || _status == InputAdapter::Double_Press)
        {
            switch(_key)
            { 
                case KEY_ESC:
                    SET_TRAJ_STATE(Idle);
                    //m_owner->stopAll();
                break;




                //case 'M':
                //{
                //    if (getState() == CamSnapShot)
                //    {
                //        i32 index = getCurrentSelectedPointIndex();
                //        if (index >=0)
                //        {
                //            Vec3d pos(CAMERA->getPos());
                //            f32 time = pTraj->getPosTimeAt(index);
                //            pTraj->setGlobalPosDataAt(pos, Spline::interp_spline, time, index);
                //            sendCurTrajectory();
                //        }
                //    }
                //}
                //break;

                //case 'P':
                //{
                //    if (pTraj)
                //    {
                //        if (getState() == ReplayCam)
                //            SET_TRAJ_STATE(ReplayCam2);
                //        else
                //            SET_TRAJ_STATE(ReplayCam);
                //    }
                //}
                //break;

                // go to next key
                case KEY_RIGHT:
                    if (evt && !INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // don't bother camera and editor::offsetSelection
                    {
                        if ( (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL)) || (INPUT_ADAPTER->isKeyPressed(KEY_RCTRL))) // go to next frame
                            goToNextKey(pTraj, evt);

                        Actor *actSequence = m_owner->getCurrentSequenceActor();
                        if (actSequence)
                            m_owner->sendCurrentFrame(actSequence->getRef(), evt->getOwnerSequence()->getCurrentFrame());

                    }
                    break;

                // go to preview key
                case KEY_LEFT:
                    if (evt && !INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // don't bother camera and editor::offsetSelection
                    {
                        if ( (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL)) || (INPUT_ADAPTER->isKeyPressed(KEY_RCTRL)) )// go to previous frame
                            goToPrevKey(pTraj, evt);

                        Actor *actSequence = m_owner->getCurrentSequenceActor();
                        if (actSequence)
                            m_owner->sendCurrentFrame(actSequence->getRef(), evt->getOwnerSequence()->getCurrentFrame());

                    }
                    break;

                // add/set a key for actor on current frame
                //case KEY_UP:
                //{
                //    if (pTraj && !INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY)) // don't bother camera
                //    {
                //        switch(getState())
                //        {
                //        case CamSnapShot:
                //            onKeyUpCamSnapShot();
                //            break;

                //        case ActorSnapShot:
                //            onKeyUpActorSnapShot();
                //            break;
                //        }
                //    }
                //}
                //break;

                // add/set a key for camera on current frame
                //case KEY_DOWN:
                //{
                //    if (pTraj && !INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY)) // don't bother camera
                //    {
                //        switch(getState())
                //        {
                //        case CamSnapShot:
                //            onKeyDownCamSnapShot(pTraj, evt);
                //            break;

                //        case ActorSnapShot:
                //            onKeyDownActorSnapShot(pTraj, evt);
                //            break;
                //        }
                //    }
                //}
                //break;

                //case 'F':
                //if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                //{
                //    i32 index = getCurrentSelectedPointIndex();
                //    if (index >= 0)
                //    {
                //        if (getState() == FollowCam)
                //        {
                //            SET_TRAJ_STATE(Idle);
                //        }
                //        else
                //        {
                //            SET_TRAJ_STATE(FollowCam);
                //            Vec3d pos = pTraj->getGlobalPosAt(index);
                //            m_deltaCam = pos - CAMERA->getPos();
                //        }
                //    }
                //}
                //break;
            }
        }

        if(INPUT_ADAPTER->isKeyPressed(KEY_INSERT))
        {
            if (getState() != AddPoints)
            {
                m_previousState = getState();
                SET_TRAJ_STATE(AddPoints);
            }
        }
        else if (getState() == AddPoints)
        {
            SET_TRAJ_STATE(m_previousState);
        }
    }

    bbool SequenceEditorTrajectory::bCanEnterEditMode ()
    {
        return (!m_currentSelectionIsLocked);
    }

    bbool SequenceEditorTrajectory::bCanLeaveEditMode ()
    {
        return (!getCurrentTrajectory());
    }


    void SequenceEditorTrajectory::updateFollowCam()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        if (m_currentSelectionIsLocked)
        {
            Trajectory          *traj           = getCurrentTrajectory();
            Trajectory_Template *trajTemplate   = getCurrentTrajectoryTemplate();
            if (traj && trajTemplate)
            {
                const PickingInfo2D* pPickingInfo = traj->getPickingInfo();
                if(pPickingInfo)
                {
                    u32 selectedPointIndex = TrajectoryExtraInfo(pPickingInfo->m_extraInformation).m_index;
                    Vec3d pos(CAMERA->getPos());                    
                    trajTemplate->setGlobalPosDataAt(pos + m_deltaCam, traj->getPos(), Spline::interp_linear, trajTemplate->getPosTimeAt(selectedPointIndex), selectedPointIndex);
                    
                    // TODO reload
                    sendCurTrajectory();
                }
            }
        }
    }

    
    void SequenceEditorTrajectory::updateActorSnapShot()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        i32 el = (i32)(ELAPSEDTIME  * 4.f);
        f32 color = (el & 1) ? 1.f : 0.f;

        Actor* act = getCurrentTrajectoryActor();
        //ITF_ASSERT(act);
        if (!act)
        {
            SET_TRAJ_STATE(Idle);
            return;
        }
        // Show Text even on F6
        u32 saveGFXHide = GFX_ADAPTER->getHideFlags();
        GFX_ADAPTER->setHideFlags(0);
        GFX_ADAPTER->drawDBGText("Actor Capture Mode", (f32)(GFX_ADAPTER->getScreenWidth() - 200.f), (f32)(GFX_ADAPTER->getScreenHeight()-64.f), color, color, color);
        GFX_ADAPTER->setHideFlags(saveGFXHide);
        EDITOR->ignoreStorePickableInitData(act);

        GFX_ADAPTER->drawDBGCircle(act->getPos().m_x, act->getPos().m_y, 1.f + 0.3f * f32_Cos(ELAPSEDTIME*4.f), 0, 1, 0, LOGICDT*2, act->getDepth());
    }

    void SequenceEditorTrajectory::updateCamSnapShot()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        if (!m_owner->getCameraModeSequence())
        {
            // cannot update camera on editor view !
            m_owner->setAutoKeyMode(bfalse); 
            m_owner->getSequenceEditorMessages().sendRecordModeState();
            return;
        }

        i32 el = (i32)(ELAPSEDTIME  * 4.f);
        f32 color = (el & 1) ? 1.f : 0.f;
        // Show Text even on F6
        u32 saveGFXHide = GFX_ADAPTER->getHideFlags();
        GFX_ADAPTER->setHideFlags(0);
        GFX_ADAPTER->drawDBGText("Cam Capture Mode", (f32)(GFX_ADAPTER->getScreenWidth() - 200.f), (f32)(GFX_ADAPTER->getScreenHeight()-64.f), color, color, color);
        GFX_ADAPTER->setHideFlags(saveGFXHide);
    }


    void SequenceEditorTrajectory::updateReplay()
    {
        Trajectory*          pTraj          = getCurrentTrajectory();
        Trajectory_Template* pTrajTemplate  = getCurrentTrajectoryTemplate();
        if (pTrajTemplate && pTrajTemplate->getPosCount() > 1)
        {
            Vec3d pos;
            pTrajTemplate->computeGlobalPosAtTime(ELAPSEDTIME, pos, pTraj->getPos());
            if (getState() == ReplayCam2)
            {
                EDITOR->getIconStock().drawIcon2D(Editor_IconStock::Icon_Trajectory_Camera, pos, 40, 40, MTH_PI);
            }
            else
            {
                CAMERA->cancelBiasMode();
                CAMERA->forcePosition(pos);
            }
        }
    }

    i32 SequenceEditorTrajectory::getCurrentSelectedPointIndex()
    {
        Trajectory*             pTraj = getCurrentTrajectory();
        Trajectory_Template*    pTrajTemplate = getCurrentTrajectoryTemplate();
        if (pTraj && pTrajTemplate->getPosCount())
        {
            const PickingInfo2D* pPickingInfo = pTraj->getPickingInfo();
            if(pPickingInfo && pPickingInfo->m_targetRef.isValid())
                return (i32)TrajectoryExtraInfo(pPickingInfo->m_extraInformation).m_index;
            else
                return -1;
        }
        return -1;
    }

    void SequenceEditorTrajectory::setCurrentSelectedPointIndex(i32 _index)
    {
        if (_index<0)
            return;

        Trajectory* pTraj = getCurrentTrajectory();
        if (pTraj)
        {
            PickingInfo2D* pPickingInfo = pTraj->getPickingInfo();
            if(pPickingInfo)
            {
                TrajectoryExtraInfo trajInfo;
                trajInfo.m_index = _index;
                trajInfo.m_type  = TYPE_SHAPE_POINT;
                pPickingInfo->m_extraInformation  = trajInfo.m_raw;
            }
        }
    }


    void SequenceEditorTrajectory::selectTrajectory(int _index)
    {
        m_bUnselectAllAuthorized  = bfalse;
        SET_CUR_TRAJ(_index);

        SequencePlayerComponent          *seqPlayer             = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayer_Template    = m_owner->getCurrentSequenceComponentTemplate();
        
        if (!seqPlayer || !seqPlayer_Template)
            return;

        if (m_curTrajectoryIndex == U32_INVALID || m_curTrajectoryIndex >= seqPlayer->getEventsListCount())
            return;

        SequenceEvent           *baseEvt            = seqPlayer->getEventAtIndex(m_curTrajectoryIndex);
        SequenceEvent_Template  *baseEvtTemplate    = seqPlayer_Template->getEventAtIndex(m_curTrajectoryIndex);
        if (!baseEvt || !baseEvtTemplate ||
            baseEvtTemplate->getType() != SequenceEvent_Template::event_PlayTrajectory)
            return;

        PlayTrajectory_evt *        evt             = (PlayTrajectory_evt *)baseEvt;
        PlayTrajectory_evtTemplate *evtTemplate     = (PlayTrajectory_evtTemplate *)baseEvtTemplate;


        DataModifier keyType  = (DataModifier)(evtTemplate->getSelected() % 16);
        int          keyFrame = evtTemplate->getSelected() / 16;

        Trajectory*         traj         = evt->getTrajectory();
        Trajectory_Template*trajTemplate = evtTemplate->getTrajectoryTemplate();

        int keyIndex = -1;
        if (keyType == translation)
        {
            PLUGINGATEWAY->setExclusiveFocus(m_owner);

            for (u32 i=0; i<trajTemplate->getPosCount(); i++)
            {
                if ((i32)trajTemplate->getPosTimeAt(i) == keyFrame)
                {
                    keyIndex = i;
                    break;
                }
            }

            m_currentPickingInfo.m_anchor = Vec2d::Zero;
            m_currentPickingInfo.m_angle = 0.f;
            m_currentPickingInfo.m_targetRef = traj->getRef();
            m_currentPickingInfo.m_type = BaseObject::eTrajectory;
            m_currentPickingInfo.m_sqrDist2D = 0.f;
            m_currentPickingInfo.m_zDist = 0.f;
            m_currentPickingInfo.m_targetRadius = 0.1f;

            if (keyIndex >= 0)
            {
                TrajectoryExtraInfo trajInfo;
                trajInfo.m_index = _index;
                trajInfo.m_type  = TYPE_SHAPE_POINT;
                m_currentPickingInfo.m_extraInformation = trajInfo.m_raw;
                traj->setSelected(&m_currentPickingInfo);
            } else
            {
                m_currentPickingInfo.m_extraInformation = U32_INVALID;
                traj->setSelected(NULL);
            }
        }

        if(evt->getActive())
        {
            if (evt->getBindedActor())
            {
                // selected the actor
                if (keyIndex == -1)
                {
                    const ObjectRef & objRef = evt->getBindedActor()->getRef();
                    if (!EDITOR->getMainPickingShapeForObject(objRef))
                    {
                        ActorEditor::createPickingShapeForActor (objRef);
                    }
                    EDITOR->selectObject(objRef, btrue);
                }
                SET_TRAJ_STATE(ActorSnapShot);
            }
            else if(evtTemplate->isBindedToCamera())
            {
                SET_TRAJ_STATE(CamSnapShot);
            } else
            {
                SET_TRAJ_STATE(Idle);
            }
        } else
        {
            SET_TRAJ_STATE(Idle);
        }
        m_bUnselectAllAuthorized  = btrue;
    }

#ifdef DEBUG_SEQUENCES
    void SequenceEditorTrajectory::setState(state _state, const char* _file, int _line)
    {
        if (_state == m_trajState)
            return;

        if (m_owner->m_log)
        {
            switch (_state)
            {
            case Idle:
                fputs("\nTrajectory new state: - Idle -", m_owner->m_log);
                break;
            case AddPoints:
                fputs("\nTrajectory new state: - AddPoints -", m_owner->m_log);
                break;
            case FollowCam:
                fputs("\nTrajectory new state: - FollowCam -", m_owner->m_log);
                break;
            case CamSnapShot:
                fputs("\n new state: - CamSnapShot -", m_owner->m_log);
                break;
            case ActorSnapShot:
                fputs("\n new state: - ActorSnapShot -", m_owner->m_log);
                break;
            case ReplayCam:
                fputs("\nTrajectory new state: - ReplayCam -", m_owner->m_log);
                break;
            case ReplayCam2:
                fputs("\nTrajectory new state: - ReplayCam2 -", m_owner->m_log);
                break;
            default:
                ITF_ASSERT(0); // unhandled case
                break;
            }
            fprintf(m_owner->m_log, "file: %s\nline: %d\n", _file, _line);
        }

        if (m_trajState == ActorSnapShot)
            stopActorSnapShot();

        if (_state == ActorSnapShot)
            startActorSnapShot();

        m_trajState = _state;
    }
#else
    void    SequenceEditorTrajectory::setState(state _state) 
    {
        if (_state == m_trajState)
            return;

        if (m_trajState == ActorSnapShot)
            stopActorSnapShot();

        if (_state == ActorSnapShot)
            startActorSnapShot();

        m_trajState = _state;
    }
#endif

    SequenceEditorTrajectory::state   SequenceEditorTrajectory::getState() 
    {
        return m_trajState;
    }

    bbool   SequenceEditorTrajectory::bIsEditingActorTrajectory(const ObjectRef _actor)
    {
        const Actor* pAct = getCurrentTrajectoryActor();
        if (!pAct)
            return bfalse;
        return pAct->getRef() == _actor;        
    }


    void    SequenceEditorTrajectory::onKeyDownActorSnapShot()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();

        i32 frame = evt->getOwnerSequence()->getCurrentFrame() - evtTemplate->getStartFrame();

        const Actor* pAct = evt->getBindedActor();
        if (!pAct)
        {
            ITF_ERROR("Ouais l� ya (encore) un bug pas encore fix�.\nJ'ai bien compris que tu voulais �diter une trajectoire sur un acteur, mais j'ai paum� l'acteur.\nReclick sur l'event que tu veux modifier dans le SequenceEditor.\nD�sol� hein, on essaie de s'am�liorer.\nEt comme c'est bien pourri, t'auras pt�t ce message plusieurs fois de suite.");
            return;
        }
        Vec3d pos = pAct->getPos();
        Vec3d rot = Vec3d(0.f,0.f,pAct->getAngle());
        Vec3d scale = pAct->getScale().to3d();

        Vec3d posInTrajectory;
        Vec3d rotInTrajectory;
        Vec3d scaleInTrajectory;

        
        trajTemplate->computeGlobalPosAtTime((f32)frame, posInTrajectory, traj->getPos());
        trajTemplate->computeRotAtTime      ((f32)frame, rotInTrajectory);
        trajTemplate->computeScaleAtTime    ((f32)frame, scaleInTrajectory);
        
        bbool   needSend = bfalse;
        bbool   keyFound;
        u32     count;

        // Translation
        if ((posInTrajectory - pos).sqrnorm() > MTH_EPSILON) // values modified?
        {
            keyFound = bfalse;
            count = trajTemplate->getPosCount();
            // find if a key exist at current frame
            for (u32 i = 0; i < count; i++)
            {
                i32 frameTraj = (i32)trajTemplate->getPosTimeAt(i);
                if(frameTraj == frame)
                {
                    trajTemplate->setGlobalPosDataAt(pos, traj->getPos(), trajTemplate->getPosInterpAt(i), trajTemplate->getPosTimeAt(i), i, btrue);
                    keyFound = btrue;
                    break;
                }
            }
            // not found key at current frame then add a new key
           if(!keyFound)
            {
                trajTemplate->addGlobalPosData(pos, traj->getPos(), m_owner->getDefaultSplineMode(), (f32)frame, btrue);
                trajTemplate->sortPosByTime();
            }
            needSend = btrue;
        }


        // Rotation
        if ((rotInTrajectory - rot).sqrnorm() > MTH_EPSILON) // values modified?
        {
            keyFound = bfalse;
            count = trajTemplate->getRotCount();
            // find if a key exist at current frame
           for (u32 i = 0; i < count; i++)
            {

                i32 frameTraj = (i32)trajTemplate->getRotTimeAt(i);
                if(frameTraj == frame)
                {
                    trajTemplate->setRotDataAt(rot, trajTemplate->getRotInterpAt(i), trajTemplate->getRotTimeAt(i), i, btrue);
                    keyFound = btrue;
                    break;
                }
            }
           // not found key at current frame then add a new key
            if(!keyFound)
            {
                trajTemplate->addRotData(rot, m_owner->getDefaultSplineMode(), (f32)frame, btrue);
                trajTemplate->sortRotByTime();
            }
            needSend = btrue;
        }

        // Scale
        if ((scaleInTrajectory - scale).sqrnorm() > MTH_EPSILON) // values modified?
        {
            keyFound = bfalse;
            count = trajTemplate->getScaleCount();
            // find if a key exist at current frame
            for (u32 i = 0; i < count; i++)
            {
                i32 frameTraj = (i32)trajTemplate->getScaleTimeAt(i);
                if(frameTraj == frame)
                {
                    trajTemplate->setScaleDataAt(scale, trajTemplate->getScaleInterpAt(i), trajTemplate->getScaleTimeAt(i), i, btrue);
                    keyFound = btrue;
                    break;
                }
            }
            // not found key at current frame then add a new key
            if(!keyFound)
            {
                trajTemplate->addScaleData(scale, m_owner->getDefaultSplineMode(), (f32)frame, btrue);
                trajTemplate->sortScaleByTime();
            }
            needSend = btrue;
        }

        if (needSend)
        {
            // TODO reload
            sendCurTrajectory(btrue, btrue);
        }
    }

    void    SequenceEditorTrajectory::onKeyDownCamSnapShot()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();


        Vec3d pos(CAMERA->getPos());
        i32 frame = (evt->getOwnerSequence()->getCurrentFrame() - evtTemplate->getStartFrame());

        Vec3d posInTrajectory;
        trajTemplate->computeGlobalPosAtTime((f32)frame, posInTrajectory, traj->getPos());

        // Translation
        if ((posInTrajectory - pos).sqrnorm() > MTH_EPSILON)
        {
            bbool keyFound = bfalse;
            u32 count = trajTemplate->getPosCount();
            for (u32 i = 0; i < count; i++)
            {
                if((i32)trajTemplate->getPosTimeAt(i) == frame)
                {
                    trajTemplate->setGlobalPosDataAt(pos, traj->getPos(), trajTemplate->getPosInterpAt(i), (f32)frame, i, btrue);
                    keyFound = btrue;
                    break;
                }
            }        
            if(!keyFound)
            {
                trajTemplate->addGlobalPosData(pos, traj->getPos(), m_owner->getDefaultSplineMode(), (f32)frame, btrue);
                trajTemplate->sortPosByTime();
            }
            
            sendCurTrajectory();
        }
    }

    void    SequenceEditorTrajectory::onKeyUpCamSnapShot()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();

        Vec3d pos(CAMERA->getPos());
        i32 index = getCurrentSelectedPointIndex();
        ITF_ASSERT(index < (i32)trajTemplate->getPosCount());
        if (index >=0 && index < (i32)trajTemplate->getPosCount())
        {
            trajTemplate->setGlobalPosDataAt(pos, traj->getPos(), m_owner->getDefaultSplineMode(), trajTemplate->getPosTimeAt(index), index, btrue); 
        }
        else
        {
            i32 frame = evt->getOwnerSequence()->getCurrentFrame() - evtTemplate->getStartFrame();
            trajTemplate->addGlobalPosData(pos, traj->getPos(), m_owner->getDefaultSplineMode(), (f32)frame, btrue);
        }
        sendCurTrajectory();
    }

    void    SequenceEditorTrajectory::onKeyUpActorSnapShot()
    {
        if(!m_owner->isAutoKeyAllowed())
            return;

        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();        
        
        const Actor* pAct = evt->getBindedActor();
        if (!pAct)
        {
            ITF_ERROR("Ouais l� ya (encore) un bug pas encore fix�.\nJ'ai bien compris que tu voulais �diter une trajectoire sur un acteur, mais j'ai paum� l'acteur.\nReclick sur l'event que tu veux modifier dans le SequenceEditor.\nD�sol� hein, on essaie de s'am�liorer.\nEt comme c'est bien pourri, t'auras pt�t ce message plusieurs fois de suite.");
            return;
        }
        Vec3d pos = pAct->getPos();
        i32 index = getCurrentSelectedPointIndex();
        ITF_ASSERT(index < (i32)trajTemplate->getPosCount());
        if (index >=0 && index < (i32)trajTemplate->getPosCount())
        {
            trajTemplate->setGlobalPosDataAt(pos, traj->getPos(), m_owner->getDefaultSplineMode(), trajTemplate->getPosTimeAt(index), index, btrue); 
        }
        else
        {
            i32 frame = evt->getOwnerSequence()->getCurrentFrame() - evtTemplate->getStartFrame();
            trajTemplate->addGlobalPosData(pos, traj->getPos(), m_owner->getDefaultSplineMode(), (f32)frame, btrue);
        }
        // TODO Reload
        sendCurTrajectory();
    }

    void    SequenceEditorTrajectory::startActorSnapShot()
    {
#ifdef DEBUG_SEQUENCES
        if (m_owner->m_log)
            fputs("startActorSnapShot()\n", m_owner->m_log);
#endif
        Actor* pAct = getCurrentTrajectoryActor();
        if (!pAct)
            return;

        //ITF_ASSERT(m_trajActor.m_objRef == pAct->getRef() || m_trajActor.m_objRef == ITF_INVALID_OBJREF);
        m_trajActor.m_objRef = pAct->getRef();
        
        //EventSequenceActorActivate sequenceEventActivate(ITF_INVALID_OBJREF, btrue);
        //pAct->onEvent(&sequenceEventActivate);

        m_trajActor.m_savedPos = pAct->getPos();
        m_trajActor.m_savedRot = pAct->getAngle();
        m_trajActor.m_savedScale = pAct->getScale();

        EDITOR->ignoreStorePickableInitData(pAct);
    }



    bbool SequenceEditorTrajectory::isSubEditAllowed()
    {
        if (getState() == ActorSnapShot)
            return bfalse;

        return btrue;
    }

    void    SequenceEditorTrajectory::stopActorSnapShot()
    {
#ifdef DEBUG_SEQUENCES
        if (m_owner->m_log)
            fputs("stopActorSnapShot()\n", m_owner->m_log);
#endif

        Actor* pAct = getCurrentTrajectoryActor();
        if (!pAct)
            return;

        m_trajActor.m_objRef = ITF_INVALID_OBJREF;

        //pAct->setPos(m_trajActor.m_savedPos);
        //pAct->setAngle(m_trajActor.m_savedRot);
        //pAct->setScale(m_trajActor.m_savedScale);

        EDITOR->allowStorePickableInitData(pAct);
    }

    Actor*  SequenceEditorTrajectory::getCurrentTrajectoryActor()
    {
        PlayTrajectory_evt* evt = NULL;
        Trajectory* pTraj = getCurrentTrajectory();
        if (pTraj)
            evt = pTraj->getBindedEvent() ;
        //ITF_ASSERT (evt);
        if (!evt)
            return NULL;
        Actor* pAct = evt->getBindedActor();
        //ITF_ASSERT (pAct);
        return pAct;
    }

    void SequenceEditorTrajectory::onSceneSave()
    {
        if (m_trajActor.m_objRef != ITF_INVALID_OBJREF)
            stopActorSnapShot();
        m_owner->UnselectAll(bfalse);
    }

    void SequenceEditorTrajectory::goToNextKey(Trajectory* _pTraj, PlayTrajectory_evt* _evt)
    {
        reachNearestNextKeyFromCurrentFrame();
    }

    void SequenceEditorTrajectory::goToPrevKey(Trajectory* _pTraj, PlayTrajectory_evt* _evt)
    {
        reachNearestPrevKeyFromCurrentFrame();
    }
    
    void SequenceEditorTrajectory::goToKey(Trajectory* _pTraj, PlayTrajectory_evt* _evt, int _index)
    {
        const Trajectory_Template *trajTemplate = _pTraj->getTemplate();

        if (_index<0) 
            _index = 0;
        if (_index >= (i32)trajTemplate->getPosCount())
            _index = trajTemplate->getPosCount()-1;

        i32 trajFrame = (i32)trajTemplate->getPosTimeAt(_index);
        _evt->getOwnerSequence()->setCurrentFrame(_evt->getTemplate()->getStartFrame() + trajFrame, btrue);
        setCurrentSelectedPointIndex(_index);

        // use for sending current time to SequenceEditor
        m_owner->frameStep(0);
    }

    void SequenceEditorTrajectory::reachNearestNextKeyFromCurrentFrame()
    {
        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        if (!evt || !evtTemplate)
            return;

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();    

        if (!traj || !trajTemplate)
            return;


        i32 transIndex, rotIndex, scaleIndex;
        i32 transFrame = 0, rotFrame = 0, scaleFrame = 0;
        bbool useTrans = bfalse;
        bbool useRot   = bfalse;
        bbool useScale = bfalse;

        // current frame
        i32 frame = evt->getOwnerSequence()->getCurrentFrame();
        frame -= evtTemplate->getStartFrame();

        // translation keys
        for (transIndex = 0; transIndex < (i32)trajTemplate->getPosCount(); transIndex++)
        {
            i32 keyFrame = (i32)trajTemplate->getPosTimeAt(transIndex);

            // distance between current frame and key of trajectory
            i32 transDist = keyFrame - frame;
            if (transDist > 0)
            {
                useTrans = btrue;
                transFrame = keyFrame;
                break;
            }
        }

        // rotation keys
        for (rotIndex = 0; rotIndex < (i32)trajTemplate->getRotCount(); rotIndex++)
        {
            i32 keyFrame = (i32)trajTemplate->getRotTimeAt(rotIndex);

            // distance between current frame and key of trajectory
            i32 rotDist = keyFrame - frame;
            if (rotDist > 0)
            {
                useRot = btrue;
                rotFrame = keyFrame;
                break;
            }
        }

        // scale keys
        for (scaleIndex = 0; scaleIndex < (i32)trajTemplate->getScaleCount(); scaleIndex++)
        {
            i32 keyFrame = (i32)trajTemplate->getScaleTimeAt(scaleIndex);

            // distance between current frame and key of trajectory
            i32 scaleDist = keyFrame - frame;
            if (scaleDist > 0)
            {
                useScale = btrue;
                scaleFrame = keyFrame;
                break;
            }
        }
        
        i32 i32Max = 999999999;

        i32 moveFrame = i32Max; 
        if (useTrans &&  transFrame < moveFrame)   moveFrame = transFrame;
        if (useRot   &&  rotFrame   < moveFrame)   moveFrame = rotFrame;
        if (useScale &&  scaleFrame < moveFrame)   moveFrame = scaleFrame;

        if (moveFrame < i32Max)
            evt->getOwnerSequence()->setCurrentFrame(evtTemplate->getStartFrame() + moveFrame, btrue);

        // use for sending current time to SequenceEditor
        m_owner->frameStep(0);
    }

    void SequenceEditorTrajectory::reachNearestPrevKeyFromCurrentFrame()
    {
        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        if (!evt || !evtTemplate)
            return;

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();    

        if (!traj || !trajTemplate)
            return;

        i32 transIndex, rotIndex, scaleIndex;
        i32 transFrame = 0, rotFrame = 0, scaleFrame = 0;
        bbool useTrans = bfalse;
        bbool useRot = bfalse;
        bbool useScale = bfalse;

        // current frame
        i32 frame = evt->getOwnerSequence()->getCurrentFrame();
        frame -= evtTemplate->getStartFrame();

        // translation keys
        for (transIndex = 0; transIndex < (i32)trajTemplate->getPosCount(); transIndex++)
        {
            i32 keyFrame = (i32)trajTemplate->getPosTimeAt(transIndex);

            // distance between current frame and key of trajectory
            i32 transDist = keyFrame - frame;
            if (transDist >= 0)
                break;
            useTrans = btrue;
            transFrame = keyFrame;
        }

        // rotation keys
        for (rotIndex = 0; rotIndex < (i32)trajTemplate->getRotCount() ; rotIndex++)
        {
            i32 keyFrame = (i32)trajTemplate->getRotTimeAt(rotIndex);

            // distance between current frame and key of trajectory
            i32 rotDist = keyFrame - frame;
            if (rotDist >= 0)
                break;
            useRot = btrue;
            rotFrame = keyFrame;
        }

        // scale keys
        for (scaleIndex = 0; scaleIndex < (i32)trajTemplate->getScaleCount() ; scaleIndex++)
        {
            i32 keyFrame = (i32)trajTemplate->getScaleTimeAt(scaleIndex);

            // distance between current frame and key of trajectory
            i32 scaleDist = keyFrame - frame;
            if (scaleDist >= 0)
                break;
            useScale = btrue;
            scaleFrame = keyFrame;
        }

        i32 i32Min = -999999999;

        i32 moveFrame = i32Min;
        if (useTrans && transFrame > moveFrame)   moveFrame = transFrame;
        if (useRot   && rotFrame   > moveFrame)   moveFrame = rotFrame;
        if (useScale && scaleFrame > moveFrame)   moveFrame = scaleFrame;

        if (moveFrame > i32Min)
            evt->getOwnerSequence()->setCurrentFrame(evtTemplate->getStartFrame() + moveFrame, btrue);

        // use for sending current time to SequenceEditor
        m_owner->frameStep(0);
    }

    PickingShape_Disc* SequenceEditorTrajectory::addPickingPoint(const ObjectRef _curTrajectoryRef, u32 _index, u32 _mode)
    {
        TrajectoryExtraInfo extraInfo;
        PickingShape_Disc*  pick        = newAlloc(mId_Editor,PickingShape_Disc(NULL));
        extraInfo.m_type    = _mode;
        extraInfo.m_index   = _index;
        pick->setData(extraInfo.m_raw); // push the point index            
        
        EDITOR->addPickingShapeForObject(_curTrajectoryRef, pick);
        pick->setSubAnchor(btrue);
        pick->setPriority(5 - _mode);
        return pick;
    }


    void SequenceEditorTrajectory::createPickingShapeForTrajPoints (const ObjectRef _curTrajectoryRef, const Trajectory_Template *trajTemplate, bbool _unselectShape)
    {
        Trajectory* curTrajectory = static_cast<Trajectory*>(GETOBJECT(_curTrajectoryRef));
        ITF_ASSERT(curTrajectory);
        if(!curTrajectory)
            return;

        if (trajTemplate == NULL)
            trajTemplate = curTrajectory->getTemplate();

        const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_curTrajectoryRef);
        const SafeArray<PickingShape*>& selection = EDITOR->getSelectionList();

        SafeArray<u32> selectedIndexList;
        if (!_unselectShape && m_currentPickingInfo.m_extraInformation != U32_INVALID)
        {
            TrajectoryExtraInfo trajExtra;
            trajExtra   = (TrajectoryExtraInfo)m_currentPickingInfo.m_extraInformation;
            selectedIndexList.push_back(trajExtra.m_raw);
        } else
		{
			m_currentPickingInfo.m_extraInformation = U32_INVALID;
		}
        if (shapes)
        {
            if (!_unselectShape)
			{
				for (u32 i=0; i<shapes->size(); i++)
				{
					PickingShape* piskShape = (*shapes)[i];
					if (selection.find(piskShape) != -1)
					{
						TrajectoryExtraInfo trajExtra = (TrajectoryExtraInfo)(piskShape->getData());
						selectedIndexList.push_back(trajExtra.m_raw);
					}
				}
			}

            EDITOR->deletePickingShapesForObject(_curTrajectoryRef);
        }

        // create sub shapes for points
        u32 count = trajTemplate->getPosCount();
        PickingShape_Disc*  pick;
        for (u32 i = 0; i < count; i++)
        {
            TrajectoryExtraInfo trjExtra;
            trjExtra.m_index = i;

            if (i != count -1 && trajTemplate->getPosInterpAt(i) == Spline::interp_bezier_standard)
            {
                pick = addPickingPoint(_curTrajectoryRef, i, TYPE_SHAPE_NORMAL_OUT);
                trjExtra.m_type = TYPE_SHAPE_NORMAL_OUT;
                if (selectedIndexList.find(trjExtra.m_raw) != -1)
                    EDITOR->selectShape(pick, bfalse);
            }
            // add control point for standard bezier
            if (i > 0 && trajTemplate->getPosInterpAt(i-1) == Spline::interp_bezier_standard)
            {
                pick = addPickingPoint(_curTrajectoryRef, i, TYPE_SHAPE_NORMAL_IN);
                trjExtra.m_type = TYPE_SHAPE_NORMAL_IN;
                if (selectedIndexList.find(trjExtra.m_raw) != -1)
                    EDITOR->selectShape(pick, bfalse);
            }
            pick = addPickingPoint(_curTrajectoryRef, i, TYPE_SHAPE_POINT);
            trjExtra.m_type = TYPE_SHAPE_POINT;
            if (selectedIndexList.find(trjExtra.m_raw) != -1)
                EDITOR->selectShape(pick, bfalse);
        }
    }

    PickingShapePluginData* SequenceEditorTrajectory::createPickingShapeData( PickingShape * _shape )
    {
        if(_shape->getShapeType() == PickingShape::ShapeType_Disc && _shape->getOwnerPtr()->getObjectType() == BaseObject::eTrajectory)
            return newAlloc(mId_Editor, PickingShapeData(_shape,m_owner));
        else
            return NULL;
    }

    void SequenceEditorTrajectory::fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene) const
    {
        Trajectory          * traj = NULL;

        if (m_curTrajectoryIndex == U32_INVALID)
            return;

        SequencePlayerComponent *seqPlayer = m_owner->getCurrentSequenceComponent();
        if (!seqPlayer || m_curTrajectoryIndex >= (i32)seqPlayer->getEventsListCount() )
            return;

        SequenceEvent *evt = seqPlayer->getEventAtIndex(m_curTrajectoryIndex);
        if (evt->getTemplate()->getType() != SequenceEvent_Template::event_PlayTrajectory)
            return;

        traj = ((PlayTrajectory_evt *)evt)->getTrajectory();
        if (!traj)
            return;

        const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(traj->getRef());            
        if (!shapes/* || shapes->size() != traj->getTemplate()->getPosCount()*/)
            return;

        _objects.push_back(traj->getRef());

        //search the main anchor,
        Trajectory_Template *   trajTemplate = ((PlayTrajectory_evtTemplate*)evt->getTemplate())->getTrajectoryTemplate();

        u32 currentShapeIdex = 0;
        for (u32 iShape = 0; iShape < shapes->size() && currentShapeIdex < traj->getTemplate()->getPosCount(); iShape++)                
        {
            PickingShape_Disc* pick = (PickingShape_Disc*)((*shapes)[iShape]);
            ITF_ASSERT(pick);
            if (pick)
            {
                TrajectoryExtraInfo trajExtra = ( TrajectoryExtraInfo) pick->getData();
                Vec3d pos = traj->getTemplate()->getGlobalPosAt(currentShapeIdex, traj->getPos());

                switch (trajExtra.m_type)
                {
                case TYPE_SHAPE_POINT:
                    currentShapeIdex++;
                    break;
                case TYPE_SHAPE_NORMAL_IN:
                    {
                        Spline * spline = (Spline *)trajTemplate->getSplinePos();
                        Spline::SplinePoint & sPt = spline->getSplinePointAt(currentShapeIdex);
                        pos -= sPt.m_normalIn;
                    }
                    break;
                case TYPE_SHAPE_NORMAL_OUT:
                    {
                        Spline * spline = (Spline *)trajTemplate->getSplinePos();
                        Spline::SplinePoint & sPt = spline->getSplinePointAt(currentShapeIdex);
                        pos += sPt.m_normalOut;
                    }
                    break;
                }

                //Update its picking shape and add to pickable list
                pick->setPos(pos);
                pick->setlastUpdateFrame(CURRENTFRAME);                        

                _orderedShapes.push_back(pick);
            }
        }
    }

    void SequenceEditorTrajectory::onObjectChanged( BaseObject* pBO )
    {
        switch (pBO->getObjectType())
        {
        case BaseObject::eActor:
            {
                if(!m_owner->isAutoKeyAllowed())
                    return;

                PlayTrajectory_evt*         evt         = getCurrentEvent();
                if (!evt)
                    return;
                const Actor* pAct = evt->getBindedActor();
                if (!pAct || pAct->getRef() != pBO->getRef())
                    return;

                onKeyDownActorSnapShot();
            }
            break;
        case BaseObject::eTrajectory:
            sendCurTrajectory(bfalse);
            break;
        }
    }


    bbool SequenceEditorTrajectory::computeInternalShape(Trajectory *pTrajectory, Trajectory_Template *trajTemplate, const Vec3d &vDelta3D)
    {
        PickingInfo2D* pPickInfo = pTrajectory->getPickingInfo();
        TrajectoryExtraInfo * trajExtraInfo = (TrajectoryExtraInfo *)&pPickInfo->m_extraInformation;
        const u32 index = trajExtraInfo->m_index;
        const u32 mode = trajExtraInfo->m_type;

        if (pTrajectory->isSelectionIsLocked() && mode == TYPE_SHAPE_POINT)
        {
            for (u32 i = 0; i < trajTemplate->getPosCount(); i++)
            {
                trajTemplate->setGlobalPosDataAt(trajTemplate->getGlobalPosAt(i, pTrajectory->getPos()) + vDelta3D, pTrajectory->getPos(), trajTemplate->getPosInterpAt(i), trajTemplate->getPosTimeAt(i), i);
            }
        }
        else
        {
            switch (mode)
            {
            case TYPE_SHAPE_POINT:
                trajTemplate->setGlobalPosDataAt(trajTemplate->getGlobalPosAt(index, pTrajectory->getPos()) + vDelta3D, pTrajectory->getPos(), trajTemplate->getPosInterpAt(index), trajTemplate->getPosTimeAt(index), index);
                break;
            case TYPE_SHAPE_NORMAL_IN:
                {
                    Spline * spline = (Spline *)trajTemplate->getSplinePos();
                    Spline::SplinePoint & sPt = spline->getSplinePointAt(index);
                    sPt.m_normalIn -= vDelta3D;

                    if (m_owner->isBezierLinked())
                    {
                        f32 lenIn = sPt.m_normalIn.norm();
                        f32 lenOut = sPt.m_normalOut.norm();
                        if (fabs(lenIn) >  MTH_EPSILON && fabs(lenOut) > MTH_EPSILON)
                        {
                            sPt.m_normalOut = sPt.m_normalIn;
                            sPt.m_normalOut = sPt.m_normalOut.normalize();
                            sPt.m_normalOut *= lenOut;
                        }
                    }
                }
                break;
            case TYPE_SHAPE_NORMAL_OUT:
                {
                    Spline * spline = (Spline *)trajTemplate->getSplinePos();
                    Spline::SplinePoint & sPt = spline->getSplinePointAt(index);
                    sPt.m_normalOut += vDelta3D;
                    if (m_owner->isBezierLinked())
                    {
                        f32 lenIn = sPt.m_normalIn.norm();
                        f32 lenOut = sPt.m_normalOut.norm();
                        if (fabs(lenIn) >  MTH_EPSILON && fabs(lenOut) > MTH_EPSILON)
                        {
                            sPt.m_normalIn = sPt.m_normalOut;
                            sPt.m_normalIn = sPt.m_normalIn.normalize();
                            sPt.m_normalIn *= lenIn;
                        }
                    }
                }
                break;
            }
        }
        Actor* actSequence = m_owner->getCurrentSequenceActor();
        if (!actSequence) 
            return bfalse;
        m_owner->propagateActorChanges(actSequence, bfalse, btrue, bfalse);

        pTrajectory->onEditorMove();
    
        return btrue;
    }

    bbool SequenceEditorTrajectory::onSetShapePosition( PickingShape* _pPickingShape, const Vec2d& _screenspacepos )
    {
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        ITF_ASSERT(evt && evtTemplate);
        if (!evt || !evtTemplate)
            return bfalse;

        BaseObject* pOwner = _pPickingShape->getOwnerPtr();
        if(pOwner->getObjectType() != BaseObject::eTrajectory)
            return bfalse;
        PickingInfo2D* pPickInfo = _pPickingShape->getPickingInfoPtr();
        Trajectory * pTrajectory = pOwner->DynamicCast<Trajectory>(ITF_GET_STRINGID_CRC(Trajectory,1495390743));

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();

        ITF_ASSERT(traj == pTrajectory);
        if (traj != pTrajectory)
            return bfalse;

        pTrajectory->setPickingInfo(pPickInfo);
        const u32 index = TrajectoryExtraInfo(pPickInfo->m_extraInformation).m_index;

        const Vec3d p1New2DPos(_screenspacepos.m_x + pPickInfo->m_anchor.m_x, _screenspacepos.m_y + pPickInfo->m_anchor.m_y, pPickInfo->m_zDist);
        Vec3d p1New3DPos, vDelta3D;
        GFX_ADAPTER->compute2DTo3D(p1New2DPos, p1New3DPos);
        
        vDelta3D        = p1New3DPos;
        switch (TrajectoryExtraInfo(pPickInfo->m_extraInformation).m_type)
        {
        case TYPE_SHAPE_POINT:
           vDelta3D -= trajTemplate->getGlobalPosAt(index, pTrajectory->getPos());
           break;
        case TYPE_SHAPE_NORMAL_IN:
            {
                Spline * spline = (Spline *)trajTemplate->getSplinePos();
                Spline::SplinePoint & sPt = spline->getSplinePointAt(index);
                vDelta3D -= trajTemplate->getGlobalPosAt(index, pTrajectory->getPos()) - sPt.m_normalIn;
            }
            break;
        case TYPE_SHAPE_NORMAL_OUT:
            {
                Spline * spline = (Spline *)trajTemplate->getSplinePos();
                Spline::SplinePoint & sPt = spline->getSplinePointAt(index);
                vDelta3D -= trajTemplate->getGlobalPosAt(index, pTrajectory->getPos()) + sPt.m_normalOut;
            }
            break;
        }
        vDelta3D.m_z    = 0.f;

        return computeInternalShape(pTrajectory, trajTemplate, vDelta3D);
#else
        return bfalse;
#endif
    }

    bbool SequenceEditorTrajectory::onOffsetShapePosition( PickingShape* _pPickingShape, const Vec3d& _deltaPos )
    {
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        ITF_ASSERT(evt && evtTemplate);
        if (!evt || !evtTemplate)
           return bfalse;

        Trajectory *            traj = evt->getTrajectory();
        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();

        BaseObject* pOwner = _pPickingShape->getOwnerPtr();
        if(pOwner->getObjectType() != BaseObject::eTrajectory)
            return bfalse;
        PickingInfo2D* pPickInfo = _pPickingShape->getPickingInfoPtr();
        Trajectory * pTrajectory = pOwner->DynamicCast<Trajectory>(ITF_GET_STRINGID_CRC(Trajectory,1495390743));

        ITF_ASSERT(traj == pTrajectory);
        if (traj != pTrajectory)
           return bfalse;

        pTrajectory->setPickingInfo(pPickInfo);

        return computeInternalShape(pTrajectory, trajTemplate, _deltaPos);
#else
        return bfalse;
#endif
    }

    ITF::bbool SequenceEditorTrajectory::fillContextMenu( EditorContextMenu& _menu, ObjectRef _target, bbool _topItems )
    {
        Pickable *pick = (Pickable *)_target.getObject();
        if (!pick)
            return bfalse;

        if (pick->getObjectType() != BaseObject::eTrajectory)
            return bfalse;

        u32   mode    = TYPE_SHAPE_POINT;
        if (EDITOR->getSelectionSize() == 1)
        {
            PickingShape* shape = EDITOR->getSelectionAt(0);
            if (shape)
            {
                TrajectoryExtraInfo  trajExtraInfo = (TrajectoryExtraInfo)(shape->getData());
                mode    = trajExtraInfo.m_type;
            }
        }

        if (mode == TYPE_SHAPE_POINT)
        {
            EditorContextMenu& subMenuIterp = _menu.addItemFromExternalPlugin(EditorContextMenu::ItemId_TrajInterpolation, "Change Interpolation")->m_subMenu;
            subMenuIterp.addItemFromExternalPlugin(EditorContextMenu::ItemId_TrajInterConstant, "Constant");
            subMenuIterp.addItemFromExternalPlugin(EditorContextMenu::ItemId_TrajInterLinear, "Linear");
            subMenuIterp.addItemFromExternalPlugin(EditorContextMenu::ItemId_TrajInterSpline, "Spline");
            subMenuIterp.addItemFromExternalPlugin(EditorContextMenu::ItemId_TrajInterBezier, "Bezier");
            subMenuIterp.addItemFromExternalPlugin(EditorContextMenu::ItemId_TrajInterBezierStandard, "Bezier Standard");

        } else
        {

        }

        return btrue;
    }


    void SequenceEditorTrajectory::onContextMenuItemSelected( ContextMenuItem* _item, ObjectRef _menuTarget )
    {
        PlayTrajectory_evt*         evt         = getCurrentEvent();
        PlayTrajectory_evtTemplate* evtTemplate = getCurrentEventTemplate();

        if (!evt || !evtTemplate)
            return;

        Trajectory_Template *   trajTemplate = evtTemplate->getTrajectoryTemplate();

        int changeTrajMode = -1;
        switch (_item->getIdFromExternal())
        {
        case EditorContextMenu::ItemId_TrajInterConstant:
            changeTrajMode = Spline::interp_constant; break;
        case EditorContextMenu::ItemId_TrajInterLinear:
            changeTrajMode = Spline::interp_linear; break;
        case EditorContextMenu::ItemId_TrajInterSpline:
            changeTrajMode = Spline::interp_spline; break;
        case EditorContextMenu::ItemId_TrajInterBezier:
            changeTrajMode = Spline::interp_bezier; break;
        case EditorContextMenu::ItemId_TrajInterBezierStandard:
            changeTrajMode = Spline::interp_bezier_standard; break;
        }

        if (changeTrajMode >= 0)
        {
            u32 nbSelection = EDITOR->getSelectionSize();
            bbool somethingDone = bfalse;
            for (u32 i=0; i<nbSelection; i++)
            {
                PickingShape* shape = EDITOR->getSelectionAt(i);
                if (shape)
                {
                    TrajectoryExtraInfo  trajExtraInfo = (TrajectoryExtraInfo)(shape->getData());
                    const u32 mode     = trajExtraInfo.m_type;
                    const u32 index    = trajExtraInfo.m_index;

                    if (mode == TYPE_SHAPE_POINT)
                    {
                        trajTemplate->setLocalPosDataAt(trajTemplate->getLocalPosAt(index),
                            (Spline::Interpolation)changeTrajMode, 
                            trajTemplate->getPosTimeAt(index), index);
                        somethingDone = btrue;
                    }
                }
            }
             trajTemplate->sortPosByTime();
            if (somethingDone)
                sendCurTrajectory(btrue);
        }
    }

    SequenceEditorTrajectory::PickingShapeData::PickingShapeData( PickingShape* _pParentShape, Plugin* _pPlugin )
        : PickingShapePluginData(_pParentShape, _pPlugin)
    {
    }

    void SequenceEditorTrajectory::PickingShapeData::draw()
    {
        Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_PolyLinePoint;
        const Vec3d& pos = static_cast<PickingShape_Disc*>(m_pickingShape)->getPos();

        float R = 1.f;
        float G = 1.f;
        float B = 1.f;
        float A = 1.f;
        u32 iconColor = Color(A, R, G, B).getAsU32();

        f32 fIconSize = 24.f; //PickingShape::m_disc_picking_tolerance;

        bbool selectionIsLocked = bfalse; 
        Vec3d mainPos;

        TrajectoryExtraInfo  trajExtraInfo = (TrajectoryExtraInfo)(m_pickingShape->getData());
        const u32   index   = trajExtraInfo.m_index;
        const u32   mode    = trajExtraInfo.m_type;


        BaseObject* baseObj = m_pickingShape->getOwnerPtr();
        if(baseObj)
        {
            Trajectory* trj = static_cast<Trajectory*>(baseObj);
            if(trj)
            {
                selectionIsLocked = trj->isSelectionIsLocked();
            }

            mainPos = trj->getTemplate()->getGlobalPosAt(index, trj->getPos());
        }
            

        const SafeArray<PickingShape*>& selection = EDITOR->getSelectionList();


        if (mode == TYPE_SHAPE_POINT)
        {
            if(selectionIsLocked || selection.find(m_pickingShape) != -1)
                icon = Editor_IconStock::Icon_SelectedActor;
            else // the sub point is not selected
                icon = Editor_IconStock::Icon_PolyLinePoint;
        } else
        {
            f32 r = 0;
            f32 g = 1;
            f32 b = 0;

            if(selectionIsLocked || selection.find(m_pickingShape) != -1)
                icon    = Editor_IconStock::Icon_GreenSquare;
            else // the sub point is not selected
            {
                icon    = Editor_IconStock::Icon_YellowSquare;
                r       = 1;
            }
            GFX_ADAPTER->drawDBG3DLine(mainPos, pos, r, g, b, 0.f, 1.f);
        }

        EDITOR->getIconStock().drawIcon(icon, pos, fIconSize, 0.0f, bfalse, bfalse, iconColor);
    }

} // namespace ITF


#endif //!ITF_FINAL
