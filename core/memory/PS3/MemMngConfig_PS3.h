#ifndef ITF_CORE_MEMMNGCONFIG_H_
#define ITF_CORE_MEMMNGCONFIG_H_

//////////////////////////////////////////////////////////////////////////
// Memory Manager configuration file
//////////////////////////////////////////////////////////////////////////
// For most of the defines, we prefer to use define with a value.
// A default define value is set according to higher level defines like ITF_RELEASE
// Please do not change anything in there.
// Note that most of the time, the behavior according to ITF_RELEASE should be OK
// The "do not edit these" is in this file as a reference

//////////////////////////////////////////////////////////////////////////
// Memory Tracer
//////////////////////////////////////////////////////////////////////////
// Info : The memory tracer is stocking a callstack in debug memory for every 
// currently allocated memory block

// comment/uncomment and edit the line below at will
//#define MEMMNG_MEMORY_TRACERALL 1
//#define MEMMNG_MEMORY_TRACER 1

// but do not edit these
#ifndef MEMMNG_MEMORY_TRACER
    #define MEMMNG_MEMORY_TRACER	0
#endif

#if MEMMNG_MEMORY_TRACER==1
	#define MEMMNG_MAX_CALLSTACK_DEPTH 16
#else
	#define MEMMNG_MAX_CALLSTACK_DEPTH 1
#endif

//////////////////////////////////////////////////////////////////////////
// Bucket
//////////////////////////////////////////////////////////////////////////

// Use or not the lock free bucket allocator
//#define MEMMNG_USE_LOCKFREE_BUCKET_SYSTEM 1

#ifndef MEMMNG_USE_LOCKFREE_BUCKET_SYSTEM
    #define MEMMNG_USE_LOCKFREE_BUCKET_SYSTEM 0
#endif

// Buckets size choice
// Info : the choice between the standard allocator (ie. malloc or malloc-like) or 
// the bucket allocator is done according to a maximized size : MEMMNG_BUCKET_SIZE_MAX
// (put only data to buckets under or equal to this size)

// try this to deactivate buckets (out of bound page fault more often happen) :
//#define	MEMMNG_BUCKET_SIZE_MAX		(0)

#ifndef MEMMNG_BUCKET_SIZE_MAX
    #define MEMMNG_BUCKET_SIZE_MAX    (256)
#endif

// This define is used to specify that the buckets are allocated in a specific range address.
// This enable an optimization to determine if the pointer we try to free is a bucket pointer
// (less cache miss for the non bucket case)
// If the buckets are built on malloc, it cannot be set to 1
// If it's undefined, it's equivalent to be defined to 1
#define MEMMNG_BUCKET_SPECIFIC_ADDRESS_RANGE    (0)

// This define is used by for the platform specific part.
// When set to 1, it means the buckets are allocated using the standard allocator.
// If it's undefined, it's equivalent to be defined to 0
#define MEMMNG_BUCKET_BUILT_ON_MALLOC    (1)


// BUCKET CHECK policy
// Info : the bucket allocator have internal linked list.
// 0 -> no integrity checked
// 1 -> integrity of the allocated/freed bucket checked
// 2 -> full integrity checked

// comment/uncomment and edit the line below at will
//#define CHECK_BUCKETS_POLICY 1

// but do not edit these
#ifndef ITF_FINAL
    #ifndef CHECK_BUCKETS_POLICY
        #define CHECK_BUCKETS_POLICY 0
    #endif
#else
    #ifndef CHECK_BUCKETS_POLICY
        #define CHECK_BUCKETS_POLICY 0
    #endif
#endif

// Big alloc size choice
// Info : the choice between the standard allocator (ie. malloc or malloc-like) or 
// the big allocator is done according to a minimized size : MEMMNG_BIGALLOC_SIZE_MIN
// (put only data to big alloc bigger of equal to this size)
// As the size allocated is aligned on the page size, this should be several times the
// page size

// to deactivate big alloc, set a huge size
//#define	MEMMNG_BIGALLOC_SIZE_MIN		(0xFFFFFFFFu)


#ifndef MEMMNG_BIGALLOC_SIZE_MIN
    #define    MEMMNG_BIGALLOC_SIZE_MIN    (256u*1024u)
#endif

// Big alloc check policy
// Info : the big alloc allocator have internal linked list.
// 0 -> no integrity checked
// 1 -> integrity of the allocated/freed blocks checked
// 2 -> full integrity checked

// comment/uncomment and edit the line below at will
//#define CHECK_BIGALLOC_POLICY 1

// but do not edit these
#ifndef CHECK_BIGALLOC_POLICY
    #define CHECK_BIGALLOC_POLICY 0
#endif

//////////////////////////////////////////////////////////////////////////
// Memory Counts
//////////////////////////////////////////////////////////////////////////
// Info : the struct MemCounts returned by CMemMng::GetMemCounts is only 
// filled if MEMMNG_MEM_COUNT is defined.
// Count of memory allocation is not very expensive on cpu.
// When MEMMNG_MEM_COUNT is set to 1, the peak is computed
// When MEMMNG_MEM_COUNT is set to 2 or more, the peak is not computed
// thus, only atomic operations are done (the peak is done by a lock)

// comment/uncomment and edit this line below at will
// #define	MEMMNG_MEM_COUNT 1

#ifdef ITF_FINAL
    #ifndef MEMMNG_MEM_COUNT
        #define MEMMNG_MEM_COUNT 0
    #endif
#else
    #ifndef MEMMNG_MEM_COUNT
        #define MEMMNG_MEM_COUNT 2
    #endif
#endif

//////////////////////////////////////////////////////////////////////////
// Memory filling
//////////////////////////////////////////////////////////////////////////
// fill the memory at alloc/free
// Note about buckets : the page newly allocated is not filled at all

// comment/uncomment and edit this line below at will
//#define MEMMNG_FILL_MEMORY_AT_ALLOC 0
//#define MEMMNG_FILL_MEMORY_AT_FREE 0

// but do not edit these
#if defined(ITF_FINAL)
    #if !defined(MEMMNG_FILL_MEMORY_AT_ALLOC)
        #define MEMMNG_FILL_MEMORY_AT_ALLOC 0
    #endif
    #if !defined(MEMMNG_FILL_MEMORY_AT_FREE)
        #define MEMMNG_FILL_MEMORY_AT_FREE 0
    #endif
#else
    #if !defined(MEMMNG_FILL_MEMORY_AT_ALLOC)
        #define MEMMNG_FILL_MEMORY_AT_ALLOC 1
    #endif
    #if !defined(MEMMNG_FILL_MEMORY_AT_FREE)
        #define MEMMNG_FILL_MEMORY_AT_FREE 1
    #endif
#endif

// free pattern, we need to study interesting value for these
// (currently, a3 generate a valid near 0 float)
//#define	FREEMEM_PATTERN		0xa3		// Used to tag free memory
//#define	ALLOCMEM_PATTERN	0xbd		// Used to tag newly allocated memory

// new values studied for float ->
// Free  = -1.48513959e+029 (Big Negative)
// Alloc = 3.39615136e+038(Big Positive)
#define	FREEMEM_PATTERN		0xfe		// Used to tag free memory
#define	ALLOCMEM_PATTERN	0x7f		// Used to tag newly allocated memory


//////////////////////////////////////////////////////////////////////////
// Activate out of mem specific code
//////////////////////////////////////////////////////////////////////////
// When out of memory, the internal function OutOfMem is called, otherwise,
// it just return NULL.

// comment/uncomment and edit the line below at will
//#define MEMNG_TEST_OUT_OF_MEM 0

// but do not edit these
#if defined(ITF_FINAL)
    #if !defined(MEMNG_TEST_OUT_OF_MEM)
        #define MEMNG_TEST_OUT_OF_MEM 0
    #endif
#else
    #if !defined(MEMNG_TEST_OUT_OF_MEM)
        #define MEMNG_TEST_OUT_OF_MEM 1
    #endif
#endif

////////////////////////////////////////////////////////////////////////////
// Assert in the memory manager
//////////////////////////////////////////////////////////////////////////
// Info : the assert is internally defined as we don't want to trust any 
// other assert probably allocating memory. Anyway, by default, we look 
// at the project ENABLE_ASSERTS define

// define it to get assert in any cases
//#define MEMMNG_ENABLE_ASSERTS 1

// but do not edit these
#if defined(ITF_FINAL) || !defined(ASSERT_ENABLED)
    #if !defined(MEMMNG_ENABLE_ASSERTS)
        #define MEMMNG_ENABLE_ASSERTS 0
    #endif
#else
    #if !defined(MEMMNG_ENABLE_ASSERTS)
        #define MEMMNG_ENABLE_ASSERTS 1
    #endif
#endif

////////////////////////////////////////////////////////////////////////////
// Quarantine for allocations
//////////////////////////////////////////////////////////////////////////
// comment/uncomment and edit the line below at will
//#define MEMMNG_QUARANTINE 1

// but do not edit these
#if !defined(MEMMNG_QUARANTINE)
    #define MEMMNG_QUARANTINE 0
#endif

#endif // #ifndef ITF_CORE_MEMMNGCONFIG_H_
