////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealthread.h  Engine Abstraction Layer Thread Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ealdef.h"
/// This module covers the thread reference API.
/*! \addtogroup Thread
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210).
/// See ealdef.h for more information.
#define EAL_THREAD_VERSION             101

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void* eal_thread_handle;            ///< A Thread handle

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Thread priority
enum eal_thread_priority
{
    EAL_THREAD_PRIORITY_IDLE           = 0,    ///< Background thread
    EAL_THREAD_PRIORITY_LOWEST         = 1,    ///< Lowest priority level
    EAL_THREAD_PRIORITY_BELOW_NORMAL   = 2,    ///< Slightly under normal priority
    EAL_THREAD_PRIORITY_NORMAL         = 3,    ///< Normal priority
    EAL_THREAD_PRIORITY_ABOVE_NORMAL   = 4,    ///< Slightly above normal priority
    EAL_THREAD_PRIORITY_HIGHEST        = 5,    ///< Highest priority
    EAL_THREAD_PRIORITY_TIME_CRITICAL  = 6,    ///< Real-time thread
    EAL_THREAD_NB_PRIORITY_LEVELS      = 7
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Initializes the library.
    \return true if initialization succeeded

    \param Version      Must be equal to current version = EAL_THREAD_VERSION

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalThreadLibInit(), so
      EalThreadLibShutdown() is called the exact same number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all APIs calls will be allowed.
    - EalThreadLibShutdown() must be called even if false is returned.

    \sa EalLogLibShutdown
*/
EAL_DLL_ENTRY bool EalThreadLibInit(eal_u32 Version = EAL_THREAD_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalThreadLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalThreadLibShutdown().

    \sa EalThreadLibInit
*/
EAL_DLL_ENTRY void EalThreadLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Create a new thread.

    \return A thread handle

    \param Tag          The eal_id identifying the thread creator & purpose. See ealdef.h.
    \param StackSize    The stack size required for the thread to execute correctly
    \param Priority     The proposed priority for this thread.
    \param pEntrypoint  A pointer on the function to be called by the thread
    \param pCookie      A value that will be passed as parameter when the entry point is invoked

    \POSTCONDITIONS
    - The thread will be destroyed when it returns from pEntryPoint. The resources for the thread
      may be only released when EalThreadClose is called.
    - Thread creation cannot fail. EalThreadCreate() should block until necessary resources are
      available to start the thread.

    \sa EalThreadClose
*/
EAL_DLL_ENTRY eal_thread_handle EalThreadCreate(eal_id               Tag,
                                                eal_u32              StackSize,
                                                eal_thread_priority  Priority,
                                                void                 (*pEntryPoint)(void*),
                                                void*                pCookie);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Wait for a thread completion & release its resources. Must be called from another thread context.

    \param Handle       The thread handle returned by EalThreadCreate

    \sa EalThreadCreate
*/
EAL_DLL_ENTRY void EalThreadClose(eal_thread_handle Handle);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL Thread functions.
struct eal_thread_dll_interface
{
    bool                (*pEalThreadLibInit)    (eal_u32 Version);
    void                (*pEalThreadLibShutdown)();
    eal_thread_handle   (*pEalThreadCreate)     (eal_id             Tag,
                                                eal_u32             StackSize,
                                                eal_thread_priority Priority,
                                                void                (*pEntryPoint)(void*),
                                                void*               pCookie);
    void                (*pEalThreadClose)      (eal_thread_handle  Handle);
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initialization code and must be done before any
    call to any EAL functions.
    \param Interface    A structure containing valid pointers to the engine EAL functions.
    \sa EalThreadDllPopulateInterface
*/
void EalThreadDllInitInterface(const eal_thread_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealthreaddll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamically connect its internal EAL
    function calls to the one in your engine.
    \param Interface    A structure that will be filled with pointers to local EAL functions.
    \sa EalThreadDllInitInterface
*/
void EalThreadDllPopulateInterface(eal_thread_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
