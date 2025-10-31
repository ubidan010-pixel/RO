#ifndef __MEM_CONSTS_WII_H__
#define __MEM_CONSTS_WII_H__

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 The free blocks are stored in lists according to their size, in MEM_Heap::m_apFreeBlocks.
 To each size corresponds an index computed with uSizeToIndex().
 The logic behind this function is the following : 
 1) Size increment from MEM_MIN_BLOCK_SIZE to MEM_SMALL_BLOCK_SIZE is 4 bytes, 
 	so that free block lists have blocks of the same size. 
 	For example if MEM_SMALL_BLOCK_SIZE is 512, m_apFreeBlocks will store lists of free blocks 
 	of size 4, 8, ..., 508 ...
 2) Size increment from MEM_SMALL_BLOCK_SIZE to MEM_MAX_BLOCK_SIZE increases with the size.
 	Just after MEM_SMALL_BLOCK_SIZE, the increment is 8 bytes, which means that the list contains 
 	free blocks of different sizes. 
 	So in our example at 512, the free block list contains blocks of size 512 or 516.
 	Each time a power of 2 is reached, the increment doubles.
 	In our example after size 512, m_apFreeBlocks stores lists of size (512->516), (520->524), 
 	..., (1016->1020), (1024->1036), ...	
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

namespace ITF
{

#ifndef ITF_FINAL
#define _DEBUG_ALLOC_
//#define _DEBUG_HEAP_
#define _HEAP_DEBUG_
#endif // ITF_FINAL

#define USE_TEMP_HEAP

#if !defined(ITF_DISABLE_DEBUGINFO) || defined(_HEAP_DEBUG_)
#define _HEAP_SIZE_DEBUG_
#endif // !defined(ITF_DISABLE_DEBUGINFO) || defined(_HEAP_DEBUG_)

#define MEM_MIN_SIZE_POW2	4 	// min free block size is 2^MEM_MIN_SIZE_POW2 (16 o)
#define MEM_MAX_SIZE_POW2	27	// max free block size is 2^MEM_MAX_SIZE_POW2 (128 Mo)
#define MEM_SMALL_SIZE_POW2 9   // small free block size is 2^MEM_SMALL_SIZE_POW2 (512 o). It can be changed ...

#define MEM_MIN_BLOCK_SIZE 		(1<<MEM_MIN_SIZE_POW2)
#define MEM_MAX_BLOCK_SIZE 		(1<<MEM_MAX_SIZE_POW2)
#define MEM_SMALL_BLOCK_SIZE 	(1<<MEM_SMALL_SIZE_POW2)

// Number of free blocks lists between 2^N and 2^(N+1)
// Is is computed so that size of free block lists over MEM_SMALL_BLOCK_SIZE are not unique
#define MEM_BUCKET_POW2			(MEM_SMALL_SIZE_POW2 - 3)
#define MEM_BUCKET_NB			(1<<MEM_BUCKET_POW2)

// Before this index, there is only one size per free block list.
// After this index, free block lists contain size between 2^(i+N) + jP and 2^(i+N) + (j+1)P 
// where N is MEM_SMALL_SIZE_POW2 and P is MEM_BUCKET_NB.
#define MEM_SMALL_BLOCK_INDEX	((MEM_SMALL_BLOCK_SIZE-MEM_MIN_BLOCK_SIZE)>>2)

// Number of freeblock lists that are in buckets
#define MEM_BIG_BLOCK_NB		(MEM_BUCKET_NB * (MEM_MAX_SIZE_POW2 - MEM_SMALL_SIZE_POW2))

// Total number of freeblock lists
#define MEM_FREE_BLOCKS_NB		(MEM_SMALL_BLOCK_INDEX + MEM_BIG_BLOCK_NB)

#define MEM_FREE_FLAG_NB		((MEM_FREE_BLOCKS_NB - 1) >> 5) + 1

#define MEM_MAX_ALLOC_SIZE_POW2	27	

#define MEM_MAX_ALLOC_SIZE  (1<<MEM_MAX_ALLOC_SIZE_POW2) 

#define M_MEM_BEGIN_GUARD   0xABADCAFE
#define M_MEM_END_GUARD     0xDEADBEEF

#define M_MEM_FILL_ALLOC    0xAAAAAAAA
#define M_MEM_FILL_FREE     0xBBBBBBBB

/*$4-******************************************************************************************************************/
#define MEM_C_BASIC_ALLOC		0x00000000	// This allocation has nothing special.
#define MEM_C_MEM1_ALLOC		0x00000001	// For Wii system, indicates that we want the allocation to be done in its memory 1 (fast CPU access, random access).
#define MEM_C_MEM2_ALLOC		0x00000002	// For Wii system, indicates that we want the allocation to be done in its memory 2 (fast GPU access, sequential access).
#define MEM_C_HARD_ALLOC		0x00000004	// For Wii system, indicates that the memory must be allocated in the specified location because of the hardware (sound, FIFO, etc.).
#define MEM_C_REALLOC_IN_PLACE	0x00000008	// Guaranties that the realloc will not change the address if the new size is smaller than the old one. 
#define MEM_C_DEBUG_ALLOC		0x00000020	// Indicates that the allocation should be done in debug heap. These allocations are not tracked. 
#define MEM_C_AFTERFX_ALLOC		0x00000040  // Indicates that the allocation should be done in the afterFX temp heap, if possible. 
#define MEM_C_FRAMEBUFFER_ALLOC 0x00000080  // Indicates that the allocation should be done in the frame buffer temp heap, if possible. 

#define MEM_C_LOG_USAGE_TITLE		0x00000001
#define MEM_C_LOG_USAGE_SUMMARY		0x00000002
#define MEM_C_LOG_USAGE_AVAILABLE	0x00000004
#define MEM_C_LOG_USAGE_HEAP		0x00000008
#define MEM_C_LOG_USAGE_PAGE		0x00000010
#define MEM_C_LOG_USAGE_CONTAINER	0x00000020
#define MEM_C_LOG_USAGE_SYSTEM		0x00000040
#define MEM_C_LOG_USAGE_ALL			0xFFFFFFFF

#define MEM_DefaultFlagStackSize 8

// Memory heaps.
// On Wii:      -> MEM1 is a fast CPU memory, FIFO must be there.
//              -> MEM2 is a fast GPU and sound memory, sound must be here
enum MEM_HeapID
{
	MEM_Heap1	,	
	MEM_Heap2	,	// MEM 2
#ifdef _DEBUG_HEAP_	
	MEM_HeapDebug,	// Used to store alloc debug data
#endif //_DEBUG_HEAP_	
	MEM_HeapTotalNb	
};

#define MEM_HeapNb	2
#define MEM_HeapInvalid	0xFFFFFFFF

#ifdef USE_TEMP_HEAP
// Temporary heaps are available in certain conditions. 
// They allow to use buffers that are not used by another part of the code.
// They don't count in memory consumption because the buffers have already been counted,
// but the 
enum MEM_TempHeap
{
	MEM_TempHeapFrameBuffer,    // One frame buffer (XFB) is available on Wii when a Bink is played (because Bink supports only 1 frame buffer). It is used to put binary loading buffers. 
	MEM_TempHeapAfterFX,        // One afterFX buffer is available on Wii when no afterFX are played. It is used to put Bink allocs. 
	MEM_TempHeapNb
};
#endif //USE_TEMP_HEAP

#define MEM_NO_DECOMMIT (void *)0xFFFFFFFF  // We should not try to decommit pages.

#ifdef _DEBUG_ALLOC_

#define MEM_M_DebugArgsTypesDeclBase	bbool _bIsNew = bfalse
#define MEM_M_DebugArgsTypesDeclAlloc	, MEM_M_DebugArgsTypesDeclBase, bbool _bIsAligned = bfalse
#define MEM_M_DebugArgsTypesDeclAligned	, MEM_M_DebugArgsTypesDeclBase, bbool _bIsAligned = btrue
#define MEM_M_DebugArgsTypesDef		    , bbool _bIsNew, bbool _bIsAligned
#define MEM_M_DebugArgs					, _bIsNew, _bIsAligned
#define MEM_M_FreeArgsTypesDecl         , bbool _bIsNew = bfalse
#define MEM_M_FreeArgsTypesDef          , bbool _bIsNew
#define MEM_M_FreeArgs                  , _bIsNew

#else// _DEBUG_ALLOC_

#define MEM_M_DebugArgsTypesDeclBase
#define MEM_M_DebugArgsTypesDeclAlloc
#define MEM_M_DebugArgsTypesDeclAligned
#define MEM_M_DebugArgsTypesDef
#define MEM_M_DebugArgs		
#define MEM_M_FreeArgsTypesDecl
#define MEM_M_FreeArgsTypesDef         
#define MEM_M_FreeArgs                  

#endif // _DEBUG_ALLOC_

} // namespace ITF

#endif // __MEM_CONSTS_WII_H__
