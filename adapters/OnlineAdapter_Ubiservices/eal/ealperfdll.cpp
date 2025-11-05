////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealperfdll.cpp  DLL wrapper implementation for the EAL (performance).
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealperf.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL Side
static eal_perf_dll_interface        gEalPerfDllInterface = {};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfDllInitInterface(const eal_perf_dll_interface& Interface)
{
    gEalPerfDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLibInit(eal_u32 Version)
{
    return (*gEalPerfDllInterface.pEalPerfLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfLibShutdown()
{
    (*gEalPerfDllInterface.pEalPerfLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfSetTrackInfo(eal_id TrackId, const char* pTrackName)
{
    (*gEalPerfDllInterface.pEalPerfSetTrackInfo)(TrackId, pTrackName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EalPerfConvertDynamicString(const char* pDynamicString)
{
    return (*gEalPerfDllInterface.pEalPerfConvertDynamicString)(pDynamicString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EalPerfConvertTemporaryStringVa(
    const char* pFormat,
    va_list Args)
{
    return (*gEalPerfDllInterface.pEalPerfConvertTemporaryStringVa)(pFormat, Args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfBeginTask(
    const eal_perf_static_point& Point,
    eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfBeginTask)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfBeginTaskWithPayload(
    const eal_perf_static_point& Point,
    eal_u64 Time,
    const char* pCustomString,
    const void* pPayload,
    eal_size_t PayloadSize)
{
    return (*gEalPerfDllInterface.pEalPerfBeginTaskWithPayload)(Point, Time, pCustomString, pPayload, PayloadSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfEndTask(
    const eal_perf_static_point& Point,
    eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfEndTask)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfEndTaskWithPayload(
    const eal_perf_static_point& Point,
    eal_u64 Time,
    const char* pCustomString,
    const void* pPayload,
    eal_size_t PayloadSize)
{
    return (*gEalPerfDllInterface.pEalPerfEndTaskWithPayload)(Point, Time, pCustomString, pPayload, PayloadSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfAddEvent(
    const eal_perf_static_point& Point,
    eal_u64 Time,
    eal_s64 Param)
{
    return (*gEalPerfDllInterface.pEalPerfAddEvent)(Point, Time, Param);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfAddEventWithPayload(
    const eal_perf_static_point& Point,
    eal_u64 Time,
    eal_s64 Param,
    const char* pCustomString,
    const void* pPayload,
    eal_size_t PayloadSize)
{
    return (*gEalPerfDllInterface.pEalPerfAddEventWithPayload)(Point, Time, Param, pCustomString, pPayload, PayloadSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfAddWarningCount(
    const eal_perf_static_point& Point,
    eal_u64 Time,
    eal_s64 Count)
{
    return (*gEalPerfDllInterface.pEalPerfAddWarningCount)(Point, Time, Count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfLogThreadName(eal_u64 ThreadID, const char* pThreadName)
{
    (*gEalPerfDllInterface.pEalPerfLogThreadName)(ThreadID, pThreadName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfTaskBegin(const eal_perf_static_point& Point, eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfTaskBegin)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfTaskEnd(const eal_perf_static_point& Point, eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfTaskEnd)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfTaskScope(const eal_perf_static_point& Point, eal_u64 BeginTime, eal_u64 EndTime)
{
    return (*gEalPerfDllInterface.pEalPerfTaskScope)(Point, BeginTime, EndTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfEvent(const eal_perf_static_point& Point, eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfEvent)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfWarningCount(const eal_perf_static_point& Point, eal_u64 Time, eal_s64 Count)
{
    return (*gEalPerfDllInterface.pEalPerfWarningCount)(Point, Time, Count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLowFrequencyCounterU64(const eal_perf_static_point& Point, eal_u64 Time, eal_u64 Value)
{
    return (*gEalPerfDllInterface.pEalPerfLowFrequencyCounterU64)(Point, Time, Value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLowFrequencyCounterF64(const eal_perf_static_point& Point, eal_u64 Time, double Value)
{
    return (*gEalPerfDllInterface.pEalPerfLowFrequencyCounterF64)(Point, Time, Value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfData(eal_id TrackId, const void* pData, eal_u32 DataSize)
{
    return (*gEalPerfDllInterface.pEalPerfData)(TrackId, pData, DataSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfVariables(const eal_perf_static_point& Point, va_list Args)
{
    return (*gEalPerfDllInterface.pEalPerfVariables)(Point, Args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfRegisterMetaDataProvider(   EalPerfMetaDataProviderCallback providerCallback,
                                        void*                           pCookie,
                                        eal_perf_metadata_flags         Flags /*= EAL_PERF_METADATA_DEFAULT*/)
{
    return (*gEalPerfDllInterface.pEalPerfRegisterMetaDataProvider)(providerCallback, pCookie, Flags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfUnregisterMetaDataProvider(EalPerfMetaDataProviderCallback providerCallback)
{
    return (*gEalPerfDllInterface.pEalPerfUnregisterMetaDataProvider)(providerCallback);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLogMetaData(const eal_perf_static_point&    Point,
                        const void*                     pMetaData,
                        eal_u32                         MetaDataSize)
{
    return (*gEalPerfDllInterface.pEalPerfLogMetaData)(Point, pMetaData, MetaDataSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLogMetaDataBlockBegin(const eal_perf_static_point& Point)
{
    return (*gEalPerfDllInterface.pEalPerfLogMetaDataBlockBegin)(Point);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLogMetaDataBlock(   const eal_perf_static_point&    Point,
                                const void*                     pMetaData,
                                eal_u32                         MetaDataSize)
{
    return (*gEalPerfDllInterface.pEalPerfLogMetaDataBlock)(Point, pMetaData, MetaDataSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLogMetaDataBlockEnd(  const eal_perf_static_point& Point)
{
    return (*gEalPerfDllInterface.pEalPerfLogMetaDataBlockEnd)(Point);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t EalPerfGetCurrentFrameNo()
{
    return (*gEalPerfDllInterface.pEalPerfGetCurrentFrameNo)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t EalPerfGetCurrentFrameBeginTime()
{
    return (*gEalPerfDllInterface.pEalPerfGetCurrentFrameBeginTime)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfDetachTrackFromThread(eal_id TrackId)
{
    (*gEalPerfDllInterface.pEalPerfDetachTrackFromThread)(TrackId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfWaitBegin(const eal_perf_static_point& Point, eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfWaitBegin)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfWaitEnd(const eal_perf_static_point& Point, eal_u64 Time)
{
    return (*gEalPerfDllInterface.pEalPerfWaitEnd)(Point, Time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfWaitScope(const eal_perf_static_point& Point, eal_u64 BeginTime, eal_u64 EndTime)
{
    return (*gEalPerfDllInterface.pEalPerfWaitScope)(Point, BeginTime, EndTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLockScope(const eal_perf_lock_profile_info& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime)
{
    return (*gEalPerfDllInterface.pEalPerfLockScope)(LockProfileInfo, LockType, UnlockTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalPerfLockScope2(const eal_perf_lock_profile_info2& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime)
{
    return (*gEalPerfDllInterface.pEalPerfLockScope2)(LockProfileInfo, LockType, UnlockTime);
}

bool EalPerfLockScope2WithWaitState(const eal_perf_lock_profile_info2& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime, eal_u32 WaitState)
{
    return (*gEalPerfDllInterface.pEalPerfLockScope2WithWaitState)(LockProfileInfo, LockType, UnlockTime, WaitState);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfTrackSyncPrimitiveCreate(eal_id Tag, eal_perf_sync_primitive_type Type, const char* Name, eal_uintptr_t InstanceId)
{
    return (gEalPerfDllInterface.pEalPerfTrackSyncPrimitiveCreate)(Tag, Type, Name, InstanceId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalPerfTrackSyncPrimitiveDestroy(eal_id Tag, eal_perf_sync_primitive_type Type, const char* Name, eal_uintptr_t InstanceId)
{
    return (gEalPerfDllInterface.pEalPerfTrackSyncPrimitiveDestroy)(Tag, Type, Name, InstanceId);
}

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine Side

void EalPerfDllPopulateInterface(eal_perf_dll_interface& Interface)
{
    Interface.pEalPerfLibInit                       = EalPerfLibInit;
    Interface.pEalPerfLibShutdown                   = EalPerfLibShutdown;

    Interface.pEalPerfRegisterMetaDataProvider      = EalPerfRegisterMetaDataProvider;
    Interface.pEalPerfUnregisterMetaDataProvider    = EalPerfUnregisterMetaDataProvider;
    Interface.pEalPerfLogMetaData                   = EalPerfLogMetaData;
    Interface.pEalPerfLogMetaDataBlockBegin         = EalPerfLogMetaDataBlockBegin;
    Interface.pEalPerfLogMetaDataBlock              = EalPerfLogMetaDataBlock;
    Interface.pEalPerfLogMetaDataBlockEnd           = EalPerfLogMetaDataBlockEnd;

    Interface.pEalPerfGetCurrentFrameNo             = EalPerfGetCurrentFrameNo;
    Interface.pEalPerfGetCurrentFrameBeginTime      = EalPerfGetCurrentFrameBeginTime;
    Interface.pEalPerfDetachTrackFromThread         = EalPerfDetachTrackFromThread;

    Interface.pEalPerfLogThreadName                 = EalPerfLogThreadName;
    Interface.pEalPerfTaskBegin                     = EalPerfTaskBegin;
    Interface.pEalPerfTaskEnd                       = EalPerfTaskEnd;
    Interface.pEalPerfTaskScope                     = EalPerfTaskScope;
    Interface.pEalPerfEvent                         = EalPerfEvent;
    Interface.pEalPerfWarningCount                  = EalPerfWarningCount;
    Interface.pEalPerfData                          = EalPerfData;
    Interface.pEalPerfVariables                     = EalPerfVariables;
    Interface.pEalPerfWaitBegin                     = EalPerfWaitBegin;
    Interface.pEalPerfWaitEnd                       = EalPerfWaitEnd;
    Interface.pEalPerfWaitScope                     = EalPerfWaitScope;
    Interface.pEalPerfLowFrequencyCounterU64        = EalPerfLowFrequencyCounterU64;
    Interface.pEalPerfLowFrequencyCounterF64        = EalPerfLowFrequencyCounterF64;

    Interface.pEalPerfLockScope2                    = EalPerfLockScope2;
    Interface.pEalPerfLockScope2WithWaitState       = EalPerfLockScope2WithWaitState;

    Interface.pEalPerfTrackSyncPrimitiveCreate      = EalPerfTrackSyncPrimitiveCreate;
    Interface.pEalPerfTrackSyncPrimitiveDestroy     = EalPerfTrackSyncPrimitiveDestroy;

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
        // BEGIN DEPRECATED in V500
        Interface.pEalPerfSetTrackInfo              = EalPerfSetTrackInfo;

        // BEGIN DEPRECATED in V400
        Interface.pEalPerfConvertDynamicString      = EalPerfConvertDynamicString;

        // BEGIN DEPRECATED in V400
        Interface.pEalPerfConvertTemporaryStringVa  = EalPerfConvertTemporaryStringVa;
        Interface.pEalPerfBeginTask                 = EalPerfBeginTask;
        Interface.pEalPerfBeginTaskWithPayload      = EalPerfBeginTaskWithPayload;
        Interface.pEalPerfEndTask                   = EalPerfEndTask;
        Interface.pEalPerfEndTaskWithPayload        = EalPerfEndTaskWithPayload;
        Interface.pEalPerfAddEvent                  = EalPerfAddEvent;
        Interface.pEalPerfAddEventWithPayload       = EalPerfAddEventWithPayload;
        Interface.pEalPerfAddWarningCount           = EalPerfAddWarningCount;

        // BEGIN DEPRECATED in V900
        Interface.pEalPerfLockScope                 = EalPerfLockScope;
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
