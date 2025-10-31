#ifndef _ITF_TCPADAPTER_ITF_H_
#define _ITF_TCPADAPTER_ITF_H_

#ifdef ITF_SUPPORT_NET

namespace ITF
{
    class TCPAdapter_ITF: public TCPAdapter
    {
    public:

        TCPAdapter_ITF() {};
        virtual     ~TCPAdapter_ITF ()   {close();}
        bbool       init            ();
        void        close           ();
        bbool       createHost      (const char* _hostAddress,  u16 _port);
        void        createClient    ();
  
    protected:
        bbool       connectToHost   ( const char* _hostAddress, u16 _hostPort   );
        void        updateHostPrivate();
        bbool       sendPacketPrivate(const NETPeer* peer,const u8* buffer, u32 size)const;
        bbool       receivePacketPrivate(const NETPeer* peer, u8*  buffer, u32& size_returned);
    };

} // namespace ITF

#endif // ITF_SUPPORT_NET


#endif //_ITF_TCPADAPTER_ITF_H_
