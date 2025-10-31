// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_UDPADAPTER_ENET_H_
    #include "adapters/UDPAdapter_ENet/UDPAdapter_ENet.h"
#endif // _ITF_UDPADAPTER_ENET_H_

#ifndef __ENET_ENET_H__
    #include "enet/enet.h" // don't change this path, "enet/include" MUST be in the include directories
#endif // __ENET_ENET_H__

namespace ITF
{

void UDPAdapter_ENet::init()
{
    m_connection = 0;
    if (enet_initialize () != 0)
    {
         ITF_ASSERT(!"An error occurred while initializing ENet.");
        return;
    }
    atexit (enet_deinitialize);
}

void UDPAdapter_ENet::close()
{
    if (m_connection)
        enet_host_destroy((ENetHost*)m_connection);

    m_connection = 0;
}

void UDPAdapter_ENet::createHost( u16 _port, u8 _maxConnections )
{
    ITF_ASSERT(!m_connection);
    ENetAddress address;

    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */

    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = _port;

    m_connection = enet_host_create (& address /* the address to bind the server host to */, 
                                 32      /* allow up to 32 clients and/or outgoing connections */,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (m_connection == NULL)
    {
        ITF_ASSERT(!"An error occurred while trying to create an ENet server host.");
    }
}

void UDPAdapter_ENet::createClient()
{
    ITF_ASSERT(!m_connection);

    m_connection = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    if (m_connection == NULL)
    {
        ITF_ASSERT(!"An error occurred while trying to create an ENet client host.");
    }
}


u32 UDPAdapter_ENet::getEvent( NETEvent& _evt )
{
    ITF_ASSERT(m_connection);
    ITF_ASSERT(!_evt.m_data);   // please cleanup your data before overwriting it

    ENetEvent event;

    _evt.m_type = NETEvent::none;
    _evt.m_peer = 0;

    if (enet_host_service ((ENetHost*)m_connection, &event, 0) > 0)
    {

        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            _evt.m_host = event.peer -> address.host;
            _evt.m_port = event.peer -> address.port;
            _evt.m_type = NETEvent::connect;
            return 1;

        case ENET_EVENT_TYPE_RECEIVE:
            _evt.m_type = NETEvent::receive;
            _evt.m_dataLength = (u32)(event.packet -> dataLength);
            _evt.m_data = new u8[_evt.m_dataLength];
            _evt.m_peer = event.peer;
            memcpy(_evt.m_data, event.packet->data, _evt.m_dataLength);

            enet_packet_destroy (event.packet);
            return 1;
            
           
        case ENET_EVENT_TYPE_DISCONNECT:
            _evt.m_type = NETEvent::disconnect;
            return 1;
        }
    }

    return 0;
}

void* UDPAdapter_ENet::connectToHost( const char* _hostAddress, u16 _hostPort )
{
    ITF_ASSERT(m_connection);
    ENetAddress address;
    ENetEvent event;
    ENetPeer *peer;


    enet_address_set_host (& address, _hostAddress);
    address.port = _hostPort;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect ((ENetHost*)m_connection, & address, 2);    
    
    if (peer == NULL)
    {
        ITF_ASSERT(!"No available peers for initiating an ENet connection.");
        return 0;
    }
    
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service ((ENetHost*)m_connection, & event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        return &((ENetHost*)m_connection)->peers[0]; // success
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset (peer);

        // should not be asserted, we can have a retry ITF_ASSERT (!"Connection to server failed.");
    }
    return 0;
}


void UDPAdapter_ENet::sendPacketToPeer(void* _peer, const char* _pckt, u32 _size)
{
    ENetPacket * packet = enet_packet_create (_pckt, 
                                              _size, 
                                              ENET_PACKET_FLAG_RELIABLE);

    
    enet_peer_send ((ENetPeer*)_peer, 0, packet);
    enet_host_flush ((ENetHost*)m_connection);
}

void UDPAdapter_ENet::broadcastPacket(const char* _pckt)
{
    ENetPacket * packet = enet_packet_create (_pckt, 
                                              strlen (_pckt) + 1, 
                                              ENET_PACKET_FLAG_RELIABLE);

    
    enet_host_broadcast ((ENetHost*)m_connection, 0, packet);
    enet_host_flush ((ENetHost*)m_connection);
}

} // namespace ITF

