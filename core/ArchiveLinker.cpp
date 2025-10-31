#include "precompiled_core.h"

#ifndef _ITF_ARCHIVELINKER_H_
#include "core/ArchiveLinker.h"
#endif //_ITF_ARCHIVELINKER_H_

namespace ITF
{

ArchiveLinker::ArchiveLinker(ArchiveMemory* _pArchive)
{
    m_pArchive = _pArchive;
    m_bReading = _pArchive->isReading();
}

uPtr ArchiveLinker::getLink(uPtr _ptr)
{
    auto iter = m_References.find(_ptr);
    if (iter == m_References.end())
    {

#if  ITF_DEBUG_LEVEL > 0
        if (_ptr != 0)
        {
            ITF_ASSERT(0);
        }
#endif //ITF_DEBUG_LEVEL
        return 0;
    }

        
    return iter->second;
}

void ArchiveLinker::serialize(uPtr _ptr)
{
    if (m_bReading)
    {
        uPtr valPtr = 0;
        m_pArchive->serialize(valPtr);
        m_References[valPtr] = _ptr;
    }
    else
    {
        m_pArchive->serialize(_ptr);
    }
}

}