#ifndef _ITF_UDPADAPTER_ENET_H_
#define _ITF_UDPADAPTER_ENET_H_

// See RAKNET for more comprehensive technology

#ifndef _ITF_UDPADAPTER_H_
    #include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif // _ITF_UDPADAPTER_H_

namespace ITF
{

class UDPAdapter_ENet : public UDPAdapter
{
private:
    void*   m_connection;

public:
                ~UDPAdapter_ENet(                                           ) {close();}
    void        init            (                                           );
    void        close           (                                           );
    void        createHost      ( u16 _port, u8 _maxConnections             );
    void        createClient    (                                           );
    u32         getEvent        ( NETEvent& _evt                            );
    void        sendPacketToPeer(   void* _peer, const char* _pckt, u32 _size          );
    void        broadcastPacket (   const char* _pckt                       );    
protected:
    void*      connectToHost   ( const char* _hostAddress, u16 _hostPort   );
};

} // namespace ITF



#endif //_ITF_UDPADAPTER_ENET_H_