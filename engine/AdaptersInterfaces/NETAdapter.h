#ifndef _ITF_NETADAPTER_H_
#define _ITF_NETADAPTER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_


#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

namespace ITF
{
    class NETPacket;
    class NETPeer;

    class NETAdapter
    {
    private:
        u32 m_uniqueID;         // peer unique ID given by the server when logging in
    public:
        virtual ~NETAdapter() {};

        // Generic function for a network adapter to send a packet
        // @param _packetBuffer the packet to send
        // @return btrue if succeed
        virtual bbool       sendPacket(NETPacket*  _packetBuffer)=0;
        virtual bbool       sendPacketToAllPeers(NETPacket*  _packetBuffer)=0;
        ITF_INLINE void     setUniqueID(u32 _id) {m_uniqueID = _id;}
        ITF_INLINE u32      getUniqueID() {return m_uniqueID;}
        virtual void        updateHost() = 0; 

        virtual bbool       canListen()  const   = 0;
        virtual NETPeer*    getHost()    const   = 0;
    
        virtual bbool       receivePacket(NETPacket**  packetbuf) = 0;
        virtual void        setPeerName(const NETPeer* _peer, const String& _name) = 0;
    };

    class NETPeer
    {   
    
    public:

        NETPeer():m_NetworkAdapter(NULL),m_IsHost(bfalse) {};
        virtual ~NETPeer() {};

        NETAdapter*     getNetworkAdpater() const       { return m_NetworkAdapter; }

        void            setHost()                       { m_IsHost = btrue; }
        void            setName(const String& _name)    { m_Name = _name; }
        const String&   getName()const                  { return m_Name; }
        
        virtual void    close() = 0;
        virtual void    restorePacket(u8* buffer,u32 size) = 0;

    protected:
        NETAdapter* m_NetworkAdapter;
        bbool m_IsHost;
        String m_Name;
    };



    class NETPacket
    {
    public:

        void compose(NETPeer* peer,String str)
        {
            m_Peer = peer;
            StringConverter chr(str);
            m_currentSize   = chr.getCharLength()+1;
            m_maxSize       = m_currentSize;  
            m_Data = new char[m_currentSize];
            ITF_Memcpy(m_Data,chr.getChar(),m_currentSize);

        }

        void append(char* buffer,size_t size)
        {
            if ( m_currentSize+size>m_maxSize )
            {
                ITF_ERROR("NETPacket::append Message too large");
            }
            else
            {
                ITF_Memcpy(m_Data+m_currentSize,buffer,size);
                m_currentSize += size;
            }
        }
        
        char* allocate(size_t size)
        {
            m_currentSize =size;
            m_maxSize = size;
            //TODO debug
            m_Data = newAlloc(mId_Plugin,char[size]);
            ITF_MemSet(m_Data,0xAA,size);
            return m_Data;
        }

        size_t getCurrentSize()const        {return m_currentSize;}
        void setCurrentSize(size_t size)
        {
            ITF_ASSERT(size<=m_maxSize);
            m_currentSize = size;
        }        

        bbool  isFilled()                   {return m_maxSize == m_currentSize;}

        size_t getMaxSize()const            {return m_maxSize;}

        void setPeer(const NETPeer* peer)     {m_Peer = peer;}
        const NETPeer* getPeer()const         {return m_Peer;}

        char* getData()const                {return m_Data;}

        NETPacket():m_maxSize(0),m_currentSize(0),m_Data(NULL),m_Peer(NULL) {};
        virtual ~NETPacket()    {reset();}
        void    reset()         {SF_DEL_ARRAY(m_Data);}

    protected:
        size_t  m_maxSize;
        size_t  m_currentSize;
        char*   m_Data;
        const NETPeer* m_Peer;
    };

}
#endif //_ITF_NETADAPTER_H_


