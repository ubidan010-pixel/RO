#include "precompiled_TCPAdapter_ITF.h"

#ifdef ITF_SUPPORT_NET

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_UDPADAPTER_H_
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif //_ITF_UDPADAPTER_H_

#ifndef _ITF_TCPADAPTER_ITF_H_
#include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"
#endif //_ITF_TCPADAPTER_ITF_H_

#ifdef ITF_PS3
    #ifndef _ITF_MODULEMANAGER_PS3_H_
    #include "core/system/PS3/ModuleManager_ps3.h"
    #endif //_ITF_MODULEMANAGER_PS3_H_
#endif

#ifdef ITF_IPAD
#include <algorithm>
#endif

#ifdef ITF_PS3
#include <cell/sysmodule.h>
#include <sys/process.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#endif //ITF_PS3

#ifdef ITF_WII
#include "core/memory/memory_WII.h"

static void* SO_Alloc(u32 i_name, s32 i_size)
{
	ITF::MEM_M_PushExt(MEM_C_MEM2_ALLOC|MEM_C_HARD_ALLOC);
	void * p = ITF::Memory::alignedMallocCategory((u32)i_size, 32, ITF::MemoryId::mId_Misc);
    ITF::MEM_M_PopExt();
	return p;
}

static void SO_Free(u32 i_name, void* o_memory_p, s32 i_size)
{
	ITF::Memory::alignedFree( o_memory_p );
}

static const char *SO_ErrorCodeStr(int _errID)
{
    switch(_errID)
    {
        case SO_SUCCESS : return "SO_SUCCESS";
        case SO_EFATAL : return "SO_EFATAL";
        case SO_E2BIG : return "SO_E2BIG";
        case SO_EACCES : return "SO_EACCES";
        case SO_EADDRINUSE : return "SO_EADDRINUSE";
        case SO_EADDRNOTAVAIL : return "SO_EADDRNOTAVAIL";
        case SO_EAFNOSUPPORT : return "SO_EAFNOSUPPORT";
        case SO_EAGAIN : return "SO_EAGAIN";
        case SO_EALREADY : return "SO_EALREADY";
        case SO_EBADF : return "SO_EBADF";
        case SO_EBADMSG : return "SO_EBADMSG";
        case SO_EBUSY : return "SO_EBUSY";
        case SO_ECANCELED : return "SO_ECANCELED";
        case SO_ECHILD : return "SO_ECHILD";
        case SO_ECONNABORTED : return "SO_ECONNABORTED";
        case SO_ECONNREFUSED : return "SO_ECONNREFUSED";
        case SO_ECONNRESET : return "SO_ECONNRESET";
        case SO_EDEADLK : return "SO_EDEADLK";
        case SO_EDESTADDRREQ : return "SO_EDESTADDRREQ";
        case SO_EDOM : return "SO_EDOM";
        case SO_EDQUOT : return "SO_EDQUOT";
        case SO_EEXIST : return "SO_EEXIST";
        case SO_EFAULT : return "SO_EFAULT";
        case SO_EFBIG : return "SO_EFBIG";
        case SO_EHOSTUNREACH : return "SO_EHOSTUNREACH";
        case SO_EIDRM : return "SO_EIDRM";
        case SO_EILSEQ : return "SO_EILSEQ";
        case SO_EINPROGRESS : return "SO_EINPROGRESS";
        case SO_EINTR : return "SO_EINTR";
        case SO_EINVAL : return "SO_EINVAL";
        case SO_EIO : return "SO_EIO";
        case SO_EISCONN : return "SO_EISCONN";
        case SO_EISDIR : return "SO_EISDIR";
        case SO_ELOOP : return "SO_ELOOP";
        case SO_EMFILE : return "SO_EMFILE";
        case SO_EMLINK : return "SO_EMLINK";
        case SO_EMSGSIZE : return "SO_EMSGSIZE";
        case SO_EMULTIHOP : return "SO_EMULTIHOP";
        case SO_ENAMETOOLONG : return "SO_ENAMETOOLONG";
        case SO_ENETDOWN : return "SO_ENETDOWN";
        case SO_ENETRESET : return "SO_ENETRESET";
        case SO_ENETUNREACH : return "SO_ENETUNREACH";
        case SO_ENFILE : return "SO_ENFILE";
        case SO_ENOBUFS : return "SO_ENOBUFS";
        case SO_ENODATA : return "SO_ENODATA";
        case SO_ENODEV : return "SO_ENODEV";
        case SO_ENOENT : return "SO_ENOENT";
        case SO_ENOEXEC : return "SO_ENOEXEC";
        case SO_ENOLCK : return "SO_ENOLCK";
        case SO_ENOLINK : return "SO_ENOLINK";
        case SO_ENOMEM : return "SO_ENOMEM";
        case SO_ENOMSG : return "SO_ENOMSG";
        case SO_ENOPROTOOPT : return "SO_ENOPROTOOPT";
        case SO_ENOSPC : return "SO_ENOSPC";
        case SO_ENOSR : return "SO_ENOSR";
        case SO_ENOSTR : return "SO_ENOSTR";
        case SO_ENOSYS : return "SO_ENOSYS";
        case SO_ENOTCONN : return "SO_ENOTCONN";
        case SO_ENOTDIR : return "SO_ENOTDIR";
        case SO_ENOTEMPTY : return "SO_ENOTEMPTY";
        case SO_ENOTSOCK : return "SO_ENOTSOCK";
        case SO_ENOTSUP : return "SO_ENOTSUP";
        case SO_ENOTTY : return "SO_ENOTTY";
        case SO_ENXIO : return "SO_ENXIO";
        case SO_EOPNOTSUPP : return "SO_EOPNOTSUPP";
        case SO_EOVERFLOW : return "SO_EOVERFLOW";
        case SO_EPERM : return "SO_EPERM";
        case SO_EPIPE : return "SO_EPIPE";

        case SO_EPROTO : return "SO_EPROTO";
        case SO_EPROTONOSUPPORT : return "SO_EPROTONOSUPPORT";
        case SO_EPROTOTYPE : return "SO_EPROTOTYPE";
        case SO_EROFS : return "SO_EROFS";
        case SO_ESPIPE : return "SO_ESPIPE";
        case SO_ESRCH : return "SO_ESRCH";
        case SO_ESTALE : return "SO_ESTALE";
        case SO_ETIME : return "SO_ETIME";
        case SO_ETIMEDOUT : return "SO_ETIMEDOUT";
        case SO_ETXTBSY : return "SO_ETXTBSY";
        case SO_EXDEV : return "SO_EXDEV";
        case SO_ERR_DHCP_TIMEOUT : return "SO_ERR_DHCP_TIMEOUT";
        case SO_ERR_DHCP_EXPIRED : return "SO_ERR_DHCP_EXPIRED";
        case SO_ERR_DHCP_NAK : return "SO_ERR_DHCP_NAK";
        case SO_ERR_LCP : return "SO_ERR_LCP";
        case SO_ERR_AUTH : return "SO_ERR_AUTH";
        case SO_ERR_IPCP : return "SO_ERR_IPCP";
        case SO_ERR_ADDR_COLLISION : return "SO_ERR_ADDR_COLLISION";
        case SO_ERR_LINK_DOWN : return "SO_ERR_LINK_DOWN";
        case SO_ERR_LINK_UP_TIMEOUT : return "SO_ERR_LINK_UP_TIMEOUT";
    }

    return "unknown SO error code";
}

void CheckSOError(int v)
{
    const char *errcode = 0;
    if(v != SO_SUCCESS)
    {
        errcode = SO_ErrorCodeStr(v);
        LOG("Socket option error : %s", errcode);
    }        
}

void SetSockOpt(SOCKET _s, bool _accept, int _port )
{
    if(_accept)
        return;
    int lenInt = sizeof(int);
    int intVal;
    BOOL boolVal;

    bool bIsFileManager = _port == 2002;

    if(bIsFileManager)
    {
        CheckSOError( SOGetSockOpt(_s, SO_SOL_SOCKET, SO_SO_RCVBUF, &intVal, &lenInt) );
        intVal = 32*1024;
        CheckSOError( SOSetSockOpt(_s, SO_SOL_SOCKET, SO_SO_RCVBUF, &intVal, sizeof(int)) );
    }

    
    CheckSOError( SOGetSockOpt(_s, SO_SOL_SOCKET, SO_SO_SNDLOWAT, &intVal, &lenInt) );
    intVal = 1;
    CheckSOError( SOSetSockOpt(_s, SO_SOL_SOCKET, SO_SO_SNDLOWAT, &intVal, sizeof(int)) );
    
    CheckSOError( SOGetSockOpt(_s, SO_SOL_SOCKET, SO_SO_RCVLOWAT, &intVal, &lenInt) );
    intVal = 1;
    CheckSOError( SOSetSockOpt(_s, SO_SOL_SOCKET, SO_SO_RCVLOWAT, &intVal, sizeof(int)) );

    intVal = SOFcntl(_s, SO_F_GETFL, 0 );
    CheckSOError( SOFcntl(_s, SO_F_SETFL, intVal | SO_O_NONBLOCK ));

    boolVal = TRUE;
    CheckSOError( SOSetSockOpt(_s, SO_SOL_TCP, SO_TCP_NODELAY, &boolVal, sizeof(BOOL)) );
}
#endif // ITF_WII

namespace ITF
{

#ifdef ITF_SONY
    #ifndef TCP_NODELAY
        #define TCP_NODELAY SCE_NET_TCP_NODELAY
    #endif
#endif

    void TCPAdapter_ITF::createClient()
    {
        if (m_Name.getLen()!=0)
            return;
#ifndef ITF_WII
#if defined(ITF_CONSOLE)
       m_Name = "CONSOLEUser";//+"consoleName/IP"
#else
        struct in_addr in;
        in.s_addr = inet_addr("");

        struct hostent * hostEntry = gethostbyaddr ((char *) & in, sizeof (struct in_addr), AF_INET);

        if (hostEntry != NULL)
        {
           m_Name = hostEntry->h_name;
        }
        else
        {
           m_Name = "DefaultUser";
        }
#endif // defined(ITF_CONSOLE)
#else // ITF_WII
        m_Name = "DefaultUser";
#endif // ITF_WII
    }

    ITF_INLINE bbool socketIsValid(SOCKET _socket)
    {
#ifdef ITF_PS3
        return (_socket>=0);
#else
        return (_socket != INVALID_SOCKET );
#endif

    }


    static void changeBufferSize(SOCKET _socketClient,u32 _buffer)
    {
        if (socketIsValid(_socketClient))
        {
            setsockopt(_socketClient,SOL_SOCKET, SO_SNDBUF, (char* )&_buffer,sizeof(int) );
            setsockopt(_socketClient,SOL_SOCKET, SO_RCVBUF, (char* )&_buffer,sizeof(int) );       
        }
    }

//---------------------------------------------------------------------------------------------------------------------------------------

#ifdef ITF_WII

    SOCKET pollAccept(TCPPeer* _peer,SOCKADDR_IN& from)
    {
        SOCKET socketClient;

        SOCKETLEN fromlen=sizeof(from);
        from = *_peer->getAddrIn();
        from.addr.addr = SOHtoNl(SO_INADDR_ANY);
        SOPollFD pool = {_peer->getSocket(), SO_POLLIN, 0};
        int pollRes = SOPoll(&pool,1,0);
        if((pollRes >= 0) && (pool.revents & SO_POLLIN))
        {
            socketClient = SOAccept(_peer->getSocket(),&from);
            SetSockOpt(socketClient,true, from.port);
        }
        else
            socketClient = -1;
        return socketClient;
    }

#elif defined(ITF_PS3)

    SOCKET pollAccept(TCPPeer* _peer,SOCKADDR_IN& from)
    {
        SOCKET socketClient = -1;
        SOCKETLEN fromlen=sizeof(from);
        from = *_peer->getAddrIn();
        from.sin_addr.s_addr = htonl(INADDR_ANY);
        pollfd  fd =  {_peer->getSocket(), POLLIN, 0};
        int pollRes = socketpoll (&fd,1,0);
        if((pollRes >= 0) && (fd.revents & POLLIN))
        {
            socketClient = accept(_peer->getSocket(),(struct sockaddr*)&from,&fromlen);
            changeBufferSize(socketClient,65635);
        }
        return socketClient;
    }

#elif defined(ITF_MICROSOFT)

    SOCKET pollAccept(TCPPeer* _peer, SOCKADDR_IN& from)
    {
        ITF_ASSERT_CRASH(_peer, "Peer must be defined on pollAccept");

        SOCKET socketClient;
        SOCKETLEN fromlen = sizeof(from);
        from = *_peer->getAddrIn();
        from.sin_addr.s_addr = htonl(INADDR_ANY);
        socketClient = accept(_peer->getSocket(), (struct sockaddr*)&from, &fromlen);
        changeBufferSize(socketClient, 65535);

        return socketClient;
    }

#else
    SOCKET pollAccept(TCPPeer* _peer, SOCKADDR_IN& from)
    {
        ITF_ASSERT_CRASH(_peer, "Peer must be defined on pollAccept");

        SOCKET socketClient;
        SOCKETLEN fromlen = sizeof(from);
        from = *_peer->getAddrIn();
        from.sin_addr.s_addr = htonl(INADDR_ANY);
        socketClient = accept(_peer->getSocket(), (struct sockaddr*)&from, &fromlen);
        changeBufferSize(socketClient, 65535);

        return socketClient;
    }
#endif

//---------------------------------------------------------------------------------------------------------------------------------------

    //Check for incoming client
    void TCPAdapter_ITF::updateHostPrivate()
    {
        ITF_ASSERT(m_tcpHost);

        if (!m_bHost)
            return;

        SOCKADDR_IN from;

        bbool exit = false;
        SOCKET socketClient;

        while(!exit)//we are looping endlessly
        {
            socketClient = pollAccept(m_tcpHost,from);

            if (socketIsValid(socketClient))     
            {
                LOG(">>new client");

    #if defined(ITF_SONY) || defined(ITF_MICROSOFT)
                i32 optvalDelay = 1;
                if (setsockopt(socketClient, IPPROTO_TCP, TCP_NODELAY, (const char*)&optvalDelay, sizeof(optvalDelay))< 0)  
                {
                    LOG("setsockopt() TCP_NODELAY failed\n");
                }
    #endif // ITF_SONY || ITF_MICROSOFT

                TCPPeer* peer = registerClient(socketClient,&from);
                if (m_funcOnNewClient)
                    (m_funcOnNewClient)(peer);

            }
            else
            {
                exit = true;
            }
        }
    }

    bbool TCPAdapter_ITF::init()
    {
        if (m_bIsStartup)
            return btrue;

#if defined (ITF_X360)
        XNetStartupParams xnsp;
        ITF_MemSet(&xnsp, 0, sizeof(xnsp));
        xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
        xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
        XNetStartup(&xnsp);
#endif

#ifdef ITF_PS3

        i32 ret;

        ret = MODULE_MANAGER->loadModule(CELL_SYSMODULE_NET);
        if (ret < 0)
        {
            LOG("cellSysmoduleLoadModule() failed (%d)\n", ret);
            return bfalse;
        }

        ret = sys_net_initialize_network();
        if (ret < 0)
        {
            LOG("sys_net_initialize_network() failed (%d)\n", ret);
            return bfalse;
        }


#endif //ITF_PS3

#if defined (ITF_WINDOWS) || defined (ITF_X360)
        WSADATA wsaData;

        if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0)
            return bfalse;

        m_bIsStartup = btrue;
#endif //(ITF_WINDOWS) || defined (ITF_X360)

#ifdef ITF_WII
    SOLibraryConfig soLibConfig = {SO_Alloc, SO_Free};
    int res = SOInit(&soLibConfig);
	switch( res )
	{
		case SO_SUCCESS:
	        res = SOStartup();
	        if(res != SO_SUCCESS)
	        {
				ITF_ERROR("Wii network : SOStartup failure : %s", SO_ErrorCodeStr(res));
                SOCleanup();
				return bfalse;
			}
	    case SO_EALREADY:
	    {
			m_bIsStartup = btrue;
			#ifndef ITF_FINAL
            u32 addr = SOGetHostID();
            LOG("Current Wii addr %u.%u.%u.%u", (addr>>24)&0xff, (addr>>16)&0xff,(addr>>8)&0xff, addr&0xff);
			#endif // ITF_FINAL
			
			return btrue;
		}
		default:
			ITF_ERROR("Wii network : SOInit failure : %s", SO_ErrorCodeStr(res));
			return bfalse;
	}
#endif // ITF_WII

        return btrue;
    }

    bbool TCPAdapter_ITF::createHost(const char* _hostAddress,  u16 _port)
    {
        i32 lConnect = -1;
        SOCKET lhSocket;
#ifdef ITF_WII
        lhSocket = SOSocket(SO_PF_INET, SO_SOCK_STREAM,0);
        if(lhSocket < 0)
            return bfalse;
        SetSockOpt(lhSocket, false, _port);
#else
        lhSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(lhSocket == INVALID_SOCKET)
            return bfalse;
#endif


        SOCKADDR_IN         lSockAddr;
        ITF_MemSet(&lSockAddr,0, sizeof(lSockAddr));

#ifdef ITF_WII
        lSockAddr.family = SO_AF_INET;
        lSockAddr.port = SOHtoNs(_port);
        lSockAddr.addr.addr = SOHtoNl(SO_INADDR_ANY);
		lSockAddr.len = sizeof(SOSockAddrIn);
        lConnect = SOBind(lhSocket, &lSockAddr);
        int res = SOListen(lhSocket,10);
        if(res < 0)
        {
			ITF_ERROR("Wii network : SOListen failure : %s", SO_ErrorCodeStr(res));
            return bfalse;
        }


#else // ITF_WII
        lSockAddr.sin_family = AF_INET;
        lSockAddr.sin_port = htons(_port);
        lSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        lConnect = ::bind(lhSocket,(SOCKADDR *)&lSockAddr,sizeof(SOCKADDR_IN));

#if	!defined(ITF_PS3) && !defined(ITF_IPAD) && !defined(ITF_PS5)
        // If iMode = 0, blocking is enabled;
        // If iMode != 0, non-blocking mode is enabled.
        u_long iMode = 1;
        ioctlsocket(lhSocket, FIONBIO, &iMode);
#endif

#if defined (ITF_SONY) || defined (ITF_WINDOWS) || defined (ITF_X360)

        changeBufferSize(lhSocket,65535);

        i32 optvalDelay = 1;
        if (setsockopt(lhSocket, IPPROTO_TCP, TCP_NODELAY,(const char*) &optvalDelay, sizeof(optvalDelay))< 0)
        {
            LOG("setsockopt() TCP_NODELAY failed\n");
        }
#endif //(ITF_PS3)  || defined (ITF_WINDOWS) || defined (ITF_X360)

#ifdef ITF_PS3
        i32 optval = 1;

        optval = 1;
        if (setsockopt(lhSocket, SOL_SOCKET, SO_NBIO, &optval, sizeof(optval))< 0)
        {
            LOG("setsockopt() SO_NBIO failed (errno=%d)\n", sys_net_errno);
        }
#endif //ITF_PS3

        if(::listen(lhSocket,10)!=0)
        {
            return bfalse;
        }
#endif // ITF_WII

        if(lConnect != 0)
            return bfalse;

        m_bHost = true;

        m_tcpHost = newAlloc(mId_System, TCPPeer(this,lhSocket,&lSockAddr));
        m_tcpHost->setHost();

        return btrue;
    }


    bbool TCPAdapter_ITF::receivePacketPrivate(const NETPeer* peer, u8* buffer, u32& size_returned)
    {
        ITF_ASSERT(peer);
        TCPPeer* tcp_peer =(TCPPeer*)peer;
#ifdef ITF_WII
        i32 read = SORecv(tcp_peer->getSocket(), (char*)buffer, size_returned, SO_MSG_DONTWAIT);
#else

#ifdef ITF_PS3
        i32 read = recv(tcp_peer->getSocket(), (char*)buffer, size_returned, MSG_DONTWAIT);
#else
        i32 read = recv(tcp_peer->getSocket(), (char*)buffer, size_returned, 0);
#endif
#endif
        size_returned =(read>0) ? read : 0;
        return (size_returned >0);
    }


    void    TCPAdapter_ITF::close()
    {
        // If this adapter is a host
        if (m_bHost && m_tcpHost)
        {
            // Than manually clean the host peer, because it is not in the peer list
            ITF_ASSERT(find(m_vPeers.begin(),m_vPeers.end(),m_tcpHost) == m_vPeers.end());
            m_tcpHost->close();
            SF_DEL(m_tcpHost);
        }
#ifdef ITF_WII
        SOFinish();
#endif
    }


    bbool   TCPAdapter_ITF::sendPacketPrivate(const NETPeer* peer, const u8* buffer, u32 size) const
    {

        ITF_ASSERT(peer);

        const TCPPeer* tcp_peer =(const TCPPeer*)peer;
        //TODO move the send to asynchronous way,right now retry

        u32 sizeToSend = size;
        const u8* sendingBuffer = buffer;

        u32 seekBuffer = 0;

#ifdef ITF_WII
        while(sizeToSend)
        {
            i32 lLength = SOSend(tcp_peer->getSocket(), (const char*)sendingBuffer+seekBuffer,sizeToSend,SO_MSG_DONTWAIT);
            if(lLength > 0)
            {
               seekBuffer += lLength;
               sizeToSend -= lLength;
            }
            else if (lLength < 0)
            {
                switch(lLength)
                {
            	    case SO_EBUSY:
            	    case SO_EAGAIN:
            		    break;
            	    default:
					    ITF_ERROR("Wii network : SOSend failure : %s", SO_ErrorCodeStr(lLength));
					    return bfalse;
                }
            }
        }
        return btrue;
#else
        bbool bcontinue = btrue;
        while (bcontinue)
        {
            i32 lLength = send(tcp_peer->getSocket(), (const char*)sendingBuffer+seekBuffer,sizeToSend,0);

            if (lLength != -1 && lLength != i32(sizeToSend) )
            {
               seekBuffer += lLength;
               sizeToSend -= lLength;
            }
            else if (lLength != -1)
            {
                seekBuffer = 0;
            }

            if (seekBuffer==0)
            {
                if(lLength < (i32)sizeToSend)
                {
    #if	defined(ITF_MICROSOFT)
                    const int iErr = WSAGetLastError();
                    switch(iErr)
                    {

                    // Detecting a Dropped Connection
                    case WSAECONNRESET:     // Connection reset by peer
                    case WSAECONNABORTED:   // Software caused connection abort
                    case WSAESHUTDOWN:      // Cannot send after socket shutdown
                        {
                            return bfalse;
                        }
                        break;

                    default:
                        break;

                    }
    #else
        NOT_IMPLEMENTED_ON_PS3
    #endif //!ITF_PS3
                }
                else
                {
                    bcontinue = bfalse;
                }
            }
        }
        return btrue;
#endif // ITF_WII
    }

    bbool   TCPAdapter_ITF::connectToHost( const char* _hostAddress, u16 _hostPort)
    {
        SOCKET lhSocket;

        i32 lConnect = -1;
#ifdef ITF_WII
		lhSocket = SOSocket(SO_PF_INET, SO_SOCK_STREAM,0);
        if(lhSocket < 0)
             return bfalse;
        SetSockOpt(lhSocket, false, _hostPort);
#else
        lhSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(lhSocket == INVALID_SOCKET)
             return bfalse;
#endif

        SOCKADDR_IN         lSockAddr{};

#ifdef ITF_WII
        lSockAddr.family = SO_AF_INET;
        lSockAddr.port = SOHtoNs(_hostPort);
        SOInetAtoN(_hostAddress, &lSockAddr.addr);
		lSockAddr.len = sizeof(SOSockAddrIn);
        lConnect = SOConnect(lhSocket,&lSockAddr);
        if(lConnect !=0)
        {
			ITF_ERROR("Wii network : SOConnect failure : %s", SO_ErrorCodeStr(lConnect));
        }
#else // ITF_WII
        lSockAddr.sin_family = AF_INET;
        lSockAddr.sin_port = htons(_hostPort);
    #if defined(ITF_PS5)
        sceNetInetPton(AF_INET, _hostAddress, &lSockAddr.sin_addr);
    #else
        lSockAddr.sin_addr.s_addr = inet_addr(_hostAddress);
    #endif
        lConnect = ::connect(lhSocket,(SOCKADDR *)&lSockAddr,sizeof(SOCKADDR_IN));

#if !defined(ITF_PS3) && !defined(ITF_IPAD) && !defined(ITF_PS5)
        // If iMode = 0, blocking is enabled;
        // If iMode != 0, non-blocking mode is enabled.
        u_long iMode = 1;
        ioctlsocket(lhSocket, FIONBIO, &iMode);
#else
        NOT_IMPLEMENTED_ON_PS3
#endif
#endif // ITF_WII
        if(lConnect != 0)
            return bfalse;

        m_tcpHost =   registerClient(lhSocket,&lSockAddr);

        return btrue;
    }

}//namespace ITF

#endif // ITF_SUPPORT_NET

#endif //!ITF_CONSOLE_FINAL
