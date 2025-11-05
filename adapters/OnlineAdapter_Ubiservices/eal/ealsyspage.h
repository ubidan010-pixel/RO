////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealsyspage.h  Engine Abstraction Layer System Page Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ealdef.h>
#include <limits>

/*
    ************************************************************************************************
    ************************************************************************************************
    ************************************************************************************************

    !!! WARNING !!!

    THIS IS AN EXPERIMENTAL API FOR MEMORY ALLOCATOR DEVELOPMENT ON ANVIL PIPELINE.

    DO NOT IMPLEMENT THIS INTERFACE UNLESS YOU ARE EXPLICITELY REQUIRED TO BY TG.

    ************************************************************************************************
    ************************************************************************************************
    ************************************************************************************************
*/

/// This module covers the system page reference API.
/*! \addtogroup SysPage
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210).
/// See ealdef.h for more information.
#define EAL_SYSPAGE_VERSION 104

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class eal_syspage_alloc_optype : eal_u8
{
    Reserve       = 0, ///< Reserves a range of virtual memory pages without allocating any physical memory.
    Commit        = 1, ///< Allocates physical memory for previously reserved virtual memory pages.
    ReserveCommit = 2, ///< Reserves a range of virtual memory pages and allocates physical memory for them in a single operation.
};

enum class eal_syspage_free_optype : eal_u8
{
    Release  = 0, ///< Frees both the virtual address space and physical memory
    Decommit = 1, ///< Frees physical memory, while keeping the virtual address space
    Discard  = 2, ///< Discards memory contents, while keeping both the virtual address space and physical memory.
};

enum class eal_syspage_protect_optype : eal_u8
{
    NoAccess    = 0, ///< Disables all access
    ReadOnly    = 1, ///< Enables read-only access
    ReadWrite   = 2, ///< Enables read/write access
    Execute     = 3, ///< Enables execute access
    ReadExecute = 4, ///< Enables read/execute access
};

enum class eal_syspage_state : eal_u8
{
    Free      = 0, ///< The page is not committed or reserved and is not accessible
    Reserved  = 1, ///< The page has no physical storage associated with it and is not accessible.
    Committed = 2, ///< Physical storage is allocated for the page
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct eal_syspage_query_info
{
    void* pBaseAddress{ nullptr };    ///< Pointer to the base address of the region of pages
    void* pAllocationBase{ nullptr }; ///< Pointer to the base address of a range of pages allocated by the EalSysPageAlloc function. The page pointed to by the pBaseAddress member is contained within this allocation range
    eal_size_t Size{ 0 };             ///< Size of the region beginning at the base address in which all pages have identical attributes, in bytes
    eal_syspage_state State{ eal_syspage_state::Free }; ///< State of the pages in the region

    static const eal_size_t UNDEFINED_SIZE{ (std::numeric_limits<eal_size_t>::max)() };
};

// Struct contains the information of a region of pages with specific alignment.
struct eal_syspage_aligned_region
{
    eal_uintptr_t Handle;   ///< Handle to free the region of pages. Don't use or modify it.
    void* Address;          ///< Pointer to the base address of the aligned region of pages.
    eal_size_t Size;        ///< Size of the aligned region of pages.
    eal_size_t Alignment;   ///< Alignment of the aligned region of pages.
};

struct eal_syspage_interface
{
    /*! Allocates a region of pages.
    \param addr         If addr is null and allocType is EAL_SYSPAGE_ALLOC_RESERVE or EAL_SYSPAGE_ALLOC_RESERVE_COMMIT, the system chooses the address at which to allocate the region. Otherwise, the system takes it as a hint about where to allocate the region. If allocType is EAL_SYSPAGE_ALLOC_COMMIT, addr must not be null and must be in the range of a virtual address space already reserved by previous allocation with EAL_SYSPAGE_ALLOC_RESERVE.
    \param size         The size of the region of pages, in bytes.
    \param allocType    Type of allocation operation in eal_syspage_alloc_optype.
    \param contextInfo  Production specific information that was provided by \ref EalSysPageAcquireInterface.
    \return             The base address of the allocated regions or 0 if failed.
    */
    void* (*AllocPages)(void* addr, eal_size_t size, eal_syspage_alloc_optype allocType, void* contextInfo);

    /*! Free a region of pages.
        \param addr         A pointer to the base address of the region of pages to be freed.
        \param size         The size of the region of pages to be freed, in bytes.
        \param freeType     Type of free operation in eal_syspage_free_optype.
        \param contextInfo  Production specific information that was provided by \ref EalSysPageAcquireInterface.
        \return             True if operation was successful, false otherwise.
    */
    bool (*FreePages)(void* addr, eal_size_t size, eal_syspage_free_optype freeType,  void* contextInfo);

    /*! Set protection on a region of pages.
        \param addr         A pointer to the base address of the region of pages.
        \param size         The size of the region of pages, in bytes.
        \param protectTypes The memory protection types. Bitwise OR of one or more of the flags in eal_syspage_protect_optype.
        \param contextInfo  Production specific information that was provided by \ref EalSysPageAcquireInterface.
        \return             True if operation was successful, false otherwise.
    */
    bool (*ProtectPages)(void* addr, eal_size_t size, eal_syspage_protect_optype protectTypes, void* contextInfo);

    /*! Query information about a region of pages.
        \param addr         Pointer to the base address of the region of pages to be queried.
        \param contextInfo  Production specific information that was provided by \ref EalSysPageAcquireInterface.
        \param queryInfoOut Reference to the eal_syspage_query_info structure in which information about the specified page range is returned.
        \return             True if operation was successful, false otherwise.
    */
    bool (*QueryPages)(void* addr, void* contextInfo, eal_syspage_query_info& queryInfoOut);

    /*! Reserve and commit an aligned region of pages.
        \param sizeBytes    The size of the region of pages to be allocate, in bytes.
        \param alignment    Alignment of the region of pages to be allocated. It should be 0 or a power of 2. Specify 0 to be aligned to the default page size on the platform.
        \param protectType  The memory protection for the region of pages to be allocated.
        \param contextInfo  Production specific information that was provided by \ref EalSysPageAcquireInterface.
        \param alignedRegionOut The result of the allocation. see \ref eal_syspage_aligned_region.
        \return             True if operation was successful, false otherwise.
    */
    bool (*AllocPagesAligned)(eal_size_t sizeBytes, eal_size_t alignment, eal_syspage_protect_optype protectType, void* contextInfo, eal_syspage_aligned_region& alignedRegionOut);

    /*! Decommit and release an aligned region of pages.
        \param alignedRegion The aligned region of pages to be freed. It should be the result of an allocation provided by AllocPagesAligned.
        \param contextInfo  Production specific information that was provided by \ref EalSysPageAcquireInterface.
        \return             True if operation was successful, false otherwise.
    */
    bool (*FreePagesAligned)(const eal_syspage_aligned_region& alignedRegion, void* contextInfo);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Initialize the library.
    \return true if initialization succeeded

    \param Version      Must be equal to current version = \ref EAL_SYSPAGE_VERSION

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalSysPageLibInit() so
      EalSysPageLibShutdown() is called the exact same number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all API calls will be allowed.
    - EalSysPageLibShutdown() must be called even if false is returned.

    \sa EalSysPageLibShutdown
*/
EAL_DLL_ENTRY bool EalSysPageLibInit(eal_u32 Version = EAL_SYSPAGE_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalSysPageLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalSysPageLibShutdown().

    \sa EalSysPageLibInit
*/
EAL_DLL_ENTRY void EalSysPageLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Acquire the system page allocation interface.
    \param tag An identifier of the intended usage of the interface. See \ref eal_id for more details in ealdef.h.
    \param allocatorInstance A unique identifier for the allocator instance that invokes the call.
    \param allocInterfaceOut The pointer to which the acquired interface will be written.
    \param contextInfoOut Production specific information that must be passed to all interface functions and \ref EalSysPageReleaseInterface.
    \return True if operation was successful, false otherwise. If false, the value in allocInterfaceOut can be undefined and cannot be used.
*/
EAL_DLL_ENTRY bool EalSysPageAcquireInterface(eal_id tag, void* allocatorInstance, eal_syspage_interface*& allocInterfaceOut, void*& contextInfoOut);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Release the system page allocation interface.
    \param tag An identifier of the intended usage of the interface. See \ref eal_id for more details.
    \param allocatorInstance The unique identifier for the allocator instance that invokes the call. Must match the one passed to \ref EalSysPageAcquireInterface.
    \param allocInterface A pointer to the allocation interface to be released.
    \param contextInfo Production specific information that was provided by \ref EalSysPageAcquireInterface.
*/
EAL_DLL_ENTRY void EalSysPageReleaseInterface(eal_id tag, void* allocatorInstance, eal_syspage_interface* allocInterface, void* contextInfo);


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL SysPage functions.
struct eal_syspage_dll_interface
{
    bool    (*pEalSysPageLibInit)(eal_u32 Version);
    void    (*pEalSysPageLibShutdown)();
    bool    (*pEalSysPageAcquireInterface)(eal_id tag,
                                           void* allocatorInstance,
                                           eal_syspage_interface*& allocInterfaceOut,
                                           void*& contextInfoOut);
    void    (*pEalSysPageReleaseInterface)(eal_id tag,
                                           void* allocatorInstance,
                                           eal_syspage_interface* allocInterface,
                                           void* contextInfo);
    };

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initalization code and must be done before any
    call to any EAL function.
    \param Interface    Structure containing valid pointers to the engine EAL functions.
    \sa EalSysPageDllPopulateInterface
*/
void EalSysPageDllInitInterface(const eal_syspage_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamicaly connect its internal EAL
    function calls to the one in your engine.
    \param Interface    A structure that will be filled up with pointers to local EAL functions.
    \sa EalSysPageDllInitInterface
*/
void EalSysPageDllPopulateInterface(eal_syspage_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
