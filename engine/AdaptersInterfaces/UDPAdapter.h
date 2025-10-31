#ifndef _ITF_UDPADAPTER_H_
#define _ITF_UDPADAPTER_H_

namespace ITF
{


class NETEvent
{
public:
    enum    eventType
    {
        none,
        connect,
        receive,
        disconnect,
        ENUM_FORCE_SIZE_32(0)
    };

                NETEvent();
                ~NETEvent();
    eventType   m_type;
    u32         m_host;
    u32         m_port;
    u32         m_dataLength;
    u8*            m_data;
    void*       m_peer;
};

class UDPAdapter
{
public:
            enum    PeerType
            {
                PeerType_GameHost           =   0,
                PeerType_LogClient,
                PeerType_CmdLineClient,
                PeerType_Count,
                ENUM_FORCE_SIZE_32(0)
            };

            enum    RecipientType
            {
                RecipientType_CommandsHandler =   0,
                RecipientType_Application,
                RecipientType_Count,
                ENUM_FORCE_SIZE_32(1)
            };

            u32         hostConnection          ( const char* _hostAddress, u16 _hostPort   );
            void        sendPacketToHost        ( RecipientType _type, void* _pckt, u32 _size );
            void        sendPacketToPeer        ( PeerType _peer, const char* _pckt, u32 _size         );
            void        packetReceivedByHost    ( void* _data, u32 _size                    );
            void        setHost                 ( u16 _port, u8 _maxConnections             );
            void        setClient               (                                           );
            void        initialize              (                                           );
            void        updateHost              (                                           );
            void        setPeer                 ( PeerType _type, void* _peer               ) {m_peers[_type] = _peer;}
            void        declareLogClient        (                                           );
            void        declareCmdLineClient    (                                           );
    virtual             ~UDPAdapter     (                                           ) {}; // always declare virtual destructor for adapters
    virtual void        close           (                                           )   =   0;
    virtual u32         getEvent        ( NETEvent& _evt                            )   =   0;
    virtual void        sendPacketToPeer( void* _peer, const char* _pckt, u32 _size   )   =   0;
    virtual void        broadcastPacket ( const char* _pckt                         )   =   0;

protected:
    virtual void        createHost      ( u16 _port, u8 _maxConnections             )   =   0;
    virtual void        createClient    (                                           )   =   0;
    virtual void*       connectToHost   ( const char* _hostAddress, u16 _hostPort   )   =   0;
    virtual void        init            (                                           )   =   0;
            void*       m_peers[PeerType_Count];
            u32         m_isHost;
};


#define GAME_SERVER_PORT     1000
#define LOG_CLIENT_ID        "logclient"
#define CMDLINE_CLIENT_ID    "cmdlineclient"

} // namespace ITF



#endif //_ITF_UDPADAPTER_H_