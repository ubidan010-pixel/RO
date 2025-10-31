#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#define _ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_SLOTALLOCATOR_H_
#include "core/memory/slotallocator.h"
#endif //_ITF_SLOTALLOCATOR_H_

namespace ITF
{



class slotAllocatorManager
{

public:

    typedef void*(*PhysicalAllocFunction)(size_t size,size_t align,u32 _protectFlagsITF);
    typedef void (*PhysicalFreeFunction)(void* _pMemory);

    slotAllocatorManager();

    void init(const memSettings& _memSettings,u32 _index);
    static void destroy();

    static void* allocate(u32 _memoryType,u32 _size,u32 _alignment,u32 _protectFlags);
    static bbool deallocate(u32 _memoryType,void* _pMemory);

    static void enablePhysicalAllocation()                  {if (m_this) {m_this->m_bEnablePhysical = btrue;}}
    static ITF_INLINE  bbool isEnablePhysicalAllocation()   {if (m_this) return m_this->m_bEnablePhysical; return bfalse;}

    static void* allocatePhysicalMemory(u32 _size, u32 _alignmentITF, u32 _protectFlagsITF);
    static void  deallocatePhysicalMemory(void* _pMemory);

    static void*  allocateHeapMemory(u32 _size,u32 _poolID);
    static void   deallocateHeapMemory(void* _ptr,u32 _poolID);
    
    static void   deallocateHeapMemory(void* _ptr);

    static u32    getSlotID(void* _ptr);

    static void dumpStats(Blob& _blob);

#ifdef USE_WARNINGS_STATS
    void addWarnings(u32 _size,u32 _alignment,u32 _protectFlags);
#endif //USE_WARNINGS_STATS

    void prepare(u32 _countAllocatorHeap,u32 _countAllocatorPhysical);
 
protected:

    void* allocateInternal(u32 _memoryType,u32 _size,u32 _alignment,u32 _protectFlags);
    bbool deallocateInternal(u32 _memoryType,void* _pMemory);
    void* allocateInternalITF(u32 _memoryType,u32 _size,u32 _alignmentITF,u32 _protectFlagsITF);

#ifndef ITF_FINAL
    void dumpStatsInternal(Blob& _blob);
#endif //ITF_FINAL
    
    ITF_INLINE bbool hasFreeSlot() {return btrue;}//must be atomic
    SafeArray<slotAllocator*> m_SlotAllocatorPhysical;
    SafeArray<slotAllocator*> m_SlotAllocatorHeap;

    static slotAllocatorManager* m_this;

    bbool   m_bEnablePhysical;
    uPtr    m_uTotalMemoryUsage;

    PhysicalAllocFunction       m_PhysicalAllocFunction;
    PhysicalFreeFunction        m_PhysicalFreeFunction;
    
#ifdef USE_WARNINGS_STATS
    ITF_THREAD_CRITICAL_SECTION     m_csWarnings;//CS to protect warnings collision
    String m_Warnings;
#endif //USE_WARNINGS_STATS

};

} // namespace ITF
			

#endif // _ITF_SLOTALLOCATORMANAGER_H_
