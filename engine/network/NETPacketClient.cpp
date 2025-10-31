#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_NET
#ifndef _ITF_NETPACKETCLIENT_H_
#include "engine/network/NETPacketClient.h"
#endif //_ITF_NETPACKETCLIENT_H_

#ifndef _ITF_NETPACKETHANDLER_H_
#include "engine/network/NETPacketHandler.h"
#endif //_ITF_NETPACKETHANDLER_H_

#ifndef _ITF_NETADAPTER_H_
#include "engine/AdaptersInterfaces/NETAdapter.h"
#endif //_ITF_NETADAPTER_H_

namespace ITF
{
    NETPacketClient::NETPacketClient()
        :   m_ID(U32_INVALID)
    {
    }

    NETPacketClient::~NETPacketClient()
    {
        // Assert that stopReceive() have been called if startReceive() was called
        ITF_ASSERT(U32_INVALID == m_ID);
    }

    bbool NETPacketClient::send(NETPacketHandler* pNetPacketHandler,Blob& _blob, const NETPeer* _peer, u32 _receiverID /* = 0*/)const
    {
        return pNetPacketHandler->send(_blob, _peer, m_ID, _receiverID);
    }

    ITF::bbool NETPacketClient::receive( Blob& _blob, const NETPeer* _peer, u32 _senderID, bbool _unknowListener )
    {
        // We must always assert here because a received packet must be properly "listened"
        ITF_ASSERT(0);

        return bfalse;
    }

    void NETPacketClient::startReceive(NETPacketHandler* pNetPacketHandler)
    {
        m_ID = pNetPacketHandler->registerClient(this);
    }

    void NETPacketClient::stopReceive(NETPacketHandler* pNetPacketHandler)
    {
        pNetPacketHandler->unregisterClient(m_ID);
 
        m_ID = U32_INVALID;
    }

    void NETPacketClient::readTick( Blob& _blob ) const
    {
        _blob.extractBool();
    }

}
#endif //ITF_SUPPORT_NET

