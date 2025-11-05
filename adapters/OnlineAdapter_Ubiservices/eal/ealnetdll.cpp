////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealnetdll.cpp  DLL wrapper implementation for the EAL (network)
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealnet.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

// DLL Side
static eal_net_dll_interface        gEalNetDllInterface = {};


void EalNetDllInitInterface(const eal_net_dll_interface& Interface)
{
    gEalNetDllInterface = Interface;
}

bool EalNetLibInit(eal_u32 Version)
{
    return (*gEalNetDllInterface.pEalNetLibInit)(Version);
}

void EalNetLibShutdown()
{
    (*gEalNetDllInterface.pEalNetLibShutdown)();
}

#else

// Engine Side

void EalNetDllPopulateInterface(eal_net_dll_interface& Interface)
{
    Interface.pEalNetLibInit        = EalNetLibInit;
    Interface.pEalNetLibShutdown    = EalNetLibShutdown;
}

#endif

