// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_LINKEDITOR_H_
#include "tools/plugins/LinkEditor/LinkEditor.h"
#endif //_ITF_LINKEDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _ITF_LINK_PLUGIN_H_
#include "rayman/ray_editor/plugins/Link_Plugin.h"
#endif //_ITF_LINK_PLUGIN_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

namespace ITF
{

    void LinkEditor::update()
    {
        m_hoveredLinkParent.invalidate();
        m_hoveredLinkChild.invalidate();

        if(!EDITOR->getUnderMouseRealPickingShape())
        {
            Vec2d parentPos, childPos;
            u32 color = 0;

            f32 smallestDist = -1.0f;
            const f32 MAX_DIST = 5.0f;

            Link_Plugin* pLP = Link_Plugin::s_instance;
            const ITF_VECTOR<Link_Plugin::DisplayedLink>& linkList = pLP->m_displayedListCurrentFrame;
            for(u32 i = 0; i < linkList.size(); ++i)
            {
                const Link_Plugin::DisplayedLink& link = linkList[i];

                const f32 localDist = fabsf(PointSignedDistToSegment(EDITOR->getMouse2d(), link.parentScreenPos, link.childScreenPos));
                
                if(localDist < MAX_DIST)
                {
                    if(smallestDist <= 0.0f || smallestDist > localDist)
                    {
                        m_hoveredLinkParent = link.parent;
                        m_hoveredLinkChild = link.child;
                        parentPos = link.parentScreenPos;
                        childPos = link.childScreenPos;
                        color = link.color.getAsU32();
                    }
                }
            }

            if(m_hoveredLinkParent.isValid() && m_hoveredLinkChild.isValid())
            {
                GFX_ADAPTER->draw2dLine(parentPos, childPos, 10.0f, color);
            }
        }
    }

    void LinkEditor::receive( Blob& _blob, const NETPeer* _peer, u32 _senderID )
    {
        String message;
        _blob.extractString(message);

        if( message == getUpdateCmdName() )
        {
            receiveUpdateTags(_blob);
        }
        else
        {
            ITF_ASSERT(0); // unknown command
        }
    }

    bbool LinkEditor::isPickingAllowed()
    {
        return !m_hoveredLinkParent.isValid() || !m_hoveredLinkChild.isValid();
    }

    void LinkEditor::onMouseButton( InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if(InputAdapter::MB_Left == _but && InputAdapter::Double_Press == _status)
        {
            if(m_hoveredLinkParent.isValid() && m_hoveredLinkChild.isValid())
            {
                if(const Actor* pParent = static_cast<const Actor*>(m_hoveredLinkParent.getObject()))
                {
                    const LinkComponent* pLC = pParent->GetComponent<LinkComponent>();
                    const LinkComponent::ChildrenList& children = pLC->getChildren();

                    for (u32 i = 0; i < children.size(); i++)
                    {
                        const ChildEntry& child = children[i];
                        const ObjectPath& path  = child.getPath();

                        Pickable* pickable = NULL;
                        if(path.getIsAbsolute())
                            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                        else
                            pickable = SceneObjectPathUtils::getObjectFromRelativePath(pParent, path);

                        if(pickable && pickable->getRef() == m_hoveredLinkChild)
                        {
                            SYSTEM_ADAPTER->launchExe("UA_LinkEditor.exe", NULL, btrue, btrue);

                            if(!m_editorPeer)
                            {
                                m_viewedActor = m_hoveredLinkParent;
                                m_viewedLink = child.getPath();
                            }
                            else
                                sendFill(m_hoveredLinkParent, child);

                            break;
                        }
                    }
                }
            }
        }
    }

    void LinkEditor::onCloseApp()
    {
        if(m_editorPeer)
        {
            sendClear();
        }
    }

    void LinkEditor::onObjectChanged( BaseObject* _pObject )
    {
        if(m_editorPeer && _pObject->getRef() == m_viewedActor)
        {
            if(Actor* pAct = _pObject->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
            {
                if(LinkComponent* pLC = pAct->GetComponent<LinkComponent>())
                {
                    const LinkComponent::ChildrenList& children = pLC->getChildren();

                    LinkComponent::ChildrenList::const_iterator it = std::find(children.begin(), children.end(), m_viewedLink);
                    if(it != children.end())
                    {
                        sendFill(pAct->getRef(), *it);
                    }
                    else
                    {
                        sendClear();
                    }
                }
            }
        }
    }

    void LinkEditor::UnselectAll( bbool _force )
    {
        if(m_editorPeer)
        {
            sendClear();
        }
    }

    void LinkEditor::sendFill( ObjectRef ownerRef, const ChildEntry& _childEntry )
    {
        // blob map
        //
        // string   plugin name
        // string   cmd type
        //
        // uint32   owner ref
        // string8  child path
        // uint32   tag count
        // ...
        // string8  tag
        // string8  value
        // ...

        Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getFillCmdName());

        blob.pushUInt32(ownerRef.getValue());

        ITF_STDSTRING str;
        _childEntry.getPath().toString(str);
        blob.pushString8(String8(str.c_str()));

        const ChildEntry::TagValueList& tags = _childEntry.getTagValueList();
        const u32 tagCount = tags.size();
        blob.pushUInt32(tagCount);

        for(u32 i = 0; i < tagCount; ++i)
        {
            const TagValue& tv = tags[i];
            blob.pushString8(tv.m_tag.getDebugString());
            blob.pushString8(tv.m_value);
        }

        if(PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID))
        {
            m_viewedActor = ownerRef;
            m_viewedLink = _childEntry.getPath();
        }
        else
        {
            m_viewedActor.invalidate();
            m_viewedLink.invalidate();
            m_editorPeer = NULL;
        }
    }

    void LinkEditor::sendClear()
    {
        Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getClearCmdName());

        if(!PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID))
            m_editorPeer = NULL;

        m_viewedActor.invalidate();
        m_viewedLink.invalidate();
    }

    void LinkEditor::receiveUpdateTags( Blob& _blob )
    {
        // blob map
        //
        // uint32   owner ref
        // string8  child path
        // uint32   tag count
        // ...
        // string8  tag
        // string8  value
        // ...

        ObjectRef oref = _blob.extractUint32();

        String childPath;
        _blob.extractString(childPath);
        ITF_STDSTRING strPath(String8(childPath.cStr()).cStr());
        ObjectPath objPath;
        objPath.fromString(strPath);
        
        const u32 tagCount = _blob.extractUint32();

        if(Actor* pActor = static_cast<Actor*>(oref.getObject()))
        {
            if(LinkComponent* pLC = pActor->GetComponent<LinkComponent>())
            {
                UndoRedoSequenceScope scope("Update tags");
                UNDOREDO_MANAGER->preChange(pActor);
                if(pLC->removeTags(objPath))
                {
                    for(u32 iTag = 0; iTag < tagCount; ++iTag)
                    {
                        String tag, value;
                        _blob.extractString(tag);
                        _blob.extractString(value);

                        pLC->setTagValue(objPath, tag, String8(value.cStr()));
                    }

                    PLUGINGATEWAY->onObjectChanged(pActor);
                }
                else
                {
                    scope.cancel();
                }
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't find the component");
            }
        }
        else
        {
            ITF_ASSERT_MSG(0, "Can't find the actor");
        }
    }

    void LinkEditor::onToolConnect( const NETPeer* _peer, u32 _senderID )
    {
        Plugin::onToolConnect(_peer, _senderID);

        if(m_viewedActor.isValid() && m_viewedLink.isValid())
        {
            if(Actor* pActor = static_cast<Actor*>(m_viewedActor.getObject()))
            {
                if(LinkComponent* pLC = pActor->GetComponent<LinkComponent>())
                {
                    const LinkComponent::ChildrenList& children = pLC->getChildren();
                    
                    LinkComponent::ChildrenList::const_iterator it = std::find(children.begin(), children.end(), m_viewedLink);
                    if(it != children.end())
                    {
                        sendFill(m_viewedActor, *it);
                    } 
                    else
                    {
                        ITF_ASSERT_MSG(0, "Can't find the viewed link!");
                    }
                }
            }
        }
    }

} // namespace ITF
