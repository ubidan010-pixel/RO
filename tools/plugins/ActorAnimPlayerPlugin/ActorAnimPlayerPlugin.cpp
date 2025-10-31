// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_ACTORANIMPLAYERPLUGIN_H_
#include "tools/plugins/ActorAnimPlayerPlugin/ActorAnimPlayerPlugin.h"
#endif //_ITF_ACTORANIMPLAYERPLUGIN_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

namespace ITF
{

ActorAnimPlayerPlugin::ActorAnimPlayerPlugin()
: m_currentSelection(ITF_INVALID_OBJREF)
, m_toolIsAware(bfalse)
{
}

ActorAnimPlayerPlugin::~ActorAnimPlayerPlugin()
{
}

void ActorAnimPlayerPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
    String message;

    _blob.extractString(message);

    if ( message == getCommandAnimData() )
    {
        sendAnimData();
    }
    else if ( message == getCommandInputValue() )
    {
        receiveInputValue(_blob);
    }
    else if ( message == getCommandActionValue() )
    {
        receiveActionValue(_blob);
    }
    else if ( message == getCommandPauseActor() )
    {
        receivePauseActor(_blob);
    }
}

void ActorAnimPlayerPlugin::onActorPicked( ObjectRef _actor )
{
    BaseObject* obj = GETOBJECT(_actor);
    if ( obj && obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
    {
        m_currentSelection = _actor;

        if ( m_editorPeer )
        {
            sendAnimData();
            return;
        }
    }

    if(m_editorPeer)
        sendClearSelection();
}

void ActorAnimPlayerPlugin::UnselectAll(bbool _force)
{
    pauseActor(bfalse);

    m_currentSelection = ITF_INVALID_OBJREF;
    if(m_editorPeer)
        sendClearSelection();
}

void ActorAnimPlayerPlugin::sendAnimData()
{
    ITF_ASSERT(m_editorPeer);

    BaseObject* obj = GETOBJECT(m_currentSelection);

    if ( !obj )
    {
        return;
    }

    Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

    if ( !actor )
    {
        return;
    }

    String8 buffer;

    collectAnimDataFromActor(actor,buffer);

    if ( buffer.isEmpty() )
    {
        return;
    }

    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString(getCommandAnimData());
    blob.pushString8(buffer.cStr());

    m_toolIsAware = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
}

void ActorAnimPlayerPlugin::sendClearSelection()
{
    ITF_ASSERT(m_editorPeer);

    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString(getCommandClear());

    m_toolIsAware = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
}

void ActorAnimPlayerPlugin::receiveInputValue( Blob& _blob )
{
    String inputName;
    String inputValue;

    _blob.extractString(inputName);
    _blob.extractString(inputValue);

    BaseObject* obj = GETOBJECT(m_currentSelection);

    if ( !obj )
    {
        return;
    }

    Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

    if ( !actor )
    {
        return;
    }

    AnimatedComponent* animComponent = actor->GetComponent<AnimatedComponent>();

    if ( !animComponent )
    {
        return;
    }

    StringID inputId = StringConverter(inputName).getChar();
    u32 index = animComponent->findInputIndex(inputId);
    InputType type = animComponent->getInputType(index);

    if ( type == InputType_F32 )
    {
        f32 num = static_cast<f32>(atof(StringConverter(inputValue).getChar()));
        animComponent->setInput(inputId,num);
    }
    else
    {
        u32 num = static_cast<u32>(strtoul(StringConverter(inputValue).getChar(),NULL,10));
        animComponent->setInput(inputId,num);
    }
}

void ActorAnimPlayerPlugin::receiveActionValue( Blob& _blob )
{
    String actionName;

    _blob.extractString(actionName);

    BaseObject* obj = GETOBJECT(m_currentSelection);

    if ( !obj )
    {
        return;
    }

    Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

    if ( !actor )
    {
        return;
    }

    AnimatedComponent* animComponent = actor->GetComponent<AnimatedComponent>();

    if ( !animComponent )
    {
        return;
    }

    StringID animId = StringConverter(actionName).getChar();
    animComponent->setAnim(animId);
}

void ActorAnimPlayerPlugin::receivePauseActor( Blob& _blob )
{
    bbool isPause = _blob.extractBool();

    pauseActor(isPause);
}

void ActorAnimPlayerPlugin::pauseActor( bbool _pause )
{
    BaseObject* obj = GETOBJECT(m_currentSelection);

    if ( !obj )
    {
        return;
    }

    Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

    if ( !actor )
    {
        return;
    }

    AnimatedComponent* animComponent = actor->GetComponent<AnimatedComponent>();

    if ( animComponent )
    {
        animComponent->setDisableInputUpdate(_pause);
    }
}

void ActorAnimPlayerPlugin::update()
{
    // If we have a selection and the tool don't knows anything about it
    if(m_editorPeer && !m_toolIsAware && m_currentSelection != ITF_INVALID_OBJREF)
        onActorPicked(m_currentSelection);
}

void ActorAnimPlayerPlugin::collectAnimDataFromActor( Actor* _actor, String8& _buffer ) const
{
    AnimatedComponent* animComponent = _actor->GetComponent<AnimatedComponent>();

    if ( !animComponent )
    {
        return;
    }

    String8 temp;

    _buffer = _actor->getUserFriendly();
    _buffer += ",";

    u32 numActions = animComponent->getNumAnimTreeNodes();
    temp.ui32toa(numActions);
    _buffer += temp;
    _buffer += ",";

    for ( u32 i = 0; i < numActions; i++ )
    {
#ifdef ITF_DEBUG_STRINGID
        const char* actionName = animComponent->getAnimTreeNodeName(i);
#else
        const char* actionName = "No String in String ID";
        ITF_ASSERT_MSG(bfalse, "String IDs are not available here");
#endif
        _buffer += actionName;
        _buffer += ",";
    }

    
    u32 numInputs = animComponent->getNumInputs();
    temp.ui32toa(numInputs);
    _buffer += temp;
    _buffer += ",";

    for ( u32 i = 0; i < numInputs; i++ )
    {
#ifdef ITF_DEBUG_STRINGID
        const char* inputName = animComponent->getInputName(i);
#else
        const char* inputName = "No String in String ID";
        ITF_ASSERT_MSG(bfalse, "String IDs are not available here");
#endif
        _buffer += inputName;
        _buffer += ",";
        InputType inputType = animComponent->getInputType(i);

        temp.ui32toa(static_cast<u32>(inputType));
        _buffer += temp;
        _buffer += ",";

        if ( inputType == InputType_U32 )
        {
            u32 value;
            animComponent->getInputValue(i,value);
            temp.ui32toa(value);
            _buffer += temp;
            _buffer += ",";
        }
        else
        {
            f32 value;
            animComponent->getInputValue(i,value);
            temp.f32toa(value);
            _buffer += temp;
            _buffer += ",";
        }
    }
}

} // namespace ITF

