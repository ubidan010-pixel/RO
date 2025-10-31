#ifndef _ITF_TCPADAPTER_H_
#define _ITF_TCPADAPTER_H_

#ifdef ITF_SUPPORT_NET

#if	defined(ITF_SONY) || defined(ITF_IPAD)
#ifdef ITF_PS3
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netex/errno.h>
#include <netex/net.h>
#include <netinet/in.h>
#include <arpa/inet.h >
#elif defined(ITF_PS5)
#include <net.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h >
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/signal.h>
#endif	//ITF_PS3

using SOCKET = int;
using SOCKADDR_IN = struct sockaddr_in;
using SOCKADDR = struct sockaddr;
constexpr SOCKET INVALID_SOCKET = -1;
using SOCKETLEN = socklen_t;

#else
typedef int         SOCKETLEN;
#endif // defined(ITF_SONY) || defined(ITF_IPAD)

#include "engine/AdaptersInterfaces/NETAdapter.h"

#include "core/system/Synchronize.h"

namespace ITF
{

class   Thread;
class   NETPacket;
class   TCPPacketRestore;

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPPeer : public NETPeer
{
    public:
        TCPPeer(NETAdapter* netAdapter, SOCKET s,sockaddr_in* addr);
        virtual ~TCPPeer();

        void                    close();
        const   SOCKET&         getSocket()const    {return m_Socket;}
        const   sockaddr_in*    getAddrIn()         {return &m_AddrIn;}

        void                    restorePacket(u8* buffer,u32 size);
        bbool                   hasPacketAvailable() const;
        NETPacket*              getPacket();

protected:
        SOCKET m_Socket;
        sockaddr_in m_AddrIn;

        TCPPacketRestore* m_packetRestore;
};

typedef void (*ptrTCP_onNewClient)(TCPPeer*);
typedef void (*ptrTCP_onReceivePacket)(NETPacket*);


#define     TCP_PACKETHEADER           "IFT_TCP_HEAD"
const u32   TCPBufferData_Size      =   8*1024;
//Change this value if you want to test different situations
const u32   TCPReceiveData_Size     =   TCPBufferData_Size;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct TCPPacketHeader
{
    TCPPacketHeader();

    void buildPacket(u32 globalBodySize,u32 currentBodySize,u32 maxIndex,u32 index,u32 ID);
    void swap();

    u32     getBodySize() const {return m_ubodySize;};
    u32     getGlobalBodySize() const {return m_uglobalBodySize; }

    bbool   compareheader()  const {return strcmp(m_uHeader,TCP_PACKETHEADER);};

    const u32 getID() const {return m_ulID;}

private:
    char            m_uHeader[sizeof(TCP_PACKETHEADER)];
    u32             m_uglobalBodySize;
    u32             m_ubodySize;
    u32             m_uIndex;
    u32             m_umaxIndex;
    u32             m_ulID;
};


const u32   TCPPacketHeader_Size    =   sizeof(TCPPacketHeader);
const u32   TCPPacketBody_Size      =   TCPBufferData_Size-TCPPacketHeader_Size;

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPPacketRestore
{
public:
    TCPPacketRestore();
    virtual ~TCPPacketRestore();

    void        get(TCPPeer* peer,u8* buffer,u32 size);
    bbool hasPacketAvailable() const {return m_bPacketAvailable;}
    NETPacket*  getPacket();

protected:

    u8      m_headerAvailable;
    bbool   m_bPacketAvailable;

    u32     m_currentPosition;
    u32     m_remainingBodySize;
    u32     m_currentPositionInHeader;

    u32     m_ulIDCurrent;

    ITF_THREAD_CRITICAL_SECTION m_csRestore;

    TCPPacketHeader m_currentHeader;
    NETPacket*      m_netCurrentPacket;

    ITF_QUEUE<NETPacket*> m_NetPacketAvailable;

};

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPAdapter : public NETAdapter
{
public:
            TCPAdapter();

            void        destroy();
            bbool       setHost                 ( const char* _hostAddress,  u16 _port      );
            u32         hostConnection          ( const char* _hostAddress, u16 _hostPort   );
            void        setClient               (                                           );
            bbool       initialize              (                                           );
    virtual             ~TCPAdapter     (                                           );
    virtual void        close           (                                           )   =   0;
            bbool       sendPacketToAllPeers(NETPacket*  _packetBuffer);

            void        updateHost(); 
            bbool       isHost()const        {return m_bHost;}
            TCPPeer*    registerClient(SOCKET s,sockaddr_in* addr);
            void        unregisterClient(TCPPeer* tcpClient);

            bbool       sendPacket(NETPacket*  packetbuf);
            bbool       receivePacket(NETPacket**  packetbuf);


            bbool       canListen()  const {return (m_tcpHost != NULL);}
            NETPeer*    getHost()    const {return  m_tcpHost;}
               
            u32         getNewSendID()      {return ++m_CurrentSendID;}


            void        process_receive();
            
            void        setPeerName(const NETPeer* _peer, const String& _name);

            
    void                addOnNewClientCB(ptrTCP_onNewClient func) {m_funcOnNewClient = func;}
    void                addOnReceiveCB(ptrTCP_onReceivePacket func) {m_funcOnReceivePacket = func;}

protected:
    virtual bbool       createHost      (const char* _hostAddress,  u16 _port)   =   0;
    virtual void        createClient    (                                           )   =   0;
    virtual bbool       connectToHost   ( const char* _hostAddress, u16 _hostPort   )   =   0;
    virtual bbool       init            (                                           )   =   0;
    virtual bbool       sendPacketPrivate (const NETPeer* peer, const u8* buffer, u32 size)const    = 0;
    virtual bbool       receivePacketPrivate(const NETPeer* peer, u8* buffer, u32& size_returned)   = 0;
    virtual void        updateHostPrivate() = 0;


    bbool               receivePacketBuilt(NETPacket*  packetbuf);
    
    bbool               m_bHost;
    bbool               m_bIsStartup;

    volatile bbool      m_bExitReceiveThread;

    String              m_Name;
    
    TCPPeer*            m_tcpHost;

    u32                 m_CurrentSendID;

    Thread*             m_ReceiveThread;
    ITF_THREAD_EVENT    m_ExitEvent;

    ptrTCP_onNewClient      m_funcOnNewClient;
    ptrTCP_onReceivePacket  m_funcOnReceivePacket;

    ITF_THREAD_CRITICAL_SECTION   m_cs;//CS to protect exclusive sent packet


    ITF_VECTOR<TCPPeer*>    m_vPeers;          
};


} // namespace ITF

#endif // ITF_SUPPORT_NET

#endif //_ITF_TCPADAPTER_H_
