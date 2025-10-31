#ifndef ITF_NETPACKETHANDLER_H_
#define ITF_NETPACKETHANDLER_H_

#pragma once

#if defined( ITF_SUPPORT_NET )

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_UDPADAPTER_H_
#include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif //_ITF_UDPADAPTER_H_

namespace ITF
{
    class NETPacket;
    class NETPacketClient;
    class NETPacket;
    class NETPeer;
    class NETAdapter;
    class TCPAdapter;

    struct NETPacketHeader
    {
        static const u32 ALL_ID = 0;

        u32 m_SenderID;
        u32 m_ReceiverID;
    };

    class NETPacketHandler
    {
    public:
        static const u32 Max_Packet_Size        =   8*1024;
        static const u32 Packet_Header_Size     =   sizeof(NETPacketHeader);
        static const u32 Max_Packet_Body_Size   =   Max_Packet_Size - Packet_Header_Size;

    public:
        NETPacketHandler();

        ~NETPacketHandler();

        /// Send a message through network
        /// @param _blob the blob
        /// @param _peer the network receiver
        /// @param _senderID the sender ID
        /// @param _receiverID the receiver ID
        /// @return btrue if the send succeed
        bbool send(Blob& _blob, const NETPeer* _peer, u32 _senderID, u32 _receiverID)const;

        // Pump incoming network messages and dispatch them to registered clients
        void update(NETAdapter* _pNetAdapter)const;

        /// Find a listener from a unique id
        /// @param _id listener id
        /// @return the listener pointer or null if not found
        NETPacketClient*   getListener(u32 _id)const;

        /// Add a listener
        /// @return the assigned client ID
        u32 registerClient(NETPacketClient * _client);

        /// Remove a listener
        /// @param _clientID a client ID
        void unregisterClient(u32 _clientID);

        /// Register an adapter
        /// @param _adapter the adapter
        void registerAdapter(NETAdapter *_adapter);

    private:
        /// Transmit the packet to the according listener
        /// @param _packet a packet
        void dispatch(const NETPacket &_packet)const;

        /// Get the pointer to the header of a packet
        /// @param _packet a packet
        NETPacketHeader* getPacketHeader(const NETPacket &_packet)const;

        /// Get the pointer to the body of a packet
        /// @param _packet a packet
        u8* getPacketBody(const NETPacket &_packet)const;

        /// Get the body size of a packet
        /// @param _packet a packet
        u32 getPacketBodySize(const NETPacket &_packet)const;

        /// Internal handling of listeners
        u32                                 m_nextFreeID;
        ITF_MAP<u32 , NETPacketClient* >    m_listeners;
        ITF_VECTOR<NETAdapter *>            m_Adapters;
        TCPAdapter*                         m_tcpAdapter;
    };
}
#endif // ITF_SUPPORT_NET

#endif // _ITF_NETPACKETHANDLER_H_
