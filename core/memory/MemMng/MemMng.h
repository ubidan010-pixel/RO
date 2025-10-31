#ifndef ITF_CORE_MEMMNG_H_
#define ITF_CORE_MEMMNG_H_

#pragma once

namespace ITF
{

class MemMng
{
public:
    // Init/Close
    static	void	Init();
    static	void	Close();

    //////////////////////////////////////////////////////////////////////////
    // classic client interface

    static void*	Malloc(u64 _size);
    static void*	MallocCategory(u32 _size, u32 _idCat);
    static void*	Realloc(void * _ptr, u64 _newSize);
    static u64 	    Size(void * _ptr);
    static void	    Free(void * _ptr);

    static void*	AlignedMalloc(u64 _size, u64 _align);
    static void*	AlignedMallocCategory(u32 size, u32 _align, u32 _idCat);
    static void*	AlignedRealloc(void * _ptr, u64 _newSize, u64 _align);
    static u64 	    AlignedSize(void * _ptr);
    static void	    AlignedFree(void * _ptr);

    // do an internal validity check
    static void CheckValidity();

    // memory stats info
    struct MemCounts
    {
        struct CountDetail
        {
            volatile u64 current;
            u64 peak;
            Mutex lock;

            CountDetail() : current(0), peak(0)
            {}
        };

        CountDetail MallocMemUsed;
        CountDetail BucketMemUsed;		// Bucket memory
        CountDetail BucketPageMemAllocated;   // memory really allocated in buckets
        CountDetail BigMemUsed;		         // Big alloc memory
        CountDetail TotalMemUsed; // should be sum of malloc+bucket+big

        CountDetail Quarantine;   // track the amount of memory put in quarantine

        CountDetail VramMngDirect;
        CountDetail VramMngDirectReserved;

        uPtr userMemoryAvailable; // probably 5Go
        uPtr memoryTakenAtInit; // memory already taken at memory manager init
    };

    static const MemCounts & GetMemCounts();

    // Methods to control memory tracing
    static void	DumpMemoryTracer();
    static void	ClearMemoryTracer();

    // Out of mem callback
    typedef void (* OutOfMemCB)(unsigned int _requestedSize);
    static bool RegisterOutOfMemCallBack(OutOfMemCB pOOM);
    static bool UnRegisterOutOfMemCallBack(OutOfMemCB pOOM);

private :
    // boolean for init
    static bool m_bInitialized;
};

} // namespace ITF

#endif	// #ifndef ITF_CORE_MEMMNG_H_
