#include "precompiled_core.h"

#ifdef ENABLE_ALLOCATOR

#ifndef _ITF_ALLOCATOR_H_
#include "core/memory/allocator/allocator.h"
#endif //_ITF_ALLOCATOR_H_


#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_

#ifdef ITF_PS3
#include <sys/memory.h>
#endif

namespace ITF
{
//$4
// ***********************************************************************************************************************
// ***********************************************************************************************************************
//

#ifdef ITF_MEM_HEAP_DEBUG_
u32 MEM_uCheckState = 0x0;
#endif //ITF_MEM_HEAP_DEBUG_

#if defined (ITF_DEBUG_ALLOC_)
// Used to override the FILE/LINE by hand
const char	*ITF_MEM_s_FILE = NULL;
u32			ITF_MEM_u_LINE = 0;
#endif

#ifdef ITF_MEM_HEAP_DEBUG_

u32 ITF_MEM_GetTotalPhysicalMemory()
{
#if defined (ITF_PS3)
	sys_memory_info_t mmsize ;
	sys_memory_get_user_memory_size(&mmsize) ;
	return (mmsize.total_user_memory > 256*1024*1024 ? 512*1024*1024 : 256*1024*1024);
#elif defined (ITF_WINDOWS)
	MEMORYSTATUSEX mstats ;
	mstats.dwLength = sizeof(mstats);
	GlobalMemoryStatusEx(&mstats) ;
	return (mstats.ullTotalPhys > 0xFFFFFFFF ? 0xFFFFFFFF : (u32) mstats.ullTotalPhys);
#elif defined(ITF_X360)
	MEMORYSTATUS mstats ;
	GlobalMemoryStatus(&mstats) ;
	return (u32)mstats.dwTotalPhys;
#elif defined(ITF_WII)
    u32 uSize = 24*1024*1024;
    uSize += (ITF_MEM_GetTotalSize(1) > 64*1024*1024 ? 128*1024*1024 : 64*1024*1024);
	return uSize ;
#elif defined(ITF_CTR)
	size_t uSize = nn::os::GetAppMemorySize();
	return ( uSize > 64*1024*1024 ? 96*1024*1024 : 64*1024*1024 );
#endif
}

u32 ITF_MEM_GetTotalRetailPhysicalMemory()
{
#if defined (ITF_PS3)
	return 256*1024*1024;
#elif defined (ITF_WINDOWS)
	MEMORYSTATUSEX mstats ;
	mstats.dwLength = sizeof(mstats);
	GlobalMemoryStatusEx(&mstats) ;
	return (mstats.ullTotalPhys > 0xFFFFFFFF ? 0xFFFFFFFF : (u32) mstats.ullTotalPhys);
#elif defined(ITF_X360)
	return 512*1024*1024;
#elif defined(ITF_WII)
	return 88*1024*1024;
#elif defined(ITF_CTR)
	return 64*1024*1024;
#endif
}

// Get the number of bytes that have been committed by the system (exe, external libs, operating system, etc.).
u32 ITF_MEM_GetSystemPhysicalMemory()
{
	return ITF_MEM_GetTotalPhysicalMemory() - (ITF_MEM_GetAvailablePhysicalMemory() + ITF_MEM_GetTotalCommittedMemory());
}
#endif //ITF_MEM_HEAP_DEBUG_

// Get the number of bytes that can be used (that are in uncommitted pages).
u32 ITF_MEM_GetAvailablePhysicalMemory()
{
#if defined (ITF_PS3)
    sys_memory_info_t mmsize ;
    sys_memory_get_user_memory_size(&mmsize) ;
    return mmsize.available_user_memory;
#elif defined (ITF_WINDOWS)
    MEMORYSTATUSEX mstats ;
    mstats.dwLength = sizeof(mstats);
    GlobalMemoryStatusEx(&mstats) ;
    return (u32)mstats.ullAvailPhys ;
#elif defined(ITF_X360)
    MEMORYSTATUS mstats ;
    GlobalMemoryStatus(&mstats) ;
    return (u32)mstats.dwAvailPhys  ;
#elif defined(ITF_WII)
	return 0;
#elif defined(ITF_CTR)
	return 0;
#endif
}

/*
 =======================================================================================================================
 On PC, the global new and delete are used for MFC allocations. The system malloc and free can also be used.
 On console, the global new and delete, malloc, free and realloc should not be used so that we can track memory usage.
 But debug memory management and some external libraries (scry for example) use them, so we can't forbid that they are used.
 =======================================================================================================================
 */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(ITF_WII) || defined (ITF_CTR)
void *std::malloc(size_t size)
{	
    return ITF_MEM_Alloc( size);      
}

void std::free(void *_ptr)
{
    ITF_MEM_Free(_ptr);
}

void *std::realloc(void *_ptr, size_t _size)
{
    return ITF_MEM_Realloc(_ptr,_size);
}
#endif // ITF_WII

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifndef ITF_WINDOWS
void *operator new(size_t _ui_Size)
{
	if(ALLOCATOR_Available())
		return ITF_MEM_AllocAlign( _ui_Size, 16);      
	return malloc(_ui_Size);
}

void *operator new(size_t _ui_Size, size_t _align)
{
	size_t align = ITF::Max<size_t>(_align, 16);
    if(ALLOCATOR_Available())
        return ITF_MEM_AllocAlign( _ui_Size, align);      
    return malloc(_ui_Size);
}

#ifdef ITF_DEBUG_ALLOC_
// global new operator can be used with ITF_New
void *operator new(size_t _ui_Size, const char *_sFile, u32 _uLine, const char *_sTag, f32 )
{
	if(ALLOCATOR_Available())
        return ALLOCATOR()->p_Alloc(_ui_Size, _sFile, _uLine, _sTag,btrue,bfalse);
	return malloc(_ui_Size);
}

// global new operator can be used with ITF_NewAligned
void *operator new(size_t _ui_Size, size_t _align, const char *_sFile, u32 _uLine, const char *_sTag, f32 )
{
    size_t align = ITF::Max<size_t>(_align, 16);
    if(ALLOCATOR_Available())
        return ALLOCATOR()->p_AllocAlign(_ui_Size,_align, _sFile, _uLine, _sTag,btrue, btrue);        
    return malloc(_ui_Size);
}
#endif //ITF_DEBUG_ALLOC_


void operator delete(void *_p_Address) 
{ 
	if(ALLOCATOR_Available())
		ITF_MEM_Free( _p_Address);      
	else
		free(_p_Address);
}
#endif //ITF_WINDOWS

} //namespace ITF
#endif //ENABLE_ALLOCATOR
