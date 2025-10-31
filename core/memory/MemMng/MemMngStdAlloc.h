#ifndef ITF_CORE_MEMMNGSTDALLOC_H_
#define ITF_CORE_MEMMNGSTDALLOC_H_

#pragma once

namespace ITF
{
    namespace MemMngStd 
    {
        bool            Init();
        void            GetRange(void * & baseAddress, void * & limitAddress);
        uPtr            GetCommittedAmount(); // The amount of memory getting physical memory assigned
        uPtr            GetCommittedPeak(); // The max amount of memory getting physical memory assigned
        void*           Alloc(u32 iSize, u32 uAlign);
        void            Free(void * pData);
        ux	            MSize(void * pData);
    }
}

#endif // ITF_CORE_MEMMNGSTDALLOC_H_
