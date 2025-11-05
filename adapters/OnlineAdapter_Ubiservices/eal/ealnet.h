////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealnet.h  Engine Abstraction Layer Network Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ealdef.h"
/// This module covers the network reference API.
/*! \addtogroup Net
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210).
/// See ealdef.h for more information.
#define EAL_NET_VERSION             213

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Initialize the library.
    Network initalization is often a problem on game consoles, and a call to the SDK network
    library often resets the whole communication stack. Many middlewares need to ensure the network
    is initalized before using it. This is the goal of this function.

    \return True if initialization succeeded

    \param Version      Must be equal to the current version = \ref EAL_NET_VERSION.

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalNetLibInit(), so
      EalNetLibShutdown() is called the exact same number of times.
    - The Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all API calls will be allowed.
    - EalNetLibShutdown() must be called even if false is returned.

    \sa EalNetLibShutdown
*/
EAL_DLL_ENTRY bool EalNetLibInit(eal_u32 Version = EAL_NET_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalNetLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalNetLibShutdown().

    \sa EalNetLibInit
*/
EAL_DLL_ENTRY void EalNetLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL Net functions.
struct eal_net_dll_interface
{
    bool    (*pEalNetLibInit)           (eal_u32 Version);
    void    (*pEalNetLibShutdown)       ();
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initialization code and must be done before any
    call to any EAL functions.
    \param Interface    A structure containing valid pointers to the engine EAL functions.
    \sa EalNetDllPopulateInterface
*/
void EalNetDllInitInterface(const eal_net_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealnetdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamically connect its internal EAL
    function calls to the one in your engine.
    \param Interface    A structure that will be filled with pointers to local EAL functions.
    \sa EalNetDllInitInterface
*/
void EalNetDllPopulateInterface(eal_net_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
