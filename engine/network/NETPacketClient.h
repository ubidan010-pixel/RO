#ifndef ITF_NETPACKETCLIENT_H_
#define ITF_NETPACKETCLIENT_H_

#pragma once

#ifdef ITF_SUPPORT_NET

#include "core/Blob.h"

namespace ITF
{
    class NETPeer;
    class NETPacketHandler;

    class NETPacketClient
    {
    public:
        NETPacketClient();

        virtual ~NETPacketClient();

        /// Send a packet over the network
        /// @param _blob the blob
        /// @param _peer the network receiver
        /// @param _receiverID the receiver ID
        /// @return btrue if the send succeed
        bbool send(NETPacketHandler* pNetPacketHandlerBlob,Blob& _blob,const NETPeer* _peer, u32 _receiverID = 0)const;

        /// Read a tick
        /// @param  _blob           The message.
        void readTick(Blob& _blob)const;

        /// Receive a packet from the network.
        /// This function is not a pure virtual because not every implementation needs to override it.
        /// @param _blob the data
        /// @param _peer the network sender
        /// @param _senderID the sender ID
        /// @param _unknowListener btrue if the sender doesn't know the receiver ID
        /// @return btrue if the client absorbs the incoming data
        virtual bbool receive(Blob& _blob, const NETPeer* _peer, u32 _senderID, bbool _unknowListener);

        /// Start receiving packets from network
        void startReceive(NETPacketHandler* pNetPacketHandler);
        /// Stop receiving packets from network
        void stopReceive(NETPacketHandler* pNetPacketHandler);

    private:
        u32 m_ID;
    };
}
#endif // ITF_SUPPORT_NET

#endif // ITF_NETPACKETCLIENT_H_
