#ifndef _ITF_ARCHIVEMEMORY_H_
#define _ITF_ARCHIVEMEMORY_H_

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
{/*
    class ArchiveMemory
    {

    public:

        virtual ~ArchiveMemory()
        {
            SF_DEL_ARRAY(m_pData);
        }

        ArchiveMemory(const u8* _pBuffer,i32 _size):m_iSeekPos(0),m_iCapacity(0)
        {
            reserve(_size);
            ITF_Memcpy(m_pData,_pBuffer,_size);

            m_iSize = _size;
            m_isReading = btrue;
        }

        ArchiveMemory(bbool _isReading = bfalse):m_pData(0),m_iSeekPos(0),m_iSize(0),m_iCapacity(0)
        {
            m_isReading = _isReading;
        };

        ArchiveMemory(u32 _Reserve, u32 _Size, bbool _isReading):m_pData(0),m_iSeekPos(0),m_iSize(0),m_iCapacity(0)
        {
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

        virtual void   rewindForReading() {m_iSeekPos = 0; m_isReading = btrue;}
        virtual void   getInfo(i32& _size, i32& _seekPos) {_size = getSize(); _seekPos = getSeekPos();}


        void seek(u32 _pos) {m_iSeekPos = _pos;}//from 0

        ITF_INLINE void padOn32()
        {
            m_iSeekPos=ARCHIVE_ROUNDUP(m_iSeekPos,4);
        }

    private:

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

         
#ifdef _DEBUG            
            //debug test
            if (m_breakOnSize!=0 && m_iSize >= m_breakOnSize)
            {
                m_breakOnSize = 0;
            }
#endif //_DEBUG
            
        }

        void serializeInternalRead(u8* _pBuffer,const i32 _size)
        {
            ITF_ASSERT(m_iSeekPos+_size <= m_iCapacity);
            ITF_ASSERT(_size >= 0);
            ITF_Memcpy(_pBuffer,m_pData+m_iSeekPos,_size);
            m_iSeekPos+=_size;
        }

        u8*     m_pData;
       

        i32 m_iSeekPos;
        i32 m_iSize;
        i32 m_iCapacity;

    };
*/



} //namespace ITF

#endif 
