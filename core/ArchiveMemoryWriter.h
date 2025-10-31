#ifndef _ITF_ARCHIVEMEMORYWRITER_H_
#define _ITF_ARCHIVEMEMORYWRITER_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_


namespace ITF
{
    class ArchiveMemoryWriter
    {

    public:

        virtual ~ArchiveMemoryWriter()
        {
            SF_DEL_ARRAY(m_pData);
        }

            ArchiveMemoryWriter():m_pData(0),m_iSeekPos(0),m_iSize(0),m_iCapacity(0)
        {
            reserve(64);
        };

        void reserve(i32 _newCapacity)        
        {
            if (m_iCapacity == 0)
            {
                m_pData = newAlloc(mId_System,u8[_newCapacity]);
            }
            else
            {
                u8* pBuffer = newAlloc(mId_System,u8[_newCapacity]);
                ITF_Memcpy(pBuffer,m_pData,m_iSize);
                SF_DEL_ARRAY(m_pData);
                m_pData = pBuffer;
            }

            m_iCapacity = _newCapacity;
        }

        i32 getSize()     const {return m_iSize;}
        i32 getSeekPos()  const {return m_iSeekPos;}
        u8* getData()     const {return m_pData;}

        void clear()       {  SF_DEL_ARRAY(m_pData);m_iSeekPos = 0;m_iSize=0;m_iCapacity=0;}

        void   getInfo(i32& _size, i32& _seekPos) {_size = getSize(); _seekPos = getSeekPos();}


        ITF_INLINE void serialize(u8 ptr)
        {
            ITF_ASSERT_MSG(0,"serialize writer on u8 is not supported");
        }

        ITF_INLINE void serialize(u32 ptr)
        {
            serializeValue<u32>(ptr);
        }

        ITF_INLINE void serialize(u64 ptr)
        {
             serializeValue<u64>(ptr);
        }

        ITF_INLINE void serialize(f32 ptr)
        {
             serializeValue<f32>(ptr);
        }

        ITF_INLINE void serialize(bbool ptr)
        {
            serializeValue<bbool>(ptr);
        }

        ITF_INLINE void serializeBlock16(const u16* _ptr,u32 _len)
        {
            serializeInternalBuffer((u8*)_ptr,_len*sizeof(u16));
        }

        ITF_INLINE void serializeBlock8(const char* _ptr,u32 _len)
        {
            serializeInternalBuffer((u8*)_ptr,_len);
        }

        ITF_INLINE void serializeBlock8(const u8* _ptr,u32 _len)
        {
            serializeInternalBuffer(_ptr,_len);
        }
        
        ITF_INLINE void padOn32()
        {
             m_iSeekPos=ARCHIVE_ROUNDUP(m_iSeekPos,4);
        }

        void seek(u32 _pos) {m_iSeekPos = _pos;}//from 0

    private:

        void serializeInternalBuffer(const u8* _pBuffer,const i32 _size)
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

        template <class T>        void serializeValue( T _pVal)
        {

#ifdef ITF_LITTLE_ENDIAN
            u8 endian[sizeof(T)];
            Endian::swapBigEndian(_pVal,(u8*) endian);
#endif // ITF_LITTLE_ENDIAN

            const i32 _size = sizeof(T);

            if (_size+m_iSize>m_iCapacity)
            {
                reserve((_size+m_iSize)*2);
            }

            u8* p8 = m_pData;
            p8+=m_iSeekPos;


#ifdef ITF_LITTLE_ENDIAN
            ITF_Memcpy(p8,endian,_size);
#else
            T* ptr = (T*)(p8);
            *ptr = _pVal;
#endif // ITF_LITTLE_ENDIAN


            i32 seekpos =  m_iSeekPos+_size;

            if (seekpos>=m_iSize)//the size only grow if the seekpos is
            {
                m_iSize = seekpos;
            }

            m_iSeekPos=seekpos;
        }

        u8*     m_pData;

        i32 m_iSeekPos;
        i32 m_iSize;
        i32 m_iCapacity;
    };




} //namespace ITF

#endif //_ITF_ARCHIVEMEMORYWRITER_H_