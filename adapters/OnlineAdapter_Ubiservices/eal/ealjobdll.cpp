////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealjobdll.cpp  DLL wrapper implementation for the EAL (jobs).
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealjob.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL Side

static eal_job_dll_interface        gEalJobDllInterface = {};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobDllInitInterface(const eal_job_dll_interface& Interface)
{
    gEalJobDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalJobLibInit(eal_u32 Version)
{
    return (*gEalJobDllInterface.pEalJobLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobLibShutdown()
{
    (*gEalJobDllInterface.pEalJobLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobCreate(const eal_job_descriptor* pJobDescriptor)
{
    (*gEalJobDllInterface.pEalJobCreate)(pJobDescriptor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobDebugCreate(const eal_job_descriptor* pJobDescriptor,
                       const char*               pFilename,
                       eal_u32                   FileLine)
{
    (*gEalJobDllInterface.pEalJobDebugCreate)(pJobDescriptor, pFilename, FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

 eal_job_id EalJobAllocate( eal_id              Tag,
                            eal_u32             ExtraMemorySize,
                            void                (*pFunction)(eal_job_id),
                            eal_job_options*    pOptions)
 {
    return (*gEalJobDllInterface.pEalJobAllocate)(  Tag,
                                                    ExtraMemorySize,
                                                    pFunction,
                                                    pOptions);

 }

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobStart(eal_job_id JobId)
{
    (*gEalJobDllInterface.pEalJobStart)(JobId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobFree(eal_job_id JobId)
{
    (*gEalJobDllInterface.pEalJobFree)(JobId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalJobCancel(eal_job_id JobId)
{
    return (*gEalJobDllInterface.pEalJobCancel)(JobId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalJobCompleted(eal_job_id* JobIdTable, eal_u32 Count)
{
    return (*gEalJobDllInterface.pEalJobCompleted)(JobIdTable, Count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobWaitForJob(eal_job_id* JobIdTable, eal_u32 Count)
{
    (*gEalJobDllInterface.pEalJobWaitForJob)(JobIdTable, Count);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_job_id EalJobGetCurrentJob()
{
    return (*gEalJobDllInterface.pEalJobGetCurrentJob)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalParallelizeLargeItemCount(eal_id Tag, eal_u32 Count)
{
    return (*gEalJobDllInterface.pEalParallelizeLargeItemCount)(Tag, Count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_job_id EalJobDebugAllocate( eal_id                  Tag,
                                eal_u32                 ExtraMemorySize,
                                void                    (*pFunction)(eal_job_id),
                                eal_job_options*        pOptions,
                                const char*             pFilename,
                                eal_u32                 FileLine)
{
    return (*gEalJobDllInterface.pEalJobDebugAllocate)( Tag,
                                                        ExtraMemorySize,
                                                        pFunction,
                                                        pOptions,
                                                        pFilename,
                                                        FileLine);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalJobParallelize(
    eal_id Tag,
    void(*pFunction)(void * pData, eal_u64 Start, eal_u64 Count),
    void * pData,
    eal_u64 Count,
    eal_s32 MinimalBatchSize)
{
    (*gEalJobDllInterface.pEalJobParallelize)(Tag, pFunction, pData, Count, MinimalBatchSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine Side

void EalJobDllPopulateInterface(eal_job_dll_interface& Interface)
{
    Interface.pEalJobLibInit                = EalJobLibInit;
    Interface.pEalJobLibShutdown            = EalJobLibShutdown;
    Interface.pEalJobCreate                 = EalJobCreate;
    Interface.pEalJobDebugCreate            = EalJobDebugCreate;
    Interface.pEalJobAllocate               = EalJobAllocate;
    Interface.pEalJobStart                  = EalJobStart;
    Interface.pEalJobFree                   = EalJobFree;
    Interface.pEalJobCancel                 = EalJobCancel;
    Interface.pEalJobCompleted              = EalJobCompleted;
    Interface.pEalJobWaitForJob             = EalJobWaitForJob;
    Interface.pEalJobGetCurrentJob          = EalJobGetCurrentJob;
    Interface.pEalParallelizeLargeItemCount = EalParallelizeLargeItemCount;
    Interface.pEalJobDebugAllocate          = EalJobDebugAllocate;
    Interface.pEalJobParallelize            = EalJobParallelize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

