#include "precompiled_core.h"

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_ARCHIVELINKER_H_
#include "core/ArchiveLinker.h"
#endif //_ITF_ARCHIVELINKER_H_

namespace ITF
{

/*
    void ArchiveMemory::serializeInternalBigDWORD(u32& ptr)
    {
        u8* addPtr = (m_pData+m_iSeekPos);
        u32* addPtr32 =(u32*) (addPtr);
        u32 val = (u32)(*addPtr32);
        
        ptr=val;
        //ITF_Memcpy(&ptr,m_pData+m_iSeekPos,sizeof(u32));
        //ITF_ASSERT(val==ptr);

        m_iSeekPos+=sizeof(u32);      
    }*/

    void ArchiveMemory::createLinker()
    {
        m_Linker = newAlloc(mId_Archive,ArchiveLinker(this));
    }

    ArchiveMemory::~ArchiveMemory()
    {
       SF_DEL(m_Linker);
       SF_DEL_ARRAY(m_pData);
    }

    void ArchiveMemory::serializeLink(uPtr _ptr)
    {
        ITF_ASSERT(m_Linker);
        m_Linker->serialize(_ptr);
    }

    void ArchiveMemory::serializePtr(uPtr & _ptr)
    {
        if (m_isReading)
        {
            uPtr toRead = 0;
            serialize(toRead);
            _ptr = m_Linker->getLink(toRead);
        }
        else
        {
            serialize(_ptr);
        }
      
    }


} //namespace ITF

