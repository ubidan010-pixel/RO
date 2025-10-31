// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_TODOCOMPONENT_H_
#include "gameplay/Components/Misc/TODOComponent.h"
#endif //_ITF_TODOCOMPONENT_H_

#ifndef _ITF_DUMMYCOMPONENT_H_
#include "engine/actors/Components/DummyComponent.h"
#endif //_ITF_DUMMYCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_


#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef SERIALIZEROBJECTBINARYMEM_H
#include "core/serializer/ZSerializerObjectBinaryMem.h"
#endif // SERIALIZEROBJECTBINARYMEM_H

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_COLORCOMPUTER_PLUGIN_H_
#include "tools/plugins/ActorEditor/Plugin_colorComputer.h"
#endif //_ITF_COLORCOMPUTER_PLUGIN_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_CHARACTERDEBUGGER_PLUGIN_H_
#include "tools/plugins/ActorEditor/Listeners/CharacterDebugger_Plugin.h"
#endif //_ITF_CHARACTERDEBUGGER_PLUGIN_H_

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_


//#define _LOGARITHMIC_SCALE
//#define _CLAMPED_SCALE

namespace ITF
{
     // in pixels
    static const f32 sSimpleClickActivationTreshold = 10.0f;
    static const f32 sMinScale = 0.01f;
    ActorEditor* ActorEditor::m_instance = NULL;


    u32 ActorEditor::m_showActorInfoMask = 0;

    ActorEditor::PickingShapeData::PickingShapeData(PickingShape *_picking, Plugin *_plugin) : PickingShapeData_RotationScale(_picking, _plugin)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void ActorEditor::PickingShapeData::update()
    {
        PickingShapeData_RotationScale::update();

        Actor *actor = (Actor*)m_pickingShape->getOwnerPtr();
        if (!actor)
            return;

        PickingShape_Disc* disc = (PickingShape_Disc*)m_pickingShape;
        disc->setAngle(actor->getAngle());

        // Draw other triggers anchors
        if(!isTaggedSubElementThisFrame())
        {
            bbool bSubEdited = EDITOR->getSubEditedItem() == m_pickingShape->getOwner();
            if (bSubEdited)
            {
                ITF_ASSERT(EDITOR->getState() == Editor::EditorState_EditMode);

                const PickableList& actors = actor->getScene()->getActors();
                const u32 actorCount = actors .size();
                for (u32 i = 0;i < actorCount;++i)
                {
                    const Actor * pCurActor = static_cast<const Actor *>(actors[i]);
                    if (!pCurActor)
                        continue;

                    if (pCurActor == actor)
                        continue;

                    //Tag each shape of pCurrActor as sub element
                    const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(actor->getRef());
                    if (!shapes)
                        continue;

                    const u32 count = shapes->size();
                    for (u32 index=0; index<count; index++)
                    {
                        PickingShape *shape = (*shapes)[index];
                        if (!shape)
                            continue;

                        PickingShapeData *data = static_cast<PickingShapeData*>(shape->getPluginData(m_plugin));
                        if (data)
                            data->tagSubElementThisFrame();
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ActorEditor::PickingShapeData::draw()
    {
        PickingShapeData_RotationScale::draw();

        Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();
        if (!owner)
            return;
        Actor *actor = (Actor*)owner;

        if (!EditorDraw::pickableDetail_canShowAnyDetail() && !getInstance()->hasActorInfoFlag(ActorInfoFlag_Any))
            return;

        if (!actor->isVisible(CAMERA))
            return;

        const Vec2d vTextOffset       = Vec2d(15.0f, Editor::fCharMaxHeight);
        SubSceneActor* pSubSceneActor = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        const bbool bIsSubSceneActor = ( pSubSceneActor!= NULL);
        bbool isSubEdited = bfalse;

        if (EDITOR->getSubEditedItem() != ITF_INVALID_OBJREF)
        {
            if (EDITOR->getSubEditedItem() != actor->getRef())
            {
                const BaseObject* pSubEditedObject = GETOBJECT(EDITOR->getSubEditedItem());
                if (!pSubEditedObject)
                {
                    if (!getInstance()->hasActorInfoFlag(ActorInfoFlag_Any))
                        return;
                }
            }
            else
            {
                isSubEdited = btrue;
            }
        }

        bbool canShowShape = bfalse;
        bbool canShowAnchor = bfalse;

        const bbool bMouseIsOver = (EDITOR->getPickingShapeOfObjectUnderMouse() == m_pickingShape);
        const bbool bSubElement = isTaggedSubElementThisFrame() || m_pickingShape->isSubAnchor();
        //// bit 0 = shape, bit 1 = anchor, bit 2 = text

        bbool someoneIsSubEditedAndThatsNotMe = bfalse;
        bbool bSubEdited = EDITOR->isSubEdited(actor->getRef());
        if (!bSubEdited && EDITOR->getSubEditedItem().getObject())
            someoneIsSubEditedAndThatsNotMe = btrue;

        const SafeArray<PickingShape*>& selection = EDITOR->getSelectionList();
        bbool isShapeSelected = selection.find(m_pickingShape) != -1;

        float R = 1.f;
        float G = 1.f;
        float B = 1.f;
        float A = 1.f;
        if (someoneIsSubEditedAndThatsNotMe)
        {
            R = 0.5f;
            G = 0.5f;
            B = 0.5f;
            A = 0.7f;
        }

        if (EditorDraw::pickableDetail_CanShowAll())
            canShowShape = canShowAnchor = btrue;
        else
        {
            if(EditorDraw::PickableDetail_CanShowAnchors())
                canShowAnchor = btrue;
            else
                canShowAnchor = getInstance()->hasActorInfoFlag(ActorInfoFlag_Anchors);
        }

        if (actor->isSelected() || bMouseIsOver)
            canShowAnchor = btrue;

        if (!bSubElement && (actor->isSelected() || bMouseIsOver))
            canShowShape = canShowAnchor = btrue;


        Editor_IconStock::EditorIconID iconActor                    = Editor_IconStock::Icon_Actor;
        Editor_IconStock::EditorIconID iconActorWithWarnings        = Editor_IconStock::Icon_ActorWithWarnings;
        Editor_IconStock::EditorIconID iconSelectedActor            = Editor_IconStock::Icon_SelectedActor;
        Editor_IconStock::EditorIconID iconSubEditActor             = Editor_IconStock::Icon_SubEditActor;
        Editor_IconStock::EditorIconID iconCinemaClap               = Editor_IconStock::Icon_Cinema_Clap;
        Editor_IconStock::EditorIconID iconLight                    = Editor_IconStock::Icon_Light;
        Editor_IconStock::EditorIconID iconGroupActor               = Editor_IconStock::Icon_Group;
        Editor_IconStock::EditorIconID iconGroupActorSel            = Editor_IconStock::Icon_GroupSelected;
        Editor_IconStock::EditorIconID iconDisabledActor            = Editor_IconStock::Icon_DisabledActor;
        Editor_IconStock::EditorIconID iconSelectedDisabledActor    = Editor_IconStock::Icon_SelectedDisabledActor;

        // Draw Anchor
        if(canShowAnchor)
        {
            f32 fAnchorSize = Anchor_DefaultSize;
            SafeArray<Editor_IconStock::EditorIconID> icons;

            //  Constraint Camera
            if(actor->GetComponent<CameraModifierComponent>())
            {
                if (actor->isSelected())
                    icons.push_back(Editor_IconStock::Icon_ConstraintCamera_Selected);
                else
                    icons.push_back(Editor_IconStock::Icon_ConstraintCamera);
            }
            else if(isShapeSelected)
            {
                if (isSubEdited)
                {
                    fAnchorSize = Anchor_DefaultSize + (Anchor_DefaultSize * 0.25f) * f32_Cos(ELAPSEDTIME*5.0f);
                    icons.push_back(iconSubEditActor);
                }
                else if (actor->isEnabled())
                {
                    icons.push_back(iconSelectedActor);
                }
                else
                {
                    icons.push_back(iconSelectedDisabledActor);
                    A = 0.5f;
                }
            }
            else
            {
                BaseObject *owner = m_pickingShape->getOwnerPtr();
                Editor_IconStock::EditorIconID normalID = iconActor;
                if (!actor->isEnabled())
                {
                    normalID = iconDisabledActor;
                    A = 0.5f;
                }

#ifndef ITF_FINAL
                if ( (owner!=NULL) && WarningManager::hasWarningForObject(owner) )
                {
                    normalID = iconActorWithWarnings;
                }
#endif
                icons.push_back(normalID);

                if(bMouseIsOver)
                    fAnchorSize = Anchor_DefaultSize + (Anchor_DefaultSize * 0.1f) * f32_Cos(ELAPSEDTIME*5.0f);
            }

            // Check for invalid link
            LinkComponent* pLC = actor->GetComponent<LinkComponent>();
            if(pLC)
            {
                const LinkComponent::ChildrenList& children = pLC->getChildren();
                for(u32 i = 0; i < children.size(); ++i)
                {
                    Pickable* pObj = NULL;
                    const ObjectPath& path = children[i].getPath();
                    if(path.getIsAbsolute())
                        pObj = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    else
                        pObj = SceneObjectPathUtils::getObjectFromRelativePath(actor, path);

                    if(!pObj)
                    {
                        icons.push_back(iconActorWithWarnings);
                        break;
                    }
                }
            }

            if (actor->GetComponent<SequencePlayerComponent>())
            {
                icons.push_back(iconCinemaClap);
            }

            if (actor->GetComponent<LightComponent>())
            {
                icons.push_back(iconLight);
            }

            TODOComponent* todocomp = actor->GetComponent<TODOComponent>();
            if (todocomp)
            {
                icons.clear();
                icons.push_back(Editor_IconStock::Icon_RedSquare);
                icons.push_back(Editor_IconStock::Icon_Interrogation);
            }

            // dummy actors
            if (actor->GetComponent<DummyComponent>())
            {
                icons.clear();
                icons.push_back(actor->isSelected()
                    ? Editor_IconStock::Icon_SelectedDummyActor
                    : Editor_IconStock::Icon_DummyActor);
                A = 1.0f;
            }

            if(!actor->hasDataError())
            {
                if (Interface_ActorEditorListener* pInterface = static_cast<Interface_ActorEditorListener*>(m_pickingShape->getCreatorPickingShapePlugin()))
                {
                    pInterface->addPickingShapeSpecificIcons(m_pickingShape, icons, fAnchorSize);
                }
                else
                {
                    for (u32 i = 0; i < getInstance()->m_listeners.size(); i++)
                    {
                        ActorEdListener& listener = getInstance()->m_listeners[i];
                        for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                        {
                            ActorComponent* pComp = actor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                            if (pComp)
                                listener.m_listener->addComponentSpecificIcons(actor, pComp, icons, fAnchorSize);
                        }
                    }
                }
            }

            u32 iconColor = Color(A, R, G, B).getAsU32();
            if (bIsSubSceneActor)
            {
                if(pSubSceneActor->getEmbedScene())
                {
                    if(actor->isSelected())
                        icons.push_back(iconGroupActorSel);
                    else
                        icons.push_back(iconGroupActor);
                }
                else
                {
                    if(actor->isSelected())
                        icons.push_back(Editor_IconStock::Icon_SubSceneReferenceSelected);
                    else
                        icons.push_back(Editor_IconStock::Icon_SubSceneReference);
                }

                fAnchorSize *= 1.1f;
                if (pSubSceneActor)
                {
                    if (pSubSceneActor->isSinglePiece())
                        iconColor = (iconColor & 0xff000000) | (0xffaa88);
                }
            }

            if (actor->hasDataError())
            {
                icons.push_back(Editor_IconStock::Icon_RedSquare);

                // Make it clearly visible !!
                fAnchorSize = 1.25f * Anchor_DefaultSize + (Anchor_DefaultSize * 0.25f) * f32_Cos(ELAPSEDTIME*5.0f);
            }

            const bbool flipH = actor->getScale().m_x<0;
            const bbool flipV = actor->getScale().m_y<0;

            for (u32 itIcon = 0; itIcon < icons.size(); ++itIcon)
            {
                ITF_ASSERT(m_pickingShape->getShapeType() == PickingShape::ShapeType_Disc);
                if (m_pickingShape->getShapeType() == PickingShape::ShapeType_Disc)
                {
                    PickingShape_Disc* disc = (PickingShape_Disc*)m_pickingShape;
                    f32 angle = disc->getAngle();

                    switch (disc->getPosType())
                    {
                    case PickingShape_Disc::InitPos:
                        iconColor = Color::green().getAsU32();
                        angle = actor->getBoundWorldInitialAngle();
                        break;
                    }
                    EDITOR->getIconStock().drawIcon(icons[itIcon], disc->getPos(), fAnchorSize, angle, flipH, flipV, iconColor);
                }
            }

            if(!bSubElement && actor->isSelected())
            {
                // Don't display initial pos if it's almost the same some pos
                if((actor->getWorldInitialPos() - actor->getPos()).sqrnorm() > MTH_EPSILON)
                {
                    GFX_ADAPTER->drawDBG3DLine(actor->getWorldInitialPos(), actor->getPos(),0.7f,0,0);
                }

                // Display difference between initial and current angle
                if(fabs(actor->getWorldInitialRot() - actor->getAngle()) > MTH_EPSILON)
                {
                    Vec2d offset = (Vec2d::XAxis / 2.0f).Rotate(actor->getWorldInitialRot() + MTH_PIBY2);
                    GFX_ADAPTER->drawDBG3DLine(actor->getPos(), actor->getPos() + offset.to3d(), 0, 0.7f, 0);
                }
            }
        }

    #if 0
// dev in progress
             if (bMouseIsOver)
        {
            PickingShape* pMouseOver = EDITOR->getPickingShapeOfObjectUnderMouse();
            if (pMouseOver && m_pickingShape && m_pickingShape->getShapeType() == PickingShape::ShapeType_Disc)
            {
                PickingShape_Disc* pDisc = (PickingShape_Disc*)m_pickingShape;
                // plut�t tester en 2d
                if ((pMouseOver->getCenter() - pDisc->getCenter()).sqrnorm() < 1.f)
                {
//                    pDisc->m_offset = Vec3d::XAxis;
//                    pDisc->setPos(pDisc->getPos() + Vec3d::XAxis);
                    GFX_ADAPTER->drawDBGCircle(m_pickingShape->getCenter(), 1.f);
                }
            }
        }
#endif // 0
    }

    void ActorEditor::PickingShapeData::rotateAroundAxisZ( f32 _deltaAngle, const Vec3d* _pRotationCenter /*= NULL*/ )
    {
        Actor* pActor = (Actor*)m_pickingShape->getOwnerPtr();

        pActor->rotate(_deltaAngle, bfalse, _pRotationCenter);

        if(EDITOR->isAllowStorePickableInitData(pActor))
        {
            if(_pRotationCenter)
                pActor->setWorldInitialPos(pActor->getPos());
            pActor->setWorldInitialRot(pActor->getAngle());
        }
    }

    void ActorEditor::PickingShapeData::scale( const Vec2d& _newScale )
    {
        Actor* pActor = (Actor*)m_pickingShape->getOwnerPtr();

        Vec2d scaleToApply = _newScale;
        if ( pActor->getTemplate() && pActor->getTemplate()->getScaleForced() != Vec2d::Zero )
            scaleToApply = pActor->getTemplate()->getScaleForced();

        pActor->setScale(scaleToApply);
        if(EDITOR->isAllowStorePickableInitData(pActor))
            pActor->setWorldInitialScale(scaleToApply);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void ActorEditor::selectActor( Actor* _pActor, bbool _bDoSelect, const PickingInfo2D* _info )
    {
        ITF_ASSERT(_pActor);
        ITF_ASSERT(_pActor->getObjectType() == BaseObject::eActor);
        ITF_ASSERT(m_state != Idle);

        const u32 uSelectionSize = m_selectionInfoList.size();

        if(_bDoSelect)
        {
            bbool bFound = bfalse;
            for(u32 i = 0; i < uSelectionSize; ++i)
            {
                if(m_selectionInfoList[i].m_targetRef == _pActor->getRef())
                {
                    ITF_Memcpy(&m_selectionInfoList[i], _info, sizeof(PickingInfo2D));
                    bFound = btrue;
                    break;
                }
            }

            if(!bFound)
            {
                ITF_ASSERT(_info);

                if(_info)
                {
                    m_selectionInfoList.push_back(*_info);
                    _pActor->setSelected(&m_selectionInfoList[m_selectionInfoList.size() - 1]);

#if !defined (ITF_FINAL) && defined (ITF_WINDOWS)
                    if (ISDEBUGGER_PRESENT && INPUT_ADAPTER->isKeyPressed(KEY_F9))
                    {
                        DebugInfo::breakOnRef(_pActor->getRef());
                    }
#endif // !defined (ITF_FINAL) && defined (ITF_WINDOWS)
                }
            }
        }
        else
        {
            for(u32 i = 0; i < uSelectionSize; ++i)
            {
                if(m_selectionInfoList[i].m_targetRef == _pActor->getRef())
                {
                    _pActor->setSelected(NULL);
                    m_selectionInfoList.eraseNoOrder(i);
                    break;
                }
            }

            if(m_selectionInfoList.size() == 0)
            {
                if (PLUGINGATEWAY->hasExclusiveFocus(this))
                    PLUGINGATEWAY->setExclusiveFocus(NULL);
            }
        }

        PLUGINGATEWAY->onActorPicked(_pActor->getRef()); // inform other plugins
    }

    bbool ActorEditor::bCanEnterEditMode ()
    {
        return (m_state != SubEdit);
    }

    bbool ActorEditor::bCanLeaveEditMode ()
    {
        return (m_state == Idle);
    }

    void ActorEditor::createPickingShapeForActor (const ObjectRef _actor)
    {
        Actor* pActor = static_cast<Actor*>(_actor.getObject());
        ITF_ASSERT(pActor);
        if(!pActor)
            return;

        const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_actor);
        if (shapes)
        {
            ITF_ASSERT_MSG(0, "The object already got shapes !");
            EDITOR->deletePickingShapesForObject(_actor);
        }


        PickingShape_Disc* pick = newAlloc(mId_Editor,PickingShape_Disc(NULL));
        // Note: Code removed for now, waiting a real fix for 2dActor
        //if(pActor->is2DActor())
        //{
        //    Vec3d actor3DPos (Vec3d::Zero);
        //    GFX_ADAPTER->compute2DTo3D(pActor->get2DPos(), 0, actor3DPos);
        //    pick->setPos(actor3DPos);
        //} else
        {
            pick->setPos(pActor->getPos());
        }

        pick->setSubAnchor(bfalse);
        EDITOR->addPickingShapeForObject(_actor, pick);

        const bbool isSubSceneActor = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) != NULL;
        if (isSubSceneActor)
            pick->setPriority(0);
        else
            pick->setPriority(1);

        if (!isSubSceneActor &&
            getInstance()->needsInitPosPickingShape(pActor))
        {
            pick->setPosType(PickingShape_Disc::InitPosFromCurrent);
        }

        // custom listeners can create custom shapes
        if(!pActor->hasDataError())
        {
            for (u32 i = 0; i < getInstance()->m_listeners.size(); i++)
            {
                ActorEdListener& listener = getInstance()->m_listeners[i];

                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                {
                    ActorComponent* pComp = pActor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                    if (pComp)
                    {
                        listener.m_listener->addComponentSpecificPickingShape(pActor, pComp);
                    }
                }
            }
        }
    }

    bbool ActorEditor::needsInitPosPickingShape( Actor* pActor )
    {
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            ActorEdListener& listener = m_listeners[i];

            for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
            {
                ActorComponent* pComp = pActor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                if (pComp)
                {
                    if ( listener.m_listener->needsInitPosPickingShape(pActor, pComp) )
                    {
                        return btrue;
                    }
                }
            }
        }

        return bfalse;
    }

    void ActorEditor::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
    {
        if(!_bSwitchDone)
        {
            UnselectAll(btrue);
        }
    }

    void ActorEditor::onDeleteScene(const Scene* _pScene)
    {
        World* pWorld = CURRENTWORLD;
        if(pWorld && pWorld->hasScene(_pScene))
        {
            for(u32 i = 0; i < m_selectionInfoList.size(); )
            {
                Actor *pActor = static_cast<Actor*>(m_selectionInfoList[i].m_targetRef.getObject());
                if(pActor && pActor->getScene() == _pScene)
                {
                    pActor->setSelected(NULL);
                    m_selectionInfoList.eraseNoOrder(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        if(EDITOR)
        {
            const PickableList& actors = _pScene->getActors();
            const u32 uSize = actors.size();
            for (u32 i = 0; i < uSize; i++)
            {
                EDITOR->deletePickingShapesForObject(actors[i]->getRef());
            }
        }
    }


    void ActorEditor::UnselectAll(bbool _bforce)
    {
        if(!_bforce && (Idle != m_state && Select != m_state))
            return;

        if(SubEdit == m_state)
        {
            ITF_ASSERT(m_selectionInfoList.size());
            if(m_selectionInfoList.size())
                leaveSubEdit(static_cast<Actor*>(GETOBJECT(m_selectionInfoList[0].m_targetRef)));
        }
        else
        {
            const u32 uSize = m_selectionInfoList.size();
            for(u32 i = 0; i < uSize; ++i)
            {
                if(EDITOR->getSubEditedItem() == m_selectionInfoList[i].m_targetRef)
                    EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);

                Actor *pActor = static_cast<Actor*>(GETOBJECT(m_selectionInfoList[i].m_targetRef));
                if(pActor)
                    pActor->setSelected(NULL);
            }

            m_selectionInfoList.clear();
        }

        m_state = Idle;
    }

    void ActorEditor::onKey(   i32 _key, InputAdapter::PressStatus _status )
    {
        if (_status == InputAdapter::Pressed )
        {
            switch (_key)
            {
            case KEY_DEL:
                {
                    if (PLUGINGATEWAY->hasExclusiveFocus(this))
                        PLUGINGATEWAY->setExclusiveFocus(NULL);
                }
                break;

            case KEY_BACKSPACE:
                cancelCurrentAction(bfalse);
            break;

            case 'B':
                beginBindActorToFrieze();
            break;

            case 'C':
                if(
                    (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT)) ||
                    (INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT))
                    )
                {
                    pickFactorColor();
                    pickFogColor();
                }
                break;

            case 'V':
                if(
                    (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT)) ||
                    (INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT))
                    )
                {
                    pasteFogColor();
                    pasteFactorColor();
                }
                break;

            default:
                break;
            }
        }
    }

    ActorEditor::ActorEditor()
        : m_state(Idle)
        , m_lastSpawnedPath("")
        , m_creationRequestFrame(0)
        , m_isRotationScaleHelperMode(bfalse)
        , m_nextCreateSceneIsGroup(bfalse)
        , m_nextCreateSceneIsMerge(bfalse)
        , m_willCheckForMultipleCreation(bfalse)
        , m_keepBindTarget(bfalse)
        , m_createBindState_enabledByShortcut(bfalse)
        , m_createActorBuffer(NULL)
    {
        m_showActorInfoMask = 0;
        m_savedActorInfoMask = 0;
        ITF_ASSERT_CRASH(!m_instance, "Singleton issue");
        m_instance = this;
    }

    void ActorEditor::enterCreateSceneState(const String& _absolutePath, bbool _isMerge, bbool _isGroup)
    {
        m_creationRequestFrame = CURRENTFRAME;
        UnselectAll(bfalse);
        m_state = CreateScene;
        m_nextCreateSceneIsGroup = _isGroup;
        m_nextCreateSceneIsMerge = _isMerge;
        m_nextCreateScenePath   = _absolutePath;

        PLUGINGATEWAY->setExclusiveFocus(this);
    }

    void ActorEditor::enterCreateActorState(const String& _file, bbool _useBuffer)
    {
        if ( !_useBuffer )
    {
            SF_DEL(m_createActorBuffer);
        }

        if (!FILESERVER->getPathFromDataFolder(m_nextCreationScriptRef, _file, bfalse))
            return;

        m_creationRequestFrame = CURRENTFRAME;

        if ( !m_createActorBuffer )
        { // update m_lastSpawnedPath
            m_lastSpawnedPath = _file;
            i32 index;
            FilePath::normPath(m_lastSpawnedPath);
            m_lastSpawnedPath.strstr('/',btrue, &index);
            if (index>0)
            {
                m_lastSpawnedPath.truncate(index+1);
            }
        }

        UnselectAll(bfalse);
        m_state = CreateActor;
        PLUGINGATEWAY->setExclusiveFocus(this);
    }

    void    ActorEditor::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID)
    {
        String message;
        _blob.extractString(message);
    }

    void ActorEditor::onMouseWheel(i32 _mouseWheel, i32 _wheelDelta )
    {
    }

    void ActorEditor::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if (_but == InputAdapter::MB_Left)
        {
            switch (m_state)
            {
            case Idle:
                break;
            case Select:
                {
                    // Subedit activation
                    if (PLUGINGATEWAY->isSubEditAllowed() && (InputAdapter::MB_Left == _but) && (InputAdapter::Double_Press == _status))
                    {
                        if(m_selectionInfoList.size() == 1)
                        {
                            Actor* selectedActor = static_cast<Actor*>(GETOBJECT(m_selectionInfoList[0].m_targetRef));
                            ITF_ASSERT(selectedActor);

                            if(selectedActor && isSubEditable(selectedActor))
                                enterSubEdit(selectedActor->getRef());
                        }
                        else
                        {
                            ITF_ASSERT_MSG(0, "Incoherent state or selection status");
                        }
                    }
                }
                break;

            case CreateScene:
                #ifndef ITF_FINAL

                if (_but ==InputAdapter::MB_Left && _status == InputAdapter::Pressed && m_creationRequestFrame < CURRENTFRAME-1)
                {
                    m_creationRequestFrame = CURRENTFRAME;

                    Scene* pScene = EDITOR->getEditedScene();
                    ITF_ASSERT(pScene);
                    if (!pScene)
                        return;

                    // Let the others plugins handle various stuff, like picking events...
                    PLUGINGATEWAY->setExclusiveFocus(NULL);

                    if (m_nextCreateSceneIsGroup)
                    {
                        LoadInfo info;
                        info.m_mapPath = m_nextCreateScenePath.getString();
                        info.m_disconnectFromCells = bfalse;
                        World* pWorld = static_cast<World*>(GETOBJECT(WORLD_MANAGER->createAndLoadWorld (info)));
                        if (pWorld) // avoid Coverity warning
                        {
                            Scene* pScene = pWorld->getRootScene();

                            pScene->removeUnserializableFromLists();
                            EDITOR->pasteSceneContentAtMouse(pScene);
                            pScene->restoreUnserializableFromLists();

                            ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));

                            m_state = CreateScene;   // reset by importScene methods
                            m_willCheckForMultipleCreation = btrue;
                        }
                    }
                    else
                    {
                        Vec3d subScenePosition;
                        GFX_ADAPTER->compute3DTo2D(Vec3d::Zero, subScenePosition);
                        GFX_ADAPTER->compute2DTo3D(EDITOR->getMouse2d().to3d(subScenePosition.m_z), subScenePosition);
                        subScenePosition.m_z = 0.f;

                        if (m_nextCreateSceneIsMerge)
                        {
                            EDITOR->importSceneAsMerge(pScene, subScenePosition, m_nextCreateScenePath.getString());
                        }
                        else
                        {
                            EDITOR->importSceneAsInstance(pScene, subScenePosition, m_nextCreateScenePath.getString());
                        }
                        cancelCurrentAction(btrue);
                    }

                }
                #endif // ITF_FINAL
                break;

            case CreateActor:
                if (_but ==InputAdapter::MB_Left && _status == InputAdapter::Pressed && m_creationRequestFrame < CURRENTFRAME-1)
                {
                    m_creationRequestFrame = CURRENTFRAME;

                    Scene* pScene = EDITOR->getEditedScene();
                    ITF_ASSERT(pScene);
                    if (!pScene)
                        return;

                    Vec3d snappedMouseWorld = EDITOR->getMouseOnCurrentWorkingDepth();
                        EDITOR->snap(snappedMouseWorld, NULL, bfalse);

                    Actor* newActor = ACTORSMANAGER->spawnActor(snappedMouseWorld, pScene, m_nextCreationScriptRef,m_createActorBuffer==NULL);
                    ITF_ASSERT_MSG(newActor, "ACTORSMANAGER->spawnActor(...) returned NULL");
                    if (newActor)
                    {
                        EDITOR->setSceneAsModified(pScene);
                        newActor->setSerializable(btrue);
                        ACTORSMANAGER->registerSerializeDataActor(newActor);
                        PLUGINGATEWAY->onObjectCreated(newActor);
                        m_state = Select;
                        EDITOR->selectObject(newActor->getRef(), bfalse);
                        m_state = CreateActor;
                        m_willCheckForMultipleCreation = btrue;

                        if ( m_createActorBuffer )
                        {
                            m_createActorBuffer->rewindForReading();

                            const ObjectId oldId = newActor->getObjectId();

                            CSerializerObjectBinary serializer(m_createActorBuffer);
                            newActor->Serialize(&serializer,ESerialize_Data_Load);
                            newActor->setWorldInitialPos(snappedMouseWorld);
                            newActor->forceObjectId(oldId);
                            newActor->onLoaded();
                        }

                        // we need the actor to be loaded to have its component list when creating picking shapes
                        EDITOR->deletePickingShapesForObject(newActor->getRef());
                        createPickingShapeForActor(newActor->getRef());

                        PathString_t luaName;

                        newActor->getLua().getString(luaName);

                        newActor->generateNewUserFriendly(String8(FilePath::getFilenameWithoutExtension(luaName).cStr()));
                        newActor->replaceReinitDataByCurrent();
                        newActor->onEditorMove();

                        UNDOREDO_MANAGER->postAdd(newActor);
                    }
                }
                break;

            case CreateBind:
                {
                    const Actor* pParent = static_cast<const Actor*>(m_bindTargetParent.m_runtimeParent.getObject());
                    ITF_ASSERT_CRASH(pParent, "update didn't work properly");

                    // Step 1 : choose a parent bone
                    if(InputAdapter::Pressed == _status)
                    {
                        if(m_potentialChildForBind == m_bindTargetParent.m_runtimeParent)
                        {
                            ActorBind underMouse;
                            if(ActorEditor::findBindTarget(pParent, EDITOR->getMouse2d(), underMouse))
                                m_bindTargetParent = underMouse;

                            m_keepBindTarget = btrue;
                        }
                    }
                    // Step 2 : maybe attach to the potential child
                    else if(InputAdapter::Released == _status)
                    {
                        if(Actor* pChild = m_potentialChildForBind.getActor())
                        {
                            if(pParent != pChild)
                            {
                                bbool doBind = bfalse;

                                // If the current parent of the soon child is the current parent to bind
                                // Then unbind if the same binding is clicked, and bind if a different is clicked
                                if(ActorBind* pChildBind = pChild->getParentBind())
                                {
                                    if(pChildBind->m_runtimeParent == pParent->getRef())
                                    {
                                        if(pChildBind->m_type == m_bindTargetParent.m_type && pChildBind->m_typeData == m_bindTargetParent.m_typeData)
                                        {
                                            unbindActor(pChild, btrue, bfalse);

                                            if(!m_createBindState_enabledByShortcut)
                                                exitCreateBind();
                                            break;
                                        }
                                        else
                                        {
                                            doBind = btrue;
                                        }
                                    }
                                }

                                if(doBind || canBindActors(pParent, pChild))
                                {
                                    ActorBind* pBind = newAlloc(mId_Editor, ActorBind);

                                    pBind->m_type               = m_bindTargetParent.m_type;
                                    pBind->m_typeData           = m_bindTargetParent.m_typeData;
                                    pBind->m_useParentFlip      = btrue;
                                    pBind->m_useParentScale     = bfalse;
                                    pBind->m_removeWithParent   = bfalse;

                                    Vec3d localPos;
                                    f32 localAngle;
                                    if(pParent->getChildrenBindHandler().computeLocalCoordinates(pBind, pChild->getPos(), pChild->getAngle(), localPos, localAngle))
                                    {
                                        pBind->setInitialAngleOffset(localAngle);
                                        pBind->setInitialPositionOffset(localPos);

                                        SceneObjectPathUtils::getRelativePathFromObject(pChild, pParent, pBind->m_parent);

                                        UNDOREDO_MANAGER->preChange(pChild);
                                        pChild->setParentBind(pBind);
                                        pParent->getChildrenBindHandler().updateWorldCoordinates(pChild, pBind);
                                        pChild->storeCurrentToInitial();
                                        PLUGINGATEWAY->onObjectChanged(pChild);
                                    }
                                    else
                                    {
                                        ITF_ASSERT(0);

                                        SF_DEL(pBind);
                                    }

                                    if(!m_createBindState_enabledByShortcut)
                                        exitCreateBind();
                                }
                            }
                        }
                    }
                }
                break;

            case SetSubScenePivot:
                if (_but ==InputAdapter::MB_Left && _status == InputAdapter::Pressed)
                {
                    if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(m_setSubScenePivotActor.getActor()))
                    {
                        UndoRedoSequenceScope changeScope("set scene pivot");

                        Vec3d newPos = EDITOR->getMouse3d();
                        EDITOR->snap(newPos, NULL, btrue);

                        if(pSSA->isZForced())
                            newPos.m_z = pSSA->getDepth();

                        const Vec3d deltaPosInv = pSSA->getPos() - newPos;

                        // Process scene's elements to place them relatively to the new origin
                        const PickableList& content = pSSA->getSubScene()->getPickableList();
                        for(u32 i = 0; i < content.size(); ++i)
                        {
                            Pickable* pPickable = content[i];

                            UNDOREDO_MANAGER->preChange(pPickable);
                            pPickable->offsetPosition(deltaPosInv, btrue);

                            if(pPickable->getObjectType() == BaseObject::eActor)
                                if(ActorBind* pAB = static_cast<Actor*>(pPickable)->getParentBind())
                                    //if(!pAB->m_isRuntimeDisabled())
                                    continue;

                            pPickable->offsetPosition(deltaPosInv, bfalse);
                            PLUGINGATEWAY->onObjectChanged(pPickable);
                        }

                        UNDOREDO_MANAGER->preChange(pSSA);
                        pSSA->setPos(newPos);
                        pSSA->setWorldInitialPos(newPos);

                        PLUGINGATEWAY->onObjectChanged(pSSA);
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                    m_setSubScenePivotActor.invalidate();
                    m_state = Select;
                }
                break;

            default:
                break;
            }
        }
    }

    void ActorEditor::cancelCurrentAction(bbool _keepEditorStuff)
    {
        if (RotationScaleHelperMode == m_state)
        {
            m_state = Idle;
        }
        else if (SubEdit == m_state)
        {
            ITF_ASSERT(m_selectionInfoList.size());
            if(!_keepEditorStuff && m_selectionInfoList.size())
                leaveSubEdit(static_cast<Actor*>(GETOBJECT(m_selectionInfoList[0].m_targetRef)));
        }
        else if (Select == m_state || CreateActor == m_state || CreateScene == m_state)
        {
            if(!_keepEditorStuff)
                UnselectAll(bfalse);
            m_state = Idle;

            if (PLUGINGATEWAY->hasExclusiveFocus(this))
                PLUGINGATEWAY->setExclusiveFocus(NULL);
        }
        else if(CreateBind == m_state)
        {
            exitCreateBind();
        }

        SF_DEL(m_createActorBuffer);
    }

    void ActorEditor::update()
    {
        // Create icons for runtime added actors
        for(u32 iAct = 0; iAct < m_addedActiveActorsLastFrame.size(); ++iAct)
        {
            ObjectRef actRef = m_addedActiveActorsLastFrame[iAct];
            if(!EDITOR->getPickingShapesForObject(actRef))
                createPickingShapeForActor(actRef);
        }
        m_addedActiveActorsLastFrame.clear();

        const Plugin* focus = PLUGINGATEWAY->getExclusiveFocus();
        if (focus)
        {
            if (focus != this)
            {
                UnselectAll(bfalse);
                return;
            }
            if (m_state != SubEdit && m_state != CreateActor && m_state != CreateScene && m_state != CreateBind)
                PLUGINGATEWAY->setExclusiveFocus(NULL);
        }

        if ((m_selectionInfoList.size() == 0) && (m_state != CreateActor) && (m_state != CreateScene))
        {
            m_state = Idle;
        }

        if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) && INPUT_ADAPTER->isKeyPressed(KEY_LALT))
        {
            if(Select == m_state && m_selectionInfoList.size() == 1)
            {
                Actor* pActor = static_cast<Actor*>(m_selectionInfoList[0].m_targetRef.getObject());
                ITF_ASSERT_CRASH(pActor, "Invalid selection");

                if(pActor->isSerializable() && pActor->getWorld() != static_cast<World*>(GAMEMANAGER->getBaseMapWorld().getObject()))
                {
                    enterCreateBind(pActor);
                    m_createBindState_enabledByShortcut = btrue;
                }
            }
        }
        else
        {
            if(CreateBind == m_state && m_createBindState_enabledByShortcut)
            {
                exitCreateBind();
                m_createBindState_enabledByShortcut = bfalse;
            }
        }

        // update all listeners
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            ActorEdListener& listener = m_listeners[i];
            listener.m_listener->onUpdate(); // default update
            for (u32 iSel = 0; iSel < m_selectionInfoList.size(); iSel++)
            {
                Actor* pActor = static_cast<Actor*>(GETOBJECT(m_selectionInfoList[iSel].m_targetRef));
                if(pActor && !pActor->hasDataError())
                {
                    for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                    {
                        ActorComponent* pComp = pActor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                        if (pComp)
                            listener.m_listener->onUpdateSelectedActor(pActor, pComp); // specific update
                    }
                }
            }
        }

        // Basic inits
        if(SubEdit == m_state)
        {
            ITF_ASSERT(m_selectionInfoList.size() == 1);
            PLUGINGATEWAY->setExclusiveFocus(this);
        }

        if (CreateActor == m_state || CreateScene == m_state)
        {
            // Preview the future snapped position
            Vec3d snappedMouseWorld = EDITOR->getMouse3d();
            EDITOR->snap(snappedMouseWorld, NULL, bfalse);
            Vec3d snappedMouseScreen;
            GFX_ADAPTER->compute3DTo2D(snappedMouseWorld, snappedMouseScreen);
            Vec2d snappedMouse = snappedMouseScreen.truncateTo2D();

            EDITOR->showMouseIcon(Editor_IconStock::Icon_Plus, 0.0f, &snappedMouse);

            if(m_willCheckForMultipleCreation && !INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                m_willCheckForMultipleCreation = bfalse;
                cancelCurrentAction(btrue);
            }
        }

        // Search for the undermouse potential child
        if(CreateBind == m_state)
        {
            m_potentialChildForBind.invalidate();
            if(const PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse())
            {
                if(const Pickable* pObj = static_cast<const Pickable*>(pShape->getOwnerPtr()))
                {
                    if(pObj->getObjectType() == BaseObject::eActor)
                    {
                        m_potentialChildForBind = pObj->getRef();
                    }
                }
            }

            // Update the bind target if not fixed by left click
            if(!m_keepBindTarget)
            {
                const Actor* pActor = m_selectionInfoList[0].m_targetRef.getObject()->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                ITF_ASSERT_CRASH(pActor, "Invalid mode");
                ITF_ASSERT_CRASH(m_bindTargetParent.m_runtimeParent == pActor->getRef(), "Invalid mode selection");

                ActorEditor::findBindTarget(pActor, EDITOR->getMouse2d(), m_bindTargetParent);
            }
        }
    }

    void ActorEditor::draw()
    {
        // draw all listeners
        for (u32 i = 0; i < m_listeners.size(); i++)
            m_listeners[i].m_listener->onDraw();

        if(CreateBind == m_state)
        {
            // Bind feedback
            {
                const Actor* pParent = static_cast<const Actor*>(m_bindTargetParent.m_runtimeParent.getObject());
                ITF_ASSERT_CRASH(pParent, "Incoherent state, in CreateBind the parent should be valid");


                // Display parent's bones
                if(AnimLightComponent* pAnimLightComponent = pParent->GetComponent<AnimLightComponent>())
                {
                    if(AnimMeshScene* pMeshScene = pAnimLightComponent->getAnimMeshScene())
                    {
                        AnimSkeleton * skeleton = NULL;

                        if (pAnimLightComponent->getNumPlayingSubAnims() > 0)
                            skeleton = pAnimLightComponent->getCurSubAnim(0)->getSkeleton();

                        const SafeArray<AnimBoneDyn>& boneList = pMeshScene->m_AnimInfo.getCurrentBoneList();
                        const f32 fZ = pParent->getDepth();

                        u32 underMouseIndex = U32_INVALID;
                        if(m_bindTargetParent.m_type == ActorBind::BoneName)
                        {
                            underMouseIndex = pAnimLightComponent->getBoneIndex(StringID(m_bindTargetParent.m_typeData));
                        }

                        const u32 bCount = boneList.size();
                        for (u32 iBone = 0; iBone < bCount; iBone++)
                        {
                            const AnimBoneDyn& bone     = boneList[iBone];
                            const Vec3d bone3D_Start    = bone.m_Pos.to3d(fZ);
                            const Vec3d bone3D_End      = bone.m_PosEnd.to3d(fZ);

                            f32 lineSize = 4.0f;
                            Color color = Color::white();

                            if (skeleton && skeleton->isBoneInMinimalBonesList(iBone))
                                color = Color::green();

                            if(underMouseIndex == iBone)
                            {
                                color = Color::red();
                                lineSize *= 1.15f;
                            }

                            DebugDraw::line3D( bone3D_Start, bone3D_End, color, lineSize );
                        }
                    }
                }

                String tip;
                Color arrowColor = Color::black();
                Vec3d destination = EDITOR->getMouse3d();
                if(const Actor* pActorChild = m_potentialChildForBind.getActor())
                {
                    if(canBindActors(pParent, pActorChild))
                    {
                        //destination = pActorChild->getPos();
                        arrowColor = Color::blue();

                        switch(m_bindTargetParent.m_type)
                        {
                        case ActorBind::BoneName:
                            {
                                const StringID boneName(m_bindTargetParent.m_typeData);
                                tip.setTextFormat("Bind child to selected actor's bone: %s", boneName.getDebugString());
                            }
                            break;

                        case ActorBind::Root:
                            tip.setTextFormat("Bind child to selected actor: \"%s\"", pParent->getUserFriendly().cStr());
                            break;

                        default:
                            ITF_ASSERT(0);
                            break;
                        }
                    }
                }

                DebugDraw::arrow3D( m_bindTargetParent.getInitialPosOffset(), destination, arrowColor );

                if(tip.getLen())
                {
                    const Vec2d& mouse = EDITOR->getMouse2d() + Vec2d(20.0f, 0.0f);
                    GFX_ADAPTER->drawDBGText(tip, Color::white(), mouse.m_x, mouse.m_y);
                    GFX_ADAPTER->draw2dBox(mouse, tip.getLen() * Editor::fCharMaxWidth, Editor::fCharMaxHeight, EditorDraw::uNormalColor, EditorDraw::uNormalColor, EditorDraw::uDarkColor, EditorDraw::uNormalColor);
                }
            }
        }
        else if(SetSubScenePivot == m_state)
        {
            if(EDITOR->getSelectionSize() > 0)
            {
                if(PickingShape* pShape = EDITOR->getSelectionAt(0))
                {
                    if(SubSceneActor* pSSA = pShape->getOwnerPtr()->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        Vec3d newPos = EDITOR->getMouseOnCurrentWorkingDepth();
                        EDITOR->snap(newPos, NULL, btrue);
                        Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_GroupSelected;
                        if(!pSSA->getEmbedScene())
                            icon = Editor_IconStock::Icon_SubSceneReferenceSelected;
                        EDITOR->getIconStock().drawIcon(icon, newPos, Anchor_DefaultSize);
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
                else
                {
                    ITF_ASSERT(0);
                }
            }
            else
            {
                ITF_ASSERT(0);
            }
        }

        // Display binds
        // TODO generic test
        if(EDITOR->getState() != Editor::EditorState_Idle || EditorDraw::pickableDetail_CanShowDbg())
        {
            ObjectRefList displayList = EDITOR->getEditorSelectionAsObjRefList();

            if(PickingShape* pUnderMouse = EDITOR->getPickingShapeOfObjectUnderMouse())
            {
                ObjectRef oRef = pUnderMouse->getOwner();
                if(displayList.find(oRef) == -1)
                    displayList.push_back(oRef);
            }

            const u32 iListCount = displayList.size();
            for(u32 iList = 0; iList < iListCount; ++iList)
            {
                if(Pickable* pDisplayableObject = static_cast<Pickable*>(displayList[iList].getObject()))
                {
                    if(pDisplayableObject->getObjectType() == BaseObject::eActor)
                    {
                        Actor* displayableAct = static_cast<Actor*>(pDisplayableObject);

                        // Find top ancestor
                        Actor* topAncestor = NULL;
                        {
                            Actor* pCurrAncestor = displayableAct;
                            while(pCurrAncestor)
                            {
                                topAncestor = pCurrAncestor;

                                if(ActorBind* pCurrBind = pCurrAncestor->getParentBind())
                                    pCurrAncestor = static_cast<Actor*>(pCurrBind->m_runtimeParent.getObject());
                                else
                                    pCurrAncestor = NULL;
                            }
                        }

                        // Here topAncestor can't be NULL

                        // Display all children from topAncestor
                        {
                            SafeArray<Actor*> parents;
                            parents.push_back(topAncestor);
                            while(parents.size())
                            {
                                Actor* pCurrentParent = parents[0];
                                parents.eraseNoOrder(0);

                                const ObjectRefList& children = pCurrentParent->getChildrenBindHandler().getChildren();
                                for(u32 iChild = 0; iChild < children.size(); ++iChild)
                                {
                                    if(Actor * pChild = static_cast<Actor*>(children[iChild].getObject()))
                                    {
                                        parents.push_back(pChild);

                                        bbool targetFlip;
                                        f32 targetAngle;
                                        Vec3d targetPos;
                                        if(ActorEditor::getBindTargetInfo(pChild, targetPos, targetAngle, targetFlip))
                                            DebugDraw::arrow3D( targetPos, pChild->getPos(), Color::black() );
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(pDisplayableObject, "Deleted object in selection!");
                }
            }
        }
    }

    bbool ActorEditor::onPickShape(PickingShape* _shape)
    {
        BaseObject* pp = _shape->getOwnerPtr();
        if (!pp || pp->getObjectType() != BaseObject::eActor)
            return bfalse;

        // listeners notification
        if(Interface_ActorEditorListener* pInterface = static_cast<Interface_ActorEditorListener*>(_shape->getCreatorPickingShapePlugin()))
            pInterface->onSelected(static_cast<Actor*>(pp));

        switch(m_state)
        {
            case Idle:
                m_state = Select;
                // break; intentional
            case CreateBind:
            case Select:
                {
                    bbool bAlreadySelected = bfalse;

                    // Basic init for projection
                    CAMERA->apply();

                    // Update picking info for good drag and drop on selection
                    for(u32 i = 0; i < m_selectionInfoList.size(); ++i)
                    {
                        // If we pick an already selected actor
                        if(_shape->getOwner() == m_selectionInfoList[i].m_targetRef)
                        {
                            bAlreadySelected = btrue;

                            if(!INPUT_ADAPTER->isKeyPressed(MULTI_SELECTION_KEY))
                            {
                                // Update picking info
                                m_selectionInfoList[i] = *_shape->getPickingInfoPtr();
                            }
                        }
                        else // Update other actor's picking infos
                        {
                            PickingInfo2D& info = m_selectionInfoList[i];
                            const Actor* pActor = static_cast<const Actor*>(GETOBJECT(info.m_targetRef));
                            ITF_ASSERT(pActor);

                            Picking::fillPickingInfo(pActor, &info);
                        }
                    }

                    if(!bAlreadySelected)
                    {
                        selectActor(static_cast<Actor*>(pp), btrue, _shape->getPickingInfoPtr());
                    }

                    return btrue;
                }
                break;

            case SubEdit:
            case CreateActor:
            case CreateScene:
            default:
                break;
        }
        return bfalse;
    }

    bbool ActorEditor::onEditorRemove(const SafeArray<PickingShape*>& _shapesWithSameOwner)
    {
        bbool bResult = bfalse;

        Actor* pActor = NULL;
        Scene* pScene = NULL;
        Pickable* pPickable = NULL;

        for(u32 i = 0; i < _shapesWithSameOwner.size(); ++i)
        {
            PickingShape* pShape = _shapesWithSameOwner[i];
            if(!pPickable)
            {
                pPickable = static_cast<Pickable*>(pShape->getOwnerPtr());
                if(!pPickable) // This should never append
                    break;

                pActor = pPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                if(!pActor) // This part of code only handle shape with owner type Actor
                    break;

                pScene = pActor->getScene();
                ITF_ASSERT(pScene);
                if(!pScene)  // This should never append
                    break;
            }

            for (u32 j = 0; j < m_listeners.size(); j++)
            {
                ActorEdListener& listener = m_listeners[j];
                if(listener.m_listener->onDeletePickingShape(pShape))
                {
                    pShape = NULL;
                    break;
                }
            }

            if (pShape && !pShape->isSubAnchor())
            {
                bResult = btrue;
            }
        }

        if(pActor)
        {
            if(bResult)
            {
                PLUGINGATEWAY->onObjectDeleted(pActor);

                // Delete actor after all shapes
                const bbool justDelete = pActor->isSpawned();
                if(!justDelete)
                    UNDOREDO_MANAGER->preDelete(pActor);

                pActor->requestDestruction();
                EDITOR->deletePickingShapesForObject(pActor->getRef());

                if(!justDelete)
                    EDITOR->setSceneAsModified(pActor->getScene());
            }
            else
            {
                // Delete all shape, and re-create shapes from data
                EDITOR->deletePickingShapesForObject(pActor->getRef());
                createPickingShapeForActor(pActor->getRef());
            }
        }

        return bResult;
    }

    void ActorEditor::updateActorPickingShape(ObjectRef _or, ObjectRefList &_objects, PickingShapeVector& _orderedShapes)
    {
        const BaseObject* pObj = GETOBJECT(_or);

        if(!pObj || pObj->getObjectType() != BaseObject::eActor)
            return;

        Actor* pActor = (Actor*)pObj;
/*
        if(EDITOR->filterByZRange())
            if(EDITOR->getWorkRangeMinZ() > pActor->getDepth() || pActor->getDepth() > EDITOR->getWorkRangeMaxZ())
                return;
*/
#if 0
        // Do not display shapes for actors bound by actorSpawnComponent (can also be bound by ActorBind or StickToPolyline...)
        if(pActor->getBinding()->isBound())
        {
            const ObjectRef updateParentRef = pActor->getBinding()->getParent();
            if(Pickable* pUpdateParent = (Pickable*)updateParentRef.getObject())    // If the parent exists
                if(pUpdateParent->getObjectType() == BaseObject::eActor)            // If it's an actor
                    if(pActor->getParent() != updateParentRef)                      // If not binded
                        return;
        }
#endif // 0

        if (!EDITOR->checkObjectVisible(_or))
            return;

        u32 basePrio = 1;
        if(pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) != NULL)
            basePrio = 0;

        bbool someoneIsSubEditedAndThatsNotMe = bfalse;
        bbool bSubEdited = EDITOR->isSubEdited(pActor->getRef());
        if (!bSubEdited && EDITOR->getSubEditedItem().getObject())
            someoneIsSubEditedAndThatsNotMe = btrue;

        if (someoneIsSubEditedAndThatsNotMe)
            basePrio = PickingShape::m_UnselectablePriority;

        _objects.push_back(pObj->getRef());

        const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_or);
        ITF_ASSERT(Editor::is2DSelection() || (shapes && shapes->size() >= 1));
        if (shapes)
        {
            //search the main anchor,
            for (u32 iShape = 0; iShape < shapes->size(); iShape++)
            {
                PickingShape* pShape = (*shapes)[iShape];

                if(!pActor->hasDataError())
                    if(Interface_ActorEditorListener* pListenerCreator = static_cast<Interface_ActorEditorListener*>(pShape->getCreatorPickingShapePlugin()))
                        pListenerCreator->updateComponentSpecificPickingShape(pActor, pShape, _orderedShapes);

                if (!pShape->isSubAnchor())
                {
                    PickingShape_Disc* pick = (PickingShape_Disc*)pShape;
                    ITF_ASSERT(pick);
                    if (pick)
                    {
                        switch (pick->getPosType())
                        {
                        case PickingShape_Disc::Common:
                        case PickingShape_Disc::InitPosFromCurrent:
                            if(pActor->is2DActor())
                            {
                                Vec3d actor3DPos ;
                                Vec3d actor2DPos = pActor->get2DPos().to3d();
                                GFX_ADAPTER->compute2DTo3D(actor2DPos, actor3DPos);
                                pick->setPos(actor3DPos);
                            } else
                            {
                                pick->setPos(pActor->getPos());
                            }
                            break;

                        default:
                            ITF_ASSERT(0);
                            break;
                        }

                        //Update its picking shape and add to pickable list
                        pick->setlastUpdateFrame(CURRENTFRAME);
                        pActor->setPickingInfo(pick->getPickingInfoPtr());
                        pick->setPriority(basePrio);
                        if(EditorDraw::PickableDetail_CanShowAnchors() || pActor->isSelected() || getInstance()->hasActorInfoFlag(ActorInfoFlag_Any) || Editor::is2DSelection())
                            _orderedShapes.push_back(pick);
                    }
                }
            }
        }
    }

    void ActorEditor::fillEditorActiveList( ObjectRefList &_pickableObjects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene) const
    {
        const World* pWorld = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));

        if (!pWorld)
            return;

        if (!Editor::is2DSelection())
        {
            if (SubEdit == m_state && m_selectionInfoList.size() && m_selectionInfoList[0].m_targetRef == EDITOR->getSubEditedItem())
            {
                Actor* selectedActor = (Actor*)GETOBJECT(m_selectionInfoList[0].m_targetRef);
                ITF_ASSERT_MSG(selectedActor, "If selection is not empty so there is a selection !");
                if(!selectedActor)
                    return;

                if (!selectedActor->GetComponent<LinkComponent>() && !selectedActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                {
                    updateActorPickingShape(m_selectionInfoList[0].m_targetRef, _pickableObjects, _orderedShapes);
                    return;
                }
            }

            // Add players except in sub scene edition
            // Because even if the picking shape is displayed, players can't be selected in this mode
            if(SubEdit != m_state)
            {
                const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
                for(u32 i = 0; i < uPlayerCount; ++i)
                {
                    const Actor* pPlayer = GAMEMANAGER->getActivePlayer(i);
                    if(!pPlayer)
                        continue;
                    updateActorPickingShape(pPlayer->getRef(), _pickableObjects, _orderedShapes);
                }
            }
        }

        // add scene objects
        static BaseObjectList pObjList;
        pObjList.clear();

        if (!Editor::is2DSelection())
        {
            if(_pFilterScene)
                pObjList = _pFilterScene->getActiveObjects();
            else
            {
                Scene* pRoot = pWorld->getRootScene();
                pObjList.mergeArray(pRoot->getActiveObjects());

                // Recursive feed the while monster
                ObjectRefList ssas = pRoot->getSubSceneActors();
                for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                    if(BaseObject* pBO = ssas[iSSA].getObject())
                        if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            if(Scene* pSubScene = pSSA->getSubScene())
                                if(EDITOR->allowEditSubScene(pSSA))
                                {
                                    pObjList.mergeArray(pSubScene->getActiveObjects());
                                    ssas.mergeArray(pSubScene->getSubSceneActors()); // feed the loop
                                }
            }

            // For each selected ssa|grp display content and selected actors and persistant subscene
            ObjectRefList addDisplay = EDITOR->getEditorSelectionAsObjRefList();
            addDisplay.mergeArray(EDITOR->getSubSceneDisplayList());

            const u32 uAddDisplaySize = addDisplay.size();
            for(u32 iList = 0; iList < uAddDisplaySize; ++iList)
            {
                ObjectRef ownerRef = addDisplay[iList];
                if(Pickable* pObj = static_cast<Pickable*>(ownerRef.getObject()))
                {
                    switch(pObj->getObjectType())
                    {
                        case BaseObject::eActor:
                            if(SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            {
                                if(Scene* pSubScene = pSSA->getSubScene())
                                {
                                    if(EDITOR->allowEditSubScene(pSSA))
                                    {
                                        const BaseObjectList& subList = pSubScene->getActiveObjects();
                                        SafeArray<BaseObject*> resolvedRefs;
                                        ID_SERVER->getObjectListNoNullV2(subList, &resolvedRefs);

                                        for(u32 iSub = 0; iSub < resolvedRefs.size(); ++iSub)
                                        {
                                            BaseObject* pSubObj = resolvedRefs[iSub];
                                            switch(pSubObj->getObjectType())
                                            {
                                            case BaseObject::eActor:
                                                pObjList.addObject(pSubObj->getRef(), bfalse);
                                                break;

                                            default:
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                pObjList.addObject(ownerRef, bfalse);
                            }
                        break;

                        default:
                        break;
                    }
                }
            }

            pObjList.mergeArray(EDITOR->getExtraSelectableObjects());


            for (u32 i = 0; i < m_listeners.size(); i++)
            {
                const ActorEdListener& listener = m_listeners[i];
                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                {
                    listener.m_listener->fillEditorActiveList (_pickableObjects, _orderedShapes, _pFilterScene);
                }
            }

            // add active camera controllers
    //         const SafeArray<CameraControllerData>& camControl = CAMERACONTROLLERMANAGER->getCameraControllerList();
    //         for (u32 i = 0; i < camControl.size(); i++)
    //         {
    //             CameraControllerData& data = camControl[i];
    //             if (data.m_controller)
    //             {
    //                 ObjectRef actor = data.m_controller->getActorRef();
    //                 if (actor.getObject())
    //                     pObjList.addObject(actor, bfalse);
    //             }
    //         }
            }

        if (Editor::is2DSelection())
        {
            ObjectRefList list2D;
            WORLD_MANAGER->getAll2DObjects(list2D, btrue);
            pObjList.mergeArray(list2D);
        }

        for (u32 iThread = 0; iThread < pObjList.m_threadIds.size(); iThread++)
        {
            SafeArray<ObjectRef>& refs = pObjList.m_refs[iThread];
            const u32 uCount = refs.size();
            for(u32 i = 0; i < uCount; ++i)
                updateActorPickingShape(refs[i], _pickableObjects, _orderedShapes);
        }
    }

    PickingShapePluginData  *ActorEditor::createPickingShapeData(PickingShape *_picking)
    {
        if (_picking && _picking->getOwnerPtr() && _picking->getOwnerPtr()->getObjectType()==BaseObject::eActor)
        {
            return newAlloc(mId_Editor, PickingShapeData(_picking, this));
        }
        else
            return NULL;
    }

    bbool ActorEditor::isPickingAllowed()
    {
        switch(m_state)
        {
        case CreateActor:
        case RotationScaleHelperMode:
        case SetSubScenePivot:
        case CreateBind:
            return bfalse;

        default:
            return btrue;
        }
    }

    void ActorEditor::onFileDrop (const String& _absoluteFileName)
    {
        String extension                        = FilePath::getExtension(_absoluteFileName);
        String relativeFileName                 = _absoluteFileName;
        const bbool dropFromOutsideDatabank     = !FilePath::transformToRelative(relativeFileName);
        bbool warnUserDropNotAllowedFromOutside = dropFromOutsideDatabank;
        bbool handledType                       = bfalse;

        if ( extension == "act" && !dropFromOutsideDatabank )
        {
            handledType = btrue;

            // don't transform to relative path here, it's done in createActor
            enterCreateActorState(_absoluteFileName);
        }
        else if  (extension == "tga" || extension == "png")
        {
            handledType = btrue;

            if(dropFromOutsideDatabank)
            {
                warnUserDropNotAllowedFromOutside = bfalse;

                if(SYSTEM_ADAPTER->messageBox("Drop source error", "The dropped file is not from the databank.\n Do you want to add the file to the databank and continue ?", ITF_MSG_ICONQUESTION | ITF_MSG_YESNO,ITF_IDNO) != ITF_IDYES)
                    return;

                Path newPath;
                if(EDITOR->copyFileToDataFolder(_absoluteFileName, newPath))
                {
                    newPath.getString(relativeFileName);
                }
                else
                    return;
            }

            m_lastDragnDropFileName_Relative = relativeFileName;

            EDITOR->resetcontextMenus();
            EditorContextMenu& menu = EDITOR->getContextMenu();

            menu.addItem(EditorContextMenu::ItemId_AddTextureAsActor, "Insert as actor");
            menu.addItem(EditorContextMenu::ItemId_AddTextureAsFrieze, "Insert as frieze");

            EDITOR->forceShowContextMenu(btrue);
        }
        else if (extension == "anm" && !dropFromOutsideDatabank)
        {
            handledType = btrue;

            String luaPath = FILESERVER->getWorkingDepot() + GETPATH_ALIAS("tpl_AnimLightActor_Spawn");
            Actor * newActor = newAlloc(mId_Gameplay, Actor);

            newActor->setLua(luaPath);
            AnimLightComponent * animL = newAlloc(mId_Editor, AnimLightComponent);
            animL->setAnimInstance(relativeFileName);
            newActor->generateNewUserFriendly(String8(FilePath::getFilenameWithoutExtension(luaPath).cStr()));
            newActor->AddComponent(animL);

            SF_DEL(m_createActorBuffer);

            m_createActorBuffer = newAlloc(mId_Editor, ArchiveMemory);

            CSerializerObjectBinary serializer(m_createActorBuffer);
            newActor->Serialize(&serializer,ESerialize_Data_Save);

            delete newActor;

            enterCreateActorState(luaPath,btrue);
        }

        if(handledType && warnUserDropNotAllowedFromOutside)
        {
            ITF_ERROR("Drop from outside the data root folder is not allowed for this file type.\nFile : '%ls'", _absoluteFileName.cStr());
        }
    }

    bbool isActorEditableWithGenAnim(Actor* _act)
    {
        AnimLightComponent* pComp  = _act->GetComponent<AnimLightComponent>();
        return pComp && pComp->getSubAnimCount();
    }

    bbool isActorEditableWithSequenceEditor(Actor* _act)
    {
        SequencePlayerComponent* pComp  = _act->GetComponent<SequencePlayerComponent>();
        return pComp != NULL;
    }

    bbool ActorEditor::fillContextMenu(EditorContextMenu& _menu, ObjectRef _target, bbool _topItems)
    {
        if (m_state == CreateActor || m_state == CreateScene)
            _menu.addItem(EditorContextMenu::ItemId_ActorDone, "Done");

        BaseObject* tgt = GETOBJECT(_target);
        if (!tgt)
            return btrue;


        Actor* actor = tgt->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        const bbool inError = static_cast<Pickable*>(tgt)->hasDataError();
        const bbool targetIsSubScene = tgt->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) != NULL;

        if (_topItems)
        {
            if (actor)
            {
                _menu.addSubItem(EditorContextMenu::ItemId_FindInExplorer, "Find in data bank", EditorContextMenu::ItemId_File, "File");

                if(!targetIsSubScene)
                    _menu.addSubItem(EditorContextMenu::ItemId_ReplaceActor, "Replace template", EditorContextMenu::ItemId_File, "File");

                if(!inError)
                {
                    _menu.addSubItem(EditorContextMenu::ItemId_LaunchFromExplorer, "Edit in OS", EditorContextMenu::ItemId_Edit, "Edit");
                    _menu.addSubItem(EditorContextMenu::ItemId_ActorEditor, "ActorEditor", EditorContextMenu::ItemId_Edit, "Edit");
                    _menu.addSubItem(EditorContextMenu::ItemId_PropertiesEditor, "Properties Editor (Ctrl+Q)", EditorContextMenu::ItemId_Edit, "Edit");
                    if (isActorEditableWithGenAnim((Actor*)tgt))
                        _menu.addSubItem(EditorContextMenu::ItemId_Genanim, "Edit Animation", EditorContextMenu::ItemId_Edit, "Edit");

                    if (isActorEditableWithSequenceEditor((Actor*)tgt))
                        _menu.addSubItem(EditorContextMenu::ItemId_SequenceEditor, "Edit Sequence", EditorContextMenu::ItemId_Edit, "Edit");

                    if (SubSceneActor* pSubsc = tgt->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(pSubsc->isZForced())
                            _menu.addItem(EditorContextMenu::ItemId_SubSceneActorZForcedToggleLock, "Unlock zForce");
                        else
                            _menu.addItem(EditorContextMenu::ItemId_SubSceneActorZForcedToggleLock, "Lock zForce");

                        if (pSubsc->isSinglePiece())
                            _menu.addSubItem(EditorContextMenu::ItemId_SetMultiplePieces, "Unset as synchronized content", EditorContextMenu::ItemId_Edit, "Edit");
                        else
                            _menu.addSubItem(EditorContextMenu::ItemId_SetSinglePiece, "Set as synchronized content", EditorContextMenu::ItemId_Edit, "Edit");
                    }
                   // _menu   = &_menu.addItem(EditorContextMenu::ItemId_More, "Sampler Test")->m_subMenu;
#ifdef DAN_TEXTURE_SELECTOR
                    _menu.addSubItem(EditorContextMenu::ItemID_FindFriseLocation, "Find File",EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker");
                    _menu.addSubItem(EditorContextMenu::ItemId_ReCook_RSampler, "ReCook Rsampler",EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker");
                    _menu.addSubItem(EditorContextMenu::ItemId_ReCook_Sampler, "ReCook Sampler",EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker");
                    _menu.addSubItem(EditorContextMenu::ItemId_ReCook_USampler, "ReCook USampler",EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker");
                    _menu.addSubItem(EditorContextMenu::ItemId_ReCook_ESRGAN, "ReCook ESRGAN",EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker");
                    _menu.addSubItem(EditorContextMenu::ItemID_ReCook_Reset, "ReCook Reset",EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker");
#endif
                }
            }
            if (tgt->getObjectType()== BaseObject::eScene && GAMEMANAGER->isPlayableScreen())
            {
                _menu.addSubItem(EditorContextMenu::ItemId_GenericInsert, "Insert", EditorContextMenu::ItemId_File, "File");
                _menu.addSubItem(EditorContextMenu::ItemId_InsertTodo, "Todo", EditorContextMenu::ItemId_Create, "Create");

                _menu.addSubItem(EditorContextMenu::ItemId_FindInExplorer, "Find scene in databank", EditorContextMenu::ItemId_File, "File");
                _menu.addSubItem(EditorContextMenu::ItemId_SceneExportAsScene, "as Scene", EditorContextMenu::ItemId_SceneExport, "Export");
                _menu.addSubItem(EditorContextMenu::ItemId_SceneExportAsGroup, "as Group", EditorContextMenu::ItemId_SceneExport, "Export");
                return btrue;
            }
        }
        else if (actor)
        {
            if(!inError)
            {
                for (u32 i = 0; i < m_listeners.size(); i++)
                {
                    ActorEdListener& listener = m_listeners[i];
                    for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                    {
                        ActorComponent* pComp = actor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                        if (pComp)
                            listener.m_listener->fillContextMenu (_menu, actor, pComp);
                    }
                }
            }

            if ( EDITOR->getSelectionSize() == 1 )
            {
                _menu.addItem(EditorContextMenu::ItemId_CopyObjectPathToClipboard, "Copy ObjectPath to clipboard");

                if(!inError)
                {
                    const bbool parentBinds = actor->getParentBind() != NULL;
                    const bbool childrenBinds = actor->getChildrenBindHandler().getChildren().size() > 0;

                    if(parentBinds || childrenBinds)
                    {
                        ContextMenuItem* pMenu = _menu.addItem(EditorContextMenu::ItemId_ClearBinds, "Clear binds");

                        if(parentBinds)
                            pMenu->m_subMenu.addItem(EditorContextMenu::ItemId_ClearBindToParent, "Clear bind to parent");
                        if(childrenBinds)
                            pMenu->m_subMenu.addItem(EditorContextMenu::ItemId_ClearBindsToChildren, "Clear binds to children");
                    }
                }

                _menu.addItem(EditorContextMenu::ItemId_EnterCreateBind, "Bind mode (ctrl + alt)");

                //_menu.addItem(EditorContextMenu::ItemId_ExportActorInstance, "Export instance");

                if(SubSceneActor* pSSA = tgt->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                {
                     if(EDITOR->allowEditSubScene(pSSA))
                    {
                        Plugin* plugin = PLUGINGATEWAY->getExclusiveFocus();
                        if(!plugin || plugin == this)
                            _menu.addItem(EditorContextMenu::ItemId_SetSubSceneActorPivot, "Set SubScene pivot");

                        _menu.addItem(EditorContextMenu::ItemId_EnterSubSceneEdition, "Enter subscene (dbl click)");

                        _menu.addItem(EditorContextMenu::ItemId_SetWorkingSubscene, "Lock working subscene");
                    }
                    else
                    {
                        _menu.addItem(EditorContextMenu::ItemId_OpenTsc, "Open TSC");
                    }

                    String msg;
                    if(!pSSA->getEmbedScene())
                        msg = "Transform to group";
                    else
                        msg = "Transform to reference";
                    _menu.addItem(EditorContextMenu::ItemId_SubSceneActor_ToggleGroupExternal, msg);

                    if(pSSA->getEmbedScene())
                        _menu.addItem(EditorContextMenu::ItemId_SubSceneActor_SaveAsTSC, "Save as template scene (tsc)");
                    else
                        _menu.addItem(EditorContextMenu::ItemId_ReplaceIscAtOriginAndKeepOtherPlacedRelatively, "Place this isc at origin and moves others elements");
                }
            }
        }

        if(EDITOR->getWorkingSubScene().isValid())
            _menu.addItem(EditorContextMenu::ItemId_UnsetWorkingSubscene, "Unlock working subscene");

        return btrue;
    }

    void ActorEditor::fillAltContextMenu(EditorContextMenu& _menu)
    {
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            ActorEdListener& listener = m_listeners[i];
            for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
            {
                listener.m_listener->fillAltContextMenu(_menu);
            }
        }
    }

    bbool ActorEditor::replaceOneActorTemplate(Actor* _target, const String& _path)
    {
        if (!FILEMANAGER->fileExists(_path))
        {
            ITF_ERROR("Can't change actgor template, file not found:\n%ls ", _path.cStr());
            return bfalse;
        }

        // Do not replace template of subscene actors
        if(_target->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) != NULL)
            return bfalse;

        UNDOREDO_MANAGER->preChange(_target);

        if(EDITOR)
            EDITOR->deletePickingShapesForObject(_target->getRef());

        String prevLua;
        _target->getLua().getString(prevLua);
        const String8 prevBaseName = FilePath::getFilenameWithoutExtension(prevLua).cStr();
        const String8 previousName = _target->getUserFriendly();

        LUA_HANDLER->unregisterLUAFileUsage(_target->getRef(), prevLua.cStr());
        _target->setLua(_path);
        LUA_HANDLER->registerLUAFileUsage(_target->getRef(), _path.cStr());

        _target->oneStepHotReload(btrue); // skip instance data

        // Only rename if default basename found in friendly
        if(previousName.strstr(prevBaseName.cStr()))
        {
            _target->generateNewUserFriendly(FilePath::getFilenameWithoutExtension(_path).cStr());
            PLUGINGATEWAY->onPickableRenamed(_target, previousName);
        }
        PLUGINGATEWAY->onObjectChanged(_target);

        createPickingShapeForActor(_target->getRef());

        return btrue;
    }

    void ActorEditor::onMainMenuItemSelected(ContextMenuItem* _item)
    {
        if (_item->isExternal())
        {
            for (u32 i = 0; i < m_listeners.size(); i++)
            {
                ActorEdListener& listener = m_listeners[i];
                if (!listener.m_isExternal)
                    continue;
                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                {
                    listener.m_listener->onMainMenuItemSelected (_item);
                }
            }
        }
    }


    void ActorEditor::onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget)
    {
        Actor* actor = NULL;
        BaseObject* pTarget = _menuTarget.getObject();
        if (pTarget && pTarget->getObjectType() == BaseObject::eActor)
            actor = (Actor*)pTarget;

        if (_item->isExternal())
        {
            if (actor)
            {
                for (u32 i = 0; i < m_listeners.size(); i++)
                {
                    ActorEdListener& listener = m_listeners[i];
                    if (!listener.m_isExternal)
                        continue;
                    for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                    {
                        ActorComponent* pComp = actor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                        if (pComp)
                            listener.m_listener->onContextMenuItemSelected (_item, actor, pComp);
                    }
                }
            }
            else
            {
                // alt (debug) menu
                for (u32 i = 0; i < m_listeners.size(); i++)
                {
                    ActorEdListener& listener = m_listeners[i];
                        listener.m_listener->onAltContextMenuItemSelected(_item);
                    }
                }
            return;
        }
        else
        {
            if (actor)
            {
                for (u32 i = 0; i < m_listeners.size(); i++)
                {
                    ActorEdListener& listener = m_listeners[i];
                    if (listener.m_isExternal)
                        continue;
                    for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                    {
                        ActorComponent* pComp = actor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                        if (pComp)
                            listener.m_listener->onContextMenuItemSelected (_item, actor, pComp);
                    }
                }
            }
        }

        switch(_item->getId())
        {
            case EditorContextMenu::ItemId_Genanim:
            {
                if (actor)
                {
                    AnimLightComponent* pComp  = actor->GetComponent<AnimLightComponent>();
                    if (pComp)
                    {
                        PathString_t animPathName;
                        pComp->getAnimPath().getString(animPathName);
                        String animPath = DATA_FOLDER;
                        animPath  += animPathName;
                        const SubAnim* pAnim = pComp->getCurSubAnim(0);
                        if (!pAnim)
                            pAnim = pComp->getSubAnim(0);
                        f32 time = pComp->getCurTime(0);
                        if (pAnim)
                        {
                            pAnim->getAnimPath().getString(animPathName);
                            animPath += animPathName;
                            ITF_VECTOR<String> vFiles;
                            if (Directory::searchFiles(vFiles, FilePath::getDirectory(animPath), "*.asc", bfalse , NULL))
                            {
                                if (vFiles.size())
                                {
                                    String param = "\"";
                                    String timestr;
                                    timestr.setTextFormat("%i", (u32)timeToFrame(time, ANIMATION_FRAMERATE)+1);
                                    param += vFiles[0];
                                    param += "\"";

                                    param += " \"";
                                    param += FilePath::getFilename(animPath);
                                    param += "\"";

                                    param += " \"";
                                    param += timestr;
                                    param += "\"";

                                    SYSTEM_ADAPTER->launchExe("..\\GenAnim\\launchScn.bat", StringConverter(param).getChar(), btrue, bfalse);
                                }
                            }
                        }
                    }
                }
            }
            break;

            case EditorContextMenu::ItemId_SequenceEditor:
                {
                    SYSTEM_ADAPTER->launchExe("UA_SequenceEditor.exe", "", bfalse, btrue);
                }
                break;

            case EditorContextMenu::ItemId_SetSinglePiece:
                if (actor)
            {
                    SubSceneActor* pSubsc = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
                    if (pSubsc)
                        pSubsc->setSinglePiece(btrue);
                }
                break;

            case EditorContextMenu::ItemId_SetMultiplePieces:
                if (actor)
                {
                    SubSceneActor* pSubsc = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
                    if (pSubsc)
                        pSubsc->setSinglePiece(bfalse);
                }
                break;

            case EditorContextMenu::ItemId_AddTextureAsActor:
            {
                String luaPath = FILESERVER->getWorkingDepot() + "Actor/Template/tpl_TextureGraphicComponent.act";
                Actor * newActor = newAlloc(mId_Gameplay, Actor);

                newActor->setLua(luaPath);
                TextureGraphicComponent * gc = newAlloc(mId_Editor, TextureGraphicComponent);
                gc->setTextureFile(m_lastDragnDropFileName_Relative);
                newActor->generateNewUserFriendly(String8(FilePath::getFilenameWithoutExtension(luaPath).cStr()));
                newActor->AddComponent(gc);

                SF_DEL(m_createActorBuffer);

                m_createActorBuffer = newAlloc(mId_Editor, ArchiveMemory);

                CSerializerObjectBinary serializer(m_createActorBuffer);
                newActor->Serialize(&serializer,ESerialize_Data_Save);

                delete newActor;

                enterCreateActorState(luaPath,btrue);
            }
            break;

            case EditorContextMenu::ItemId_AddTextureAsFrieze:
                break;

            case EditorContextMenu::ItemId_ActorDone:
                cancelCurrentAction(bfalse);
            break;

            case EditorContextMenu::ItemId_InsertTodo:
            {
                String path;
                FILEMANAGER->TranslatePath(path, "Actor/LogicActor/TODO.act");
                enterCreateActorState(path);
            }
            break;

            case EditorContextMenu::ItemId_SceneExportAsScene:
            {
                String path = "*.isc";
                String initialPath = DATA_FOLDER;
                if(!FILEMANAGER->fileChooser(btrue, path, L"Scene (*.isc)\0*.isc\0", initialPath))
                    return;
            }
            break;

            case EditorContextMenu::ItemId_SceneExportAsGroup:
            {
                String path = "*.grp";
                String initialPath = DATA_FOLDER;
                if(!FILEMANAGER->fileChooser(btrue, path, L"Group (*.grp)\0*.grp\0", initialPath))
                    return;
            }
            break;


            case EditorContextMenu::ItemId_ReplaceActor:
                {
                    BaseObject* pTarget = _menuTarget.getObject();
                    if (pTarget && pTarget->getObjectType() == BaseObject::eActor)
                    {
                        Actor* target = (Actor*)pTarget;
                        String path;
                        target->getLua().getString(path);
                        path = FilePath::getDirectory(path);
                        path += "*.act";
                        if (FILEMANAGER->fileChooser(bfalse, path, L"Actor template (*.act)\0*.act\0"))
                        {
                            UndoRedoSequenceScope scope("ItemId_ReplaceActor");

                            FilePath::transformToRelative(path);
                            replaceOneActorTemplate(target, path);
                            if (EDITOR->getSelectionSize() > 1)
                            {
                                for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
                                {
                                    PickingShape* shape = EDITOR->getSelectionAt(iSel);
                                    if (shape)
                                    {
                                        BaseObject* owner = shape->getOwnerPtr();
                                        if (owner && owner->getObjectType() == BaseObject::eActor)
                                        {
                                            replaceOneActorTemplate((Actor*)owner, path);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;

            case EditorContextMenu::ItemId_ActorEditor:
                {
                    String8 command;
                    command.setTextFormat("%u", EDITOR->getContextMenuTarget().getValue());
                    SYSTEM_ADAPTER->launchExe("UA_ActorCreate.exe", command.cStr(), bfalse, btrue);
                }
            break;

            case EditorContextMenu::ItemId_CopyObjectPathToClipboard:
            {
                copyObjectPathToClipboard();
            }
            break;

            case EditorContextMenu::ItemId_ClearBinds:
            {
                unbindActor(actor, btrue, btrue);
            }
            break;

            case EditorContextMenu::ItemId_ClearBindToParent:
            {
                unbindActor(actor, btrue, bfalse);
            }
            break;

            case EditorContextMenu::ItemId_ClearBindsToChildren:
            {
                unbindActor(actor, bfalse, btrue);
            }
            break;

            case EditorContextMenu::ItemId_EnterCreateBind:
            {
                ITF_ASSERT_CRASH(actor, "Invalid context callback");
                enterCreateBind(actor);
            }
            break;

            case EditorContextMenu::ItemId_SetSubSceneActorPivot:
            {
                if(PickingShape* pShape = EDITOR->getSelectionAt(0))
                {
                    if(SubSceneActor* pSSA = pShape->getOwnerPtr()->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(pSSA->isZForced() && DEBUGINFO)
                        {
                            String userInfo = "The selected subscene is zForced!";
                            DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
                        }
                        PLUGINGATEWAY->setExclusiveFocus(this);
                        m_state = SetSubScenePivot;
                        m_setSubScenePivotActor = pSSA->getRef();
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
                else
                {
                    ITF_ASSERT(0);
                }
            }
            break;

            case EditorContextMenu::ItemId_OpenTsc:
            {
                if (actor)
                {
                    if(SubSceneActor* pSSA = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        PathString_t pathName;
                        pSSA->getSceneRelativePath().getString(pathName);
                        EDITOR->setPendingDragAndDropMap(pathName);
                    }
                }
            }
            break;

            case EditorContextMenu::ItemId_EnterSubSceneEdition:
            {
                if(PickingShape* pShape = EDITOR->getSelectionAt(0))
                {
                    if(SubSceneActor* pSSA = pShape->getOwnerPtr()->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        enterSubEdit(pSSA->getRef());
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
                else
                {
                    ITF_ASSERT(0);
                }
            }
            break;

            case EditorContextMenu::ItemId_SubSceneActor_ToggleGroupExternal:
            {
                if (actor)
                {
                    if(SubSceneActor* pSSA = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(pSSA->getEmbedScene())
                        {
                            String scenePath(pSSA->getUserFriendly() + ".isc");
                            if(FILEMANAGER->fileChooser(btrue, scenePath, L"All supported files (*.isc;*.tsc)\0*.isc;*.tsc;\0Scene (*.isc)\0*.isc\0Subscene (*.tsc)\0*.tsc\0"))
                            {
                                String relativePath;
                                if (FILESERVER->getPathFromDataFolder(relativePath, scenePath))
                                {
                                    const bbool bFileExistBeforeSave = FILEMANAGER->fileExists(scenePath);
                                    if(SOURCECONTROL_ADAPTER && bFileExistBeforeSave)
                                        SOURCECONTROL_ADAPTER->checkout(scenePath);

                                    Scene* subscene = pSSA->getSubScene();
                                    if(XMLAll::saveScene(scenePath, subscene, bfalse))
                                    {
                                        UNDOREDO_MANAGER->preChange(pSSA);

                                        pSSA->setSceneRelativePath(relativePath);
                                        pSSA->setEmbedScene(bfalse);
                                        subscene->setPath(relativePath);

                                        EDITOR->setSceneAsModified(subscene);
                                        EDITOR->setSceneAsModified(pSSA->getScene());

                                        if(SOURCECONTROL_ADAPTER && !bFileExistBeforeSave)
                                            SOURCECONTROL_ADAPTER->add(scenePath);
                                    }
                                    else
                                    {
                                        ITF_ERROR("Failed to save here: %ls", scenePath.cStr());
                                    }
                                }
                                else
                                {
                                    ITF_ERROR("Choose a target file into the data folder: %ls", DATA_FOLDER.cStr());
                                }
                            }
                        }
                        else
                        {
                            UNDOREDO_MANAGER->preChange(pSSA);

                            pSSA->setEmbedScene(btrue);
                            EDITOR->setSceneAsModified(pSSA->getScene());
                        }
                    }
                }
                else
                {
                    ITF_ASSERT(0);
                }
            }
            break;

            case EditorContextMenu::ItemId_ReplaceIscAtOriginAndKeepOtherPlacedRelatively:
            {
                if(actor)
                {
                    if(SubSceneActor* pSSA = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(!pSSA->getEmbedScene())
                        {
                            const Vec3d refLocalPos = pSSA->getLocalPos();

                            Scene* pScene = pSSA->getScene();

                            UndoRedoSequenceScope unbindScope("ReplaceIscAtOriginAndKeepOtherPlacedRelatively scope");
                            for(u32 i = 0; i < pScene->getPickableList().size(); ++i)
                            {
                                Pickable* pObj = pScene->getPickableList()[i];

                                UNDOREDO_MANAGER->preChange(pObj);
                                pObj->setLocalPos(pObj->getLocalPos() - refLocalPos);
                                pObj->setWorldInitialPos(pObj->getPos());
                                PLUGINGATEWAY->onObjectChanged(pObj);
                            }
                            EDITOR->setSceneAsModified(pScene);
                        }
                    }
                }
                break;
            }

            case EditorContextMenu::ItemId_SubSceneActor_SaveAsTSC:
            {
                if (actor)
                {
                    if(SubSceneActor* pSSA = actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(pSSA->getEmbedScene())
                        {
                            String scenePath(pSSA->getUserFriendly() + ".tsc");
                            if(FILEMANAGER->fileChooser(btrue, scenePath, L"Template scene (*.tsc)\0*.tsc\0"))
                            {
                                Scene* subscene = pSSA->getSubScene();

                                if(!XMLAll::saveScene(scenePath, subscene, bfalse))
                                {
                                    ITF_ERROR("Couldn't save the file: '%ls'", scenePath.cStr());
                                }
                            }
                        }
                    }
                }
            }
            break;

#if 0
            case EditorContextMenu::ItemId_ExportActorInstance:
            {
                String path;
                if (FILEMANAGER->fileChooser(btrue, path, L"Actor instance (*.aci)\0*.aci\0"))
                {
                    /*
                    CSerializerObjectLua serializer;
                    if ( serializer.OpenToWrite(val) )
                    {
                        obj->Serialize(&serializer,ESerialize_Instance_Save);
                        serializer.Close();
                    }
                    */
                    /*
                    String pathRel = path;
                    FilePath::transformToRelative(pathRel);

                    TiXmlDocument doc;
                    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
                    doc.LinkEndChild( decl );
                    TiXmlElement * root = new TiXmlElement( "root" );
                    doc.LinkEndChild( root );
                    CSerializerObjectParser parser(root);
                    actor->Serialize(&parser,ESerialize_Instance_Save);
                    doc.SaveFile( StringConverter(path).getChar() );
                    */
                }
            }
            break;
#endif
            case EditorContextMenu::ItemId_SubSceneActorZForcedToggleLock:
            {
                if (actor)
                    toggleSubSceneLock(static_cast<SubSceneActor*>(actor));
            }
            break;

            case EditorContextMenu::ItemId_Actor_CopyFogProperties:
            {
                pickFogColor();
            }
            break;

            case EditorContextMenu::ItemId_Actor_PasteFogProperties:
            {
                pasteFogColor();
            }
            break;
#ifdef DAN_TEXTURE_SELECTOR
        case EditorContextMenu::ItemID_FindFriseLocation:
            {
                ITF_VECTOR<String> paths = GetTexturePathFromFrise(actor);
                for (u32 u = 0; u < paths.size();u++)
                {
                    String& path = paths[u];
                    if (FILEMANAGER->fileExists(path))
                    {
                        String fullPath;
                        if(FILEMANAGER->TranslatePath(fullPath, path))
                        {
                            fullPath.replace('/', '\\');
                            SystemAdapter::findInExplorer(fullPath);
                        }
                    }
                }
                break;
            }
        case EditorContextMenu::ItemId_ReCook_USampler:
                RecookWithSampler(actor,samplerFiles[SAMPLER_TYPE::USAMPLER]);
                break;
            case EditorContextMenu::ItemId_ReCook_RSampler:
                RecookWithSampler(actor,samplerFiles[SAMPLER_TYPE::RSAMPLER]);
                break;
            case EditorContextMenu::ItemId_ReCook_Sampler:
                {
                    RecookWithSampler(actor,samplerFiles[SAMPLER_TYPE::SAMPLER]);
                    break;
                }
            case EditorContextMenu::ItemId_ReCook_ESRGAN:
                {
                    RecookWithSampler(actor,samplerFiles[SAMPLER_TYPE::ESRGAN]);
                    break;
                }
            case EditorContextMenu::ItemID_ReCook_Reset:
            {
                RecookReset(actor);
                break;
            }
#endif

            default:
            break;
        }
    }
#ifdef DAN_TEXTURE_SELECTOR
    String              ActorEditor::samplerFiles[] = { SAMPLERSEQUENCE};
    String              ActorEditor::samplerExtensions[] = { TEXTUREFORMATS };
    void ActorEditor::RecookWithSampler(Actor* target,String& sampler)
    {
        if (target == nullptr) return;
        RecookReset(target);
        Vector<String> paths = GetTexturePathFromFrise(target);
        for (u32 u = 0; u < paths.size();u++)
        {
            if (FILEMANAGER->fileExists(paths[u]))
            {

                String folderPath = FilePath::getDirectory(paths[u]);
                String fileNameWithoutExtension =FilePath::getFilenameWithoutExtension(paths[u]);
                for (const String& extension : samplerExtensions)
                {
                    for (const String& samplerfile : samplerFiles)
                    {
                        if (sampler.equals(samplerfile,true)) continue;
                        String newPath;
                        FILESERVER->getAbsolutePath(folderPath + fileNameWithoutExtension +"_"+ samplerfile +"."+extension,newPath);
                        renameFile(newPath,newPath+".delete",false);
                    }
                }

                FILEMANAGER->flushTimeWriteAccess(paths[u],true);
            }
        }
        PLUGINGATEWAY->onObjectChanged(target);
    }
    void ActorEditor::RecookReset(Actor* target)
    {
        if (target == nullptr) return;
        Vector<String> paths = GetTexturePathFromFrise(target);
        for (u32 u = 0; u < paths.size();u++)
        {
            if (FILEMANAGER->fileExists(paths[u]))
            {

                String folderPath = FilePath::getDirectory(paths[u]);
                String fileNameWithoutExtension =FilePath::getFilenameWithoutExtension(paths[u]);
                for (const String& extension : samplerExtensions)
                {
                    for (const String& samplerfile : samplerFiles)
                    {
                        String relativePath,relativeDeletedPath;
                        relativePath =folderPath + fileNameWithoutExtension +"_"+ samplerfile+"." + extension;
                        relativeDeletedPath = relativePath + ".delete";
                        if (FILEMANAGER->fileExists(relativeDeletedPath))
                        {
                            String asoluteDeletePath,originPath;
                            FILESERVER->getAbsolutePath(relativeDeletedPath,asoluteDeletePath);
                            FILESERVER->getAbsolutePath(relativePath,originPath);
                            renameFile(asoluteDeletePath,originPath,false);
                        }

                    }
                }

                FILEMANAGER->flushTimeWriteAccess(paths[u],true);
            }
        }
    }
    ITF_VECTOR<String> ActorEditor::GetTexturePathFromFrise(Actor* target)
    {
        ITF_VECTOR<String> paths;
        if (target == nullptr) return paths;
        ResourceGroup::ResourceList list = target->getResourceGroup()->getResourcesList();
        for (u32 u = 0; u < list.size();u++)
        {
            Resource* res =(list[u].getResource());
            if (res->isCooked())
            {
                auto group = res->getGroup();
                if (group != NULL)
                {
                    const ITF_VECTOR<ResourceID>& listResourceInActor = res->getGroup()->getResourcesList();
                    for (u32 i = 0; i < listResourceInActor.size();i++)
                    {
                        const ResourceID& resource = listResourceInActor[i];
                        if ( !resource.getPath()->isEmpty())
                        {
                            String extension = resource.getPath()->getExtension();
                            for (const String& ext : samplerExtensions)
                            {
                                if (extension.equals(ext,true) || extension.equals(ext,true))
                                {
                                    String texturePath;
                                    resource.getPath()->getString(texturePath);
                                    if (!texturePath.isEmpty() && FILEMANAGER->fileExists(texturePath))
                                    {
                                        paths.push_back(texturePath);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    String extension = res->getPath().getExtension();
                    for (const String& ext : samplerExtensions)
                    {
                        if (extension.equals(ext,true) || extension.equals(ext,true))
                        {
                            String texturePath;
                            res->getPath().getString(texturePath);
                            if (!texturePath.isEmpty() && FILEMANAGER->fileExists(texturePath))
                            {
                                paths.push_back(texturePath);
                            }
                        }
                    }
                }

            }
        }
        return paths;
    }

    bool ActorEditor::deleteFileIfExists(const String & _fileToDelete)
    {
        if (_fileToDelete.isEmpty())
            return false;

        // Check that file exist using GetFileAttributesW
        DWORD fileAttributes = ::GetFileAttributesW((LPCWSTR)_fileToDelete.cStr());
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
            return true;

        return deleteFile(_fileToDelete);
    }

    bool ActorEditor::deleteFile(String _fileToDelete)
    {
        BOOL res = 0;
        u32 retrymax = 5;
        u32 retry = 0;

        while (res == 0)
        {
            res = ::DeleteFileW((LPCWSTR)_fileToDelete.cStr());
            if (retry > retrymax)
                break;

            if (res == 0)
            {
                retry++;
                Sleep(100);
            }
        }
        if (retry > retrymax)
        {
            DWORD error = GetLastError();
            LOG_COOKER("[TEXTURE COOKER] Failed to delete file '%ls' after %d retries (error: 0x%X)",
                _fileToDelete.cStr(),
                retry,
                error);
            return false;
        }
        else if (retry > 0)
        {
            LOG_COOKER("[TEXTURE COOKER] Successfully deleted file '%ls' after %d retries", _fileToDelete.cStr(), retry);
        }
        return true;
    }

    bool ActorEditor::renameFile(const String & _srcFileName, const String & _newFileName, bool _deleteIfDstExists = true)
    {
        if (_srcFileName.isEmpty() || _newFileName.isEmpty())
            return false;

        if (_deleteIfDstExists)
        {
            if (!deleteFileIfExists(_newFileName))
            {
                LOG_COOKER("[TEXTURE COOKER] Failed to delete existing file '%ls' before renaming", _newFileName.cStr());
                return false;
            }
        }

        BOOL success = ::MoveFileW(
            (LPCWSTR)_srcFileName.cStr(),
            (LPCWSTR)_newFileName.cStr()
        );

        if (!success)
        {
            DWORD error = GetLastError();
            LOG_COOKER("[TEXTURE COOKER] Failed to rename file from '%ls' to '%ls' (error: 0x%X)",
                _srcFileName.cStr(),
                _newFileName.cStr(),
                error);
            return false;
        }

        return true;
    }
#endif
    void ActorEditor::onObjectSpawned(const BaseObject* _newObject)
    {
        ITF_ASSERT(_newObject);

        const Pickable* pPickable = _newObject->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
        if (pPickable)
        {
            Scene* pScene = pPickable->getScene();
            if(pScene && pScene->isActive())
                createPickingShapeForActor(_newObject->getRef());
        }

        // Special case for subscene actors
        const SubSceneActor* pSSA = _newObject->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        if(pSSA)
        {
            const Scene* pSubScene = pSSA->getSubScene();
            const u32 pickablesCount = pSubScene ? pSubScene->getPickableList().size() : 0;
            for(u32 i = 0; i < pickablesCount; ++i)
            {
                Pickable* pObj = pSubScene->getPickableList()[i];
                PLUGINGATEWAY->onObjectSpawned(pObj);
            }
        }
    }

    void ActorEditor::onObjectsDuplicated(const SafeArray <BaseObject*>& _from, const SafeArray <BaseObject*>& _to)
    {
        const u32 count = _to.size();
        for(u32 iNew = 0; iNew < count; ++iNew)
        {
            BaseObject* objectTo = _to[iNew];
            const BaseObject* objectFrom = _from[iNew];

            // If the source obj still exist and it's copied within the same world
            if(objectFrom && static_cast<const Pickable*>(objectFrom)->getWorld() == static_cast<const Pickable*>(objectTo)->getWorld())
            {
                // If a linked child is duplicated without it's parent then link the new child to the parent
                if(ObjectRefList* pParentFromRefList = LINKMANAGER->getParents(objectFrom->getRef()))
                {
                    SafeArray<BaseObject*> pParentsFrom;
                    ID_SERVER->getObjectListNoNull(*pParentFromRefList, pParentsFrom);

                    for(u32 iP = 0; iP < pParentsFrom.size(); ++iP)
                    {
                        const Actor* pParentFrom = static_cast<const Actor*>(pParentsFrom[iP]);

                        if(_from.find((BaseObject*)pParentFrom) != -1)
                            continue;

                        if(LinkComponent* pLinkComponentParent = pParentFrom->GetComponent<LinkComponent>())
                        {
                            const LinkComponent::ChildrenList& children = pLinkComponentParent->getChildren();

                            LinkComponent::ChildrenList::const_iterator it = children.begin();
                            while(it != children.end())
                            {
                                Pickable* pObj = NULL;
                                ChildEntry fromChild = *it;
                                const ObjectPath& path = fromChild.getPath();
                                if ( path.getIsAbsolute() )
                                    pObj = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                                else
                                    pObj = SceneObjectPathUtils::getObjectFromRelativePath(pParentFrom, path);

                                if(pObj == objectFrom)
                                {
                                    ObjectPath newPath;
                                    if(SceneObjectPathUtils::getRelativePathFromObject(pParentFrom,static_cast<Pickable*>(objectTo),newPath))
                                    {
                                        pLinkComponentParent->addChild(newPath, btrue);

                                        ChildEntry& childEntryTo = pLinkComponentParent->getMutableChildren().back();
                                        childEntryTo = fromChild; // copy tags
                                        childEntryTo.setPath(newPath); // preserve path
                                    }

                                    break;
                                }

                                ++it;
                            }
                        }
                    }
                }
            }

            // Only handle actors
            Actor* actorTo = objectTo->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if(!actorTo)
                continue;

            const Actor* actorFrom          = objectFrom ? objectFrom->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) : NULL;

            if(Actor* pOldParent = actorTo->getParent().getActor())
            {
                // If the parent is already up to date then do nothing
                if(_to.find(pOldParent) != -1)
                    continue;

                // If old parent is duplicated bind to duplicate
                const i32 oldParentIndex = _from.find((BaseObject*)pOldParent);
                if(oldParentIndex != -1)
                {
                    if(!actorFrom)
                        continue;

                    ActorBind* bind = actorTo->getParentBind();
                    Actor* duplicateParent = static_cast<Actor*>(_to[oldParentIndex]);
                    if(!SceneObjectPathUtils::getRelativePathFromObject(actorTo, duplicateParent, bind->m_parent))
                        continue;

                    pOldParent->getChildrenBindHandler().removeChild(actorTo);

                    duplicateParent->getChildrenBindHandler().addChild(actorTo);
                }
                else
                {
                    actorTo->setParentBind(NULL); // Do not keep old bind if parent not duplicated
                }
            }

            LinkComponent* pLinkComponentTo         = actorTo->GetComponent<LinkComponent>();
            const LinkComponent* pLinkComponentFrom = actorFrom ? actorFrom->GetComponent<LinkComponent>() : NULL;

            if(pLinkComponentTo && pLinkComponentFrom && actorFrom->getWorld() == actorTo->getWorld())
            {
                const LinkComponent::ChildrenList& childrenListFrom = pLinkComponentFrom->getChildren();
                const u32 numChildrenFrom = childrenListFrom.size();

                pLinkComponentTo->clearLinks(); // The children list if fully rebuilt in the next loop

                // For each old child, if duplicated then update link to duplicate
                for(u32 iChildFrom = 0; iChildFrom < numChildrenFrom; ++iChildFrom)
                {
                    const ChildEntry& childEntryFrom = childrenListFrom[iChildFrom];
                    const ObjectPath& childFromPath = childEntryFrom.getPath();

                    Pickable* childFrom = NULL;
                    if ( childFromPath.getIsAbsolute() )
                    {
                        childFrom = SceneObjectPathUtils::getObjectFromAbsolutePath(childFromPath);
                    }
                    else
                    {
                        childFrom = SceneObjectPathUtils::getObjectFromRelativePath(actorFrom, childFromPath);
                    }

                    const i32 childFromIndex = _from.find(childFrom);

                    //Pickable* childTo = NULL; // Do not keep old link if not duplicated
                    Pickable* childTo = childFrom; // Keep existing link
                    if(childFromIndex != -1)
                    {
                        childTo = static_cast<Pickable*>(_to[childFromIndex]);
                    }

                    if(childTo)
                    {
                        ObjectPath newPath;
                        SceneObjectPathUtils::getRelativePathFromObject(actorTo,childTo,newPath);

                        if ( !newPath.isValid() )
                        {
                            SceneObjectPathUtils::getAbsolutePathFromObject(childTo,newPath);
                        }

                        if ( newPath.isValid() )
                        {
                            pLinkComponentTo->addChild(newPath, btrue);

                            ChildEntry& childEntryTo = pLinkComponentTo->getMutableChildren().back();
                            childEntryTo = childEntryFrom; // copy tags
                            childEntryTo.setPath(newPath); // preserve path
                        }
                    }
                }
            }

            for (u32 i = 0; i < m_listeners.size(); i++)
            {
                ActorEdListener& listener = m_listeners[i];
                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                    if (ActorComponent* pComp = actorTo->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]))
                        listener.m_listener->onObjectDuplicated(actorTo, const_cast<Actor*>(actorFrom), pComp);
            }

            PLUGINGATEWAY->onObjectCreated(actorTo);
        }
    }

    void ActorEditor::enterSubEdit( ObjectRef _or )
    {
        BaseObject* bo = GETOBJECT(_or);
        if (!bo || bo->getObjectType() != BaseObject::eActor)
            return;

        Actor* _pActor = static_cast<Actor*>(bo);

        SubSceneActor* pSSA = _pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        if(pSSA) // Editor sub edit
        {
            if(EDITOR->allowEditSubScene(pSSA))
            {
                ObjectRefList list;
                EDITOR->startToEditSubScene(pSSA, list);
            }
        }
        else
        {
            EDITOR->unselectAll();
            m_state = SubEdit;
            PickingInfo2D info;
            Picking::fillPickingInfo(_pActor, &info);
            selectActor(_pActor, btrue, &info);
            EDITOR->setSubEditedItem(_or);
            PLUGINGATEWAY->onActorSubEdit(_or); // inform other plugins
        }
    }

    void ActorEditor::leaveSubEdit(Actor* _pActor)
    {
        //return;
        ITF_ASSERT(SubEdit == m_state);
        if(SubEdit != m_state)
            return;

        ITF_ASSERT(_pActor);
        if(!_pActor)
            return;

        SubSceneActor* pSSA = _pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        if(pSSA)
        {
            // If the sub scene is unique in this world do nothing
            // Else reload all others subscenes instances
            LOG_OLIV("Do nothing for the moment");
            EDITOR->unselectAll();
        }

        m_state = Select;
        EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
    }

    bbool ActorEditor::isSubEditable( const BaseObject* _pObj )const
    {
        bbool result = bfalse;

        if(_pObj)
        {
            const Actor* pActor = _pObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            if(pActor)
            {
                if( pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) )
                {
                    result = btrue;
                }
            }
        }

        return result;
    }

    void ActorEditor::onObjectAdded( const Scene* _pScene, BaseObject* _pObj )
    {
        if(_pScene->isActive() && _pObj->getObjectType() == BaseObject::eActor)
            m_addedActiveActorsLastFrame.push_back(_pObj->getRef());
    }

    void ActorEditor::onObjectRemoved(const Scene* _pScene, BaseObject* _pObj, bbool _delete)
    {
        ObjectRef objRef = _pObj->getRef();
        for(u32 i = 0; i < m_selectionInfoList.size(); ++i)
        {
            if(m_selectionInfoList[i].m_targetRef == objRef)
            {
                m_selectionInfoList.eraseNoOrder(i);
                break;
            }
        }

        switch(m_state)
        {
        case CreateBind:
            if(m_selectionInfoList.size() == 0)
                cancelCurrentAction(btrue);
            break;

        default:
            break;
        }

        if(_delete)
        {
            const i32 index = m_addedActiveActorsLastFrame.find(objRef);
            if(index != -1)
                m_addedActiveActorsLastFrame.eraseNoOrder(index);

            if(EDITOR)
                EDITOR->deletePickingShapesForObject(objRef);
        }
    }

    void ActorEditor::onPostSceneActivation( const Scene* _pScene, bbool _bActive )
    {
        ITF_ASSERT_MSG(_pScene, "Invalid parameters");

        if(_bActive)
        {
            const PickableList& actors = _pScene->getActors();
            for(u32 i = 0; i < actors.size(); ++i)
            {
                Pickable* pObj = actors[i];
                if(EDITOR->getMainPickingShapeForObject(pObj->getRef()) == NULL)
                    createPickingShapeForActor(pObj->getRef());
            }
        }
    }

    void ActorEditor::onObjectCreated( BaseObject* _pObject )
    {
        if (_pObject && _pObject->getObjectType() == BaseObject::eActor)
        {
            Actor* pActor = (Actor*)_pObject;

            if(Scene* pScene = pActor->getScene())
                if(pScene->isActive())
                    createPickingShapeForActor(pActor->getRef());

            pActor->onEditorCreated();

            if (EDITOR->isAllowStorePickableInitData(pActor))
            {
                pActor->replaceReinitDataByCurrent();
            }
        }
    }

    void ActorEditor::onObjectChanged( BaseObject* _pObject )
    {
        if (_pObject && _pObject->getObjectType() == BaseObject::eActor)
        {
            Actor* pActor = (Actor*)_pObject;

            const bbool allowEditInitialPos = EDITOR->isAllowStorePickableInitData(pActor);

            // If it's a binded actor update the relative pos
            if(pActor->isActive())
            {
                if(allowEditInitialPos)
                {
                    if(ActorBind* pBindToParent = pActor->getParentBind())
                    {
                        pActor->setBoundWorldInitialPos(pActor->getPos());
                        pActor->setBoundWorldInitialAngle(pActor->getAngle());
                    }
                }
                else
                {
                    pActor->setBoundWorldPos(pActor->getPos());
                    pActor->setBoundWorldAngle(pActor->getAngle());
                }
            }

            if(allowEditInitialPos)
            {
                // pActor->storeCurrentToInitial();
                pActor->replaceReinitDataByCurrent();
            }

            for (u32 i = 0; i < m_listeners.size(); i++)
            {
                ActorEdListener& listener = m_listeners[i];
                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                {
                    ActorComponent* pComp = pActor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                    if (pComp)
                        listener.m_listener->onObjectChanged(pActor, pComp);
                }
            }

            if(pActor->isPhysicalReady())
            {

#ifndef ITF_FINAL
                pActor->validate();
#endif ///ITF_FINAL
                pActor->onEditorMove(EDITOR->isAllowStorePickableInitData(pActor));
            }
        }
    }

    u32 ActorEditor::getActorInfoMask()
    {
        return EDITOR->getPluginsInfoFlags() & m_showActorInfoMask;
    }

    bbool ActorEditor::hasActorInfoFlag(u32 _flag)
    {
        return (m_showActorInfoMask & _flag & EDITOR->getPluginsInfoFlags()) != 0;
    }

    void ActorEditor::copyObjectPathToClipboard() const
    {
#ifdef ITF_WINDOWS
        const u32 selSize = EDITOR->getSelectionSize();
        ITF_ASSERT(selSize == 1);
        if(selSize == 1 && ::OpenClipboard(NULL))
        {
            ObjectPath path;
            SceneObjectPathUtils::getAbsolutePathFromObject(static_cast<const Pickable*>(EDITOR->getSelectionAt(0)->getOwnerPtr()), path);

            ITF_STDSTRING str;
            path.toString(str);

            String str16 = str.c_str();

            size_t size = sizeof(u16)*(1+str16.getLen());
            HGLOBAL hResult = ::GlobalAlloc(GMEM_MOVEABLE, size);
            wchar_t* gmem = (wchar_t*)::GlobalLock(hResult);
            ITF_Memcpy(gmem, str16.cStr(), size);
            ::GlobalUnlock(hResult);

            if (::SetClipboardData( CF_UNICODETEXT, hResult ) == NULL )
            {
                ::GlobalFree(hResult);
                ::CloseClipboard();
                return;
            }
            else
            {
                if (DEBUGINFO)
                {
                    String userInfo;
                    userInfo.setTextFormat("ObjectPath copied to clipboard : %ls", str16.cStr());
                    DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
                }
            }
            ::CloseClipboard();
        }
#endif //ITF_WINDOWS
    }

    ObjectPath ActorEditor::getObjectPathFromClipboard()
    {
        ObjectPath path;

#ifdef ITF_WINDOWS
        if (::OpenClipboard(NULL))
        {
            HGLOBAL hglb = ::GetClipboardData(CF_UNICODETEXT);
            if (hglb != NULL)
            {
                wchar_t* gmem = (wchar_t*)::GlobalLock(hglb);
                if (gmem != NULL)
                {
                    String8 str8((u16*)gmem);
                    ITF_STDSTRING strr = str8.cStr();

                    path.fromString(strr);

                    ::GlobalUnlock(hglb);
                }
            }

            ::CloseClipboard();
        }
#endif //ITF_WINDOWS
        return path;
    }



    bbool findActorDistToFrieze(const Actor* _pAct, const Frise* _pFrise, Vec3d& _foundPos, f32& _dist)
    {
        f32 z = _pFrise->getDepth();
        u32 edgeCount = _pFrise->getPointsData().getVectorsCount();
        const Vec2d pos = _pAct->getPos().truncateTo2D();
        bbool oneProcessed = bfalse;
        f32 bestDist = 0.f;
        for (u32 iEdg = 0; iEdg < edgeCount; iEdg++)
        {
            const PolyLineEdge& edge = _pFrise->getEdgeAt(iEdg);
            Vec2d delta = pos - edge.getPos();
            f32 inrange = delta.dot(edge.m_normalizedVector);
            if (inrange >= 0 && inrange <= edge.m_length)
            {
                f32 dist = fabsf(delta.dot(edge.m_normalizedVector.getPerpendicular()));
                if (!oneProcessed || dist < bestDist)
                {
                    oneProcessed = btrue;
                    bestDist = dist;
                    Vec2d posOnEdge = edge.getPos() + edge.m_normalizedVector * inrange;
                    _foundPos = posOnEdge.to3d(z);
                }
            }
        }
        if (oneProcessed)
            _dist = bestDist;
        return oneProcessed;
    }

    void ActorEditor::beginBindActorToFrieze()
    {
#if 0 // WIP FRED
        const SafeArray<PickingShape*>& sel = EDITOR->getSelectionList();
        for (u32 i = 0; i < sel.size(); i++)
        {
            const PickingShape* shape = sel[i];
            if (shape && !shape->isSubAnchor())
            {
                BaseObject* pObj = shape->getOwnerPtr();
                if (pObj && pObj->getObjectType() == BaseObject::eActor)
                {
                    Actor* pAct = (Actor*)pObj;
                    Scene* pScene = pAct->getScene();
                    if (pScene)
                    {
                        f32     lowestDist = 0.f;
                        Vec3d   vFoundPos;
                        bbool   oneProcessed = bfalse;


                        const SafeArray<ObjectRef>& active = pScene->getActiveObjects();
                        for (u32 iAct = 0; iAct < active.size(); iAct++)
                        {
                            BaseObject* piAct = active[iAct].getObject();
                            if (piAct && piAct->getObjectType() == BaseObject::eFrise)
                            {
                                Frise* pFrise = (Frise*)piAct;
                                if (DepthRange(pFrise->getDepth()) == DepthRange(pAct->getDepth()))
                                {
                                    Vec3d foundPos;
                                    f32 thisDist;
                                    if (findActorDistToFrieze(pAct, pFrise, foundPos, thisDist))
                                    {
                                        if (thisDist < lowestDist || !oneProcessed)
                                        {
                                            oneProcessed = btrue;
                                            lowestDist = thisDist;
                                            vFoundPos = foundPos;
                                        }
                                    }
                                }
                            }
                        }
                        if (oneProcessed)
                        {
                            GFX_ADAPTER->drawArrow(pAct->getPos(), vFoundPos, Color::white(), 0.3f, 10.f);
                        }
                    }
                }
            }
        }
#endif // 0

    }

    void ActorEditor::onPickableRenamed( Pickable* _pObj, const String8& _previousUserFriendly )
    {
        if(_pObj->getObjectType() != BaseObject::eActor)
            return;
        Actor* pActor = (Actor*)_pObj;

        // Update binding when a parent is renamed
        const ActorBindHandler& childrenHandler = pActor->getChildrenBindHandler();
        for(u32 i = 0; i < childrenHandler.getChildren().size(); ++i)
        {
            if(Actor* pChild = static_cast<Actor*>(childrenHandler.getChildren()[i].getObject()))
            {
                if(ActorBind* pBind = pChild->getParentBind())
                {
                    UNDOREDO_MANAGER->preChange(pChild);
                    SceneObjectPathUtils::getRelativePathFromObject(pChild, pActor, pBind->m_parent);
                }
            }
        }

        // Updates actors links
        ObjectRef child = pActor->getRef();
        if(const ObjectRefList* parents = LINKMANAGER->getParents(child))
        {
            const u32 uParentCount = parents->size();
            ObjectRefList relinkList;
            for(u32 ii = 0; ii < uParentCount; ++ii)
            {
                ObjectRef parent = (*parents)[0];

                if(Actor* pParent = static_cast<Actor*>(parent.getObject()))
                {
                    if(LinkComponent* pLC = pParent->GetComponent<LinkComponent>())
                    {
                        // Try to find the previous objectPath
                        static const u32 numberOfPossibilities = 2;
                        ObjectPath names[numberOfPossibilities];

                        {
                            u32 index = 0;
                            SceneObjectPathUtils::getRelativePathFromObject(pParent,pActor,names[index]);
                            names[index].setStrId(_previousUserFriendly.cStr());

                            index++;
                            SceneObjectPathUtils::getAbsolutePathFromObject(pActor,names[index]);
                            names[index].setStrId(_previousUserFriendly.cStr());
                        }


                        for(u32 iObjP = 0; iObjP < numberOfPossibilities; ++iObjP)
                        {
                            const ObjectPath& path = names[iObjP];
                            if(pLC->hasChild(path))
                            {
                                pLC->removeChild(path, bfalse); // Do not remove/add them from the manager as their Ref doesn't changes
                                relinkList.push_back(parent);

                                break;
                            }
                        }
                    }
                }
            }
            parents = NULL;

            for(u32 iParent = 0; iParent < relinkList.size(); ++iParent)
            {
                ObjectRef parent = relinkList[iParent];
                if(Actor* pParent = static_cast<Actor*>(parent.getObject()))
                {
                    if(LinkComponent* pLC = pParent->GetComponent<LinkComponent>())
                    {
                        ObjectPath newPathToChild;
                        if(SceneObjectPathUtils::getRelativePathFromObject(pParent,pActor,newPathToChild))
                            pLC->addChild(newPathToChild, bfalse);  // Do not remove/add them from the manager as their Ref doesn't changes
                    }
                }
            }
        }
    }

    void ActorEditor::startEditor()
    {
        colorComputer_Plugin::create();
        CharacterDebugger_Plugin::create();

        ACTORSMANAGER->setHotreloadCallback(onActorHotreload);
    }

    void ActorEditor::stopEditor()
    {
        ACTORSMANAGER->setHotreloadCallback(NULL);
    }

    bbool ActorEditor::findBindTarget( const Actor* _pObj, const Vec2d& _screenSpacePos, ActorBind& _result )
    {
        bbool result = bfalse;
        const f32 screenSpaceSnapDist = 5.0f;

        if(AnimLightComponent* pAnimLightComponent = _pObj->GetComponent<AnimLightComponent>())
        {
            if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
            {
                if(AnimSkeleton *skeleton = pAnimLightComponent->getCurSubAnim(0)->getSkeleton())
                {
                    if(AnimMeshScene* pMeshScene = pAnimLightComponent->getAnimMeshScene())
                    {
                        const SafeArray<AnimBoneDyn>& boneList = pMeshScene->m_AnimInfo.getCurrentBoneList();
                        const f32 fZ = _pObj->getDepth();
                        f32 bestDist = -1.0f;

                        Vec3d boneSS_Start, boneSS_End;
                        const u32 bCount = boneList.size();
                        for (u32 bIdx = 0; bIdx < bCount; bIdx++)
                        {
                            const AnimBoneDyn& bone     = boneList[bIdx];
                            const Vec3d bone3D_Start    = bone.m_Pos.to3d(fZ);
                            const Vec3d bone3D_End      = bone.m_PosEnd.to3d(fZ);
                            const AnimBone* pBone       = skeleton->getBoneAt(bIdx);

                            GFX_ADAPTER->compute3DTo2D(bone3D_Start, boneSS_Start);
                            GFX_ADAPTER->compute3DTo2D(bone3D_End, boneSS_End);

                            const f32 boneDist = fabsf(PointSignedDistToSegment(_screenSpacePos, boneSS_Start.truncateTo2D(), boneSS_End.truncateTo2D()));
                            if(boneDist < screenSpaceSnapDist && (bestDist < 0.0f || bestDist > boneDist))
                            {
                                _result.m_type = ActorBind::BoneName;
                                _result.m_typeData = pBone->m_Name.GetValue();
                                _result.setInitialPositionOffset( bone3D_Start );
                                _result.setInitialAngleOffset( bone.getAngle() );
                                result = btrue;
                            }
                        }
                    }
                }
            }
        }

        Vec3d actorPos2d;
        GFX_ADAPTER->compute3DTo2D(_pObj->getPos(), actorPos2d);
        const f32 rootDist = (actorPos2d.truncateTo2D() - _screenSpacePos).norm();
        if(rootDist < Anchor_DefaultSize)
        {
            _result.m_type = ActorBind::Root;
            _result.setInitialPositionOffset( _pObj->getPos() );
            _result.setInitialAngleOffset( _pObj->getAngle() );
            result = btrue;
        }

        if(result)
        {
            SceneObjectPathUtils::getAbsolutePathFromObject(_pObj, _result.m_parent);
            _result.m_runtimeParent = _pObj->getRef();
        }

        return result;
    }

    bbool ActorEditor::getBindTargetInfo( const Actor* _pChild, Vec3d& _targetPosition, f32& _targetAngle, bbool& _targetFlip )
    {
        bbool result = bfalse;

        if(const ActorBind* pBind = _pChild->getParentBind())
        {
            if(const Actor* pParent = static_cast<const Actor*>(pBind->m_runtimeParent.getObject()))
            {
                result = pParent->getChildrenBindHandler().getTransform(pBind, _targetPosition, _targetAngle);
                _targetFlip = pParent->isFlipped();
            }
            else
            {
                ITF_ASSERT_MSG(0, "Runtime parent not found !!");
            }
        }

        return result;
    }

    bbool ActorEditor::canBindActors( const Actor* _pParent, const Actor* _pChild )
    {
        if(_pParent && _pChild)
        {
            if(!_pParent->isSerializable())
                return bfalse;

            // Can't bind between two different worlds
            if(_pParent->getWorld() != _pChild->getWorld())
                return bfalse;

            // Prevent incorrect down bind
            // Do not bind if "parent" is an hierarchy ancestor of "child"
            if(const SubSceneActor* pSSA_Parent = _pParent->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                const SubSceneActor* currParent = _pChild->getScene()->getSubSceneActorHolder();
                while(currParent)
                {
                    if(pSSA_Parent == currParent)
                        return bfalse;

                    currParent = currParent->getScene()->getSubSceneActorHolder();
                }
            }

            // Prevent up bind
            // Do not bind if "child" is an hierarchy ancestor of "parent"
            {
                const Actor* currObj = _pParent;
                while(currObj)
                {
                    if(_pChild == currObj)
                        return bfalse;

                    currObj = currObj->getScene()->getSubSceneActorHolder();
                }
            }

            // Prevent recursive bind
            // Do not bind if "child" is a binded ancestor of "parent"
            {
                const Actor* currObj = _pParent;
                while(currObj)
                {
                    if(_pChild == currObj)
                        return bfalse;

                    if(const ActorBind* pCurrParentBind = currObj->getParentBind())
                        currObj = static_cast<const Actor*>(pCurrParentBind->m_runtimeParent.getObject());
                    else
                        currObj = NULL;
                }
            }

            // Allow parent change
            // Check if "child" has already "parent" as current parent
            if(const ActorBind* pChildBind = _pChild->getParentBind())
            {
                const Actor* pCurrentParent = static_cast<const Actor*>(pChildBind->m_runtimeParent.getObject());

                if(_pParent == pCurrentParent)
                    return bfalse;
            }

            return btrue;
        }

        return bfalse;
    }

    void ActorEditor::unbindActor( Actor* pActor, bbool parent, bbool children )
    {
        ITF_ASSERT_CRASH(pActor, "Invalid parameters");

        UndoRedoSequenceScope unbindScope("Unbind scope");

        if(parent)
        {
            UNDOREDO_MANAGER->preChange(pActor);
            pActor->setParentBind(NULL);
            PLUGINGATEWAY->onObjectChanged(pActor);
        }

        if(children)
        {
            ObjectRefList childListCopy = pActor->getChildrenBindHandler().getChildren();
            for(u32 iChild = 0; iChild < childListCopy.size(); ++iChild)
            {
                if(Actor* child = static_cast<Actor*>(childListCopy[iChild].getObject()))
                {
                    UNDOREDO_MANAGER->preChange(child);
                    child->setParentBind(NULL);
                    PLUGINGATEWAY->onObjectChanged(child);
                }
            }
            pActor->getChildrenBindHandler().clear();
        }
    }

    void ActorEditor::enterCreateBind( const Actor* _parent )
    {
        ITF_ASSERT_CRASH(_parent, "Invalid parent");

        if(CreateBind != m_state)
        {
            m_state = CreateBind;
            m_keepBindTarget = bfalse;

            EDITOR->pushPause();
            PLUGINGATEWAY->setExclusiveFocus(this); // Prevent editor action like duplication, translation...

            // Set default bind target
            m_bindTargetParent.m_type = ActorBind::Root;
            m_bindTargetParent.m_runtimeParent = _parent->getRef();
            m_bindTargetParent.setInitialPositionOffset( _parent->getPos() );
            m_bindTargetParent.setInitialAngleOffset( _parent->getAngle() );

            // Find best target undermouse
            ActorEditor::findBindTarget(_parent, EDITOR->getMouse2d(), m_bindTargetParent);
        }
    }

    void ActorEditor::exitCreateBind()
    {
        if(CreateBind == m_state)
        {
            m_state = Select;

            EDITOR->popPause();
            PLUGINGATEWAY->setExclusiveFocus(NULL);

            m_bindTargetParent.m_runtimeParent.invalidate();
        }
    }

    void    ActorEditor::onFillMainMenu(EditorContextMenu& _menu)
    {
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            ActorEdListener& listener = getInstance()->m_listeners[i];
            for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
            {
                listener.m_listener->fillMainMenu(_menu);
            }
        }
    }

    void ActorEditor::toggleSubSceneLock( SubSceneActor* _pSSA ) const
    {
        UndoRedoSequenceScope scope("Toggle ssa lock");

        UNDOREDO_MANAGER->preChange(_pSSA);

        String userInfo;

        if(_pSSA->isZForced())
        {
            _pSSA->setZLock(bfalse);
            userInfo = "Subscene unlocked";
        }
        else
        {
            _pSSA->setZLock(btrue);
            userInfo = "Subscene locked";
        }

        if (DEBUGINFO)
        {
            DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
        }

        PLUGINGATEWAY->onObjectChanged(_pSSA);
    }

    bbool ActorEditor::onSetShapePosition( PickingShape* _pPickingShape, const Vec2d& _screenspacepos )
    {
        BaseObject* pOwner = _pPickingShape->getOwnerPtr();

        if(pOwner->getObjectType() != BaseObject::eActor)
            return bfalse;

        Actor* pActor = pOwner->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        if (!pActor)
            return bfalse;

        bbool positioningRequestHandled = bfalse;
        Vec3d prevPos = pActor->getPos();

        if(Interface_ActorEditorListener* pInterface = static_cast<Interface_ActorEditorListener*>(_pPickingShape->getCreatorPickingShapePlugin()))
            if(pInterface->onSetShapePosition(_pPickingShape, _screenspacepos))
                positioningRequestHandled = btrue;

        if(!positioningRequestHandled)
        {
            ITF_ASSERT(_pPickingShape->getShapeType() == PickingShape::ShapeType_Disc);
            if (_pPickingShape->getShapeType() != PickingShape::ShapeType_Disc)
            {
                return bfalse;
            }

            const f32 z = pActor->getDepth();
            Vec3d p1New3DPos;
            PickingInfo2D* pPickingInfo = _pPickingShape->getPickingInfoPtr();
            const Vec3d p1New2DPos(_screenspacepos.m_x + pPickingInfo->m_anchor.m_x, _screenspacepos.m_y + pPickingInfo->m_anchor.m_y, pPickingInfo->m_zDist);
            GFX_ADAPTER->compute2DTo3D(p1New2DPos, p1New3DPos);

            p1New3DPos.m_z = z;

            PickingShape_Disc* disc = (PickingShape_Disc*)_pPickingShape;

            switch (disc->getPosType())
            {
            case PickingShape_Disc::Common:
                {
                    if(pActor->is2DActor())
                        pActor->setPos(p1New2DPos);
                    else
                        pActor->setPos(p1New3DPos);

                    if(EDITOR->isAllowStorePickableInitData(pActor))
                        pActor->setLocalInitialPos(pActor->getLocalPos());

                    positioningRequestHandled = btrue;
                }
                break;

            case PickingShape_Disc::InitPosFromCurrent:
                {
                    Vec3d offset = pActor->getPos() - pActor->getWorldInitialPos();
                    offset.m_z = 0.f;
                    pActor->setWorldInitialPos(p1New3DPos - offset);
                    pActor->setPos(p1New3DPos);
                    positioningRequestHandled = btrue;
                }
                break;

            default:
                ITF_ASSERT(0);
                break;
            }
        }

        if(positioningRequestHandled)
        {
            Vec3d deltaPos = pActor->getPos() - prevPos;

            for (u32 i = 0; i < m_listeners.size(); i++)
            {
                ActorEdListener& listener = m_listeners[i];
                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                {
                    ActorComponent* pComp = pActor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                    if (pComp)
                        listener.m_listener->onShapePosChanged(pActor, pComp, _pPickingShape, deltaPos);
                }
            }

            pActor->onEditorMove(EDITOR->isAllowStorePickableInitData(pActor));
        }
        return positioningRequestHandled;
    }

    bbool ActorEditor::onOffsetShapePosition( PickingShape* _pPickingShape, const Vec3d& _deltaPos )
    {
        BaseObject* pOwner = _pPickingShape->getOwnerPtr();

        if(pOwner->getObjectType() != BaseObject::eActor)
            return bfalse;

        Actor* pActor = static_cast<Actor*>(pOwner);

        bbool positioningRequestHandled = bfalse;
        Vec3d prevPos = pActor->getPos();

        if(Interface_ActorEditorListener* pInterface = static_cast<Interface_ActorEditorListener*>(_pPickingShape->getCreatorPickingShapePlugin()))
            if(pInterface->onOffsetShapePosition(_pPickingShape, _deltaPos))
                positioningRequestHandled = btrue;

        if(!positioningRequestHandled)
        {
            Vec3d delta = _deltaPos;
            if(pActor->isZForced())
                delta.m_z = 0.0f;

            ITF_ASSERT(_pPickingShape->getShapeType() == PickingShape::ShapeType_Disc);
            if (_pPickingShape->getShapeType() != PickingShape::ShapeType_Disc)
            {
                return bfalse;
            }

            PickingShape_Disc* disc = (PickingShape_Disc*)_pPickingShape;

            switch (disc->getPosType())
            {
            case PickingShape_Disc::Common:
                {
                    pActor->setPos(pActor->getPos() + delta);
                    if(EDITOR->isAllowStorePickableInitData(pActor))
                        pActor->setLocalInitialPos(pActor->getLocalPos());
                    positioningRequestHandled = btrue;
                }
                break;

            case PickingShape_Disc::InitPosFromCurrent:
                {
                    Vec3d offset = pActor->getPos() - pActor->getWorldInitialPos();
                    pActor->setLocalInitialPos(pActor->getLocalInitialPos() + delta);
                    pActor->setPos(pActor->getWorldInitialPos() + offset);
                    positioningRequestHandled = btrue;
                }
                break;

            default:
                ITF_ASSERT(0);
                break;
            }
        }

        if(positioningRequestHandled)
        {
            Vec3d deltaPos = pActor->getPos() - prevPos;

            for (u32 i = 0; i < m_listeners.size(); i++)
            {
                ActorEdListener& listener = m_listeners[i];
                for (u32 j = 0; j < listener.m_listenedComponents.size(); j++)
                {
                    ActorComponent* pComp = pActor->getComponentFromStaticClassCRC(listener.m_listenedComponents[j]);
                    if (pComp)
                        listener.m_listener->onShapePosChanged(pActor, pComp, _pPickingShape, deltaPos);
                }
            }
            pActor->onEditorMove(EDITOR->isAllowStorePickableInitData(pActor));
        }
        return positioningRequestHandled;
    }

    void ActorEditor::pickFactorColor()
    {
        if (m_selectionInfoList.size() == 1)
        {
            if(Actor* pActor = static_cast<Actor*>(m_selectionInfoList[0].m_targetRef.getObject()))
            {
                GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                if (graphcomp)
                {
                    if (DEBUGINFO)
                        DEBUGINFO->addDebugInfoDisplay("picked Fog color", 3.0f);

                    EDITOR->getCopiedProperties().m_color = graphcomp->getColorFactor();
                }
            }
        }
    }

    void ActorEditor::pasteFactorColor()
    {
        UndoRedoSequenceScope scope("ItemId_Actor_PasteFogProperties");
        for (u32 iSel = 0; iSel < m_selectionInfoList.size(); ++iSel)
        {
            if(Actor* pActor = static_cast<Actor*>(m_selectionInfoList[iSel].m_targetRef.getObject()))
            {
                GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                if (graphcomp)
                {
                    UNDOREDO_MANAGER->preChange(pActor);
                    graphcomp->setColorFactor(EDITOR->getCopiedProperties().m_color);

                    PLUGINGATEWAY->onObjectChanged(pActor);

                }
            }
        }
    }

    void ActorEditor::pickFogColor()
    {
        if (m_selectionInfoList.size() == 1)
        {
            if(Actor* pActor = static_cast<Actor*>(m_selectionInfoList[0].m_targetRef.getObject()))
            {
                GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                if (graphcomp)
                {
                    if (DEBUGINFO)
                        DEBUGINFO->addDebugInfoDisplay("picked Fog color", 3.0f);

                    EDITOR->getCopiedProperties().m_colorFog = graphcomp->getColorFog();
                }
            }
        }
    }

    void ActorEditor::pasteFogColor()
    {
        UndoRedoSequenceScope scope("ItemId_Actor_PasteFogProperties");
        for (u32 iSel = 0; iSel < m_selectionInfoList.size(); ++iSel)
        {
            if(Actor* pActor = static_cast<Actor*>(m_selectionInfoList[iSel].m_targetRef.getObject()))
            {
                GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                if (graphcomp)
                {
                    UNDOREDO_MANAGER->preChange(pActor);
                    graphcomp->setColorFog(EDITOR->getCopiedProperties().m_colorFog);

                    PLUGINGATEWAY->onObjectChanged(pActor);

                }
            }
        }
    }

    void ActorEditor::onPickableSceneChanged( Pickable* _pPickable, Scene* _src, Scene* _dst )
    {
        if(Actor* pActor = _pPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            // Update binds
            const ActorBindHandler& childrenHandler = pActor->getChildrenBindHandler();
            for(u32 i = 0; i < childrenHandler.getChildren().size(); ++i)
            {
                if(Actor* pChild = static_cast<Actor*>(childrenHandler.getChildren()[i].getObject()))
                {
                    if(ActorBind* pBind = pChild->getParentBind())
                    {
                        SceneObjectPathUtils::getRelativePathFromObject(pChild, pActor, pBind->m_parent);
                        PLUGINGATEWAY->onObjectChanged(pChild);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "Bind data not found on child");
                    }
                }
            }

            // Update links
            PickableList changedObjects;
            LINKMANAGER->onPickableSceneChange(pActor, _src, _dst, changedObjects);
            for(u32 iObj = 0; iObj < changedObjects.size(); ++iObj)
                PLUGINGATEWAY->onObjectChanged(changedObjects[iObj]);

            // Hierarchical notification
            if(SubSceneActor* pSSA = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                Scene* pSubScene = pSSA->getSubScene();
                const PickableList& pickables = pSubScene->getPickableList();
                for(u32 iPick = 0; iPick < pickables.size(); ++iPick)
                {
                    Pickable* pPickable = pickables[iPick];
                    Scene* pPickableScene = pActor->getScene();
                    PLUGINGATEWAY->onPickableSceneChanged(pPickable, pPickableScene, pPickableScene); // same scene but different hierarchy
                }
            }
        }
    }

    void ActorEditor::changeTemplate(const String& _relativeFilePath, Actor_Template* _template, bbool _useEditorCallbacks)
    {
#ifndef ITF_FINAL
        ITF_ASSERT(FilePath::getExtension(_relativeFilePath) == "act");
        ActorHotReload reloadActor;

        String fixedRelativeFilePath = FilePath::normalizePath(_relativeFilePath);
        String8 fixedRelativeFilePath8(fixedRelativeFilePath.cStr());
        ITF_VECTOR<ObjectRef>* objList = LUA_HANDLER->getLUAFileClients(fixedRelativeFilePath8);

        if (!objList)
            return;

        ITF_VECTOR<ObjectRef>::const_iterator sit = objList->begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objList->end();

        for (; sit != send; ++sit)
        {
            ObjectRef actRef = *sit;
            Actor* act = static_cast<Actor*>(actRef.getObject());
            ITF_ASSERT(act); //must be not null;if null the object has been deleted and still used in this list..
            if (act)
            {
                act->fillHotReloadData(reloadActor);
                act->clear();
                act->setOverrideTemplate(_template);
                act->hotReload(reloadActor);

                if(_useEditorCallbacks && EDITOR)
                {
                    EDITOR->deletePickingShapesForObject(actRef);
                    m_addedActiveActorsLastFrame.push_back(actRef);
                    PLUGINGATEWAY->onObjectChanged(act);
                }
            }
        }
#endif
    }

    bbool ActorEditor::modifyOverridedTemplate(const String & _relativeFilePath, Actor_Template *_copyTemplate)
    {
        return modifyOverridedTemplate(_relativeFilePath, _copyTemplate, ESerialize_Data_Save, ESerialize_Data_Load);
    }

    bbool ActorEditor::modifyOverridedTemplate(const String & _relativeFilePath, Actor_Template *_copyTemplate, ESerializeFlags _saveFlag, ESerializeFlags _loadFlag)
    {
#ifndef ITF_FINAL
        ITF_ASSERT(FilePath::getExtension(_relativeFilePath) == "act");
        Actor_Template * overridedTemplate = NULL;
        ITF_VECTOR <ActorHotReload> reloadActors;

        String fixedRelativeFilePath = FilePath::normalizePath(_relativeFilePath);
        String8 fixedRelativeFilePath8(fixedRelativeFilePath.cStr());
        ITF_VECTOR<ObjectRef>* objList = LUA_HANDLER->getLUAFileClients(fixedRelativeFilePath8);

        if (!objList)
            return bfalse;


        reloadActors.reserve(objList->size());
        ITF_VECTOR<ObjectRef>::const_iterator sit = objList->begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objList->end();

        for (; sit != send; ++sit)
        {
            ObjectRef actRef = *sit;
            Actor* act = static_cast<Actor*>(actRef.getObject());
            ITF_ASSERT(act); //must be not null;if null the object has been deleted and still used in this list..
            if (act && act->getOverrideTemplate())
            {

                reloadActors.resize(reloadActors.size()+1);
                act->fillHotReloadData(reloadActors[reloadActors.size()-1]);

                // remove actor from active object
                Scene *scn = act->getScene();
                if (scn)
                    scn->removeActiveObject(act->getRef());

                act->clear();

                if (!overridedTemplate)
                {
                    overridedTemplate = act->getOverrideTemplate();
                }
                else if (overridedTemplate != act->getOverrideTemplate())
                {
                    return bfalse;
                }
            }
        }

        if (!overridedTemplate)
            return bfalse;

        BinaryClone(_copyTemplate, overridedTemplate, _saveFlag, _loadFlag);
        overridedTemplate->onTemplateLoaded();

        for ( u32 i = 0; i < reloadActors.size(); i++ )
        {
            ActorHotReload& hotreloadInfo = reloadActors[i];
            if(Actor* pActor = hotreloadInfo.m_actorRef.getActor())
                pActor->hotReload(hotreloadInfo);

            if(EDITOR)
            {
                if(Actor* pActor = hotreloadInfo.m_actorRef.getActor())
                {
                    EDITOR->deletePickingShapesForObject(pActor->getRef());
                    m_addedActiveActorsLastFrame.push_back(pActor->getRef());
                    PLUGINGATEWAY->onObjectChanged(pActor);
                }
            }
        }
#endif
        return btrue;
    }

    ActorEditor::~ActorEditor()
    {
        unregisterAllListeners();
        SF_DEL(m_createActorBuffer);

        ActorEditor::m_instance = NULL;
    }

    void ActorEditor::addActorToIconCreationList( ObjectRef oref )
    {
        m_addedActiveActorsLastFrame.push_back(oref);
    }

    void onActorHotreload( Actor* _actor, bbool _before )
    {
        const ObjectRef& oref = _actor->getRef();

        if(_before)
        {
            if(EDITOR)
                EDITOR->deletePickingShapesForObject(oref);
        }
        else
        {
            if(EDITOR)
                ActorEditor::getInstance()->addActorToIconCreationList(oref);

            PLUGINGATEWAY->onObjectChanged(_actor);
        }
    }

} // namespace ITF
