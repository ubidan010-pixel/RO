#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_CAMERA_PLUGIN_H_
#include "rayman/ray_editor/plugins/Camera_Plugin.h"
#endif //_ITF_CAMERA_PLUGIN_H_

#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantNodeComponent.h"
#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_PlatformerCameraComponent.h"
#endif //_ITF_RAY_PLATFORMERCAMERACOMPONENT_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{
    Camera_Plugin* Camera_Plugin::m_instance = NULL;

    Interface_ActorEditorListener* Camera_Plugin::create()
    {
        ITF_ASSERT(!m_instance);
        m_instance = newAlloc(mId_Editor, Camera_Plugin());
        ActorEditor::getInstance()->registerListener(m_instance, BaseCameraComponent::GetClassCRCStatic());
        ActorEditor::getInstance()->registerListener(m_instance, CameraModifierComponent::GetClassCRCStatic());
        
        return m_instance;
    }

    void Camera_Plugin::destroy()
    {
        ITF_ASSERT(m_instance);
    }

    void Camera_Plugin::onObjectChanged(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void Camera_Plugin::onSelected(Actor* _pActor)
    {
    }

    void Camera_Plugin::fillContextMenu(EditorContextMenu& _menu, Actor* _pActor, ActorComponent* _pComp)
    {
        _menu.addSubItemFromExternalPlugin(Camera_AlignL_L, "Align left left", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignL_R, "Align left right", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignR_R, "Align right right", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignR_L, "Align right left", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignWidth, "Align width", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignT_T, "Align top top", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignT_B, "Align top bottom", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignB_B, "Align bottom bottom", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignB_T, "Align bottom top", CameraComponent_Item, "Camera Component");
        _menu.addSubItemFromExternalPlugin(Camera_AlignHeight, "Align height", CameraComponent_Item, "Camera Component");




//         _menu.addSubItemFromExternalPlugin(Camera_ShowDebugInfo, "show debug info", CameraComponent_Item, "Camera Component");
//         _menu.addSubItemFromExternalPlugin(Camera_Stuff1, "activate stuff 1", CameraComponent_Item, "Camera Component");
//         _menu.addSubItemFromExternalPlugin(Camera_Stuff2, "activate stuff 2", CameraComponent_Item, "Camera Component");
    }

    void Camera_Plugin::onContextMenuItemSelected(ContextMenuItem* _item, Actor* _pRefActor, ActorComponent* _pComp)
    {
        switch (_item->getIdFromExternal())
        {
        case Camera_AlignL_L:
            {
                UndoRedoSequenceScope scope("Align left left");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleX = pActor->getAABB().getMax().m_x - _pRefActor->getAABB().getMin().m_x;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_x = newScaleX;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setX(pActor->getAABB().getMax().m_x - newScaleX / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignL_R:
            {
                UndoRedoSequenceScope scope("Align left right");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleX = pActor->getAABB().getMax().m_x - _pRefActor->getAABB().getMax().m_x;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_x = newScaleX;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setX(pActor->getAABB().getMax().m_x - newScaleX / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignR_R:
            {
                UndoRedoSequenceScope scope("Align right right");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleX = _pRefActor->getAABB().getMax().m_x - pActor->getAABB().getMin().m_x;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_x = newScaleX;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setX(pActor->getAABB().getMin().m_x + newScaleX / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignR_L:
            {
                UndoRedoSequenceScope scope("Align right left");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleX = _pRefActor->getAABB().getMin().m_x - pActor->getAABB().getMin().m_x;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_x = newScaleX;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setX(pActor->getAABB().getMin().m_x + newScaleX / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignWidth:
            {
                UndoRedoSequenceScope scope("Align the selection");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                Vec2d newscale = pActor->getScale();
                                newscale.m_x = _pRefActor->getScale().m_x;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setX(_pRefActor->getPos().getX());
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);

                            }
                        }
                    }
                }

            }
            break;

        case Camera_AlignT_T:
            {
                UndoRedoSequenceScope scope("Align top top");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleY = _pRefActor->getAABB().getMax().m_y - pActor->getAABB().getMin().m_y;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_y = newScaleY;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setY(pActor->getAABB().getMin().m_y + newScaleY / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignT_B:
            {
                UndoRedoSequenceScope scope("Align top bottom");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleY = _pRefActor->getAABB().getMin().m_y - pActor->getAABB().getMin().m_y;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_y = newScaleY;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setY(pActor->getAABB().getMin().m_y + newScaleY / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignB_B:
            {
                UndoRedoSequenceScope scope("Align bottom bottom");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleY = pActor->getAABB().getMax().m_y - _pRefActor->getAABB().getMin().m_y;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_y = newScaleY;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setY(pActor->getAABB().getMax().m_y - newScaleY / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignB_T:
            {
                UndoRedoSequenceScope scope("Align bottom top");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                const f32 newScaleY = pActor->getAABB().getMax().m_y - _pRefActor->getAABB().getMax().m_y;

                                Vec2d newscale = pActor->getScale();
                                newscale.m_y = newScaleY;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setY(pActor->getAABB().getMax().m_y - newScaleY / 2.0f);
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);
                            }
                        }
                    }
                }
            }
            break;

        case Camera_AlignHeight:
            {
                UndoRedoSequenceScope scope("Align the selection");

                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    if(Actor* pActor = resolvedRefs[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                    {
                        if(CameraModifierComponent* pComp = pActor->GetComponent<CameraModifierComponent>())
                        {
                            if(_pRefActor != pActor)
                            {
                                UNDOREDO_MANAGER->preChange(pActor);

                                Vec2d newscale = pActor->getScale();
                                newscale.m_y = _pRefActor->getScale().m_y;
                                pActor->setScale(newscale);
                                pActor->setWorldInitialScale(newscale);

                                Vec3d pos = pActor->getPos();
                                pos.setY(_pRefActor->getPos().getY());
                                pActor->setPos(pos);
                                pActor->setWorldInitialPos(pos);
                                
                                PLUGINGATEWAY->onObjectChanged(pActor);

                            }
                        }
                    }
                }

            }
            break;

        default:
            break;
        }
    }

    void Camera_Plugin::onUpdate()
    {

    }
    
    void Camera_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _pComp)
    {
        if ( _pComp->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(CameraModifierComponent,1821234798) )
        {
            ((CameraModifierComponent*)_pComp)->init();
        }
    }

    void Camera_Plugin::addComponentSpecificIcons(Actor* _pActor, ActorComponent* _pComp, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize)
    {
        if ( _pComp->GetObjectClassCRC() != ITF_GET_STRINGID_CRC(CameraModifierComponent,1821234798))
        {
            _icons.clear();
            _icons.push_back(Editor_IconStock::Icon_CameraActor);
        }
    }

    void    Camera_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
    }
    
    void    Camera_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
    }

} // namespace ITF
