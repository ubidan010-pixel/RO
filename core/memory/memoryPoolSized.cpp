#include "precompiled_core.h"

#ifndef _ITF_MEMORYPOOLSIZED_H_
#include "core/memory/memoryPoolSized.h"
#endif //_ITF_MEMORYPOOLSIZED_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

//------------------------------------------------------------------------------------------------------------------------
#if !defined(ITF_IPAD) && !defined(ITF_CTR) && !defined(ITF_PS3) && !defined(ITF_WII)

namespace ITF
{

    void memoryPoolSized::deallocate(void* ptr)
    {
        ITF::slotAllocatorManager::deallocateHeapMemory(ptr,getPoolID());
    }

}


void* operator new(size_t _size,ITF::memoryPoolSized&  pool)
{
    return ITF::slotAllocatorManager::allocateHeapMemory(ITF::u32(_size), pool.getPoolID());
}

#endif//ITF_IPAD
