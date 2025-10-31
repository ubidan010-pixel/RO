// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_COLORCOMPUTER_PLUGIN_H_
#include "tools/plugins/ActorEditor/Plugin_colorComputer.h"
#endif //_ITF_COLORCOMPUTER_PLUGIN_H_

#ifndef _ITF_COLORCOMPUTERCOMPONENT_H_
#include "engine/actors/components/colorComputercomponent.h"
#endif //_ITF_COLORCOMPUTERCOMPONENT_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{
    colorComputer_Plugin* colorComputer_Plugin::m_instance = NULL;

    Interface_ActorEditorListener* colorComputer_Plugin::create()
    {
        ITF_ASSERT(!m_instance);
        m_instance = newAlloc(mId_Editor, colorComputer_Plugin());
        ActorEditor::getInstance()->registerListener(m_instance, colorComputerComponent::GetClassCRCStatic(), bfalse);
     
        return m_instance;
    }

    void colorComputer_Plugin::destroy()
    {
        ITF_ASSERT(m_instance);
    }

    void colorComputer_Plugin::fillContextMenu(EditorContextMenu& _menu, Actor* _pActor, ActorComponent* _pComp)
    {
        _menu.addSubItem(EditorContextMenu::ItemId_colorComputerApplyTagIdToSelection, "Apply TagId to selection", EditorContextMenu::ItemId_Edit, "Edit");
        _menu.addSubItem(EditorContextMenu::ItemId_colorComputerFogToSelection, "Compute Fog to selection", EditorContextMenu::ItemId_Edit, "Edit");
        _menu.addSubItem(EditorContextMenu::ItemId_colorComputerColorToSelection, "Compute Color to selection", EditorContextMenu::ItemId_Edit, "Edit");
        _menu.addSubItem(EditorContextMenu::ItemId_colorComputerApplyFogToTagId, "Compute Fog to tagId", EditorContextMenu::ItemId_Edit, "Edit");
        _menu.addSubItem(EditorContextMenu::ItemId_colorComputerApplyColorToTagId, "Compute Color to tagId", EditorContextMenu::ItemId_Edit, "Edit");
    }

    void colorComputer_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        PickingShape* firstshape = EDITOR->getSelectionAt(0);
        BaseObject* firstowner = firstshape->getOwnerPtr();
        Actor* pfirstActor = (Actor*)firstowner;
        colorComputerComponent* colorComp = pfirstActor->GetComponent<colorComputerComponent>();
        
        UndoRedoSequenceScope changes("colorapply");

        if (colorComp)
        {
            switch (_item->getId())
            {
                case EditorContextMenu::ItemId_colorComputerFogToSelection:
                case EditorContextMenu::ItemId_colorComputerColorToSelection:
                case EditorContextMenu::ItemId_colorComputerApplyTagIdToSelection:
                {
                    for (u32 iSel = 1; iSel < EDITOR->getSelectionSize(); iSel++)
                    {
                        PickingShape* shape = EDITOR->getSelectionAt(iSel);
                        if (shape)
                        {
                            BaseObject* owner = shape->getOwnerPtr();
                            if (owner && owner->getObjectType() == BaseObject::eFrise)
                            {
                                UNDOREDO_MANAGER->preChange(((Frise*)owner));
                                f32 f = colorComp->getFactorFromZ( ((Frise*)owner)->getDepth());
                                Color out = Color::colorBlend(&colorComp->getNearColor(), &colorComp->getFarColor(), f);
                                
                                if (_item->getId() == EditorContextMenu::ItemId_colorComputerFogToSelection)
                                    ((Frise*)owner)->setColorFog(out);
                                else if (_item->getId() == EditorContextMenu::ItemId_colorComputerColorToSelection)
                                    ((Frise*)owner)->setColorFactor(out);
                                else if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyTagIdToSelection)
                                    ((Frise*)owner)->setColorComputerTagId(colorComp->getTagId());

                                PLUGINGATEWAY->onObjectChanged(owner);
                            }
                            else if (owner && owner->getObjectType() == BaseObject::eActor)
                            {
                                SubSceneActor* pSSA = owner->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
                                if(pSSA)
                                {
                                     // scan all actors from all subscenes.
                                    Scene* pScene = pSSA->getSubScene();
                                    if (!pScene)
                                        continue;
                                    SafeArray<ActorRef> AllActors;
                                    SafeArray<ObjectRef> AllFriezes;
                                    const PickableList&  actors = pScene->getActors();
                                    for (u32 itActor = 0; itActor < actors.size(); itActor++)
                                    {
                                        Actor * pActor = static_cast<Actor*>(actors[itActor]);
                                        if (pActor->isSerializable() && !pActor->isSpawned())
                                        {
                                            pActor->registerContainedPickables(AllActors,AllFriezes);
                                        }
                                    }
                                    const PickableList& friezeList = pScene->getFrises();
                                    for (u32 i=0; i<friezeList.size(); i++)
                                    {
                                        const Frise* pFrz = (const Frise*)friezeList[i];
                                        AllFriezes.push_back(pFrz->getRef());
                                    }       

                                    for (u32 itActor = 0; itActor < AllActors.size(); itActor++)
                                    {
                                        Actor * pActor = AllActors[itActor].getActor();
                                        if (pActor && pActor->isSerializable() && !pActor->isSpawned())
                                        {
                                            GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                                            if (graphcomp)
                                            {
                                                UNDOREDO_MANAGER->preChange(pActor);
                                                f32 f = colorComp->getFactorFromZ( pActor->getDepth());
                                                Color out = Color::colorBlend(&colorComp->getNearColor(), &colorComp->getFarColor(), f);
                                                if (_item->getId() == EditorContextMenu::ItemId_colorComputerFogToSelection)
                                                    graphcomp->setColorFog(out);
                                                else if (_item->getId() == EditorContextMenu::ItemId_colorComputerColorToSelection)
                                                    graphcomp->setColorFactor(out);                                       
                                                else if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyTagIdToSelection)
                                                    graphcomp->setTagId(colorComp->getTagId());
                                                PLUGINGATEWAY->onObjectChanged(pActor);
                                             }
                                        }
                                    }

                                    for (u32 it = 0; it < AllFriezes.size(); it++)
                                    {
                                        Frise * frz = (Frise *)AllFriezes[it].getObject();
                                        if (frz)
                                        {
                                            UNDOREDO_MANAGER->preChange(frz);
                                            f32 f = colorComp->getFactorFromZ( frz->getDepth());
                                            Color out = Color::colorBlend(&colorComp->getNearColor(), &colorComp->getFarColor(), f);
                                            
                                            if (_item->getId() == EditorContextMenu::ItemId_colorComputerFogToSelection)
                                                frz->setColorFog(out);
                                            else if (_item->getId() == EditorContextMenu::ItemId_colorComputerColorToSelection)
                                                frz->setColorFactor(out);
                                            else if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyTagIdToSelection)
                                                frz->setColorComputerTagId(colorComp->getTagId());

                                            PLUGINGATEWAY->onObjectChanged(frz);
                                        }
                                    }
                                }
                                else if (Actor* pActor = ((Actor*)owner))
                                {
                                    GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                                    if (graphcomp)
                                    {
                                        UNDOREDO_MANAGER->preChange(pActor);
                                        f32 f = colorComp->getFactorFromZ( pActor->getDepth());
                                        Color out = Color::colorBlend(&colorComp->getNearColor(), &colorComp->getFarColor(), f);
                                        if (_item->getId() == EditorContextMenu::ItemId_colorComputerFogToSelection)
                                            graphcomp->setColorFog(out);
                                        else if (_item->getId() == EditorContextMenu::ItemId_colorComputerColorToSelection)
                                            graphcomp->setColorFactor(out);                                       
                                        else if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyTagIdToSelection)
                                            graphcomp->setTagId(colorComp->getTagId());
                                        PLUGINGATEWAY->onObjectChanged(pActor);
                                     }
                                }                       
                            }
                        }
                    }
                }
                break;
                case EditorContextMenu::ItemId_colorComputerApplyFogToTagId:
                case EditorContextMenu::ItemId_colorComputerApplyColorToTagId:
                {
                    if (!colorComp->getTagId())
                        break;

                    World* pWorld = CURRENTWORLD;
                    if(!pWorld)
                        break;

                    // scan all actors from all scenes.
                    Scene* pScene = pWorld->getRootScene();
                    if (!pScene)
                        break;
                    SafeArray<ActorRef> AllActors;
                    SafeArray<ObjectRef> AllFriezes;
                    const PickableList&  actors = pScene->getActors();
                    for (u32 itActor = 0; itActor < actors.size(); itActor++)
                    {
                        Actor * pActor = static_cast<Actor*>(actors[itActor]);
                        if (pActor->isSerializable() && !pActor->isSpawned())
                        {
                            pActor->registerContainedPickables(AllActors,AllFriezes);
                        }
                    }
                    const PickableList& friezeList = pScene->getFrises();
                    for (u32 i=0; i<friezeList.size(); i++)
                    {
                        const Frise* pFrz = (const Frise*)friezeList[i];
                        AllFriezes.push_back(pFrz->getRef());
                    } 

                    // find same Id.
                    for (u32 itActor = 0; itActor < AllActors.size(); itActor++)
                    {
                        Actor * pActor = AllActors[itActor].getActor();
                        if (pActor && pActor->isSerializable() && !pActor->isSpawned())
                        {
                            GraphicComponent* graphcomp = pActor->GetComponent<GraphicComponent>();
                            if (graphcomp)
                            {
                                if (graphcomp->getTagId() == colorComp->getTagId())
                                {
                                    UNDOREDO_MANAGER->preChange(pActor);
                                    f32 f = colorComp->getFactorFromZ( pActor->getDepth());
                                    Color out = Color::colorBlend(&colorComp->getNearColor(), &colorComp->getFarColor(), f);
                                    if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyFogToTagId)
                                        graphcomp->setColorFog(out);
                                    else if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyColorToTagId)
                                        graphcomp->setColorFactor(out);                                       
                                    PLUGINGATEWAY->onObjectChanged(pActor);
                                }
                             }                    
                        }
                    }
                    // find same Id.
                    for (u32 it = 0; it < AllFriezes.size(); it++)
                    {
                        Frise * frz = (Frise *)AllFriezes[it].getObject();
                        if (frz && (frz->getColorComputerTagId() == colorComp->getTagId()))
                        {
                            UNDOREDO_MANAGER->preChange(frz);
                            f32 f = colorComp->getFactorFromZ( frz->getDepth());
                            Color out = Color::colorBlend(&colorComp->getNearColor(), &colorComp->getFarColor(), f);
                            
                            if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyFogToTagId)
                                frz->setColorFog(out);
                            else if (_item->getId() == EditorContextMenu::ItemId_colorComputerApplyColorToTagId)
                                frz->setColorFactor(out);

                            PLUGINGATEWAY->onObjectChanged(frz);
                            
                        }
                    }
                }
                break;
            }
        }
    }

    void colorComputer_Plugin::addComponentSpecificIcons(Actor* _pActor, ActorComponent* _pComp, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize)
    {
        _icons.clear();
        if (_pActor->isSelected())
            _icons.push_back(Editor_IconStock::Icon_colorComputerSelected);
        else
            _icons.push_back(Editor_IconStock::Icon_colorComputer);
    }

} // namespace ITF
