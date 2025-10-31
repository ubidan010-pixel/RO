#include "precompiled_core.h"
#include "MemMngPrivate_PS3.h"
#include "MemMngBucketsPlatform_PS3.h"

namespace ITF
{

namespace MemMngBucketsPlatform
{

#if MEMMNG_BUCKET_BUILT_ON_MALLOC==0

    // address mapping info
    void * baseAddress = 0; // init by Init
    void * limitAddress = 0; // init by Init 

    void Init()
    {
	#ifdef VM_ENABLE_VIRTUAL_MEM

		if(CVirtualMemoryManager::GetInstance()->IsEnabled())
		{
            #if defined(VM_SMALL_PAGE_SIZE)
            baseAddress = limitAddress = ((char *)CVirtualMemoryManager::GetInstance()->GetVirtualMemoryEnd()) - BUCKET_MEMORY_ADRESSABLE;
            limitAddress = ((char *)CVirtualMemoryManager::GetInstance()->GetVirtualMemoryEnd()) - 1;
            #else
			baseAddress = CVirtualMemoryManager::GetInstance()->GetVirtualMemoryStart();
			limitAddress = ((char *)CVirtualMemoryManager::GetInstance()->GetVirtualMemoryEnd()) - 1;
            #endif

			return;
		}

	#endif //#ifdef VM_ENABLE_VIRTUAL_MEM

    #if defined(VM_SMALL_PAGE_SIZE)
        return;
    #endif

        // reserve the virtual space
        sys_addr_t pRet = 0;
        int retError = sys_mmapper_allocate_address(   BUCKET_MEMORY_ADRESSABLE,
            SYS_MEMORY_PAGE_SIZE_64K 
            | SYS_MEMORY_ACCESS_RIGHT_PPU_THR
            | SYS_MEMORY_ACCESS_RIGHT_SPU_THR
            | SYS_MEMORY_ACCESS_RIGHT_RAW_SPU,
            256*1024*1024,
            &pRet);
        MEMMNG_ASSERT(pRet); // Out of virtual space !!
        MEMMNG_ASSERT(retError==CELL_OK); // Problem reported by OS
        baseAddress = (void *)pRet;
        limitAddress = ((char *)pRet+BUCKET_MEMORY_ADRESSABLE)-1;

    }

    void Close()
    {
		#ifdef VM_ENABLE_VIRTUAL_MEM

		if(CVirtualMemoryManager::GetInstance()->IsEnabled())
		{
			return;
		}

		#endif //#ifdef VM_ENABLE_VIRTUAL_MEM

        int retError = sys_mmapper_free_address((sys_addr_t)baseAddress);
        MEMMNG_ASSERT(retError==CELL_OK); // Problem reported by OS
    }

#else // MEMMNG_BUCKET_BUILT_ON_MALLOC!=0

    // Address mapping info (set by Init)
    void * baseAddress = 0;
    void * limitAddress = 0;

    void Init()
    {
        MemMngStd::GetRange(baseAddress, limitAddress);
    }

    void Close()
    {

    }

#endif

} // namespace MemMngBucketsPlatform

} // namespace ITF
