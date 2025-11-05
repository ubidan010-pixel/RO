////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file eallogdll.cpp  DLL wrapper implementation for the EAL (logging)
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "eallog.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL Side
static eal_log_dll_interface        gEalLogDllInterface = {};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalLogDllInitInterface(const eal_log_dll_interface& Interface)
{
    gEalLogDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalLogLibInit(eal_u32 Version)
{
    return (*gEalLogDllInterface.pEalLogLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalLogLibShutdown()
{
    (*gEalLogDllInterface.pEalLogLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalLogIsEnabled(eal_id Tag, eal_log_level Level)
{
    return (*gEalLogDllInterface.pEalLogIsEnabled)(Tag, Level);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalLogOutput(eal_id Tag, eal_log_level Level, const char* pMessage, const char* pFile, eal_u32 Line)
{
    (*gEalLogDllInterface.pEalLogOutput)(Tag, Level, pMessage, pFile, Line);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalLogOutputWithFormatVa(eal_id Tag, eal_log_level Level, const char* pFile, eal_u32 Line, const char* pFormat, va_list Args)
{
    (*gEalLogDllInterface.pEalLogOutputWithFormatVa)(Tag, Level, pFile, Line, pFormat, Args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalLogAssert(  eal_id      Tag,
                    const char* pCondition,
                    const char* pMessage,
                    const char* pFile,
                    eal_u32     Line,
                    eal_u32*    pStatic)
{
    return (*gEalLogDllInterface.pEalLogAssert)(Tag, pCondition, pMessage, pFile, Line, pStatic);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalLogAssert2( eal_id              Tag,
                    const char*         pCondition,
                    const char*         pMessage,
                    const char*         pFile,
                    eal_u32             Line,
                    eal_log_assert_opt  Options,
                    eal_u32*            pStatic)
{
    return (*gEalLogDllInterface.pEalLogAssert2)(Tag, pCondition, pMessage, pFile, Line,
                                                 Options, pStatic);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalLogSetTagName(eal_id Tag, const char* pTagName)
{
    (*gEalLogDllInterface.pEalLogSetTagName)(Tag, pTagName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EalLogGetTagName(eal_id Tag)
{
    return (*gEalLogDllInterface.pEalLogGetTagName)(Tag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_s32 EalLogOutputWithSize(eal_id          Tag,
                             eal_log_level   Level,
                             eal_u64         Reserved,
                             const char*     pMessage,
                             eal_size_t      MessageSize,
                             const char*     pFile,
                             eal_u32         Line)
{
    return (*gEalLogDllInterface.pEalLogOutputWithSize)(Tag, Level, Reserved, pMessage, MessageSize, pFile, Line);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine Side

void EalLogDllPopulateInterface(eal_log_dll_interface& Interface)
{
    Interface.pEalLogLibInit            = EalLogLibInit;
    Interface.pEalLogLibShutdown        = EalLogLibShutdown;
    Interface.pEalLogOutput             = EalLogOutput;
    Interface.pEalLogIsEnabled          = EalLogIsEnabled;
    Interface.pEalLogAssert2            = EalLogAssert2;
    Interface.pEalLogOutputWithFormatVa = EalLogOutputWithFormatVa;
    Interface.pEalLogSetTagName         = EalLogSetTagName;
    Interface.pEalLogGetTagName         = EalLogGetTagName;
    Interface.pEalLogOutputWithSize     = EalLogOutputWithSize;

    // BEGIN DEPRECATED in V502
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
    Interface.pEalLogAssert             = EalLogAssert;
#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(_GCC)
#   pragma GCC diagnostic pop
#endif
    // END DEPRECATED
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
