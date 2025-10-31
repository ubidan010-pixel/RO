#pragma once

#ifndef _ITF_ALLOCATOR_CONSTS_H_
#define _ITF_ALLOCATOR_CONSTS_H_

#ifdef ENABLE_ALLOCATOR
namespace ITF
{

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 The free blocks are stored in lists according to their size, in Heap::m_apFreeBlocks.
 To each size corresponds an index computed with uSizeToIndex().
 The logic behind this function is the following : 
 1) Size increment from ITF_MEM_MIN_BLOCK_SIZE to ITF_MEM_SMALL_BLOCK_SIZE is 4 bytes, 
 	so that free block lists have blocks of the same size. 
 	For example if ITF_MEM_SMALL_BLOCK_SIZE is 512, m_apFreeBlocks will store lists of free blocks 
 	of size 4, 8, ..., 508 ...
 2) Size increment from ITF_MEM_SMALL_BLOCK_SIZE to ITF_MEM_MAX_BLOCK_SIZE increases with the size.
 	Just after ITF_MEM_SMALL_BLOCK_SIZE, the increment is 8 bytes, which means that the list contains 
 	free blocks of different sizes. 
 	So in our example at 512, the free block list contains blocks of size 512 or 516.
 	Each time a power of 2 is reached, the increment doubles.
 	In our example after size 512, m_apFreeBlocks stores lists of size (512->516), (520->524), 
 	..., (1016->1020), (1024->1036), ...	
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef ITF_FINAL
#define ITF_MEM_HEAP_DEBUG_
#endif //_FINAL

#ifdef ITF_WII
#define ITF_USE_TEMP_HEAP
#endif //ITF_WII


#define ITF_MEM_MIN_SIZE_POW2	4 	// min free block size is 2^ITF_MEM_MIN_SIZE_POW2 (16 o)
#ifdef ITF_WII
#define ITF_MEM_MAX_SIZE_POW2	27	// max free block size is 2^ITF_MEM_MAX_SIZE_POW2 (128 Mo)
#else //ITF_WII
#define ITF_MEM_MAX_SIZE_POW2	31	// max free block size is 2^ITF_MEM_MAX_SIZE_POW2 (2 Go) (Windows Seven cannot allocate blocks of 2Go or bigger in 32 bits mode).
#endif // ITF_WII
#define ITF_MEM_SMALL_SIZE_POW2 9   // small free block size is 2^ITF_MEM_SMALL_SIZE_POW2 (512 o). It can be changed ...

#define ITF_MEM_MIN_BLOCK_SIZE 		(1<<ITF_MEM_MIN_SIZE_POW2)
#define ITF_MEM_MAX_BLOCK_SIZE 		(1<<ITF_MEM_MAX_SIZE_POW2)
#define ITF_MEM_SMALL_BLOCK_SIZE 	(1<<ITF_MEM_SMALL_SIZE_POW2)

// Number of free blocks lists between 2^N and 2^(N+1)
// Is is computed so that size of free block lists over ITF_MEM_SMALL_BLOCK_SIZE are not unique
#define ITF_MEM_BUCKET_POW2			(ITF_MEM_SMALL_SIZE_POW2 - 3)
#define ITF_MEM_BUCKET_NB			(1<<ITF_MEM_BUCKET_POW2)

// Before this index, there is only one size per free block list.
// After this index, free block lists contain size between 2^(i+N) + jP and 2^(i+N) + (j+1)P 
// where N is ITF_MEM_SMALL_SIZE_POW2 and P is ITF_MEM_BUCKET_NB.
#define ITF_MEM_SMALL_BLOCK_INDEX	((ITF_MEM_SMALL_BLOCK_SIZE-ITF_MEM_MIN_BLOCK_SIZE)>>2)

// Number of freeblock lists that are in buckets
#define ITF_MEM_BIG_BLOCK_NB		(ITF_MEM_BUCKET_NB * (ITF_MEM_MAX_SIZE_POW2 - ITF_MEM_SMALL_SIZE_POW2))

// Total number of freeblock lists
#define ITF_MEM_FREE_BLOCKS_NB		(ITF_MEM_SMALL_BLOCK_INDEX + ITF_MEM_BIG_BLOCK_NB)

#define ITF_MEM_FREE_FLAG_NB		((ITF_MEM_FREE_BLOCKS_NB - 1) >> 5) + 1

#ifdef ITF_WINDOWS
#define ITF_MEM_BEGIN_GUARD   0xFECAADAB // Easier to read in memory watch because PC is little endian
#define ITF_MEM_END_GUARD     0xEFBEADDE
#else //ITF_WINDOWS
#define ITF_MEM_BEGIN_GUARD   0xABADCAFE
#define ITF_MEM_END_GUARD     0xDEADBEEF
#endif // ITF_WINDOWS

#define ITF_MEM_FILL_ALLOC    0xAAAAAAAA
#define ITF_MEM_FILL_FREE     0xBBBBBBBB

/*$4-******************************************************************************************************************/
#define ITF_MEM_BASIC_ALLOC		0x00000000	// This allocation has nothing special.
#define ITF_MEM_MEM1_ALLOC		0x00000001	// For Wii system, indicates that we want the allocation to be done in its memory 1 (fast CPU access, random access).
#define ITF_MEM_MEM2_ALLOC		0x00000002	// For Wii system, indicates that we want the allocation to be done in its memory 2 (fast GPU access, sequential access).
#define ITF_MEM_HARD_ALLOC		0x00000004	// For Wii system, indicates that the memory must be allocated in the specified location because of the hardware (sound, FIFO, etc.).
#define ITF_MEM_REALLOC_IN_PLACE	0x00000008	// Guaranties that the realloc will not change the address if the new size is smaller than the old one. 
#define ITF_MEM_NOPAGE_ALLOC		0x00000010	// Indicates that the allocation may not be done in pages.
#define ITF_MEM_DEBUG_ALLOC		0x00000020	// Indicates that the allocation should be done in debug heap. These allocations are not tracked. 
#define ITF_MEM_FRAMEBUFFER_ALLOC 0x00000040	// Indicates that the allocation should be done in the frame buffer temp heap, if possible. 
#define ITF_MEM_AFTERFX_ALLOC  	0x00000080	// Indicates that the allocation should be done in the afterFX temp heap, if possible. 
#define ITF_MEM_PS3_MAIN_ALLOC	0x00000100	// For PS3 system, indicates that we want the allocation to be done in its main memory 
#define ITF_MEM_PS3_HOST_ALLOC	0x00000200	// For PS3 system, indicates that we want the allocation to be done in its main memory for GPU 
#define ITF_MEM_PS3_VRAM_ALLOC	0x00000400	// For PS3 system, indicates that we want the allocation to be done in its local memory (VRAM)

#define ITF_MEM_PS3_RENDERBUFFER_ALLOC	0x00001000	// For PS3 system, indicates that we allocate RenderBuffer
#define ITF_MEM_PS3_CG_PROGRAM_ALLOC		0x00002000	// For PS3 system, indicates that we allocate CG Program (shader)
#define ITF_MEM_PS3_OTHERGPU_ALLOC		0x00004000	// For PS3 system, indicates that we allocate other GPU object
#define ITF_MEM_PS3_USERCB_ALLOC		    0x00008000	// For PS3 system, indicates that we allocate user command buffer object

#define ITF_MEM_XB360_PHYSICAL_ALLOC      0x00010000  // For Xbox 360, indicates that we allocate physical memory (using XPhysicalAlloc).
#define ITF_MEM_XB360_WRITE_COMBINE       0x00020000  // For Xbox 360, indicates that a physical alloc should be made with write combine.

#if defined(LYN_FOR_3DS)
#define ITF_MEM_CTR_FCRAM_ALLOC			0x00040000	// For CTR, indicates that we allocate FCRAM memory
#define ITF_MEM_CTR_VRAMA_ALLOC			0x00080000	// For CTR, indicates that we allocate VRAMA memory
#define ITF_MEM_CTR_VRAMB_ALLOC			0x00100000	// For CTR, indicates that we allocate VRAMB memory
#endif

#define ITF_MEM_IA_ALLOC	    			0x10000000	// IA allocation (MEM1 on Wii)

#define ITF_MEM_LOG_DUMP_COUNT		0x00000001
#define ITF_MEM_LOG_DUMP_FILE			0x00000002
#define ITF_MEM_LOG_DUMP_LINE			0x00000004
#define ITF_MEM_LOG_DUMP_HEAP			0x00000008
#define ITF_MEM_LOG_DUMP_STEP			0x00000010
#define ITF_MEM_LOG_DUMP_KEYS			0x00000020
#define ITF_MEM_LOG_DUMP_ALIGN		0x00000040
#define ITF_MEM_LOG_DUMP_VRAM			0x00000080
#define ITF_MEM_LOG_DUMP_ADDRESS		0x00000100

#define ITF_MEM_LOG_DUMP_LEAKS		 (ITF_MEM_LOG_DUMP_COUNT | ITF_MEM_LOG_DUMP_ADDRESS | ITF_MEM_LOG_DUMP_FILE | ITF_MEM_LOG_DUMP_LINE) // Used to display leaks (one line per alloc !)
#define ITF_MEM_LOG_DUMP_ALLOC		 (ITF_MEM_LOG_DUMP_FILE | ITF_MEM_LOG_DUMP_LINE | ITF_MEM_LOG_DUMP_HEAP)  // Displays summary of memory usage.
#define ITF_MEM_LOG_DUMP_ALLOC_VERBOSE (ITF_MEM_LOG_DUMP_FILE | ITF_MEM_LOG_DUMP_LINE | ITF_MEM_LOG_DUMP_HEAP | ITF_MEM_LOG_DUMP_STEP | ITF_MEM_LOG_DUMP_KEYS | ITF_MEM_LOG_DUMP_ALIGN | ITF_MEM_LOG_DUMP_VRAM) // Displays details of memory usage.

#define ITF_MEM_LOG_USAGE_TITLE		0x00000001
#define ITF_MEM_LOG_USAGE_SUMMARY		0x00000002
#define ITF_MEM_LOG_USAGE_AVAILABLE	0x00000004
#define ITF_MEM_LOG_USAGE_HEAP		0x00000008
#define ITF_MEM_LOG_USAGE_PAGE		0x00000010
#define ITF_MEM_LOG_USAGE_CONTAINER	0x00000020
#define ITF_MEM_LOG_USAGE_SYSTEM		0x00000040
#define ITF_MEM_LOG_USAGE_LINES		0x00000080

// Use with logs.
#define ITF_MEM_LOG_USAGE_ALL			0xFFFFFFFF

// Used for on screen display (when the space is scarse)
#define ITF_MEM_LOG_USAGE_COMPACT		ITF_MEM_LOG_USAGE_ALL & ~(ITF_MEM_LOG_USAGE_TITLE|ITF_MEM_LOG_USAGE_LINES) 

#define ITF_MEM_DEFAULT_FLAG_STACK_SIZE 8

// Memory heaps.
// On PS3 and PC, there only one heap (other memory use other functions)
// On Wii:      -> MEM1 is a fast CPU memory, FIFO must be there.
//              -> MEM2 is a fast GPU and sound memory, sound must be here
// On Xbox 360: -> Heap 1 is for normal allocs (uses virtual addresses).
//              -> Heap 2 is for small physical allocs using write combine
//              -> Heap 3 is for small physical allocs not using write combine

// On CTR:		-> Heap 1 is for normal allocs
//				-> Heap 2 is for device memory allocs

enum MEM_HeapID
{
	ITF_MEM_HEAP1	,	
#if (defined ITF_WII) 
	ITF_MEM_HEAP2	,	// MEM 2
#elif (defined ITF_X360)
    ITF_MEM_HEAP_WC	,	// Write Combine Heap
	ITF_MEM_HEAP_NWC,	// No Write Combine Heap
#elif (defined ITF_CTR)
	ITF_MEM_HEAP2	,   // FCRAM (Device Memory)
#endif
#ifdef ITF_DEBUG_ALLOC_	
	ITF_MEM_HEAP_DEBUG,	// Used to store alloc debug data
#endif //ITF_DEBUG_ALLOC_	
	ITF_MEM_HEAP_TOTAL_NB	
};

#ifdef ITF_USE_TEMP_HEAP
// Temporary heaps are available in certain conditions. 
// They allow to use buffers that are not used by another part of the code.
// They don't count in memory consumption because the buffers have already been counted,
// but the 
enum ITF_MEM_TempHeap
{
    ITF_MEM_TempHeapFrameBuffer,    // One frame buffer (XFB) is available on Wii when a Bink is played (because Bink supports only 1 frame buffer). It is used to put binary loading buffers. 
    ITF_MEM_TempHeapAfterFX,        // One afterFX buffer is available on Wii when no afterFX are played. It is used to put Bink allocs. 
    ITF_MEM_TempHeapNb
};
#endif //ITF_USE_TEMP_HEAP


//  In which step have the alloc been made ? 
enum ITF_MEM_Step
{
	ITF_MEM_StepInit, 	// Allocs made here should be freed at exit of program
	ITF_MEM_StepLoop	// Allocs made here should be freed at exit of map
};

#if (defined ITF_X360)
#define ITF_MEM_HEAP_NB	3
#elif (defined ITF_WII) 
#define ITF_MEM_HEAP_NB	2
#elif (defined ITF_CTR)
#define ITF_MEM_HEAP_NB	2
#else 
#define ITF_MEM_HEAP_NB	1
#endif

#define ITF_MEM_HeapInvalid	0xFFFFFFFF

#define ITF_MEM_PS3MaxPageNb    1024
#define ITF_MEM_MaxPageNb       (1<<(ITF_MEM_MAX_SIZE_POW2-12)) // The max number of pages is equal to the max block size (1<<ITF_MEM_MAX_SIZE_POW2) divided by the min page size (1<<12).

#define ITF_MEM_NO_DECOMMIT (void *)0xFFFFFFFF  // We should not try to decommit pages.

#ifdef ITF_DEBUG_ALLOC_

#define ITF_MEM_DebugArgsTypesDeclBase	const char *_sFile, u32 _uLine, const char *_sTag, bbool _bIsNew = bfalse
#define ITF_MEM_DebugArgsTypesDeclPage	, ITF_MEM_DebugArgsTypesDeclBase, bbool _bIsAligned = bfalse
#define ITF_MEM_DebugArgsTypesDeclAlloc	, ITF_MEM_DebugArgsTypesDeclBase, bbool _bIsAligned = bfalse
#define ITF_MEM_DebugArgsTypesDeclAligned	, ITF_MEM_DebugArgsTypesDeclBase, bbool _bIsAligned = btrue
#define ITF_MEM_DebugArgsTypesDef		    , const char *_sFile, u32 _uLine, const char *_sTag, bbool _bIsNew, bbool _bIsAligned
#define ITF_MEM_DebugArgs					, _sFile, _uLine, _sTag, _bIsNew, _bIsAligned
#define ITF_MEM_FreeArgsTypesDecl         , bbool _bIsNew = bfalse
#define ITF_MEM_FreeArgsTypesDef          , bbool _bIsNew
#define ITF_MEM_FreeArgs                  , _bIsNew
#define ITF_MEM_NewArgsTypes				, const char *_sFile, u32 _uLine, const char *_sTag, f32 
#define ITF_MEM_NewArgsTypesPriv			, const char *_sFile, u32 _uLine, const char *_sTag
#define ITF_MEM_NewArgs					, _sFile, _uLine, _sTag, btrue
#define ITF_MEM_NewArgsPriv			    , _sFile, _uLine, _sTag
#define ITF_MEM_DeleteArgs				, btrue
#define ITF_MEM_FreeTmpArgs				, bfalse
#define ITF_ITF_MEM_ContainerCreateDecl		, const char *_sFile, u32 _uLine, const char *_sTag
#define ITF_ITF_MEM_ContainerCreateDef		, const char *_sFile, u32 _uLine, const char *_sTag
#define ITF_ITF_MEM_ContainerCreateArg		, _sFile, _uLine, _sTag

#else// ITF_DEBUG_ALLOC_

#define ITF_MEM_DebugArgsTypesDeclBase
#define ITF_MEM_DebugArgsTypesDeclPage
#define ITF_MEM_DebugArgsTypesDeclAlloc
#define ITF_MEM_DebugArgsTypesDeclAligned
#define ITF_MEM_DebugArgsTypesDef
#define ITF_MEM_DebugArgs		
#define ITF_MEM_FreeArgsTypesDecl
#define ITF_MEM_FreeArgsTypesDef         
#define ITF_MEM_FreeArgs                  
#define ITF_MEM_NewArgsTypes				
#define ITF_MEM_NewArgsTypesPriv			
#define ITF_MEM_NewArgs		
#define ITF_MEM_NewArgsPriv			    
#define ITF_MEM_DeleteArgs				
#define ITF_MEM_FreeTmpArgs			
#define ITF_ITF_MEM_ContainerCreateDecl
#define ITF_ITF_MEM_ContainerCreateDef
#define ITF_ITF_MEM_ContainerCreateArg
#endif // ITF_DEBUG_ALLOC_


} //namespace ITF

#endif //ENABLE_ALLOCATOR
#endif //_ITF_ALLOCATOR_CONSTS_H_
