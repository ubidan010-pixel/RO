////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealmemdll.cpp  DLL wrapper implementation for the EAL (memory)
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealmem.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL Side
static eal_mem_dll_interface        gEalMemDllInterface = {};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemDllInitInterface(const eal_mem_dll_interface& Interface)
{
    gEalMemDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalMemLibInit(eal_u32 Version)
{
    return (*gEalMemDllInterface.pEalMemLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemLibShutdown()
{
    (*gEalMemDllInterface.pEalMemLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemAlloc(eal_u32 Size, eal_u32 Alignment, eal_mem_type MemType, eal_id Tag)
{
    return (*gEalMemDllInterface.pEalMemAlloc)(Size, Alignment, MemType, Tag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemAllocWithContext(eal_u32 Size, eal_u32 Alignment, eal_mem_type MemType, eal_id Tag, void *& Context)
{
    return (*gEalMemDllInterface.pEalMemAllocWithContext)(Size, Alignment, MemType, Tag, Context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemRealloc(void* pOldPtr, eal_u32 NewSize, eal_u32 Alignment)
{
    return (*gEalMemDllInterface.pEalMemRealloc)(pOldPtr, NewSize, Alignment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemReallocWithContext(void* pOldPtr, eal_u32 NewSize, eal_u32 Alignment, void *const & Context)
{
    return (*gEalMemDllInterface.pEalMemReallocWithContext)(pOldPtr, NewSize, Alignment, Context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemFree(void* pMemoryToFree)
{
    (*gEalMemDllInterface.pEalMemFree)(pMemoryToFree);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemFreeWithContext(void* pMemoryToFree, void *const & Context)
{
    (*gEalMemDllInterface.pEalMemFreeWithContext)(pMemoryToFree, Context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalMemGetPageSize()
{
    return (*gEalMemDllInterface.pEalMemGetPageSize)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalMemGetAllocatedSize(const void* pMemory)
{
    return (*gEalMemDllInterface.pEalMemGetAllocatedSize)(pMemory);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalMemGetAllocated()
{
    return (*gEalMemDllInterface.pEalMemGetAllocated)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemDebugAlloc( eal_u32         Size,
                        eal_u32         Alignment,
                        eal_mem_type    MemType,
                        eal_id          Tag,
                        eal_mem_optype  OpType,
                        const char*     pType,
                        const char*     pFilename,
                        eal_u32         FileLine,
                        const char*     pStringTag)
{
    return (*gEalMemDllInterface.pEalMemDebugAlloc)(Size, Alignment, MemType, Tag, OpType, pType,
                                                    pFilename, FileLine, pStringTag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemDebugAllocWithContext( eal_u32         Size,
                        eal_u32         Alignment,
                        eal_mem_type    MemType,
                        eal_id          Tag,
                        eal_mem_optype  OpType,
                        void *&         Context,
                        const char*     pType,
                        const char*     pFilename,
                        eal_u32         FileLine,
                        const char*     pStringTag)
{
    return (*gEalMemDllInterface.pEalMemDebugAllocWithContext)(Size, Alignment, MemType, Tag, OpType, Context,
                                                    pType, pFilename, FileLine, pStringTag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemDebugRealloc(   void*           pOldPtr,
                            eal_u32         NewSize,
                            eal_u32         Alignment,
                            const char*     pFilename,
                            eal_u32         FileLine)
{
    return (*gEalMemDllInterface.pEalMemDebugRealloc)(pOldPtr, NewSize, Alignment, pFilename, FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void* EalMemDebugReallocWithContext(   void*           pOldPtr,
                            eal_u32         NewSize,
                            eal_u32         Alignment,
                            void *const &   Context,
                            const char*     pFilename,
                            eal_u32         FileLine)
{
    return (*gEalMemDllInterface.pEalMemDebugReallocWithContext)(pOldPtr, NewSize, Alignment, Context,
                                                                pFilename, FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemDebugFree(void* pMemoryToFree, eal_mem_optype OpType, const char* pFilename, eal_u32 FileLine)
{
    (*gEalMemDllInterface.pEalMemDebugFree)(pMemoryToFree, OpType, pFilename, FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemDebugFreeWithContext(void* pMemoryToFree, eal_mem_optype OpType, void *const & Context, const char* pFilename, eal_u32 FileLine)
{
    (*gEalMemDllInterface.pEalMemDebugFreeWithContext)(pMemoryToFree, OpType, Context, pFilename, FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemDebugSetTagName(eal_id Tag, const char* pTagName)
{
    (*gEalMemDllInterface.pEalMemDebugSetTagName)(Tag, pTagName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalMemDebugDeclareMemory(  const void*     pMemory,
                                eal_u32         Size,
                                eal_u32         Alignment,
                                eal_mem_type    MemType,
                                eal_id          Tag,
                                eal_mem_optype  OpType,
                                const char*     pTypename,
                                const char*     pFilename,
                                eal_u32         FileLine)
{
    (*gEalMemDllInterface.pEalMemDebugDeclareMemory)(   pMemory,
                                                        Size,
                                                        Alignment,
                                                        MemType,
                                                        Tag,
                                                        OpType,
                                                        pTypename,
                                                        pFilename,
                                                        FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine Side

void EalMemDllPopulateInterface(eal_mem_dll_interface& Interface)
{
    Interface.pEalMemLibInit                    = EalMemLibInit;
    Interface.pEalMemLibShutdown                = EalMemLibShutdown;
    Interface.pEalMemAlloc                      = EalMemAlloc;
    Interface.pEalMemAllocWithContext           = EalMemAllocWithContext;
    Interface.pEalMemRealloc                    = EalMemRealloc;
    Interface.pEalMemReallocWithContext         = EalMemReallocWithContext;
    Interface.pEalMemFree                       = EalMemFree;
    Interface.pEalMemFreeWithContext            = EalMemFreeWithContext;
    Interface.pEalMemGetPageSize                = EalMemGetPageSize;
    Interface.pEalMemGetAllocatedSize           = EalMemGetAllocatedSize;
    Interface.pEalMemDebugAlloc                 = EalMemDebugAlloc;
    Interface.pEalMemDebugAllocWithContext      = EalMemDebugAllocWithContext;
    Interface.pEalMemDebugRealloc               = EalMemDebugRealloc;
    Interface.pEalMemDebugReallocWithContext    = EalMemDebugReallocWithContext;
    Interface.pEalMemDebugFree                  = EalMemDebugFree;
    Interface.pEalMemDebugFreeWithContext       = EalMemDebugFreeWithContext;
    Interface.pEalMemDebugDeclareMemory         = EalMemDebugDeclareMemory;

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable:4995 4996)
#elif defined(_GCC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    // BEGIN DEPRECATED in V400
    Interface.pEalMemGetAllocated               = EalMemGetAllocated;

    // BEGIN DEPRECATED in V500
    Interface.pEalMemDebugSetTagName            = EalMemDebugSetTagName;
    // END DEPRECATED
#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(_GCC)
#   pragma GCC diagnostic pop
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

