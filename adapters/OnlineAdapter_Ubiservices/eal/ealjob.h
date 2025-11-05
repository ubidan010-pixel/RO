////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealjob.h  Technology Group Job Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ealdef.h"
/// This module covers the job reference API.
/*! \addtogroup Job
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210)
/// See ealdef.h for more information.
#define EAL_JOB_VERSION         303

////////////////////////////////////////////////////////////////////////////////////////////////////

EAL_DEPRECATED("Deprecated; there is no replacement") typedef void* eal_job_id; ///< Job Id \deprecated DEPRECATED since version 3.01. There is no replacement.

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Job priority
/// \deprecated DEPRECATED since version 2.00. There is no replacement.
enum EAL_DEPRECATED("Deprecated; there is no replacement") eal_job_priority
{
    EAL_JOB_PRIORITY_LOWEST  = 0,           ///< Lowest priority
    EAL_JOB_PRIORITY_NORMAL  = 127,         ///< Normal priority
    EAL_JOB_PRIORITY_HIGHEST = 255          ///< Highest priority
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Job flags
/// \deprecated DEPRECATED since version 3.01. There is no replacement.
enum EAL_DEPRECATED("Deprecated; there is no replacement") eal_job_flags
{
    EAL_JOB_REAL_TIME         = 0x00000001,     ///< Job with this flag should be executed ASAP.
    EAL_JOB_MAY_BLOCK         = 0x00000002      ///< Job with this flag may waste CPU blocking.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Structure used to describe a job to execute. 
/// \deprecated DEPRECATED since version 2.00. There is no replacement.
struct EAL_DEPRECATED("Deprecated; there is no replacement") eal_job_descriptor
{
    eal_id          Tag;                    ///< The tag identifying the origin of the call. See ealdef.h.
    void            (*pFunction)(void*);    ///< A pointer to a static function.
    void*           pCookie;                ///< A pointer that will be passed back to the function when called.
    eal_u32         Priority;               ///< Priority of this job, should be an integer between
                                            ///< \ref EAL_JOB_PRIORITY_LOWEST and \ref EAL_JOB_PRIORITY_HIGHEST.
                                            ///< \sa eal_job_priority.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Structure with a list of options to be passed as arguments.
/// \deprecated DEPRECATED since version 3.01. There is no replacement.
struct EAL_DEPRECATED("Deprecated; there is no replacement") eal_job_options
{
    eal_u32                 Flags;          ///< See \ref eal_job_flags.
    eal_u32                 TimerUs;        ///< A minimal delay in microseconds before starting the task
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Initialize the library.
    \return true if initialization succeeded

    \param Version      Must be equal to current version = \ref EAL_JOB_VERSION

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EaljobLibInit(), so
      EalJobLibShutdown() is called the exact number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initalized and all APIs call will be allowed.
    - EalJobLibShutdown() must be called even if false is returned.

    \sa EalJobLibShutdown
*/
EAL_DLL_ENTRY bool EalJobLibInit(eal_u32 Version = EAL_JOB_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalJobLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalJobLibShutdown().

    \sa EalJobLibInit
*/
EAL_DLL_ENTRY void EalJobLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Create a new job.
    \deprecated DEPRECATED since version 2.00. There is no replacement.
    \param pJobDescriptor   A pointer to a valid job descriptor. *** See preconditions.

    \PRECONDITIONS
    - The job is expected to be for a short period of time. Typically, it should not last more
      than a couple of seconds (do not create a job for the entire game; use threads in that case).
    - The Job descriptor must stay in memory and not be modified until the job callback is called.

    \POSTCONDITIONS
    - The Job/thread will be destroyed when it returns from pFunction.
    - Job creation cannot fail. EalJobCreate() should block until necessary resources are
      available to queue the requested job.

    \sa EalJobDebugCreate
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY void EalJobCreate(const eal_job_descriptor* pJobDescriptor);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Create a new job (V2).
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    The goal of this function is twofold; It creates a task (but doesn not start it), and reserve
    memory for the caller.

    \param Tag              The tag identifying the origin of the call. See ealdef.h.
    \param ExtraMemorySize  The memory space allocated for the caller data.
    \param pFunction        A pointer to a static function that will be called when task starts.
    \param pOptions         Optional options. See eal_job_options for details.
    \return A valid job ID, which is also a pointer on the memory allocated for the caller.

    \POSTCONDITIONS
    - The job/thread will be destroyed when it returns from pFunction AND EalJobFree is called.
    - Job creation cannot fail. EalJobAllocate() should block until necessary resources are
      available to queue the requested job.
    - The job is not started yet. You must call EalJobStart to start it.

    \sa EalJobDebugAllocate, EalJobStart, EalJobFree
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY eal_job_id EalJobAllocate(eal_id              Tag,
                                        eal_u32             ExtraMemorySize,
                                        void                (*pFunction)(eal_job_id),
                                        eal_job_options*    pOptions);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Start a job previously created with EalJobAllocate.
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    \param JobId        A job identifier returned by EalJobAllocate

    \PRECONDITIONS
    - Should only be called once.

    \sa EalJobAllocate, EalJobCompleted
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY void EalJobStart(eal_job_id JobId);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Release a job previously created with EalJobAllocate.
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    If this function is called before EalJobStart, the job will be destroyed.
    If called AFTER EalJobStart, the job will destroyed itself as soon as it returns from its
    pFunction.

    \param JobId        A Job identifier returned by EalJobAllocate.

    \sa EalJobAllocate
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY void EalJobFree(eal_job_id JobId);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Cancel a scheduled job.
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    The cancel function is done on a best effort basis, and has no garanteed of succeeding.
    If the underlying code does not support cancels OR the job is already executing, False should
    be returned.
    When false is returned, the caller should assume that its pFunction will be called.

    \param JobId        A job identifier returned by EalJobAllocate
    \return True if cancel was possible, false otherwise

    \sa EalJobAllocate, EalJobStart
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY bool EalJobCancel(eal_job_id JobId);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Verify if a list of jobs completed execution.
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    \param JobIdTable   A pointer on a list of job IDs returned by EalJobAllocate.
    \param Count        The size of JobIdTable.
    \return True if all jobs completed.

    \PRECONDITIONS
    - JobIdTable must be a valid value.
    - Count must be greater than 0.

    \sa EalJobAllocate, EalJobWaitForJob
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY bool EalJobCompleted(eal_job_id* JobIdTable, eal_u32 Count);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Wait for a list of jobs to complete execution.
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    \param JobIdTable   A pointer on a list of Job IDs returned by EalJobAllocate.
    \param Count        The size of JobIdTable.

    \PRECONDITIONS
    - JobIdTable must be a valid value.
    - Count must be greater than 0.

    \sa EalJobGroupCreate
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY void EalJobWaitForJob(eal_job_id* JobIdTable, eal_u32 Count);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Return the current Job in execution.
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    \return The current Job ID.

    \PRECONDITIONS
    - Can only be called by a Job currently executing.
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY eal_job_id EalJobGetCurrentJob();

////////////////////////////////////////////////////////////////////////////////////////////////////

//  Return the optimal number of jobs to create in order to process a large set of micro tasks.
//  \deprecated DEPRECATED since version 3.00. There is no replacement.
/*
    Typical usage for this function is to minimize the number of jobs created when procesing a
    large amount of micro tasks in a parralel_for for example. It may be better to create 1 job
    per core fore example, each fo them processing a batch of jobs.

    \param Tag      The tag identifying the origin of the call. See ealdef.h.
    \param Count    The number of tasks to process.
    \return The number of jobs to create.
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY eal_u32 EalParallelizeLargeItemCount(eal_id Tag, eal_u32 Count);


////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FRIENDLY FUNCTIONS
// All the following functions are debug friendly. You should \b not use them when compiling in retail.
// The supplier on the other side must provide an implementation for all targets since it may happen
// that some clients are compiled in debug while some others are compiled in retail. So they must be
// supported at all times. In the case where the supplier is in Retail, the debug info should simply
// be discarded. In retail, the client side should avoid using these functions because carrying the
// extra parameters adds useless overhead on function calls.
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Create a new job.
    \deprecated DEPRECATED since version 2.00. There is no replacement.
    \param pJobDescriptor   A pointer to a valid job descriptor. *** See preconditions.
    \param pFilename        A static string containing the file name from where the job is created.
    \param FileLine         The line number where the allocation the job is created.

    \PRECONDITIONS
    - The Job is expected to be for a short period of time. Typically, it should not last more
      than a couple of seconds (do not create a job for the entire game; use threads in that case).
    - The Job descriptor must stay in memory and not be modified until the job callback is called.

    \POSTCONDITIONS
    - The Job/thread will be destroyed when it returns from pFunction.
    - Job creation cannot fail. EalJobDebugCreate() should block until necessary resources are
      available to queue the requested job.

    \sa EalJobCreate
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY void EalJobDebugCreate(const eal_job_descriptor*  pJobDescriptor,
                                     const char*                pFilename,
                                     eal_u32                    FileLine);


////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Create a new job (V2).
    \deprecated DEPRECATED since version 3.01. There is no replacement.
    The goal of this function is twofold; It creates a task (but doesn not start it), and reserve
    memory for the caller.

    \param Tag              The tag identifying the origin of the call. See ealdef.h.
    \param ExtraMemorySize  The memory space allocated for the caller data.
    \param pFunction        A pointer to a static function that will be called when task starts.
    \param pOptions         Optional options. See eal_job_options for details.
    \param pFilename        A static string containing the file name from where the job is created
    \param FileLine         The line number where the allocation the job is created
    \return A valid job Id, which is also a pointer on the memory allocated for the caller.

    \POSTCONDITIONS
    - The Job/thread will be destroyed when it returns from pFunction AND EalJobFree is called.
    - Job creation cannot fail. EalJobAllocate() should block until necessary resources are
      available to queue the requested job.
    - The job is not started yet. You must call EalJobStart to start it.

    \sa EalJobAllocate, EalJobStart, EalJobFree
*/
EAL_DEPRECATED("Deprecated; there is no replacement")
EAL_DLL_ENTRY eal_job_id EalJobDebugAllocate(eal_id             Tag,
                                             eal_u32            ExtraMemorySize,
                                             void               (*pFunction)(eal_job_id),
                                             eal_job_options*   pOptions,
                                             const char*        pFilename,
                                             eal_u32            FileLine);


/*! Tell the engine to split batches of work on multiple threads.

    \RATIONALE               Batching is required to reduce the scheduling overhead of small work items.

                             By giving the batch splitting control over to the production, we don't have
                             to know about the number of threads or any other implementation details
                             from our side.

                             This function allows us to implement simple high performance constructs
                             like parallel_for and parallel_for_each without any implicit allocations.

                             Other constructs like task_group and parallel_invoke can also be implemented
                             over this single Eal function call.

                             The wait is also implicit in this function so the production can implement
                             the waiting mechanism it desires by joining processing or doing
                             some other useful work.

    \param Tag               The tag identifying the origin of the call. See ealdef.h.
    \param pFunction         The function to callback for each separate batch of items to process.
    \param Data              User data pointer passed back to the function.
    \param Count             The number of items to process.
    \param MinimalBatchSize  Provides a batch size under which multi-threading would introduce too much overhead.
                             A value of 0 will let the engine decide how to split work among threads.
                             A value < 0 is reserved for future use and is currently not supported.

    \POSTCONDITIONS
    - All items have been processed either in multiple threads or not.

*/
EAL_DLL_ENTRY void EalJobParallelize(eal_id Tag,
                                     void (* pFunction)(void * pData, eal_u64 Start, eal_u64 Count),
                                     void * pData,
                                     eal_u64 Count,
                                     eal_s32 MinimalBatchSize);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL Job functions.
struct eal_job_dll_interface
{
    bool        (*pEalJobLibInit)       (eal_u32                    Version);
    void        (*pEalJobLibShutdown)   ();

    // BEGIN DEPRECATED
    void        (*pEalJobCreate)        (const eal_job_descriptor*  pJobDescriptor);
    void        (*pEalJobDebugCreate)   (const eal_job_descriptor*  pJobDescriptor,
                                         const char*                pFilename,
                                         eal_u32                    FileLine);
    eal_job_id  (*pEalJobAllocate)              (eal_id                     Tag,
                                                eal_u32                     ExtraMemorySize,
                                                void                        (*pFunction)(eal_job_id),
                                                eal_job_options*            pOptions);
    void        (*pEalJobStart)                 (eal_job_id JobId);
    void        (*pEalJobFree)                  (eal_job_id                 JobId);
    bool        (*pEalJobCancel)                (eal_job_id                 JobId);
    bool        (*pEalJobCompleted)             (eal_job_id*                JobIdTable,
                                                eal_u32                     Count);
    void        (*pEalJobWaitForJob)            (eal_job_id*                JobIdTable,
                                                eal_u32                     Count);
    eal_job_id  (*pEalJobGetCurrentJob)         ();
    eal_u32     (*pEalParallelizeLargeItemCount)(eal_id                     Tag,
                                                eal_u32                     Count);
    eal_job_id  (*pEalJobDebugAllocate)         (eal_id                     Tag,
                                                eal_u32                     ExtraMemorySize,
                                                void                        (*pFunction)(eal_job_id),
                                                eal_job_options*            pOptions,
                                                const char*                 pFilename,
                                                eal_u32                     FileLine);
    // END DEPRECATED

    void        (*pEalJobParallelize)          (eal_id                      Tag,
                                               void                         (* pFunction)(void * pData, eal_u64 Start, eal_u64 Count),
                                               void *                       pData,
                                               eal_u64                      Count,
                                               eal_s32                      MinimalBatchSize);

};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initialization code and must be done before any
    call to any EAL function.
    \param Interface    Structure containing valid pointers to the engine EAL functions.
    \sa EalJobDllPopulateInterface
*/
void EalJobDllInitInterface(const eal_job_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so, it will dynamically connect it's internal Eal
    function calls to the one in your engine.
    \param Interface    A structure that will be filled up with pointers to local EAL functions.
    \sa EalJobDllInitInterface
*/
void EalJobDllPopulateInterface(eal_job_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
