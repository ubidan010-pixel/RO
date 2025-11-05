////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealsyspagedll.cpp  DLL wrapper implementation for the EAL (syspage)
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealsyspage.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Library/Client/DLL Side
static eal_syspage_dll_interface gEalSysPageDllInterface{};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalSysPageDllInitInterface(const eal_syspage_dll_interface& Interface)
{
    gEalSysPageDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalSysPageLibInit(eal_u32 Version)
{
    return (*gEalSysPageDllInterface.pEalSysPageLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalSysPageLibShutdown()
{
    (*gEalSysPageDllInterface.pEalSysPageLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalSysPageAcquireInterface(eal_id tag, void* allocatorInstance, eal_syspage_interface*& allocInterfaceOut, void*& contextInfoOut)
{
    return (*gEalSysPageDllInterface.pEalSysPageAcquireInterface)(tag, allocatorInstance, allocInterfaceOut, contextInfoOut);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalSysPageReleaseInterface(eal_id tag, void* allocatorInstance, eal_syspage_interface* allocInterface, void* contextInfo)
{
    return (*gEalSysPageDllInterface.pEalSysPageReleaseInterface)(tag, allocatorInstance, allocInterface, contextInfo);
}

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine/Implementation/Host Side

void EalSysPageDllPopulateInterface(eal_syspage_dll_interface& Interface)
{
    Interface.pEalSysPageLibInit          = EalSysPageLibInit;
    Interface.pEalSysPageLibShutdown      = EalSysPageLibShutdown;
    Interface.pEalSysPageAcquireInterface = EalSysPageAcquireInterface;
    Interface.pEalSysPageReleaseInterface = EalSysPageReleaseInterface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
