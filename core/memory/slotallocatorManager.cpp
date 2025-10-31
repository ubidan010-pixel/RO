#include "precompiled_core.h"

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
    #include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_MEMORYPROXY_H_
#include "core/memory/memoryProxy.h"
#endif //_ITF_MEMORYPROXY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_


//------------------------------------------------------------------------------------------------------------------------

namespace ITF
{
 
slotAllocatorManager* slotAllocatorManager::m_this = NULL;

//------------------------------------------------------------------------------------------------------------------------

slotAllocatorManager::slotAllocatorManager():m_PhysicalAllocFunction(NULL),m_PhysicalFreeFunction(NULL)
{
    m_this              = this;
    m_bEnablePhysical   = bfalse;
    m_uTotalMemoryUsage = 0;

    m_PhysicalAllocFunction = MemoryProxy::memoryPhysicalAllocFunction;
    m_PhysicalFreeFunction  = MemoryProxy::memoryPhysicalFreeFunction;

}

void slotAllocatorManager::init(const memSettings& _memSettings,u32 _index)
{
    slotAllocator* slotallocator = NULL;
    
    if (_memSettings.m_MemoryType == MemoryId::PHYSICAL_MEMORY)
        slotallocator = m_SlotAllocatorPhysical[_index];
    else
        slotallocator = m_SlotAllocatorHeap[_index];

    if (_memSettings.m_poolID)
        *_memSettings.m_poolID = _index;

    uPtr memoryUsage = slotallocator->create(_memSettings.m_MemoryType,_memSettings.m_slotSize,_memSettings.m_slotCount,_memSettings.m_alignment,_memSettings.m_protectFlags);
    m_uTotalMemoryUsage += memoryUsage;
    slotAllocator::createCriticalSection();

#ifdef USE_WARNINGS_STATS
    Synchronize::createCriticalSection(&m_csWarnings);
#endif //USE_WARNINGS_STATS
}

//------------------------------------------------------------------------------------------------------------------------

void slotAllocatorManager::destroy()
{
    if (m_this)
    {
        slotAllocator::destroyCriticalSection();
    #ifdef USE_WARNINGS_STATS
        Synchronize::leaveCriticalSection(&m_this->m_csWarnings);
    #endif //USE_WARNINGS_STATS

        SF_DEL(m_this);
    }
}

//------------------------------------------------------------------------------------------------------------------------
#ifdef USE_WARNINGS_STATS
void slotAllocatorManager::addWarnings(u32 _size,u32 _alignment,u32 _protectFlags)
{
     String tmp;
     tmp.setTextFormat("Missing slot for size: %d bytes with alignment:%d using flags:%d\n",_size,_alignment,_protectFlags);

     {
        csAutoLock cs(m_csWarnings);
        m_Warnings+=tmp;
     }
}
#endif //USE_WARNINGS_STATS

//------------------------------------------------------------------------------------------------------------------------

void* slotAllocatorManager::allocateInternal(u32 _memoryType,u32 _size,u32 _alignment,u32 _protectFlags)
{
    SafeArray<slotAllocator*>& slotType =  (_memoryType == MemoryId::PHYSICAL_MEMORY) ? m_SlotAllocatorPhysical : m_SlotAllocatorHeap;
    const u32 slotCount = slotType.size();


    for (u32 index = 0; index<slotCount;index++)
    {
        slotAllocator* slotAllocator = slotType[index];

        if (_size <= slotAllocator->getSizeSlot())
        {
            //allocator is found
            if (slotAllocator->matchAlignmentAttribute(_alignment) && slotAllocator->matchProtectFlags(_protectFlags))
                return slotAllocator->allocate();
        }

    }

#ifdef USE_WARNINGS_STATS
    addWarnings(_size,MemoryProxy::getITFAlignmentPhysicalFromAttribute(_alignment),MemoryProxy::getITFAttributeProtectFlagsFromX360(_protectFlags));
#endif //USE_WARNINGS_STATS

    return NULL;
}

//------------------------------------------------------------------------------------------------------------------------

void* slotAllocatorManager::allocateInternalITF(u32 _memoryType,u32 _size,u32 _alignmentITF,u32 _protectFlagsITF)
{

    SafeArray<slotAllocator*>& slotType =  (_memoryType == MemoryId::PHYSICAL_MEMORY) ? m_SlotAllocatorPhysical : m_SlotAllocatorHeap;
    const u32 slotCount = slotType.size();

    for (u32 index = 0; index<slotCount;index++)
    {
        slotAllocator* slotAllocator = slotType[index];

        if (_size <= slotAllocator->getSizeSlot())
        {
            //allocator is found
            if (slotAllocator->matchAlignmentITF(_alignmentITF) && slotAllocator->matchProtectFlagsITF(_protectFlagsITF))
                return slotAllocator->allocate();
        }

    }

#ifdef USE_WARNINGS_STATS
        addWarnings(_size,_alignmentITF,_protectFlagsITF);
#endif //USE_WARNINGS_STATS


    return NULL;
}

//------------------------------------------------------------------------------------------------------------------------

bbool slotAllocatorManager::deallocateInternal(u32 _memoryType,void* _pMemory)
{
    SafeArray<slotAllocator*>& slotType =  (_memoryType == MemoryId::PHYSICAL_MEMORY) ? m_SlotAllocatorPhysical : m_SlotAllocatorHeap;
    const u32 slotCount = slotType.size();

    for (u32 index = 0; index<slotCount;index++)
    {
        slotAllocator* slotAllocator = slotType[index];

        if (slotAllocator->isInRange(uPtr(_pMemory)))
        {
            //allocator is found
            slotAllocator->deallocate(_pMemory);
            return true;
        }

    }

    return bfalse;
}

//------------------------------------------------------------------------------------------------------------------------

void slotAllocatorManager::prepare(u32 _countAllocatorHeap,u32 _countAllocatorPhysical)
{
    {
        m_SlotAllocatorHeap.resize(_countAllocatorHeap);
        slotAllocator* pSlot = newAlloc(mId_Allocator, slotAllocator[_countAllocatorHeap]);

        for (u32 index = 0;index<_countAllocatorHeap;++index)
        {
            m_SlotAllocatorHeap[index] = pSlot+index;
        }
    }

    {
        m_SlotAllocatorPhysical.resize(_countAllocatorPhysical);
        slotAllocator* pSlot = newAlloc(mId_Allocator, slotAllocator[_countAllocatorPhysical]);

        for (u32 index = 0;index<_countAllocatorPhysical;++index)
        {
            m_SlotAllocatorPhysical[index] = pSlot+index;
        }
    }

}
  
//------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------
#ifndef ITF_FINAL
void slotAllocatorManager::dumpStats(Blob& _blob)
{
    _blob.pushUInt64(u64(m_this));//enable  ?
    if (m_this)
        m_this->dumpStatsInternal(_blob);
}

void slotAllocatorManager::dumpStatsInternal(Blob& _blob)
{
    const u32 slotCount = m_SlotAllocatorPhysical.size();

    _blob.pushUInt64(u64(m_uTotalMemoryUsage));
    _blob.pushUInt32(slotCount);
    for (u32 index = 0; index<slotCount;index++)
    {
        const slotAllocator* slotAllocator = m_SlotAllocatorPhysical[index];
        slotAllocator->dumpStats(_blob);
    }
#ifdef USE_WARNINGS_STATS
    _blob.pushString(m_Warnings);
    m_Warnings.clear();
#endif //USE_WARNINGS_STATS
        
}
#endif //ITF_FINAL
//------------------------------------------------------------------------------------------------------------------------


void* slotAllocatorManager::allocatePhysicalMemory(u32 _size, u32 _alignmentITF, u32 _protectFlagsITF)
{
    void* pMemory = NULL;

    pMemory = m_this->allocateInternalITF(MemoryId::PHYSICAL_MEMORY,_size,_alignmentITF,_protectFlagsITF);

    if (!pMemory)
    {
        ITF_ASSERT(m_this->m_PhysicalAllocFunction);
//        LOG("Allocation outside the slot %d %d %d",_size,_alignmentITF,_protectFlagsITF);
        pMemory = m_this->m_PhysicalAllocFunction(_size,_alignmentITF,_protectFlagsITF);
    }

    return pMemory;
}

//------------------------------------------------------------------------------------------------------------------------

void slotAllocatorManager::deallocatePhysicalMemory(void* _pMemory)
{
    if (!deallocate(MemoryId::PHYSICAL_MEMORY,_pMemory))
    {
        ITF_ASSERT(m_this->m_PhysicalFreeFunction);                
        m_this->m_PhysicalFreeFunction(_pMemory);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void*  slotAllocatorManager::allocateHeapMemory(u32 _size,u32 _poolID)
{
    if (!m_this)
        return ITF::Memory::malloc(_size);

    slotAllocator* slotAllocator = m_this->m_SlotAllocatorHeap[_poolID];

    ITF_ASSERT(slotAllocator);
    void* res = slotAllocator->allocate();
    
    ITF_ASSERT(res);
    return  res;
}
//------------------------------------------------------------------------------------------------------------------------

void  slotAllocatorManager::deallocateHeapMemory(void* _ptr,u32 _poolID)
{
    if (!m_this)
    {
        ITF::Memory::free(_ptr);
        return;
    }

    slotAllocator* slotAllocator = m_this->m_SlotAllocatorHeap[_poolID];
    slotAllocator->deallocate(_ptr);
}

//------------------------------------------------------------------------------------------------------------------------

void  slotAllocatorManager::deallocateHeapMemory(void* _ptr)
{
    m_this->deallocateInternal(MemoryId::HEAP_MEMORY,_ptr);
}

//------------------------------------------------------------------------------------------------------------------------
//assume from heap memory
u32    slotAllocatorManager::getSlotID(void* _ptr)
{
    SafeArray<slotAllocator*>& slotType =  m_this->m_SlotAllocatorHeap;
    const u32 slotCount = slotType.size();

    for (u32 index = 0; index<slotCount;index++)
    {
        slotAllocator* slotAllocator = slotType[index];

        if (slotAllocator->isInRange((uPtr) _ptr))
        {
         
            return index;
        }

    }

    return U32_INVALID;

}

//------------------------------------------------------------------------------------------------------------------------

bbool slotAllocatorManager::deallocate(u32 _memoryType,void* _pMemory)
{
    return m_this->deallocateInternal(_memoryType,_pMemory);
}

//------------------------------------------------------------------------------------------------------------------------

void* slotAllocatorManager::allocate(u32 _memoryType,u32 _size,u32 _alignment,u32 _protectFlags)
{
    return m_this->allocateInternal(_memoryType,_size,_alignment,_protectFlags);
}

//------------------------------------------------------------------------------------------------------------------------


}
