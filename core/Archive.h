#ifndef _ITF_ARCHIVE_H_
#define _ITF_ARCHIVE_H_

#include "core/memory/memory.h"

#include "core/ByteEndian.h"

namespace ITF
{

#define ARCHIVE_ROUNDUP(x, a)   (((x)+((a)-1))&(~((a)-1)))

 class ArchiveLinker;

 class ArchiveMemory
 {

 public:

     ~ArchiveMemory();
   
     ArchiveMemory(const u8* _pBuffer,i32 _size):m_iSeekPos(0),m_iCapacity(0)
     {
         m_Linker = NULL;
         reserve(_size);
         ITF_Memcpy(m_pData,_pBuffer,_size);

         m_iSize = _size;
         m_isReading = btrue;
     }

     ArchiveMemory(bbool _isReading = bfalse):m_pData(0),m_iSeekPos(0),m_iSize(0),m_iCapacity(0)
     {
         m_Linker = NULL;
         m_isReading = _isReading;
     };

     ArchiveMemory(u32 _Reserve, u32 _Size, bbool _isReading):m_pData(0),m_iSeekPos(0),m_iSize(0),m_iCapacity(0)
     {
         m_Linker = NULL;
         m_isReading = _isReading;
         reserve(_Reserve);
         m_iSize = _Size;
     };

     void reserve(i32 _newCapacity)        
     {
         if (m_iCapacity == 0)
         {
             m_pData = newAlloc(mId_Archive,u8[_newCapacity]);
         }
         else
         {
             u8* pBuffer = newAlloc(mId_Archive,u8[_newCapacity]);
             ITF_Memcpy(pBuffer,m_pData,m_iSize);
             SF_DEL_ARRAY(m_pData);
             m_pData = pBuffer;
         }

         m_iCapacity = _newCapacity;
     }

     i32 getSize()     const {return m_iSize;}
     i32 getSeekPos()  const {return m_iSeekPos;}
     u8* getData()     const {return m_pData;}

     u8* getPtrForWrite() {return m_pData;}

     void clear()       {  SF_DEL_ARRAY(m_pData);m_iSeekPos = 0;m_iSize=0;m_iCapacity=0;}

     void   rewindForReading()                  {m_iSeekPos = 0; m_isReading = btrue;}
     void   getInfo(i32& _size, i32& _seekPos)  {_size = getSize(); _seekPos = getSeekPos();}


     void seek(u32 _pos) {m_iSeekPos = _pos;}//from 0

     ITF_INLINE void padOn32()
     {
         m_iSeekPos=ARCHIVE_ROUNDUP(m_iSeekPos,4);
     }


     void createLinker();

     void serializeLink(uPtr _ptr);

     void serializePtr(uPtr & _ptr);

     ArchiveLinker* getLinker() {return m_Linker;}

     ITF_INLINE void serializeByte(u8 val)
     {
         serializeInternal(val);
     }

     ITF_INLINE void serialize(u16& ptr)
     {
         serializeInternal<u16>(ptr);
     }

     ITF_INLINE void serialize(u32& ptr)
     {
#ifdef ITF_LITTLE_ENDIAN
         serializeInternal<u32>(ptr);
#else
         if (!m_isReading)
         {
            serializeInternal<u32>(ptr);
         }
         else
         {
            serializeInternalBigEngian4(ptr);
         }
         
#endif 
     }

     ITF_INLINE void serialize(u64& ptr)
     {
         serializeInternal<u64>(ptr);
     }

     ITF_INLINE void serialize(void*& ptr)
     {
         serializeInternal<void*>(ptr);
     }

     ITF_INLINE void serialize(short& ptr)
     {
         serializeInternal<short>(ptr);
     }

     ITF_INLINE void serialize(int& ptr)
     {

#ifdef ITF_LITTLE_ENDIAN
         serializeInternal<int>(ptr);
#else
         if (!m_isReading)
         {
             serializeInternal<int>(ptr);
         }
         else
         {
             serializeInternalBigEngian4((u32&)ptr);
         }

#endif 
     }

     ITF_INLINE void serialize(i8& ptr)
     {
         serializeInternal<i8>(ptr);
     }

     ITF_INLINE void serialize(char& ptr)
     {
         serializeInternal<char>(ptr);
     }

     ITF_INLINE void serialize(u8& ptr)
     {
         serializeInternal(ptr);
     }

     ITF_INLINE void serialize(f32& ptr)
     {
#ifdef ITF_LITTLE_ENDIAN
         serializeInternal<f32>(ptr);
#else
         if (!m_isReading)
         {
             serializeInternal<f32>(ptr);
         }
         else
         {
             serializeInternalBigEngian4((u32&)(ptr));
         }

#endif 
     }

     ITF_INLINE void serialize(f64& ptr)
     {
         serializeInternal<f64>(ptr);
     }

     ITF_INLINE void serialize(bool& ptr)
     {
         serializeInternal<bool>(ptr);
     }

    ITF_INLINE bbool isReading()  const {return m_isReading;}


    void serializeBlock8(u8* ptr,const u32 len)
    {
        serializeBuffer(ptr,len);
    }

    void serializeBlock16(u16* ptr,const u32 len)
    {
#ifndef ITF_LITTLE_ENDIAN
        serializeBuffer((u8*)ptr, (len<<1));
#else
        {
            u16* src = ptr;
            for (u32 i = 0;i<len;i++)
            {
                serializeInternal(*src);
                src++;
            }
        }

#endif
    }

     
 private:

     ITF_INLINE void serializeBuffer(u8* _ptr,u32 _size)
     {
         if (!m_isReading)
             serializeInternalWrite(_ptr,_size);
         else
             serializeInternalRead(_ptr,_size);
     }

     template <class T> ITF_INLINE void serializeInternal(T& ptr)
     {
#ifdef ITF_LITTLE_ENDIAN
         u8 endian[sizeof(T)];

         if (!m_isReading)
         {
             Endian::swapBigEndian(ptr,(u8*) endian);
             serializeInternalWrite((u8*) &endian,sizeof(T));
         }
         else
         {
             serializeInternalRead((u8*) &endian,sizeof(T));
             Endian::restoreLittleEndian(ptr,(u8*)&endian);
         }
#else
         if (!m_isReading)
             serializeInternalWrite((u8*) &ptr,sizeof(T));
         else
             serializeInternalRead((u8*) &ptr,sizeof(T));

#endif // ITF_LITTLE_ENDIAN
     }

     // u8 specification, no need of swapping
     ITF_INLINE void serializeInternal(u8& ptr)
     {
         if (!m_isReading)
             serializeInternalWrite(&ptr,sizeof(u8));
         else
             serializeInternalRead(&ptr,sizeof(u8));
     }


     ITF_INLINE void serializeInternalBigEngian4(u32& ptr)
     {
         //let the compilo optimize
         u8* addPtr = (m_pData+m_iSeekPos);
         u32* addPtr32 =(u32*) (u8*)(addPtr);
         ptr = (u32)(*addPtr32);
         m_iSeekPos+=sizeof(u32);      
     }
 



     void serializeInternalWrite(const u8* _pBuffer,const i32 _size)
     {
         if (_size+m_iSize>m_iCapacity)
         {
             reserve((_size+m_iSize)*2);
         }

         ITF_Memcpy(m_pData+m_iSeekPos,_pBuffer,_size);
         i32 seekpos =  m_iSeekPos+_size;

         if (seekpos>=m_iSize)//the size only grow if the seekpos is
         {
             m_iSize = seekpos;
         }

         m_iSeekPos=seekpos;

     }


     ITF_INLINE void serializeInternalRead(u8* _pBuffer,const i32 _size)
     {
         ITF_ASSERT(m_iSeekPos+_size <= m_iCapacity);
         ITF_ASSERT(_size >= 0);

         ITF_Memcpy(_pBuffer,m_pData+m_iSeekPos,_size);
         m_iSeekPos+=_size;
     }

     

     u8*     m_pData;

     bbool   m_isReading;
     ArchiveLinker* m_Linker;

     i32 m_iSeekPos;
     i32 m_iSize;
     i32 m_iCapacity;

 };

 template <class T>
 ITF_INLINE void serializeValue( T& _pVal,ArchiveMemory& _Archive)
 {
     _pVal.serialize(_Archive);
 }

 ITF_INLINE void serializeValue(int& _val,ArchiveMemory& _Archive )
 {
    _Archive.serialize(_val);
 }


 ITF_INLINE void serializeValue( f32& _val,ArchiveMemory& _Archive )
 {
     _Archive.serialize(_val);
 }

 ITF_INLINE void serializeValue( u32& _val,ArchiveMemory& _Archive )
 {
     _Archive.serialize(_val);
 }


 ITF_INLINE void serializeValue( u64& _val,ArchiveMemory& _Archive )
 {
     _Archive.serialize(_val);
 }

} //namespace ITF

#endif 
