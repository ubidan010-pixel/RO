////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealmem.h  Engine Abstraction Layer Memory Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ealdef.h>

/// This module covers the memory reference API.
/*! \addtogroup Mem
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210).
/// See ealdef.h for more information.
#define EAL_MEM_VERSION             504

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Operation Types

    Used in the debug functions, see \ref EalMemDebugAlloc and \ref EalMemDebugFree
    They are used to:
    - Provide richer information to logging tools; and
    - To verify, by debugging tools, that matching operators are used in the client code.
*/
enum eal_mem_optype
{
    EAL_MEM_ALLOC       = 0x00000001,       ///< malloc used in calling code
    EAL_MEM_NEW         = 0x00000002,       ///< new operator used in calling code
    EAL_MEM_NEWARRAY    = 0x00000003,       ///< new[] operator used in calling code
    EAL_MEM_FREE        = 0x00000004,       ///< free used in calling code
    EAL_MEM_DELETE      = 0x00000005,       ///< delete operator used in calling code
    EAL_MEM_DELETEARRAY = 0x00000006        ///< delete[] operator used in calling code
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Memory Types - Type of memory which indicate its intended usage.
typedef eal_u32     eal_mem_type;


/*!  Memory type flags

    Specify how an allocation should be handled.
*/
enum eal_mem_type_flag
{
    EAL_MEM_TYPE_NONE       = 0x00000000,   ///< No type or usage specified
    EAL_MEM_TYPE_PHYSICAL   = 0x00000001,   ///< Physical memory allocation
    EAL_MEM_TYPE_STATIC     = 0x00000004,   ///< Memory used for the whole game duration
    EAL_MEM_TYPE_DEV        = 0x00000020,   ///< Memory used for development that will disappear in final game builds

    // These flags are deprecated, do not use them. They will be removed in version 7.00.
    EAL_MEM_TYPE_MEM2       EAL_DEPRECATED("Deprecated; there is no replacement")
                            = 0x00000002,   ///< \deprecated DEPRECATED since version 4.00 Using secondary memory pool (for older consoles with two banks of memory, same effect could be achieved through an EAL ID if we need)
    EAL_MEM_TYPE_FRAME      EAL_DEPRECATED("Deprecated; there is no replacement")
                            = 0x00000008,   ///< \deprecated DEPRECATED since version 4.00 Memory will be freed before the next frame (unclear behavior, the concept of frame is engine-specific)
    EAL_MEM_TYPE_GROW       EAL_DEPRECATED("Deprecated; there is no replacement")
                            = 0x00000010    ///< \deprecated DEPRECATED since version 4.00 Hint to optimize realloc on this allocation (makes implementation unnecessarily complex)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \page alloctags Allocation Tags
/// Allocation tags are used to identify the intended usage of an allocated memory block and are a
/// very important tool for both debugging and management of memory.
/// The tags can be used for routing allocations in different allocators on the supplier side.
/// See \ref uuid for more details on this topic.
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Initialize the library.
    \return true if initialization succeeded

    \param Version      Must be equal to current version = \ref EAL_MEM_VERSION

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalMemLibInit() so
      EalMemLibShutdown() is called the exact same number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all API calls will be allowed.
    - EalMemLibShutdown() must be called even if false is returned.

    \sa EalMemLibShutdown
*/
EAL_DLL_ENTRY bool EalMemLibInit(eal_u32 Version = EAL_MEM_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalMemLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalMemLibShutdown().

    \sa EalMemLibInit
*/
EAL_DLL_ENTRY void EalMemLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Allocate a memory block.
    \param Size       The size in bytes of the block to allocate
    \param Alignment  The required alignment
    \param MemType    The type of memory to allocate
    \param Tag        An identifier of the intended usage for the block. See \ref alloctags.

    \PRECONDITIONS
    - Size must be greater than 0.
    - Alignment must be greater than 0 and in base 2 (1,2,4,8,16,32,64, and so on).

    \POSTCONDITIONS
    - Returns a pointer on the block of memory allocated, with an address multiple of Alignment
    - Returns NULL if the memory allocation failed
    - Memory is expected to be freed calling EalMemFree().

    \sa EalMemFree EalMemRealloc EalMemDebugAlloc
*/
EAL_DLL_ENTRY void* EalMemAlloc(eal_u32 Size, eal_u32 Alignment, eal_mem_type MemType, eal_id Tag);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Allocate a memory block. The Context argument is assigned by the implementer to keep track of
    the required information so that \ref EalMemReallocWithContext and \ref EalMemFreeWithContext will
    avoid the need of a lookup to proceed. Typically, it can be implemented as being a pointer
    to an allocator. The Context value must always be the same for a given eal_id.
    \param Size       The size in bytes of the block to allocate.
    \param Alignment  The required alignment.
    \param MemType    The type of memory to allocate.
    \param Tag        An identifier of the intended usage for the block. See \ref alloctags.
    \param [in,out] Context
                      An opaque ref to a context information pointer meaningful to the implementer.

    \PRECONDITIONS
    - Size must be greater than 0.
    - Alignment must be greater than 0 and in base 2 (1,2,4,8,16,32,64, and so on).
    - A non-null Context may be passed only if it is the same as received from a previous call
      to this function for a given eal_id.

    \POSTCONDITIONS
    - Returns a pointer on the block of memory allocated, with an address multiple of Alignment.
    - Returns NULL if the memory allocation failed.
    - Memory is expected to be freed calling \ref EalMemFreeWithContext.
    - The same Context is expected to be passed to \ref EalMemReallocWithContext and
      \ref EalMemFreeWithContext.

    \sa EalMemFreeWithContext, EalMemReallocWithContext, EalMemDebugAllocWithContext
*/
EAL_DLL_ENTRY void* EalMemAllocWithContext(eal_u32 Size, eal_u32 Alignment, eal_mem_type MemType, eal_id Tag, void*& Context);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Readjust the size of an already allocated memory block.
    \return A pointer to the new block of memory, with an address multiple of Alignment
    \return NULL if the memory allocation failed

    \param pOldPtr    A pointer to the previously allocated memory block.
    \param NewSize    The new byte-size of the memory block.
    \param Alignment  The required alignment.

    \PRECONDITIONS
    \param pOldPtr must be valid. If not, call Alloc instead.
    \param NewSize must absolutely be greater than 0. If Size is 0, you should call Free() instead.
    \param Alignment must be greater than 0 and in base 2 (1,2,4,8,16,32,64, and so on).

    \POSTCONDITIONS
    - Returns a pointer on the block of memory allocated, with an address multiple of Alignment
    - Returns NULL if the memory allocation failed
    - Memory is expected to be freed later by calling EalMemFree().
    - The Tag of pOldPtr will be preserved for the new memory block.

    \sa EalMemFree EalMemAlloc EalMemDebugRealloc
*/
EAL_DLL_ENTRY void* EalMemRealloc(void* pOldPtr, eal_u32 NewSize, eal_u32 Alignment);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Readjust the size of an already allocated memory block.
    \return A pointer to the new block of memory, with an address multiple of Alignment
    \return NULL if the memory allocation failed

    \param pOldPtr    A pointer to the previously allocated memory block.
    \param NewSize    The new byte-size of the memory block.
    \param Alignment  The required alignment.
    \param [in] Context
                      An opaque ref to the same context information pointer received from
                      \ref EalMemAllocWithContext.

    \PRECONDITIONS
    \param pOldPtr must be valid. If not, call Alloc instead.
    \param NewSize must absolutely be greater than 0. If Size is 0, you should call Free() instead.
    \param Alignment must be greater than 0 and in base 2 (1,2,4,8,16,32,64, and so on).

    \POSTCONDITIONS
    - Returns a pointer on the block of memory allocated, with an address multiple of Alignment
    - Returns NULL if the memory allocation failed
    - Memory is expected to be freed later by calling \ref EalMemFreeWithContext.
    - The Tag of pOldPtr will be preserved for the new memory block.

    \sa EalMemFreeWithContext, EalMemAllocWithContext, EalMemDebugReallocWithContext
*/
EAL_DLL_ENTRY void* EalMemReallocWithContext(void* pOldPtr, eal_u32 NewSize, eal_u32 Alignment, void *const & Context);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Free memory previously allocated.
    \param pMemoryToFree  A pointer to the memory block to free.

    \PRECONDITIONS
    - pMemoryToFree must not be NULL.

    \POSTCONDITIONS
    - The free memory is returned to the system.
    - After the call to EalMemFree, the freed memory must no longer be used.

    \sa EalMemAlloc, EalMemAlloc, EalMemDebugFree
*/
EAL_DLL_ENTRY void EalMemFree(void* pMemoryToFree);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Free memory previously allocated.
    \param pMemoryToFree  A pointer to the memory block to free.
    \param [in] Context   An opaque ref to the same context information pointer received from
                          \ref EalMemAllocWithContext.

    \PRECONDITIONS
    - pMemoryToFree must not be NULL.

    \POSTCONDITIONS
    - The free memory is returned to the system.
    - After the call to EalMemFreeWithContext, the freed memory must no longer be used.

    \sa EalMemAllocWithContext, EalMemReallocWithContext, EalMemDebugFreeWithContext
*/
EAL_DLL_ENTRY void EalMemFreeWithContext(void* pMemoryToFree, void *const & Context);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Return the system page size.
    \return The size of a system page in bytes.
*/
EAL_DLL_ENTRY eal_u32 EalMemGetPageSize();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Return the total amount of memory allocated for a specific block of memory.
    \return The size in bytes of the memory allocated. This may includes system overhead.

    \param pMemory  A valid memory block previously allocated

    \PRECONDITIONS
    - pMemory must not be NULL & be a valid memory block

    \POSTCONDITIONS
    - The amount of memory returned, is the actual size consumed by the system for that memory
      block, and not the actual size passed in EalMemAlloc.

    \sa EalMemAlloc, EalMemFree
*/
EAL_DLL_ENTRY eal_u32 EalMemGetAllocatedSize(const void* pMemory);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Returns the total amount of memory allocated by the client.
    \deprecated DEPRECATED since version 4.00. There is no replacement.
    Client should avoid using this version ASAP. This function will be removed in version 7.00.
    Implementer must still support this function until version 7.00.

    \return The size in bytes of the sum of memory allocated by all clients.

    \POSTCONDITIONS
    - Due to the 32-bit return type, if the amount of memory exceeds 0xFFFFFFFE, the returned value is 0xFFFFFFFF. Use \ref EalMemDebugGetAllocatedById instead.
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY eal_u32 EalMemGetAllocated();


////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FRIENDLY FUNCTIONS
// All the following functions are debug friendly. You should NOT use them when compiling in retail.
// The supplier on the other side must provide an implementation for all targets since it may happen
// that some clients are compiled in debug while some others are compiled in retail. So they must be
// supported at all times. In the case where the supplier is in Retail, the debug info should simply
// be discarded. In retail, the client side should avoid using these functions, since carrying the
// extra parameters adds useless overhead to function calls.
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Allocate a memory block with extra debug info.
    \param Size         The size in bytes of the block to allocate.
    \param Alignment    The required alignment.
    \param MemType      The type of memory to allocate
    \param Tag          The tag identifying the origin of the call. See \ref alloctags.
    \param OpType       Operation type; used to identify the type of allocation in caller's code.
    \param pTypename    A static string containing the type of data allocated.
    \param pFilename    A static string containing the file name from where the
                        allocation is made.
    \param FileLine     The line number where the allocation is made
    \param pExtraTag    When a string tag is provided (!= 0), it will supersede the one linked
                        with the Tag parameter.

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - Size must be greater than 0.
    - Alignment must be > 0, and in base 2 (1,2,4,8,16,32,64, and so on).
    - pTypename & pFileLine must be static in memory as long as the Memory library is
      not shut down.
    - pExtraTag can be a temporary string.
    - All strings can be NULL.

    \POSTCONDITIONS
    - Returns a pointer to the block of memory allocated, with an address multiple of Alignment.
    - Returns NULL if the memory allocation failed.
    - Memory is expected to be freed calling EalMemFree().
*/
EAL_DLL_ENTRY void* EalMemDebugAlloc(eal_u32        Size,
                                     eal_u32        Alignment,
                                     eal_mem_type   MemType,
                                     eal_id         Tag,
                                     eal_mem_optype OpType,
                                     const char*    pTypename,
                                     const char*    pFilename,
                                     eal_u32        FileLine,
                                     const char*    pExtraTag);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Allocate a memory block with extra debug info. The Context argument is assigned by the
    implementer to keep track of the required information so that \ref EalMemReallocWithContext and
    \ref EalMemFreeWithContext will avoid the need of a lookup to proceed. Typically, it can be
    implemented as being a pointer to an allocator. The Context value must always be the same for
    a given eal_id.
    \param Size         The size in bytes of the block to allocate.
    \param Alignment    The required alignment.
    \param MemType      The type of memory to allocate
    \param Tag          The tag identifying the origin of the call. See \ref alloctags.
    \param OpType       Operation type; used to identify the type of allocation in caller's code.
    \param [in,out] Context
                        An opaque ref to a context information pointer meaningful to the implementer.
    \param pTypename    A static string containing the type of data allocated.
    \param pFilename    A static string containing the file name from where the
                        allocation is made.
    \param FileLine     The line number where the allocation is made
    \param pExtraTag    When a string tag is provided (!= 0), it will supersede the one linked
                        with the Tag parameter.

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - Size must be greater than 0.
    - Alignment must be > 0, and in base 2 (1,2,4,8,16,32,64, and so on).
    - A non-null Context may be passed only if it is the same as received from a previous call
      to this function for a given eal_id.
    - pTypename & pFileLine must be static in memory as long as the Memory library is
      not shut down.
    - pExtraTag can be a temporary string.
    - All strings can be NULL.

    \POSTCONDITIONS
    - Returns a pointer to the block of memory allocated, with an address multiple of Alignment.
    - Returns NULL if the memory allocation failed.
    - Memory is expected to be freed calling \ref EalMemFree.
    - The same Context is expected to be passed to \ref EalMemDebugReallocWithContext and
      \ref EalMemDebugFreeWithContext.

      \sa EalMemDebugFreeWithContext, EalMemDebugReallocWithContext, EalMemAllocWithContext
*/
EAL_DLL_ENTRY void* EalMemDebugAllocWithContext(eal_u32         Size,
                                                eal_u32         Alignment,
                                                eal_mem_type    MemType,
                                                eal_id          Tag,
                                                eal_mem_optype  OpType,
                                                void*&          Context,
                                                const char*     pTypename,
                                                const char*     pFilename,
                                                eal_u32         FileLine,
                                                const char*     pExtraTag);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Readjust the size of a memory block already allocated with extra debug info.
    \param pOldPtr    A pointer to the previous memory block allocated
    \param NewSize    The new byte size of the memory block
    \param Alignment  The required alignment.
    \param pFilename  A static string containing the file name from where the reallocation is made.
    \param FileLine   The line number where the reallocation is made

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - pOldPtr must be valid. If not, call Alloc instead.
    - NewSize MUST be greater than 0. If Size is 0, you should call Free instead.
    - Alignment must be > 0, and and in base 2 (1,2,4,8,16,32,64, and so on).
    - pFilename must be static in memory as long as the Memory library is not shutdown

    \POSTCONDITIONS
    - Returns a pointer on the new block of memory, with an address multiple of Alignment
    - Returns NULL if the memory allocation failed
    - Memory is expected to be freed calling EalMemFree
    - The Tag of pOldPtr will be preserved for the new memory bloc
*/
EAL_DLL_ENTRY void* EalMemDebugRealloc(void*        pOldPtr,
                                       eal_u32      NewSize,
                                       eal_u32      Alignment,
                                       const char*  pFilename,
                                       eal_u32      FileLine);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Readjust the size of a memory block already allocated with extra debug info.
    \param pOldPtr    A pointer to the previous memory block allocated
    \param NewSize    The new byte size of the memory block
    \param Alignment  The required alignment.
    \param [in] Context
                      An opaque ref to the same context information pointer received from
                      \ref EalMemDebugAllocWithContext.
    \param pFilename  A static string containing the file name from where the reallocation is made.
    \param FileLine   The line number where the reallocation is made

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - pOldPtr must be valid. If not, call Alloc instead.
    - NewSize MUST be greater than 0. If Size is 0, you should call Free instead.
    - Alignment must be > 0, and and in base 2 (1,2,4,8,16,32,64, and so on).
    - pFilename must be static in memory as long as the Memory library is not shutdown

    \POSTCONDITIONS
    - Returns a pointer on the new block of memory, with an address multiple of Alignment
    - Returns NULL if the memory allocation failed
    - Memory is expected to be freed calling \ref EalMemDebugFreeWithContext
    - The Tag of pOldPtr will be preserved for the new memory bloc

    \sa EalMemDebugFreeWithContext, EalMemDebugAllocWithContext, EalMemReallocWithContext
*/
EAL_DLL_ENTRY void* EalMemDebugReallocWithContext(  void*           pOldPtr,
                                                    eal_u32         NewSize,
                                                    eal_u32         Alignment,
                                                    void *const &   Context,
                                                    const char*     pFilename,
                                                    eal_u32         FileLine);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Free previously allocated memory.
    \param pMemoryToFree    A pointer to the memory block to free.
    \param OpType           Operation type; used to identify what type of free was used in the
                            caller's code.
    \param pFilename        A static string containing the file name from where the
                            allocation is made.
    \param FileLine         The line number where the allocation is made

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - pMemoryToFree must not be NULL.
    - pFilename must be static in memory as long as the Memory library is
      not shut down.
    - pFilename can be NULL

    \POSTCONDITIONS
    - The free memory is returned to the system.
    - The memory freed must not be used anymore after the call to EalMemDebugFree
*/
EAL_DLL_ENTRY void EalMemDebugFree(void*            pMemoryToFree,
                                   eal_mem_optype   OpType,
                                   const char*      pFilename,
                                   eal_u32          FileLine);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Free previously allocated memory.
    \param pMemoryToFree    A pointer to the memory block to free.
    \param OpType           Operation type; used to identify what type of free was used in the
                            caller's code.
    \param [in] Context     An opaque ref to the same context information pointer received from
                            \ref EalMemAllocWithContext.
    \param pFilename        A static string containing the file name from where the
                            allocation is made.
    \param FileLine         The line number where the allocation is made

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - pMemoryToFree must not be NULL.
    - pFilename must be static in memory as long as the Memory library is not shut down.
    - pFilename can be NULL

    \POSTCONDITIONS
    - The free memory is returned to the system.
    - The memory freed must not be used anymore after the call to EalMemDebugFreeWithContext

    \sa EalMemFreeWithContext, EalMemDebugReallocWithContext, EalMemDebugAllocWithContext
*/
EAL_DLL_ENTRY void EalMemDebugFreeWithContext(  void*           pMemoryToFree,
                                                eal_mem_optype  OpType,
                                                void *const &   Context,
                                                const char*     pFilename,
                                                eal_u32         FileLine);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \deprecated DEPRECATED since version 5.00 (use \ref EalLogSetTagName instead), will be removed in version 8.00
    Associate a tag with a string.
    NOTE: This function is used by memory tools to link a numerical tag to its textual equivalent.

    \param Tag          The tag for which you want to provide a string. See \ref alloctags.
    \param pTagName     A string containing a textual tag. Can be a temporary string.

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - pTagName must be different than NULL.
    - If the function is called more than once with the same Tag, it should be ignored.
*/
EAL_DEPRECATED("Deprecated; use EalLogSetTagName instead")
EAL_DLL_ENTRY void EalMemDebugSetTagName(eal_id Tag, const char* pTagName);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Declare a memory block allocated before/outside EalMem.
    \param pMemory      A pointer on the memory block to declare
    \param Size         The size in bytes of the block to allocate.
    \param Alignment    The block alignment.
    \param MemType      The type of memory to allocate
    \param Tag          The tag identifying the origin of the call. See \ref alloctags.
    \param OpType       Operation type; used to identify the type of allocation in caller's code.
    \param pTypename    A static string containing the type of data allocated.
    \param pFilename    A static string containing the file name from where the
                        allocation is made.
    \param FileLine     The line number where the allocation is made

    \PRECONDITIONS
    - This function should not be called if you compile in retail.
    - Size must be greater than 0.
    - Alignment must be > 0, and in base 2 (1,2,4,8,16,32,64, and so on).
    - pTypename & pFileLine must be static in memory as long as the Memory library is
      not shut down.
    - pExtraTag can be a temporary string.
    - All strings can be NULL.
*/
EAL_DLL_ENTRY void EalMemDebugDeclareMemory(const void*     pMemory,
                                            eal_u32         Size,
                                            eal_u32         Alignment,
                                            eal_mem_type    MemType,
                                            eal_id          Tag,
                                            eal_mem_optype  OpType,
                                            const char*     pTypename,
                                            const char*     pFilename,
                                            eal_u32         FileLine);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL Mem functions.
struct eal_mem_dll_interface
{
    bool        (*pEalMemLibInit)           (eal_u32        Version);
    void        (*pEalMemLibShutdown)       ();
    void*       (*pEalMemAlloc)             (eal_u32        Size,
                                            eal_u32         Alignment,
                                            eal_mem_type    MemType,
                                            eal_id          Tag);
    void*       (*pEalMemAllocWithContext)  (eal_u32        Size,
                                            eal_u32         Alignment,
                                            eal_mem_type    MemType,
                                            eal_id          Tag,
                                            void*&          Context);
    void*       (*pEalMemRealloc)           (void*          pOldPtr,
                                            eal_u32         NewSize,
                                            eal_u32         Alignment);
    void*       (*pEalMemReallocWithContext)(void*          pOldPtr,
                                            eal_u32         NewSize,
                                            eal_u32         Alignment,
                                            void *const &   Context);
    void        (*pEalMemFree)              (void*          pMemoryToFree);
    void        (*pEalMemFreeWithContext)   (void*          pMemoryToFree,
                                            void *const &   Context);
    eal_u32     (*pEalMemGetPageSize)       ();
    eal_u32     (*pEalMemGetAllocatedSize)  (const void*    pMemory);
    eal_u32     (*pEalMemGetAllocated)      ();
    void*       (*pEalMemDebugAlloc)        (eal_u32        Size,
                                            eal_u32         Alignment,
                                            eal_mem_type    MemType,
                                            eal_id          Tag,
                                            eal_mem_optype  OpType,
                                            const char*     pType,
                                            const char*     pFilename,
                                            eal_u32         FileLine,
                                            const char*     pExtraTag);
    void*       (*pEalMemDebugAllocWithContext)(eal_u32     Size,
                                            eal_u32         Alignment,
                                            eal_mem_type    MemType,
                                            eal_id          Tag,
                                            eal_mem_optype  OpType,
                                            void*&          Context,
                                            const char*     pType,
                                            const char*     pFilename,
                                            eal_u32         FileLine,
                                            const char*     pExtraTag);
    void*       (*pEalMemDebugRealloc)      (void*          pOldPtr,
                                            eal_u32         NewSize,
                                            eal_u32         Alignment,
                                            const char*     pFilename,
                                            eal_u32         FileLine);
    void*       (*pEalMemDebugReallocWithContext)(void*     pOldPtr,
                                            eal_u32         NewSize,
                                            eal_u32         Alignment,
                                            void *const &   Context,
                                            const char*     pFilename,
                                            eal_u32         FileLine);
    void        (*pEalMemDebugFree)         (void*          pMemoryToFree,
                                            eal_mem_optype  OpType,
                                            const char*     pFilename,
                                            eal_u32         FileLine);
    void        (*pEalMemDebugFreeWithContext)(void*        pMemoryToFree,
                                            eal_mem_optype  OpType,
                                            void *const &   Context,
                                            const char*     pFilename,
                                            eal_u32         FileLine);

    // BEGIN DEPRECATED in V500
    void        (*pEalMemDebugSetTagName)   (eal_id         Tag,
                                            const char*     pTagName);
    // END DEPRECATED

    void        (*pEalMemDebugDeclareMemory)(const void*     pMemory,
                                            eal_u32         Size,
                                            eal_u32         Alignment,
                                            eal_mem_type    MemType,
                                            eal_id          Tag,
                                            eal_mem_optype  OpType,
                                            const char*     pTypename,
                                            const char*     pFilename,
                                            eal_u32         FileLine);
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initalization code and must be done before any
    call to any EAL function.
    \param Interface    Structure containing valid pointers to the engine EAL functions.
    \sa EalMemDllPopulateInterface
*/
void EalMemDllInitInterface(const eal_mem_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamicaly connect its internal EAL
    function calls to the one in your engine.
    \param Interface    A structure that will be filled up with pointers to local EAL functions.
    \sa EalMemDllInitInterface
*/
void EalMemDllPopulateInterface(eal_mem_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
