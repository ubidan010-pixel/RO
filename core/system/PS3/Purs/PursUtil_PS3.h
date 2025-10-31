#ifndef ITF_CORE_PURSUTIL_H_
#define ITF_CORE_PURSUTIL_H_

#include <stdio.h>
#include <libsn.h>

#include "core/error/ErrorHandler.h"
#include "core/memory/memory.h"

#define SYS_ERROR_CHECK(cmd)                    \
    do {                                        \
    int ret = cmd;                          \
    if (ret!=CELL_OK) { LOG("%s failed: %s(0x%08X)\n", #cmd, strerror(ret), ret); snPause(); }  \
    } while(0)

#define PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(align) \
inline void * operator new(size_t size) \
{ \
    return ITF::Memory::alignedMalloc(u32(size), align); \
} \
inline void* operator new[](size_t size) \
{ \
    return ITF::Memory::alignedMalloc(u32(size), align); \
} \
inline void operator delete(void* ptr) \
{ \
    ITF::Memory::alignedFree(ptr); \
} \
inline void operator delete[](void* ptr) \
{ \
    ITF::Memory::alignedFree(ptr); \
}

namespace ITF
{
namespace PursUtil
{
    int GetCurrentThreadPriority();

    inline bool IsHeapArea(void * ptr) // check memory region (heap or bss)
    {
        u32 address = u32(ptr);
        return (address>=0x10000000u && address<0xC0000000u);
    }
}
}
#endif // #ifndef _CORE_PURSUTIL_H_
