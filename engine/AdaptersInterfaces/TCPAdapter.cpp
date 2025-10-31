#include "precompiled_engine.h"

#ifndef _ITF_NETADAPTER_H_
#include "engine/AdaptersInterfaces/NetAdapter.h"
#endif //_ITF_NETADAPTER_H_

#ifndef _ITF_TCPADAPTER_H_
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif //_ITF_TCPADAPTER_H_

#include <algorithm>

#ifdef ITF_SUPPORT_NET

namespace ITF
{
void   TCPPacketHeader::swap() 
{
    u32 tmp;

    Endian::hideEndian<u32>(m_uglobalBodySize,(u8*) &tmp);
    m_uglobalBodySize = tmp;

    Endian::hideEndian<u32>(m_ubodySize,(u8*) &tmp);
    m_ubodySize = tmp;

    Endian::hideEndian<u32>(m_umaxIndex,(u8*) &tmp);
    m_umaxIndex = tmp;

    Endian::hideEndian<u32>(m_ulID,(u8*) &tmp);
    m_ulID = tmp;
    
    Endian::hideEndian<u32>(m_uIndex,(u8*) &tmp);
    m_uIndex = tmp;
};

TCPPeer::TCPPeer(NETAdapter* netAdapter,SOCKET s,sockaddr_in* addr):m_Socket(s),m_AddrIn(*addr),m_packetRestore(NULL)
{
    m_NetworkAdapter    = netAdapter;
    m_packetRestore     = newAlloc(mId_System, TCPPacketRestore());
};

TCPPeer::~TCPPeer()
{
    close();
    SF_DEL(m_packetRestore);
}


void   TCPPeer::close() 
{
    if (m_Socket != INVALID_SOCKET)
    {
#ifdef ITF_PS3
        socketclose(m_Socket);
#elif defined ITF_SONY
        sceNetSocketClose(m_Socket);
#elif defined(ITF_WII)
        SOClose(m_Socket);
#elif defined(ITF_CAFE)
#ifdef ITF_CAFE_TCP
        socketclose(m_Socket);
#endif // ITF_CAFE_TCP
#else
#ifndef ITF_CONSOLE_FINAL
        closesocket(m_Socket);
#endif //ITF_CONSOLE_FINAL
#endif //ITF_PS3
        m_Socket = INVALID_SOCKET;
    }
}

void  TCPPeer::restorePacket(u8* buffer,u32 size)
{
    m_packetRestore->get(this,buffer,size);
}


bbool   TCPPeer::hasPacketAvailable() const
{
    ITF_ASSERT(m_packetRestore!=NULL);
    return m_packetRestore->hasPacketAvailable();
}

NETPacket*  TCPPeer::getPacket()
{
    ITF_ASSERT(m_packetRestore!=NULL);
    return m_packetRestore->getPacket(); 
}

//---------------------------------------------------------------------------------------------------------------------------------------

TCPPacketRestore::TCPPacketRestore(): m_headerAvailable(0),m_bPacketAvailable(bfalse),m_remainingBodySize(0),
                                      m_currentPositionInHeader(0),m_netCurrentPacket(NULL),m_ulIDCurrent(0)
{
    Synchronize::createCriticalSection(&m_csRestore);
};

TCPPacketRestore::~TCPPacketRestore()
{
    while(!m_NetPacketAvailable.empty())
    {
        NETPacket* pPacket = m_NetPacketAvailable.front();

        SF_DEL(pPacket);

        m_NetPacketAvailable.pop();
    }
    Synchronize::destroyCriticalSection(&m_csRestore);
}

NETPacket* TCPPacketRestore::getPacket()
{   
    NETPacket* packet = NULL;
    csAutoLock cs(m_csRestore);
    if (m_bPacketAvailable)
    {
        packet = m_NetPacketAvailable.front();
        m_NetPacketAvailable.pop();
        m_bPacketAvailable = (m_NetPacketAvailable.size()>0);
    }

    return packet;
}

void TCPPacketRestore::get(TCPPeer* peer,u8* bufferReceived,u32 sizeReceived)
{
    u8*     currentBuffer       = bufferReceived;
    u32     avaibableSize       = sizeReceived;

    //HEADER SECTION
    while (avaibableSize!=0)
    {

        //No header available,get one..
        if  (!m_headerAvailable)
        {
            u32 headerSizeAllowed = std::min(avaibableSize,TCPPacketHeader_Size-m_currentPositionInHeader);
            u8* addr =(u8*) &m_currentHeader;
            addr += m_currentPositionInHeader;
            ITF_Memcpy(addr,currentBuffer,headerSizeAllowed);   

            m_currentPositionInHeader += headerSizeAllowed;
            currentBuffer += headerSizeAllowed;
            avaibableSize -= headerSizeAllowed;

            //Header is done ?
            if (m_currentPositionInHeader == TCPPacketHeader_Size)
            {
                m_currentHeader.swap();

                m_headerAvailable = btrue;
                m_currentPositionInHeader = 0;
                m_remainingBodySize = m_currentHeader.getBodySize();
            }
        }
                

        //BODY SECTION
        //we have a valid header;work with it
        if (m_headerAvailable)
        {
            //check Header integrity
            ITF_ASSERT(m_currentHeader.compareheader()==bfalse);

            //make a new buffer if doesn't exist..
            if (!m_netCurrentPacket)
            {
                m_netCurrentPacket = newAlloc(mId_Plugin,NETPacket());

                m_ulIDCurrent = m_currentHeader.getID();
                //Allocate a buffer with complete global size;
                m_netCurrentPacket->allocate(m_currentHeader.getGlobalBodySize());
                m_netCurrentPacket->setCurrentSize(0);//start from 0;
            }

            //help to track bad packet insertion
            ITF_ASSERT(m_ulIDCurrent == m_currentHeader.getID());
            //get the size we need to fill
            u32 needToCopy = std::min(avaibableSize,m_remainingBodySize);
            m_remainingBodySize -= needToCopy;

            //Add to the pending net packet...
            //LOG("server:current receive:%d id msg %d %d/%d  %d/%d %d\n",m_currentHeader.m_ulID,needToCopy);
        
            m_netCurrentPacket->append((char*)currentBuffer,needToCopy);
            currentBuffer += needToCopy;
            avaibableSize -= needToCopy;


            
            //The current body size is complete ?
            //Yes = the next packet should contain a header..
            if (m_remainingBodySize == 0)
            {
                m_headerAvailable = bfalse;

                //check the net Packet is filled ?
                //yes ,push the buffer in the restored FIFO queue
                if (m_netCurrentPacket->isFilled())
                {
                    m_netCurrentPacket->setPeer(peer);
                    {
                        csAutoLock cs(m_csRestore);
                        m_NetPacketAvailable.push(m_netCurrentPacket);
                        m_bPacketAvailable = btrue;
                    }
                    
                    m_netCurrentPacket = NULL;//tell that we are waiting for a new one...
                }
                
            } //else ... just continue
            

        }//still availableSize continue..

    }
}

TCPPacketHeader::TCPPacketHeader()
{
    buildPacket(0, 0, 0, 0, 0);
}
//---------------------------------------------------------------------------------------------------------------------------------------

void TCPPacketHeader::buildPacket(u32 globalBodySize,u32 currentBodySize,u32 maxIndex,u32 index,u32 ID)
{
    strcpy(m_uHeader,TCP_PACKETHEADER);

    Endian::hideEndian<u32>(globalBodySize,(u8*) &m_uglobalBodySize);
    Endian::hideEndian<u32>(currentBodySize,(u8*) &m_ubodySize);
    Endian::hideEndian<u32>(maxIndex,(u8*) &m_umaxIndex);
    Endian::hideEndian<u32>(ID,(u8*) &m_ulID);
    Endian::hideEndian<u32>(index,(u8*) &m_uIndex);
};

//---------------------------------------------------------------------------------------------------------------------------------------

TCPAdapter::TCPAdapter():m_bHost(bfalse),m_bIsStartup(bfalse),m_bExitReceiveThread(bfalse),
                         m_tcpHost(NULL),m_CurrentSendID(0),m_ReceiveThread(NULL),m_funcOnNewClient(NULL),
                         m_funcOnReceivePacket(NULL),m_ExitEvent()
{
    Synchronize::createCriticalSection(&m_cs);
};

TCPAdapter::~TCPAdapter()
{
    destroy();
    Synchronize::destroyCriticalSection(&m_cs);
}

void TCPAdapter::destroy()
{
    if(m_ReceiveThread)
    {
        m_bExitReceiveThread = btrue;
        Synchronize::waitEvent(&m_ExitEvent);
        m_ReceiveThread = NULL;
        Synchronize::destroyEvent(&m_ExitEvent);
    }

    for (i32 currentClient = (m_vPeers.size() - 1); currentClient >= 0; --currentClient)
    {
        unregisterClient(m_vPeers[currentClient]);
    }
    m_vPeers.clear();
}

u32 WINAPI funcReceive(void* user)
{
    TCPAdapter* adapter = (TCPAdapter*)(user);
    adapter->process_receive();
    
    return 0;
}

void TCPAdapter::process_receive()
{
    m_bExitReceiveThread  = bfalse;

    // TODO make a member of it
    u8 bufferToReceive[TCPReceiveData_Size];

    while (!m_bExitReceiveThread)
    {   
        ThreadSettings::threadStartFrame();

        //[TODO] make it thread safe
        for (u32 currentClient = 0;currentClient<m_vPeers.size();++currentClient)
        {
            TCPPeer* tcpClient = m_vPeers[currentClient];
            u32 sizeReturned = TCPReceiveData_Size;

            while (sizeReturned!=0)
            {
                receivePacketPrivate(tcpClient, bufferToReceive, sizeReturned);

                if (sizeReturned)
                    tcpClient->restorePacket(bufferToReceive, sizeReturned);

                //TODO find away to avoid server overloading...
            }
        }

        sleep(1);
    }

    if(m_bExitReceiveThread)
    {        
        Synchronize::setEvent(&m_ExitEvent);
    }
}


u32 WINAPI funcReceive2(void* user)
{

    return 0;
}

bbool TCPAdapter::setHost(const char* _hostAddress,  u16 _port)
{
    bbool res = createHost(_hostAddress,  _port);

    //if the host creation is successful,we run a thread in background to manage new receive..
    if (res == btrue)
    {
        m_ReceiveThread = THREADMANAGER->createThread(funcReceive, this, ThreadSettings::m_settings[eThreadId_TCPReceivePackage]);
        Synchronize::createEvent(&m_ExitEvent,NULL);
        Synchronize::resetEvent(&m_ExitEvent);
    }

    return res;
}

u32 TCPAdapter::hostConnection(const char* _hostAddress, u16 _hostPort)
{
   bbool  res = connectToHost(_hostAddress, _hostPort);
   if (res == btrue)
   {
       m_ReceiveThread = THREADMANAGER->createThread(funcReceive, this, ThreadSettings::m_settings[eThreadId_TCPReceivePackage]);
       Synchronize::createEvent(&m_ExitEvent,NULL);
       Synchronize::resetEvent(&m_ExitEvent);
   }

   return res;
}

bbool TCPAdapter::sendPacketToAllPeers(NETPacket*  _packetBuffer)
{
    bbool ret = btrue;
    ITF_VECTOR<TCPPeer*>::iterator iterPeer = m_vPeers.begin();
    for (; iterPeer != m_vPeers.end(); ++iterPeer)
    {   
        _packetBuffer->setPeer((*iterPeer));
        if (!sendPacket(_packetBuffer))
        {
            ret = bfalse;
        }
    }
    return ret;
}


bbool TCPAdapter::sendPacket(NETPacket*  packetbuf)
{
    csAutoLock cs(m_cs);
    //in client mode,we send always to the server
    if (!isHost())
        packetbuf->setPeer(m_tcpHost);

    //Construct each tcp_packet and send them

    u32 myBufferSize = u32(packetbuf->getCurrentSize());
    u8* buffer = (u8*)  packetbuf->getData();
    u32 bufferSizeBody              = 0;
    u32 currentIndexPacket          = 0;
    u32 positionInBuffer            = 0;
    u32 sendID                      = getNewSendID();
    
    //[TODO] use a pool instead
    u8 bufferToSend[TCPBufferData_Size];

    u32 maxIndex =  (myBufferSize+1)/TCPPacketBody_Size;

    const NETPeer* peer = packetbuf->getPeer();

    while (positionInBuffer<myBufferSize)
    {
        if ((positionInBuffer+TCPPacketBody_Size)>myBufferSize)
        {
            bufferSizeBody = myBufferSize-positionInBuffer;
        }
        else
        {
            bufferSizeBody = TCPPacketBody_Size;
        }

        TCPPacketHeader packetHeader;
        packetHeader.buildPacket(myBufferSize,bufferSizeBody,maxIndex,currentIndexPacket, sendID);

        ITF_Memcpy(bufferToSend,&packetHeader,TCPPacketHeader_Size);
        ITF_Memcpy(bufferToSend+TCPPacketHeader_Size,(buffer+positionInBuffer),bufferSizeBody);
        u32 packetSize      = (bufferSizeBody+TCPPacketHeader_Size);
        positionInBuffer    += bufferSizeBody;

        //   sleep(1);
        //[TODO] check for result..
        if(!sendPacketPrivate(peer, (const u8*)bufferToSend, packetSize))
            return bfalse;

        currentIndexPacket++;
    }

    return btrue;
}


void TCPAdapter::setClient()
{
    createClient();
}

void TCPAdapter::updateHost()
{
    updateHostPrivate();
    
}

bbool TCPAdapter::initialize()
{
    return init();
}

void  TCPAdapter::unregisterClient(TCPPeer* Peer)
{
    if (Peer)
    {
        Peer->close();
        ITF_VECTOR<TCPPeer*>::iterator iterPeer = find(m_vPeers.begin(),m_vPeers.end(),Peer);
        if (iterPeer != m_vPeers.end())
        {   
            m_vPeers.erase(iterPeer);
            SF_DEL(Peer);
        }
    }
}


TCPPeer*  TCPAdapter::registerClient(SOCKET s,sockaddr_in* addr)
{
    TCPPeer* peer = newAlloc(mId_System,TCPPeer(this,s,addr));
    
    m_vPeers.push_back(peer);
    return peer;
}

bbool TCPAdapter::receivePacketBuilt(NETPacket*  packetbuf)
{
    return bfalse;
}

bbool TCPAdapter::receivePacket(NETPacket**  packetbuf)
{
    for (u32 currentClient = 0;currentClient<m_vPeers.size();++currentClient)
    {
        TCPPeer* tcpClient = m_vPeers[currentClient];
        if (tcpClient->hasPacketAvailable())
        {
            //pass the the first packet available...
            *packetbuf = tcpClient->getPacket();
            return btrue;
        }
    }

    return bfalse;
}

void TCPAdapter::setPeerName( const NETPeer* _peer, const String& _name )
{
    for (u32 currentClient = 0;currentClient<m_vPeers.size();++currentClient)
    {
        TCPPeer* tcpClient = m_vPeers[currentClient];

        if(_peer == tcpClient)
        {
            tcpClient->setName(_name);
            break;
        }
    }
}
}// namespace ITF

#endif // ITF_SUPPORT_NET
