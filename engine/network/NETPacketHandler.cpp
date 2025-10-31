#include "precompiled_engine.h"

#if defined( ITF_SUPPORT_NET )

#ifndef _ITF_NETPACKETHANDLER_H_
#include "engine/network/NETPacketHandler.h"
#endif //_ITF_NETPACKETHANDLER_H_

#ifndef _ITF_NETPACKETCLIENT_H_
#include "engine/network/NETPacketClient.h"
#endif //_ITF_NETPACKETCLIENT_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TCPADAPTER_H_
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif //_ITF_TCPADAPTER_H_

#ifndef _ITF_NETADAPTER_H_
#include "engine/AdaptersInterfaces/NETAdapter.h"
#endif //_ITF_NETADAPTER_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_TCPADAPTER_ITF_H_
#include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"
#endif //_ITF_TCPADAPTER_ITF_H_

namespace ITF
{

NETPacketHandler::NETPacketHandler()
:   m_nextFreeID(0)
{
    m_tcpAdapter = newAlloc(mId_System, TCPAdapter_ITF);
}

NETPacketHandler::~NETPacketHandler()
{
    SF_DEL(m_tcpAdapter);
}

// TODO pool packet allocation
// TODO generalize MAX BODY PACKET SIZE
bbool NETPacketHandler::send(Blob& _blob,const NETPeer* _peer, u32 _senderID, u32 _receiverID)const
{
    // Allocate the packet
    NETPacket packet;
    packet.allocate(NETPacketHandler::Packet_Header_Size + _blob.getSize());

    // Fill the header
    NETPacketHeader* pHeader = getPacketHeader(packet);
    ITF_ASSERT(pHeader != NULL);
    pHeader->m_SenderID = _senderID;
    pHeader->m_ReceiverID = _receiverID;

    // Fill the body
    u8* pBody = getPacketBody(packet);
    ITF_ASSERT(pBody != NULL);
    ITF_Memcpy(pBody, _blob.getData(), _blob.getSize());

    // Send to the peer
    if (_peer)
    {
        packet.setPeer(_peer);
        return _peer->getNetworkAdpater()->sendPacket(&packet);
    }
    else
    {
        if(m_tcpAdapter)
            return m_tcpAdapter->sendPacketToAllPeers(&packet);
        else
            return bfalse;
    }
}


// TODO Timeout instead of message count limit
void NETPacketHandler::update(NETAdapter* _pNetAdapter) const
{
    if(!_pNetAdapter) return;

    NETPacket* packet = NULL;
        
    static const u32 MAX_PUMP = 500;
    u32 counter = 0;

    if (_pNetAdapter->canListen())
    {
        _pNetAdapter->updateHost();

        while(_pNetAdapter->receivePacket(&packet) && counter++ < MAX_PUMP)
        {
            dispatch(*packet);
            SF_DEL(packet);
        }
    }
}

void NETPacketHandler::dispatch(const NETPacket &_packet) const
{
    NETPacketHeader* pHeader = getPacketHeader(_packet);
    ITF_ASSERT(pHeader != NULL);

    Blob blob(getPacketBody(_packet), getPacketBodySize(_packet), BLOB_READONLY);
    if(blob.getSize() == 0) return;
    const NETPeer *pPeer = _packet.getPeer();

    ITF_ASSERT(pPeer != NULL);

    // Transmit the packet to every listeners
    if(pHeader->m_ReceiverID == NETPacketHeader::ALL_ID)
    {
        bbool bOk = bfalse;
        ITF_MAP<u32 , NETPacketClient*>::const_iterator iter;
        for(iter = m_listeners.begin(); iter != m_listeners.end(); ++iter)
        {
            blob.resetSeekPos();
            NETPacketClient *pListener =  iter->second;
            ITF_ASSERT(pListener != NULL);
            if(pListener->receive(blob, pPeer, pHeader->m_SenderID, btrue))
            {
                bOk = btrue;
                break;
            }
        }

        // A packet must have a receiver
        ITF_ASSERT(bOk);
    }
    else // Transmit the packet to a specific listener
    {
        NETPacketClient *pListener = getListener(pHeader->m_ReceiverID);

        // A packet must have a receiver
        ITF_ASSERT(pListener != NULL);

        // The listener must accept the packet
        ITF_VERIFY(pListener->receive(blob, pPeer, pHeader->m_SenderID, bfalse));
    }

    // Assert if the blob has still some data to extract
    ITF_ASSERT_MSG(blob.getSeekPos() == blob.getSize(), "The blob was not fully consumed - Please make sure all data has been read");
}

NETPacketHeader* NETPacketHandler::getPacketHeader(const NETPacket &_packet)const
{
    return (NETPacketHeader *)_packet.getData();
}

u8* NETPacketHandler::getPacketBody(const NETPacket &_packet)const
{
    return (u8*)_packet.getData() + sizeof(NETPacketHeader);
}

u32 NETPacketHandler::getPacketBodySize(const NETPacket &_packet)const
{
    return u32(_packet.getCurrentSize() - sizeof(NETPacketHeader));
}

NETPacketClient*    NETPacketHandler::getListener(u32 _id)const
{
    ITF_MAP<u32 , NETPacketClient*>::const_iterator iter = m_listeners.find(_id);
    if(iter == m_listeners.end())
    {
        return NULL;
    }

    return iter->second;
}

u32 NETPacketHandler::registerClient(NETPacketClient * _client)
{
    ITF_ASSERT(_client);

    if(m_nextFreeID == NETPacketHeader::ALL_ID)
        m_nextFreeID++;

    // Make sure we do not erase an existing record
    ITF_ASSERT(m_listeners.find(m_nextFreeID) == m_listeners.end());

    m_listeners[m_nextFreeID] = _client;

    return m_nextFreeID++;
}

void NETPacketHandler::unregisterClient(u32 _clientID)
{
    ITF_MAP<u32 , NETPacketClient*>::iterator iter = m_listeners.find(_clientID);
    // Make sure we unregister a registered listener
    ITF_ASSERT(iter != m_listeners.end());

    m_listeners.erase(iter);
}

void NETPacketHandler::registerAdapter(NETAdapter *_adapter)
{
    ITF_ASSERT(_adapter != NULL);

#if ITF_DEBUG_LEVEL > 0
    // Check that the adapter is added only once
    for(u32 i = 0; i < m_Adapters.size(); ++i)
    {
        ITF_ASSERT(m_Adapters[i] != _adapter);
    }
#endif

    m_Adapters.push_back(_adapter);
}


}
#endif //ITF_SUPPORT_NET
