////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealthreaddll.cpp  DLL wrapper implementation for the EAL thread
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealthread.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

// DLL Side
static eal_thread_dll_interface        gEalThreadDllInterface = {};


void EalThreadDllInitInterface(const eal_thread_dll_interface& Interface)
{
    gEalThreadDllInterface = Interface;
}

bool EalThreadLibInit(eal_u32 Version)
{
    return (*gEalThreadDllInterface.pEalThreadLibInit)(Version);
}

void EalThreadLibShutdown()
{
    (*gEalThreadDllInterface.pEalThreadLibShutdown)();
}

eal_thread_handle EalThreadCreate(  eal_id               Tag,
                                    eal_u32              StackSize,
                                    eal_thread_priority  Priority,
                                    void                 (*pEntryPoint)(void*),
                                    void*                pCookie)
{
    return (*gEalThreadDllInterface.pEalThreadCreate)(Tag, StackSize, Priority, pEntryPoint, pCookie);
}

void EalThreadClose(eal_thread_handle Handle)
{
    (*gEalThreadDllInterface.pEalThreadClose)(Handle);
}

#else

// Engine Side

void EalThreadDllPopulateInterface(eal_thread_dll_interface& Interface)
{
    Interface.pEalThreadLibInit        = EalThreadLibInit;
    Interface.pEalThreadLibShutdown    = EalThreadLibShutdown;
    Interface.pEalThreadCreate         = EalThreadCreate;
    Interface.pEalThreadClose          = EalThreadClose;
}

#endif

