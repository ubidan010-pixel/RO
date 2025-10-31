#include "precompiled_core.h"

#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_macros_WII.h"

namespace ITF
{
	
//$4
// ***********************************************************************************************************************
// ***********************************************************************************************************************
//

#ifdef _HEAP_DEBUG_
u32 MEM_uCheckState = 0x0;
#endif //_HEAP_DEBUG_

#ifdef _HEAP_SIZE_DEBUG_
u32 MEM_GetTotalPhysicalMemory()
{
    u32 uSize = 24*1024*1024;
    uSize += (MEM_GetTotalSize(1) > 64*1024*1024 ? 128*1024*1024 : 64*1024*1024);
	return uSize ;
}

u32 MEM_GetTotalRetailPhysicalMemory()
{
    return (24+64)*1024*1024;
}

// Get the number of bytes that have been committed by the system (exe, external libs, operating system, etc.).
u32 MEM_GetSystemPhysicalMemory()
{
    return MEM_GetTotalPhysicalMemory() - MEM_GetITFTotalCommittedMemory();
}
#endif // _HEAP_SIZE_DEBUG_

} // namespace ITF

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *std::malloc(size_t size)
{	
    return ITF::MEM_M_Alloc(size);      
}

void *std::calloc(size_t num, size_t size)
{	
    return ITF::MEM_M_Alloc(num * size);      
}

void std::free(void *_ptr)
{
    ITF::MEM_M_Free(_ptr);
}

void *std::realloc(void *_ptr, size_t _size)
{
    return ITF::MEM_M_Realloc(_ptr,_size);
}
