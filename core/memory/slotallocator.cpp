#include "precompiled_core.h"

#ifndef _ITF_SLOTALLOCATOR_H_
    #include "core/memory/slotallocator.h"
#endif //_ITF_SLOTALLOCATOR_H_

#ifndef _ITF_MEMORYPROXY_H_
#include "core/memory/memoryProxy.h"
#endif //_ITF_MEMORYPROXY_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#include "core/math/BitTweak.h"

#if !defined(ITF_MICROSOFT)
#define PAGE_READWRITE         0x04   
#endif

namespace ITF
{   
//------------------------------------------------------------------------------------------------------------------------

    ITF_INLINE u32 mem_align_size(u32 _size,u32 _alignment)
    {
        u32 alignMask = _alignment - 1;
        return (_size + alignMask) & ~alignMask;
    }

//------------------------------------------------------------------------------------------------------------------------

    ITF_INLINE bbool mem_IsPtrAligned(void *_ptr,u32 _alignment)
    {
        return BitTweak::IsAligned(_ptr, _alignment);
    }

//------------------------------------------------------------------------------------------------------------------------
    
    ITF_THREAD_CRITICAL_SECTION     slotAllocator::m_csSlot;

    void slotAllocator::createCriticalSection()
    {
       Synchronize::createCriticalSection(&m_csSlot);
    }
   
//------------------------------------------------------------------------------------------------------------------------

    void slotAllocator::destroyCriticalSection()
    {
       Synchronize::destroyCriticalSection(&m_csSlot);
    }

//------------------------------------------------------------------------------------------------------------------------

    uPtr slotAllocator::create(u32 _memoryType,u32 _sizeSlotByte,u32 _slotCount,u32 _alignmentITF,u32 _protectionFlagsITF)
    {
        //assume it's physical memory for the moment
        m_globalMemorySize  = mem_align_size(_sizeSlotByte,_alignmentITF)*_slotCount; //align this memory
        m_sizeSlotByte      = _sizeSlotByte;
        m_AlignmentITF      = _alignmentITF;
        m_slotCount         = _slotCount;

        m_freeIndexes.reserve(_slotCount);
        for (u32 index=0;index<_slotCount;index++)
        {
            m_freeIndexes.push_back(index);
        }

        m_protectFlagsPlatform = MemoryProxy::getAttributeProtectFlagsFromITF(m_globalMemorySize,_protectionFlagsITF);
        m_protectFlagsITF = _protectionFlagsITF;
        m_AlignmentAtributePlatform = MemoryProxy::getAlignmentPhysicalFromITF(_alignmentITF);

        if (_memoryType == MemoryId::PHYSICAL_MEMORY)
            m_pMemoryStartRange =(u8*) MemoryProxy::memoryPhysicalAllocFunction(m_globalMemorySize,_alignmentITF,_protectionFlagsITF | PAGE_READWRITE);
        else
            m_pMemoryStartRange =(u8*) MemoryProxy::memoryHeapAllocFunction(m_globalMemorySize,_alignmentITF, _protectionFlagsITF | PAGE_READWRITE);

        ITF_ASSERT(m_pMemoryStartRange);
        ITF_ASSERT(mem_IsPtrAligned(m_pMemoryStartRange,_alignmentITF));
        m_MemoryEndRange = uPtr(m_pMemoryStartRange+m_globalMemorySize);//not true because of alignment
        
        
        return m_globalMemorySize;
    }     

//------------------------------------------------------------------------------------------------------------------------

    bbool  slotAllocator::deallocate(void* _pMemory)
    {
        if ( uPtr(_pMemory)>=uPtr(m_pMemoryStartRange) && uPtr(_pMemory)<m_MemoryEndRange)
        {
            //compute the index
            u32 index = u32(uPtr(_pMemory)-uPtr(m_pMemoryStartRange))/m_sizeSlotByte;
            
            {
                csAutoLock cs(m_csSlot);
                m_freeIndexes.push_back(index);
            }

            return btrue;
        }
      
        ITF_ASSERT(0);
        return bfalse;
    }


 //------------------------------------------------------------------------------------------------------------------------

    void* slotAllocator::allocate()
    {
        csAutoLock cs(m_csSlot);

        const u32 size =  m_freeIndexes.size();
        if (size == 0)
            return NULL;

        u32 index = m_freeIndexes[size-1];
        m_freeIndexes.eraseNoOrder(size-1);

        //compute a memory address using the index
        u32 alignedSize = mem_align_size(m_sizeSlotByte,m_AlignmentITF);
        u32 pad = alignedSize*index;
        void* pMemory =  pad+m_pMemoryStartRange;
        ITF_ASSERT((uPtr(pMemory)+alignedSize)<=m_MemoryEndRange);
        ITF_ASSERT(mem_IsPtrAligned(pMemory,m_AlignmentITF));

        return pMemory;
    }

//------------------------------------------------------------------------------------------------------------------------

#ifndef ITF_FINAL

    void slotAllocator::dumpStats(Blob& _blob) const
    {
        _blob.pushUInt32(m_sizeSlotByte);
        _blob.pushUInt32(m_freeIndexes.size());
        _blob.pushUInt32(m_slotCount);
        _blob.pushUInt32(m_AlignmentITF);
        _blob.pushUInt32(m_protectFlagsITF);
        _blob.pushUInt64(m_globalMemorySize);
    }
#endif //ITF_FINAL

//------------------------------------------------------------------------------------------------------------------------

}
