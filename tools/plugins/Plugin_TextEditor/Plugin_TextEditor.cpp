// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_PLUGIN_TEXTEDITOR_H_
#include "tools/plugins/Plugin_TextEditor/Plugin_TextEditor.h"
#endif //_ITF_PLUGIN_TEXTEDITOR_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_TODOCOMPONENT_H_
#include "gameplay/Components/Misc/TODOComponent.h"
#endif //_ITF_TODOCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

namespace ITF
{

Plugin_TextEditor::Plugin_TextEditor()
{
    m_curActor = ITF_INVALID_OBJREF;
    m_lastShownActor = ITF_INVALID_OBJREF;
    m_lastAlive = 0.f;
}

void    Plugin_TextEditor::onTextEntered   (   Blob& _blob  )
{
    String str;
    _blob.extractString(str);
    TODOComponent* pTodo = getCurrentTODO();
    if (pTodo)
    {
        pTodo->setTextLabel(str);
    }
}

void    Plugin_TextEditor::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID)
{
    String message;
    _blob.extractString(message);

    if (message == getTextEnteredCmdName())
    {
        onTextEntered(_blob);
    }
    else if (message == getAliveCmdName())
    {
        m_lastAlive = CURRENTFRAME;
    }
    else
    {
        ITF_ASSERT(0); // unknown command
    }
}

void    Plugin_TextEditor::update()
{
    //   PickingShape* pShapeUnderMouse = EDITOR->getUnderMousePickingShape();
    PickingShape* pShapeUnderMouse = NULL;
    if (EDITOR->getSelectionSize())
        pShapeUnderMouse = EDITOR->getSelectionAt(0);

    if(pShapeUnderMouse)
    {
        ObjectRef oRef = pShapeUnderMouse->getOwner();
        
        if (oRef != m_curActor)
        {
            ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
            blob.pushString(getPluginName());
            blob.pushString(getHideCmdName());
            PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
            m_lastShownActor = ITF_INVALID_OBJREF;
            m_curActor = ITF_INVALID_OBJREF;
        }
    }
}

void    Plugin_TextEditor::onCloseApp( )
{
    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString(getCloseCmdName());
    PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
}


void    Plugin_TextEditor::showMyActor()
{
 /*   if (m_lastShownActor == m_curActor)
        return;*/
    m_lastShownActor = m_curActor;

    TODOComponent* pTodo = getCurrentTODO();
    if (pTodo)
    {
        f64 thisTime = SYSTEM_ADAPTER->getEllaspedFromStart();
        if (fabs(thisTime - m_lastAlive) > 3.f)
        {
            m_lastAlive = thisTime;
            SYSTEM_ADAPTER->launchExe("UA_TextEditor.exe");
        }
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(COMMAND_WATCH);
        blob.pushString(pTodo->getTextLabel());
        int wx, wy;
        SYSTEM_ADAPTER->getWindowPos(wx, wy);
        Vec3d pos2d;
        GFX_ADAPTER->compute3DTo2D(pTodo->GetActor()->getPos(), pos2d);
        blob.pushInt32((i32)pos2d.m_x + wx);
        blob.pushInt32((i32)pos2d.m_y + wy);
        PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
    }
}

void    Plugin_TextEditor::onActorPicked(ObjectRef _actor)
{
    m_curActor =  _actor;
}

TODOComponent*  Plugin_TextEditor::getCurrentTODO()
{
    Actor* act = (Actor*)(GETOBJECT(m_curActor));
    if (act)
    {
        return act->GetComponent<TODOComponent>();
    }
    return NULL;
}

void Plugin_TextEditor::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
{
    if (_but == InputAdapter::MB_Left && _status == InputAdapter::Double_Press)
    {
        PickingShape* pShapeUnderMouse = EDITOR->getPickingShapeOfObjectUnderMouse();
        if(pShapeUnderMouse)
        {
            ObjectRef oRef = pShapeUnderMouse->getOwner();
            {
                BaseObject* bo = GETOBJECT(oRef);
                if (bo && bo->getObjectType() == BaseObject::eActor)
                {
                    Actor* act = (Actor*)bo;
                    if (act->GetComponent<TODOComponent>())
                    {
                        m_curActor = oRef;
                        showMyActor();
                        return;
                    }
                }
                ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
                blob.pushString(getPluginName());
                blob.pushString(getHideCmdName());
                PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
                m_lastShownActor = ITF_INVALID_OBJREF;
                m_curActor = ITF_INVALID_OBJREF;
            }
        }
    }
}

void Plugin_TextEditor::onToolConnect( const NETPeer* _peer, u32 _senderID )
{
    Plugin::onToolConnect(_peer, _senderID);

    showMyActor();
}
} // namespace ITF


