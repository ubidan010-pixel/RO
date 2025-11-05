////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealperf.h Engine Abstraction Layer Performance Logging Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ealdef.h"
#include <cstdarg>

/// This module covers the performance logging reference API.
/*! \addtogroup Perf
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The format is an integer value equal to Major.Minor multiplied by 100 (Ex: Version 2.10 = 210).
/// See ealdef.h for more information.
#define EAL_PERF_VERSION            1100

////////////////////////////////////////////////////////////////////////////////////////////////////

/// \page Performance Tracks
/// The performance logging system is based on a track system. Each track can contain a variable
/// amount of events.
/// Each client is allowed to declare as many tracks as they want. It is up to the engine to select
/// which track to record and how much memory to dedicate to each one.
/// The track format is based on the unique identifier system. See ealdef.h for details.

/// \page Performance Time Unit
/// The unit of time is different for each platform, but usually maps to the unit
/// of the most precise hardware clock available. We call this unit "cycle" even
/// though the value may not represent an actual number of CPU cycles.
/// Here is what you should use on each platform:
///     - \b Win32/Win64/Durango/Orbis: __rdtsc
///     - \b NX: nn::os::GetSystemTick
///     - \b Mac/iOS/Android/Linux ARM64: asm volatile("mrs %0, cntvct_el0" : "=r" (val));
///     - \b Mac x64: mach_absolute_time
///     - \b Linux/Android x64: __builtin_readcyclecounter on Clang, __builtin_ia32_rdtsc on GCC

////////////////////////////////////////////////////////////////////////////////////////////////////

/// This is used to contain everything that is static for an entry in the performance log.
struct eal_perf_static_point
{
    const char* pStaticCaption;     /// A static string describing the entry. Cannot be NULL.
    const char* pFileName;          /// A static string to the source file where the entry is logged (usually taken from __FILE__). Can be NULL.
    eal_id      TrackId;            /// An EAL ID used to organize and filter events in the performance log. Should at least be a product ID.
    eal_u32     FileLine;           /// The line number in the source file where the entry is logged (usually taken from __LINE__). Can be 0.
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Lock profiling info cannot be static because it can be held in a dynamically allocated lock instance.
/// The entire contents of this structure must be copied at unlock time and cannot be referenced later.
/// Caution: as described below, 'pStaticCaption' and 'pFileName' still have static requirements; only the pointers are copied.
struct eal_perf_lock_profile_info
{
    const char*   pStaticCaption;   /// A static string describing the entry. Can be NULL.
    const char*   pFileName;        /// A static string to the source file where the entry is logged (usually taken from __FILE__). Can be NULL.
    eal_id        TrackId;          /// An EAL ID used to organize and filter events in the performance log. Should at least be a product ID.
    eal_u32       FileLine;         /// The line number in the source file where the entry is logged (usually taken from __LINE__). Can be 0.
    eal_uintptr_t LockId;           /// A unique ID for the lock instance, used to connect lock wait chains in a visualization tool
    eal_u64       StartWaitTime;    /// The value of the hardware counter when the lock is requested and the thread starts waiting (in "cycles").
    eal_u64       EndWaitTime;      /// The value of the hardware counter when the lock is acquired and the thread stops waiting (in "cycles").
};

/// Lock profiling info cannot be static because it can be held in a dynamically allocated lock instance.
/// The entire contents of this structure must be copied at unlock time and cannot be referenced later.
/// Caution: as described below, 'pStaticCaption' and 'pFileName' still have static requirements; only the pointers are copied.
struct eal_perf_lock_profile_info2
{
    const char*   pStaticCaption;   /// A static string describing the entry. Can be NULL.
    const char*   pFileName;        /// A static string to the source file where the entry is logged (usually taken from __FILE__). Can be NULL.
    eal_id        TrackId;          /// An EAL ID used to organize and filter events in the performance log. Should at least be a product ID.
    eal_u32       FileLine;         /// The line number in the source file where the entry is logged (usually taken from __LINE__). Can be 0.
    eal_uintptr_t LockId;           /// A unique ID for the lock instance, used to connect lock wait chains in a visualization tool
    eal_u64       StartWaitTime;    /// The value of the hardware counter when the lock is requested and the thread starts waiting (in "cycles").
    eal_u64       EndWaitTime;      /// The value of the hardware counter when the lock is acquired and the thread stops waiting (in "cycles").
    eal_u32       TryCount;         /// The number of attempts to acquire the lock. For all lock types, a value of 0 means the lock was acquired instantly.
                                    /// For adaptive or spin locks, it is the spin count. For blocking locks, 1 means the thread had to wait for the lock.
};

/// Possible types for a lock, including the reader/writer cases for RW locks
/// Used by /ref EalPerfLockScope and /ref EalPerfLockScope2
enum eal_perf_lock_type
{
    EAL_PERF_LOCK_TYPE_ADAPTIVE,
    EAL_PERF_LOCK_TYPE_BLOCKING,
    EAL_PERF_LOCK_TYPE_RW_READER,
    EAL_PERF_LOCK_TYPE_RW_WRITER,
    EAL_PERF_LOCK_TYPE_RWSPIN_READER,
    EAL_PERF_LOCK_TYPE_RWSPIN_WRITER,
    EAL_PERF_LOCK_TYPE_SPIN,
    EAL_PERF_LOCK_TYPE_RSWSPIN_READER,
    EAL_PERF_LOCK_TYPE_RSWSPIN_STAGER,
    EAL_PERF_LOCK_TYPE_RSWSPIN_WRITER
};

/// A list of synchronization primitives provided by the OS for which we want to track the lifetime
/// Used by /ref EalPerfTrackSyncPrimitiveCreate and /ref EalPerfTrackSyncPrimitiveDestroy
enum class eal_perf_sync_primitive_type : eal_u32
{
    LWMutex,   /// Critical Section on Windows, Lightweight Mutex on other platforms
    CondVar,
    RWlock,
    Barrier,
    Semaphore,
    Event,     /// Event on Windows, EventFlag on SCE platforms
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Initialize the library.


    \param Version      Must be equal to current version = \ref EAL_PERF_VERSION
    \return             true if initialization succeeded

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalPerfLibInit(), so
      EalPerfLibShutdown() is called the exact same number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all APIs calls will be allowed.
    - EalPerfLibShutdown() must be called even if false is returned

    \sa EalPerfLibShutdown
*/
EAL_DLL_ENTRY bool EalPerfLibInit(eal_u32 Version = EAL_PERF_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Shut down the library after usage.
    \PRECONDITIONS
    - It must be called once for every call to EalPerfLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library members after calling EalPerfLibShutdown().

    \sa EalPerfLibInit
*/
EAL_DLL_ENTRY void EalPerfLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \deprecated DEPRECATED since version 4.00 (use \ref EalPerfData or EalPerfVariables to pass strings), will be removed in version 8.00
    Copies a dynamic string into a permanent storage on the implementation
    side and returns a pointer to that copy, which can then safely be used as
    the 'customString' parameter of EalPerf*WithPayload functions.
    If the implementation cannot store the string, the return value is
    implementation-defined. It can be NULL or set to a fixed string indicating
    the problem.
    Unlike \ref EalPerfConvertTemporaryStringVa, this function only needs to be
    called once per dynamic string.
    NOTE: As dynamic strings cannot be removed and will keep accumulating over
          time, please use this function with parsimony.

    \param pDynamicString   A pointer on a string that needs to be copied. Can be NULL.
    \return                 A pointer on a string that is safe to be passed to the EalPerf*WithPayload functions. Can be NULL.

    \PRECONDITIONS
    - Multithread safe call

    \POSTCONDITIONS
    - The returned string pointer can be used until \ref EalPerfLibShutdown is called.

    \sa EalPerfConvertTemporaryStringVa, EalPerfBeginTaskWithPayload, EalPerfEndTaskWithPayload, EalPerfAddEventWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfData or EalPerfVariables to pass strings")
EAL_DLL_ENTRY const char* EalPerfConvertDynamicString(const char* pDynamicString);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \deprecated DEPRECATED since version 5.00 (there is no replacement), will be removed in version 8.00
    For associations inbetween eal_ids (track names), have a look at the EalLog API
    Provide information for a specific track.
    \param  TrackId             A unique track ID. See ealdef.h for details on how to define IDs.
    \param  pTrackName          A static string containing the name of the track. Cannot be NULL.

    \PRECONDITIONS
    - Multithread safe call
    - This function can be called only once per track.
    - It can be called at any time, even after logging on this track using EalPerf logging functions.

    \sa EalPerfLogEndEvent2
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY void EalPerfSetTrackInfo(eal_id TrackId, const char* pTrackName);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Formats and registers a temporary string for use as a custom string in
    EalPerf*WithPayload functions.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfData or EalPerfVariables to pass strings), will be removed in version 8.00
    Unlike strings returned from \ref EalPerfConvertDynamicString, the
    returned string pointer becomes invalid every time the log is flushed on
    the implementer's side, so this function needs to be called every time
    before calling the EalPerf*WithPayload function with a temporary string.
    This is intended for debugging complex cases where the formatted string
    may vary a lot and cannot be accumulated indefinitely. A performance hit
    is expected so do not use temporary strings in your default operating mode.

    \param pFormat      The printf-style format string for the temporary string.
    \param Args         The arguments that match the format string.
    \return             A pointer on a string that is safe to be passed to EalPerf*WithPayload functions.

    \PRECONDITIONS
    - Multithread safe call

    \POSTCONDITIONS
    - The returned pointer can only be used within the scope where the function is called.

    \sa EalPerfConvertDynamicString, EalPerfBeginTaskWithPayload, EalPerfEndTaskWithPayload, EalPerfAddEventWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfData or EalPerfVariables to pass strings")
EAL_DLL_ENTRY const char* EalPerfConvertTemporaryStringVa(const char* pFormat, va_list Args);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Begins a task interval in the performance log.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfTaskBegin instead), will be removed in version 8.00

    \param Point            The static profile point where the task started.
    \param Time             The value of the hardware counter when the task started (in "cycles").

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.

    \POSTCONDITIONS
    - Must be followed symmetrically with a call to \ref EalPerfEndTask or \ref EalPerfEndTaskWithPayload using a static point with the same track and static caption pointer.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa, EalPerfEndTask, EalPerfEndTaskWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfTaskBegin instead")
EAL_DLL_ENTRY void EalPerfBeginTask(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Begins a task interval in the performance log, with a custom payload.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfTaskBegin with EalPerfData or EalPerfVariables instead), will be removed in version 8.00

    \param Point            The static profile point where the task started.
    \param Time             The value of the hardware counter when the task started (in "cycles").
    \param pCustomString    An optional custom string that will be passed in the performance log. Can be NULL.
                            Must remain valid until the log is flushed (which can be unsafe),
                            or be the result of \ref EalPerfConvertDynamicString or \ref EalPerfConvertTemporaryStringVa (both of which guarantees validity).
    \param pPayload         Optional data to be copied in the performance log. Can be NULL.
                            Because it will be copied, the data does not have to be persistent.
    \param PayloadSize      Size of the payload. If payload is NULL, must be 0.

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.
    - The data pointed to by 'payload' must remain stable and valid until the call returns.

    \POSTCONDITIONS
    - Must be followed symmetrically with a call to \ref EalPerfEndTask or \ref EalPerfEndTaskWithPayload using a static point with the same track and static caption pointer.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa, EalPerfEndTask, EalPerfEndTaskWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfTaskBegin with EalPerfData or EalPerfVariables instead")
EAL_DLL_ENTRY void EalPerfBeginTaskWithPayload( const eal_perf_static_point&    Point,
                                                eal_u64                         Time,
                                                const char*                     pCustomString,
                                                const void*                     pPayload,
                                                eal_size_t                      PayloadSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Ends a task interval in the performance log.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfTaskEnd instead), will be removed in version 8.00

    \param Point            The static profile point where the task ended.
    \param Time             The value of the hardware counter when the task ended (in "cycles").

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.
    - Must be preceded symmetrically with a call to \ref EalPerfBeginTask or \ref EalPerfBeginTaskWithPayload using a static point with the same track and static caption pointer.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa, EalPerfBeginTask, EalPerfBeginTaskWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfTaskEnd instead")
EAL_DLL_ENTRY void EalPerfEndTask(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Ends a task interval in the performance log, with a custom payload.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfTaskEnd instead), will be removed in version 8.00

    \param Point            The static profile point where the task ended.
    \param Time             The value of the hardware counter when the task ended (in "cycles").
    \param pCustomString    An optional custom string that will be passed in the performance log. Can be NULL.
                            Must remain valid until the log is flushed (which can be unsafe),
                            or be the result of \ref EalPerfConvertDynamicString or \ref EalPerfConvertTemporaryStringVa (both of which guarantees validity).
    \param pPayload         Extra data to be copied in the performance log. It must be copied because it can be dynamic. Can be NULL.
    \param PayloadSize      Size of the payload. If payload is NULL, must be 0.

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.
    - Must be preceded symmetrically with a call to \ref EalPerfBeginTask or \ref EalPerfBeginTaskWithPayload using a static point with the same track and static caption pointer.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa, EalPerfBeginTask, EalPerfBeginTaskWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfTaskEnd instead")
EAL_DLL_ENTRY void EalPerfEndTaskWithPayload(   const eal_perf_static_point&    Point,
                                                eal_u64                         Time,
                                                const char*                     pCustomString,
                                                const void*                     pPayload,
                                                eal_size_t                      PayloadSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Adds a discrete event to the performance log.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfEvent instead), will be removed in version 8.00
    A discrete event is useful to indicate when a certain point is reached in
    the code (e.g. entering/ending a wait, entering a certain condition).

    \param Point            The static profile point where the event occurred.
    \param Time             The value of the hardware counter when the event occurred (in "cycles").
    \param Param            An optional 64-bit signed value associated with the event.

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa, EalPerfAddEventWithPayload
*/
EAL_DEPRECATED("Deprecated; use EalPerfEvent instead")
EAL_DLL_ENTRY void EalPerfAddEvent( const eal_perf_static_point&    Point,
                                    eal_u64                         Time,
                                    eal_s64                         Param);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Adds a discrete event to the performance log, with a custom payload.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfEvent with EalPerfData or EalPerfVariables instead), will be removed in version 8.00
    A discrete event is useful to indicate when a certain point is reached in
    the code (e.g. entering/ending a wait, entering a certain condition).

    \param Point            The static profile point where the event occurred.
    \param Time             The value of the hardware counter when the event occurred (in "cycles").
    \param Param            An optional 64-bit signed value associated with the event.
    \param pCustomString    An optional custom string that will be passed in the performance log. Can be NULL.
                            Must remain valid until the log is flushed (which can be unsafe),
                            or be the result of \ref EalPerfConvertDynamicString or \ref EalPerfConvertTemporaryStringVa (both of which guarantees validity).
    \param pPayload         Extra data to be copied in the performance log. It must be copied because it can be dynamic. Can be NULL.
    \param PayloadSize      Size of the payload. If payload is NULL, must be 0.

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa, EalPerfAddEvent
*/
EAL_DEPRECATED("Deprecated; use EalPerfEvent with EalPerfData or EalPerfVariables instead")
EAL_DLL_ENTRY void EalPerfAddEventWithPayload(  const eal_perf_static_point&    Point,
                                                eal_u64                         Time,
                                                eal_s64                         Param,
                                                const char*                     pCustomString,
                                                const void*                     pPayload,
                                                eal_size_t                      PayloadSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Adds a warning count to the performance log.
    \deprecated DEPRECATED since version 4.00 (use \ref EalPerfWarningCount instead), will be removed in version 8.00
    A warning count can be used to indicate that a quantity (e.g. time, memory,
    number of instances) has reached/exceeded a soft limit.

    \param Point            The static profile point where the event occurred.
    \param Time             The value of the hardware counter when the warning occurred (in "cycles").
    \param Count            The warning count value. Can be negative.

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfConvertDynamicString, EalPerfConvertTemporaryStringVa
*/
EAL_DEPRECATED("Deprecated; use EalPerfWarningCount instead")
EAL_DLL_ENTRY void EalPerfAddWarningCount(const eal_perf_static_point&  Point,
                                          eal_u64                       Time,
                                          eal_s64                       Count);

////////////////////////////////////////////////////////////////////////////////////////////////////
/*!  Provide thread name.

    \param  ThreadID    The system thread ID
    \param  pThreadName A valid string containing the thread name. Does not need to be static.

    \PRECONDITIONS
    - Multithread safe call
*/
EAL_DLL_ENTRY void EalPerfLogThreadName(eal_u64 ThreadID, const char* pThreadName);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Begins a task interval in the performance log.

    \param Point            The static profile point where the task started.
    \param Time             The value of the hardware counter when the task started (in "cycles").
    \return                 true if the task has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.

    \POSTCONDITIONS
    - Must be followed symmetrically with a call to \ref EalPerfTaskEnd using a static point with the same track and static caption pointer.

    \sa EalPerfTaskEnd, EalPerfTaskScope, EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfTaskBegin(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Ends a task interval in the performance log.

    \param Point            The static profile point where the task ended.
    \param Time             The value of the hardware counter when the task ended (in "cycles").
    \return                 true if the task has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.
    - Must be preceded symmetrically with a call to \ref EalPerfTaskBegin using a static point with the same track and static caption pointer.

    \sa EalPerfTaskBegin, EalPerfTaskScope, EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfTaskEnd(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Record a task interval in the performance log.

    \param Point            The static profile point describing the task.
    \param BeginTime        The value of the hardware counter when the task started (in "cycles").
    \param EndTime          The value of the hardware counter when the task ended (in "cycles").
    \return                 true if the task has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfTaskBegin, EalPerfTaskEnd, EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfTaskScope(const eal_perf_static_point& Point, eal_u64 BeginTime, eal_u64 EndTime);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Record an event in the performance log.

    \param Point            The static profile point where the event occured.
    \param Time             The value of the hardware counter when the task started (in "cycles").
    \return                 true if the event has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfEvent(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Begins a wait interval in the performance log.

    \param Point            The static profile point where the wait started.
    \param Time             The value of the hardware counter when the wait started (in "cycles").
    \return                 true if the wait has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.

    \POSTCONDITIONS
    - Must be followed symmetrically with a call to \ref EalPerfWaitEnd using a static point with the same track and static caption pointer.

    \sa EalPerfWaitEnd, EalPerfWaitScope, EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfWaitBegin(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Ends a wait interval in the performance log.

    \param Point            The static profile point where the wait ended.
    \param Time             The value of the hardware counter when the wait ended (in "cycles").
    \return                 true if the wait has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.
    - Must be preceded symmetrically with a call to \ref EalPerfWaitBegin using a static point with the same track and static caption pointer.

    \sa EalPerfWaitBegin, EalPerfWaitScope, EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfWaitEnd(const eal_perf_static_point& Point, eal_u64 Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Records a wait interval in the performance log.

    \param Point            The static profile point describing the wait.
    \param BeginTime        The value of the hardware counter when the wait started (in "cycles").
    \param EndTime          The value of the hardware counter when the wait ended (in "cycles").
    \return                 true if the wait has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'point' follow the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfWaitBegin, EalPerfWaitEnd, EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfWaitScope(const eal_perf_static_point& Point, eal_u64 BeginTime, eal_u64 EndTime);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \deprecated DEPRECATED since version 9.00 (use \ref EalPerfLockScope2 instead), will be removed in version 12.00
    Records a lock scope in the performance log.
    Three times are used for a lock scope:
    - LockProfileInfo.StartWaitTime --> When the lock is requested (thread starts waiting)
    - LockProfileInfo.EndWaitTime   --> When the lock is acquired (thread stops waiting)
    - UnlockTime                    --> When the lock is released

    \param LockProfileInfo  The \ref eal_perf_lock_profile_info instance describing the lock and its wait times. Unlike \ref eal_perf_static_point, this structure is intended to be copied and can be non-static.
    \param LockType         The type of the lock; see \ref eal_perf_lock_type.
    \param UnlockTime       The value of the hardware counter when the lock is released (in "cycles").
    \return                 true if the lock scope has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
*/
EAL_DEPRECATED("Deprecated; use EalPerfLockScope2 instead")
EAL_DLL_ENTRY bool EalPerfLockScope(const eal_perf_lock_profile_info& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Records a lock scope in the performance log.
    Three times are used for a lock scope:
    - LockProfileInfo.StartWaitTime --> When the lock is requested (thread starts waiting)
    - LockProfileInfo.EndWaitTime   --> When the lock is acquired (thread stops waiting)
    - UnlockTime                    --> When the lock is released

    \param LockProfileInfo  The \ref eal_perf_lock_profile_info instance describing the lock and its wait times. Unlike \ref eal_perf_static_point, this structure is intended to be copied and can be non-static.
    \param LockType         The type of the lock; see \ref eal_perf_lock_type.
    \param UnlockTime       The value of the hardware counter when the lock is released (in "cycles").
    \return                 true if the lock scope has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
*/
EAL_DLL_ENTRY bool EalPerfLockScope2(const eal_perf_lock_profile_info2& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Records a lock scope in the performance log.
    Three times are used for a lock scope:
    - LockProfileInfo.StartWaitTime --> When the lock is requested (thread starts waiting)
    - LockProfileInfo.EndWaitTime   --> When the lock is acquired (thread stops waiting)
    - UnlockTime                    --> When the lock is released

    \param LockProfileInfo  The \ref eal_perf_lock_profile_info instance describing the lock and its wait times. Unlike \ref eal_perf_static_point, this structure is intended to be copied and can be non-static.
    \param LockType         The type of the lock; see \ref eal_perf_lock_type.
    \param UnlockTime       The value of the hardware counter when the lock is released (in "cycles").
    \param WaitState        If true (non-null), this indicates the lock caused contention with other threads, i.e. a waiting state was detected for at least one thread that shares this lock.
                            This is useful to only log lock scopes that caused a waiting state in order to reduce logs' size.
    \return                 true if the lock scope has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
*/
EAL_DLL_ENTRY bool EalPerfLockScope2WithWaitState(const eal_perf_lock_profile_info2& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime, eal_u32 WaitState);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Adds a warning count to the performance log.
    A warning count can be used to indicate that a quantity (e.g. time, memory,
    number of instances) has reached/exceeded a soft limit.

    \param Point            The static profile point where the event occurred.
    \param Time             The value of the hardware counter when the warning occurred (in "cycles").
    \param Count            The warning count value. Can be negative.
    \return                 true if the warning has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.

    \sa EalPerfData, EalPerfVariables
*/
EAL_DLL_ENTRY bool EalPerfWarningCount(const eal_perf_static_point& Point, eal_u64 Time, eal_s64 Count);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Adds a low frequency counter value to the performance log (64-bit unsigned integer variant).
    This value can be used to track any metric, such as the quantity or the size of items, at a given time.
    As the name implies, low frequency counters are expected to be called infrequently (still, this can be more than once per frame).

    \param Point            The static profile point where the value is updated.
    \param Time             The value of the hardware counter when the value is updated (in "cycles").
    \param Count            The low frequency counter value.
    \return                 true if the warning has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.

    \sa eal_perf_static_point
*/
EAL_DLL_ENTRY bool EalPerfLowFrequencyCounterU64(const eal_perf_static_point& Point, eal_u64 Time, eal_u64 Value);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Adds a low frequency counter value to the performance log (64-bit floating point variant).
    This value can be used to track any metric, such as a ratio or a distance, at a given time.
    As the name implies, low frequency counters are expected to be called infrequently (still, this can be more than once per frame).

    \param Point            The static profile point where the value is updated.
    \param Time             The value of the hardware counter when the value is updated (in "cycles").
    \param Count            The low frequency counter value.
    \return                 true if the warning has been logged (false when log filtered)

    \PRECONDITIONS
    - Multithread safe call
    - Data in 'Point' follows the conditions explained in \ref eal_perf_static_point.

    \sa eal_perf_static_point
*/
EAL_DLL_ENTRY bool EalPerfLowFrequencyCounterF64(const eal_perf_static_point& Point, eal_u64 Time, double Value);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Append additional data to the last logged event.
    This function is used to provide additional data to the last event logged by the same thread

    \param TrackId          The track identifier (see EalPerfSetTrackInfo()).
    \param pData            Data to be stored with the event. Cannot be NULL. This data will
                            be copied and thus can be modified or freed after this call.
    \param DataSize         Size of the data to store (in bytes).
    \return                 true if the data has been logged (false when log filtered)

    \sa EalPerfTaskBegin, EalPerfTaskScope, EalPerfWaitBegin, EalPerfWaitScope, EalPerfEvent, EalPerfWarningCount, EalPerfSetTrackInfo
*/
EAL_DLL_ENTRY bool EalPerfData(eal_id TrackId, const void* pData, eal_u32 DataSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Provides variadic parameters for a profile points with a printf-style caption.
    This function to provides parameter to put in the caption of an event/task/warning.
    This function must be called right after the task/wait/event/warning log, in the same thread
    context.

    \param Point            The static profile point, containing a context format
    \param Args             All the variables corresponding to the context format.
    \return                 true if the variables have been logged (false when log filtered)

    \sa EalPerfTaskBegin, EalPerfTaskScope, EalPerfWaitBegin, EalPerfWaitScope, EalPerfEvent, EalPerfWarningCount
*/
EAL_DLL_ENTRY bool EalPerfVariables(  const eal_perf_static_point& Point, va_list Args);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Callback invoked when it's time to write metadata.
    Your callback is multi-thread protected, and will be called by only one thread at the time. So
    it is allowed to use shared ressources inside your callback.

    \param pCookie  Value provided when registering your callback in EalPerfRegisterMetaDataProvider.
*/
typedef void (*EalPerfMetaDataProviderCallback)(void* pCookie);

enum eal_perf_metadata_flags
{
    EAL_PERF_METADATA_BEGIN     = 0x00000001,       ///< MetaData will be placed in front of the capture
    EAL_PERF_METADATA_END       = 0x00000002,       ///< MetaData will be placed at the end of the capture
    EAL_PERF_METADATA_DUMP      = 0x00000004,       ///< Metadata placed in "snapshot" mode
    EAL_PERF_METADATA_STREAM    = 0x00000008,       ///< Metadata sent in streaming mode
    EAL_PERF_METADATA_DEFAULT   = EAL_PERF_METADATA_BEGIN | EAL_PERF_METADATA_DUMP | EAL_PERF_METADATA_STREAM
};

/*! Let the caller connect a callback function, that will be called when a dump or stream
    is started/ending. This is the optimal moment to write metadata in a file/stream.

    \param ProviderCallback     A pointer on the callback function to invoke
    \param pCookie              A user provided pointer that will be returned in ProviderCallback
    \param Flags                Flags used to control when the callback will be called.
                                Use Begin and/or End, to indicate where you want the metadata in a dump/stream
                                Use Dump And/Or Stream top indicate on which action you want to add metadata
    \return                     true when the callback has been registered with success. In that case you
                                need to call EalPerfUnregisterMetaDataProvider, at shutdown time.


    \sa EalPerfUnregisterMetaDataProvider, EalPerfLogMetaData
*/
EAL_DLL_ENTRY bool EalPerfRegisterMetaDataProvider( EalPerfMetaDataProviderCallback ProviderCallback,
                                                    void*                           pCookie,
                                                    eal_perf_metadata_flags         Flags = EAL_PERF_METADATA_DEFAULT);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Disconnect a previously registered metadata callback

    \param ProviderCallback     Same function pointer that has been provided in your call to
                                EalPerfRegisterMetaDataProvider
    \return                     true when the callback has been unregistered with success

    \sa EalPerfRegisterMetaDataProvider
*/
EAL_DLL_ENTRY bool EalPerfUnregisterMetaDataProvider(EalPerfMetaDataProviderCallback ProviderCallback);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Log a single chunk of MetaData.

    \param Point        The static profile point, with the metadata key as caption.
    \param pMetaData    A pointer on the chunk of data to write.
    \param MetaDataSize The metadata size
    \return             true when the metadata has been logged with success (false when filtered)

    \sa EalPerfLogMetaDataBlockBegin, EalPerfLogMetaDataBlock
*/
EAL_DLL_ENTRY bool EalPerfLogMetaData(const eal_perf_static_point&  Point,
                                        const void*                 pMetaData,
                                        eal_u32                     MetaDataSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Begin a sequence of metadata logs. Use this set of functions when your metadata is scattered
    in memory, but you need to log it as a single chunk.
    The begin-block-end sequence must be called in the same thread, in sequence.

    \param Point    The static profile point, with the metadata key as caption.
    \return         true when the metadata has been logged with success (false when filtered)

    \sa EalPerfLogMetaDataBlock, EalPerfLogMetaDataBlockEnd
*/
EAL_DLL_ENTRY bool EalPerfLogMetaDataBlockBegin(const eal_perf_static_point& Point);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Log a chunk of scattered MetaData.
    The begin-block-end sequence must be called in the same thread, in sequence.

    \param Point        The static profile point, with the metadata key as caption.
    \param pMetaData    A pointer on the chunk of data to write.
    \param MetaDataSize The metadata size
    \return             true when the metadata has been logged with success (false when filtered)

    \sa EalPerfLogMetaDataBlockBegin, EalPerfLogMetaDataBlockEnd
*/
EAL_DLL_ENTRY bool EalPerfLogMetaDataBlock( const eal_perf_static_point&    Point,
                                            const void*                     pMetaData,
                                            eal_u32                         MetaDataSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Mark the end of a sequence of scattered metadata.
    The begin-block-end sequence must be called in the same thread, in sequence.

    \param Point    The static profile point, with the metadata key as caption.
    \return         true when the metadata has been logged with success (false when filtered)

    \sa EalPerfLogMetaDataBlockBegin, EalPerfLogMetaDataBlock
*/
EAL_DLL_ENTRY bool EalPerfLogMetaDataBlockEnd(const eal_perf_static_point& Point);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Return the current frame number
    \return Current frame number, as determined by the game engine

    \sa EalPerfGetCurrentFrameBeginTime
*/
EAL_DLL_ENTRY uint64_t EalPerfGetCurrentFrameNo();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Return the current frame start time
    \return Current frame starting time (see "Performance Time Unit" for time format)

    \sa EalPerfGetCurrentFrameNo
*/
EAL_DLL_ENTRY uint64_t EalPerfGetCurrentFrameBeginTime();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Declare that the logged information for a specific TrackID, should be logged in a independant,
    detached timeline than the data logged in the current thread.
    The best example to picture this, is data logged for the GPU. The events/tasks are run on
    another track or context, and should not be mixed with events/tasks happening on the CPU who
    just happen to be logging the log information on behalf of the GPU.

    \param The Track ID to display outside its other thread's tasks
*/
EAL_DLL_ENTRY void EalPerfDetachTrackFromThread(eal_id TrackId);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Notify the engine that an OS synchronization primitive was created.
    This allows the engine to take into account such primitives created from middleware products.
    This tracking is necessary due to the synchronization primitive limit on SCE platforms.
    Must be called after the primitive was created.

    \param Tag          The tag identifying the origin of the call (see ealdef.h). Should at least be a product ID.
    \param Type         The type of synchronization primitive being created.
    \param Name         The name of the synchronization primitive, if available. Can be null.
    \param InstanceId   A unique identifier for the synchronization primitive, usually the handle or address. Must be unique; can also be 0.

    \sa EalPerfTrackSyncPrimitiveDestroy, eal_perf_sync_primitive_type
*/
EAL_DLL_ENTRY void EalPerfTrackSyncPrimitiveCreate(eal_id Tag, eal_perf_sync_primitive_type Type, const char* Name, eal_uintptr_t InstanceId);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Notify the engine that an OS synchronization primitive is being destroyed.
    Must be called before the primitive is destroyed.

    \param Tag          The tag identifying the origin of the call (see ealdef.h). Should at least be a product ID.
    \param Type         The type of synchronization primitive being destroyed.
    \param Name         The name of the synchronization primitive, if available. Can be null.
    \param InstanceId   A unique identifier for the synchronization primitive, usually the handle or address. Must match what was passed at creation.

    \sa EalPerfTrackSyncPrimitiveCreate, eal_perf_sync_primitive_type
*/
EAL_DLL_ENTRY void EalPerfTrackSyncPrimitiveDestroy(eal_id Tag, eal_perf_sync_primitive_type Type, const char* Name, eal_uintptr_t InstanceId);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL Perf functions.
struct eal_perf_dll_interface
{
    bool (*pEalPerfLibInit)(eal_u32 Version);
    void (*pEalPerfLibShutdown)();

    // BEGIN DEPRECATED in V500
    void (*pEalPerfSetTrackInfo)(               eal_id TrackId,
                                                const char* pTrackName);
    // END DEPRECATED

    bool (*pEalPerfRegisterMetaDataProvider)(   EalPerfMetaDataProviderCallback providerCallback,
                                                void*                           pCookie,
                                                eal_perf_metadata_flags         Flags);

    bool (*pEalPerfUnregisterMetaDataProvider)( EalPerfMetaDataProviderCallback providerCallback);

    bool (*pEalPerfLogMetaData)(                const eal_perf_static_point&    Point,
                                                const void*                     pMetaData,
                                                eal_u32                         MetaDataSize);
    bool (*pEalPerfLogMetaDataBlockBegin)(      const eal_perf_static_point&    Point);
    bool (*pEalPerfLogMetaDataBlock)(           const eal_perf_static_point&    Point,
                                                const void*                     pMetaData,
                                                eal_u32                         MetaDataSize);
    bool (*pEalPerfLogMetaDataBlockEnd)(        const eal_perf_static_point&    Point);

    // BEGIN DEPRECATED in V400
    const char* (*pEalPerfConvertDynamicString)(const char* pDynamicString);
    // END DEPRECATED

    uint64_t (*pEalPerfGetCurrentFrameNo)();
    uint64_t (*pEalPerfGetCurrentFrameBeginTime)();
    void (*pEalPerfDetachTrackFromThread)(eal_id TrackId);

    // BEGIN DEPRECATED in v300
    void        *pDEPRECATEDEalPerfLogRangeEvent2;
    void        *pDEPRECATEDEalPerfLogEventData2;
    // END DEPRECATED

    // BEGIN DEPRECATED in V400
    const char* (*pEalPerfConvertTemporaryStringVa)(const char* pFormat, va_list Args);
    void (*pEalPerfBeginTask)(const eal_perf_static_point& Point, eal_u64 Time);
    void (*pEalPerfBeginTaskWithPayload)(const eal_perf_static_point& Point, eal_u64 Time, const char* pCustomString, const void* pPayload, eal_size_t PayloadSize);
    void (*pEalPerfEndTask)(const eal_perf_static_point& Point, eal_u64 Time);
    void (*pEalPerfEndTaskWithPayload)(const eal_perf_static_point& Point, eal_u64 Time, const char* pCustomString, const void* pPayload, eal_size_t PayloadSize);
    void (*pEalPerfAddEvent)(const eal_perf_static_point& Point, eal_u64 Time, eal_s64 Param);
    void (*pEalPerfAddEventWithPayload)(const eal_perf_static_point& Point, eal_u64 Time, eal_s64 Param, const char* pCustomString, const void* pPayload, eal_size_t PayloadSize);
    void (*pEalPerfAddWarningCount)(const eal_perf_static_point& Point, eal_u64 Time, eal_s64 Count);
    // END DEPRECATED

    void (*pEalPerfLogThreadName)(eal_u64 ThreadID, const char* pThreadName);
    bool (*pEalPerfTaskBegin)(const eal_perf_static_point& Point, eal_u64 Time);
    bool (*pEalPerfTaskEnd)(const eal_perf_static_point& Point, eal_u64 Time);
    bool (*pEalPerfTaskScope)(const eal_perf_static_point& Point, eal_u64 BeginTime, eal_u64 EndTime);
    bool (*pEalPerfEvent)(const eal_perf_static_point& Point, eal_u64 Time);
    bool (*pEalPerfWarningCount)(const eal_perf_static_point& Point, eal_u64 Time, eal_s64 Count);
    bool (*pEalPerfData)(eal_id TrackId, const void* pData, eal_u32 DataSize);
    bool (*pEalPerfVariables)(  const eal_perf_static_point& Point, va_list Args);
    bool (*pEalPerfWaitBegin)(const eal_perf_static_point& Point, eal_u64 Time);
    bool (*pEalPerfWaitEnd)(const eal_perf_static_point& Point, eal_u64 Time);
    bool (*pEalPerfWaitScope)(const eal_perf_static_point& Point, eal_u64 BeginTime, eal_u64 EndTime);
    bool (*pEalPerfLowFrequencyCounterU64)(const eal_perf_static_point& Point, eal_u64 Time, eal_u64 Value);
    bool (*pEalPerfLowFrequencyCounterF64)(const eal_perf_static_point& Point, eal_u64 Time, double Value);

    // BEGIN DEPRECATED in V900
    bool (*pEalPerfLockScope)(const eal_perf_lock_profile_info& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime);
    // END DEPRECATED

    bool (*pEalPerfLockScope2)(const eal_perf_lock_profile_info2& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime);
    bool (*pEalPerfLockScope2WithWaitState)(const eal_perf_lock_profile_info2& LockProfileInfo, eal_perf_lock_type LockType, eal_u64 UnlockTime, eal_u32 WaitState);

    void (*pEalPerfTrackSyncPrimitiveCreate)(eal_id Tag, eal_perf_sync_primitive_type Type, const char* Name, eal_uintptr_t InstanceId);
    void (*pEalPerfTrackSyncPrimitiveDestroy)(eal_id Tag, eal_perf_sync_primitive_type Type, const char* Name, eal_uintptr_t InstanceId);
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initialization code and must be done before any
    call to any EAL functions.
    \param Interface    A structure containing valid pointers to the engine EAL functions.
    \sa EalPerfDllPopulateInterface
*/
void EalPerfDllInitInterface(const eal_perf_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealperfdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamically connect its internal EAL
    function calls to the one in your engine.
    \param Interface    A structure that will be filled with pointers to local EAL functions.
    \sa EalPerfDllInitInterface
*/
void EalPerfDllPopulateInterface(eal_perf_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
