// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_GLOBALWATCHPLUGIN_H_
#include "GlobalWatch.h"
#endif //_ITF_GLOBALWATCHPLUGIN_H_


namespace ITF
{


void GlobalWatch::updateWatchedValues( Blob& _blob  )
{
    if (!m_editorPeer)
        return;

    String name, val;
    _blob.extractString(name);
    _blob.extractString(val);

    for (u32 i = 0; i < m_watch.size(); i++)
    {
        if (name == m_watch[i].m_WatchName)
        {
           switch (m_watch[i].m_WatchType)
           {
            case Plugin::type_int8:
                *((i8*)m_watch[i].m_WatchAdrs) = (i8)(val.atoi32());
                break;
            case Plugin::type_uint8:
                *((u8*)m_watch[i].m_WatchAdrs) = (u8)(val.atoui32());
                break;
            case Plugin::type_int16:
                *((i16*)m_watch[i].m_WatchAdrs) = (i16)(val.atoi32());
                break;
            case Plugin::type_uint16:
                *((u16*)m_watch[i].m_WatchAdrs) = (u16)(val.atoui32());
                break;
            case Plugin::type_int32:
                *((i32*)m_watch[i].m_WatchAdrs) = val.atoi32();
                break;
            case Plugin::type_uint32:
                *((u32*)m_watch[i].m_WatchAdrs) = val.atoui32();
                break;
            case Plugin::type_int64:
                *((i64*)m_watch[i].m_WatchAdrs) = val.atoi64();
                break;
            case Plugin::type_uint64:
                *((u64*)m_watch[i].m_WatchAdrs) = val.atoui64();
                break;
            case Plugin::type_float:
                *((f32*)m_watch[i].m_WatchAdrs) = val.atof32();
                break;
            case Plugin::type_string:
                *((String*)m_watch[i].m_WatchAdrs) = val;
                break;
            default:
                ITF_ASSERT(0);
                return;
            }
           break;
        }
    }
}



GlobalWatch::GlobalWatch()
{
    m_editorPeer = NULL;
    m_lastSentTime = ELAPSEDTIME;
}


void    GlobalWatch::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID)
{
    String message;
    _blob.extractString(message);

    if (message == getSetValueCmdName())
        updateWatchedValues(_blob);
    else
    {
        ITF_ASSERT(0); // unknown command
    }
}


void GlobalWatch::update()
{
    if (!m_editorPeer)
        return;

    float time = ELAPSEDTIME;
    if (fabs(time - m_lastSentTime) < getUpdateRate())
        return;

    m_lastSentTime = time;

    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString(COMMAND_WATCH);
    blob.pushInt32(m_watch.size()); // watch count
    
    for (u32 i = 0; i < m_watch.size(); i++)
    {
        blob.pushString(m_watch[i].m_WatchName);
        blob.pushInt32(m_watch[i].m_WatchType);
        switch (m_watch[i].m_WatchType)
        {
        case Plugin::type_int8:
            blob.pushInt8(*((i8*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_uint8:
            blob.pushUInt8(*((u8*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_int16:
            blob.pushInt16(*((i16*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_uint16:
            blob.pushUInt16(*((u16*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_int32:
            blob.pushInt32(*((i32*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_uint32:
            blob.pushUInt32(*((u32*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_int64:
            blob.pushInt64(*((i64*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_uint64:
            blob.pushUInt64(*((u64*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_float:
            blob.pushFloat32(*((f32*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        case Plugin::type_string:
            blob.pushString(*((String*)m_watch[i].m_WatchAdrs)); // watch value
            break;
        default:
            ITF_ASSERT(0);
            return;
        }
    }

    PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
}

bbool GlobalWatch::watchExists (const String& _name)
{
    ITF_VECTOR<GlobalWatchData>::iterator it = m_watch.begin();
    for (; it != m_watch.end(); ++it)
    {
        if ((*it).m_WatchName == _name)
        {
            return btrue;
        }
    }
    return bfalse;
}

void GlobalWatch::addWatch (const String& _name, const i8& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const u8& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}



void GlobalWatch::addWatch (const String& _name, const i16& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const u16& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const i32& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const u32& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const i64& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const u64& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const f32& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::addWatch (const String& _name, const String& _var)
{
    if (watchExists(_name))
        return;
    m_watch.push_back(GlobalWatchData(_name, _var));
}

void GlobalWatch::removeWatch (const String& _name)
{
    ITF_VECTOR<GlobalWatchData>::iterator it = m_watch.begin();
    for (; it != m_watch.end(); ++it)
    {
        if ((*it).m_WatchName == _name)
        {
            m_watch.erase(it);
            return;
        }
    }
}

GlobalWatchData::GlobalWatchData(const String& _name, const i8& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_int8;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const u8& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_uint8;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const i16& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_int16;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const u16& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_uint16;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const i32& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_int32;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const u32& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_uint32;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const i64& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_int64;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const u64& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_uint64;
    m_WatchAdrs = (void*)&_var;
}

GlobalWatchData::GlobalWatchData(const String& _name, const f32& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_float;
    m_WatchAdrs = (void*)&_var;
}


GlobalWatchData::GlobalWatchData(const String& _name, const String& _var)
{
    m_WatchName = _name;
    m_WatchType = Plugin::type_string;
    m_WatchAdrs = (void*)&_var;
}




} // namespace ITF

