#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_UDPADAPTER_H_
#include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif //_ITF_UDPADAPTER_H_

namespace ITF
{


NETEvent::NETEvent()
: m_data(0)
, m_peer(NULL)
{
}

NETEvent::~NETEvent()
{
    SF_DEL_ARRAY(m_data);
}



u32 UDPAdapter::hostConnection(const char* _hostAddress, u16 _hostPort)
{
    m_peers[PeerType_GameHost] = connectToHost(_hostAddress, _hostPort);
    if (m_peers[PeerType_GameHost])
        return 1;
    return 0;
}


void UDPAdapter::sendPacketToHost(RecipientType _type, void* _pckt, u32 _size)
{
    if (!m_peers[PeerType_GameHost])
        return;
    u32 finalsize = _size + 1 + sizeof(RecipientType);
    char* packet = new char[_size + 1 + sizeof(RecipientType)];
    char* dst = packet;
    ITF_Memcpy(dst, &_type, sizeof(RecipientType));
    dst += sizeof(RecipientType);
    ITF_Memcpy(dst, _pckt, _size);
    dst[_size] = 0;
    sendPacketToPeer(m_peers[PeerType_GameHost], packet, finalsize);
    *packet = 0; // make sure no use is done after packet is freed
    delete[] packet;
}


void UDPAdapter::sendPacketToPeer( PeerType _peerType, const char* _pckt, u32 _size )
{
    if (!m_peers[_peerType])
        return;
    sendPacketToPeer(m_peers[_peerType], _pckt, _size);
}


void UDPAdapter::setHost(u16 _port, u8 _maxConnections)
{
    m_isHost = 1;
    createHost(_port, _maxConnections);
}

void UDPAdapter::setClient()
{
    m_isHost = 0;
    createClient();
}

void UDPAdapter::initialize()
{
    for (int i = 0 ; i < PeerType_Count; i++)
    {
        m_peers[i] = 0;
    }
    m_isHost = 0;
    init();
}

void UDPAdapter::updateHost()
{/*
    ITF_ASSERT(m_isHost);
    NETEvent evt;
    if (getEvent(evt))
    {
        switch (evt.m_type)
        {
        case NETEvent::connect :
            break;
        case NETEvent::disconnect :
            break;
        case NETEvent::receive :
            {
                if (evt.m_peer && evt.m_peer == m_peers[PeerType_CmdLineClient])
                {
                    CMD_HANDLER->handleCmdLine(&evt);
                }
                else
                {
                    switch (*((RecipientType*)evt.m_data))
                    {
                    case RecipientType_CommandsHandler:
                        CMD_HANDLER->handleNetCommand(&evt);
                        break;
                    case RecipientType_Application:
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }
    }*/
}

void UDPAdapter::declareLogClient()
{
    String str(LOG_CLIENT_ID);
    sendPacketToHost(RecipientType_CommandsHandler, (void*)str.cStr(), str.getLen()); // tell the game we are client for the log messages
}

void UDPAdapter::declareCmdLineClient()
{
    String str(CMDLINE_CLIENT_ID);
    sendPacketToHost(RecipientType_CommandsHandler, (void*)str.cStr(), str.getLen()); // tell the game we are client for the cmdline messages
}

} // namespace ITF

