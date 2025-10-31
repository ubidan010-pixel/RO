#pragma once

#ifndef _ITF_PAGE_H_
#define _ITF_PAGE_H_

#ifdef ENABLE_ALLOCATOR

namespace ITF
{

#if !defined(ITF_WII) && !defined(ITF_CTR)
// This class allocates pages of memory.
// Not valid on Wii. 

enum ITF_MEM_PageSize
{
#if (defined ITF_WINDOWS) || (defined ITF_X360)
	ITF_MEM_PageSize4kB,
#endif 
#if (defined ITF_PS3) || (defined ITF_X360)
	ITF_MEM_PageSize64kB,
#endif 
#if (defined ITF_PS3)
	ITF_MEM_PageSize1MB,
#endif 
	ITF_MEM_PageSizeMAX,
	ITF_MEM_PageSizeBEST // Let the system choose the best page size according to the size and the alignment requirements
};

#if (defined ITF_WINDOWS)
#define ITF_MEM_PageSizeDEFAULT  ITF_MEM_PageSize4kB
#else
#define ITF_MEM_PageSizeDEFAULT ITF_MEM_PageSize64kB
#endif 

#define ITF_MEM_PageHashSize 28

class Page 
{
public:
	Page(){}
	~Page(){}

	void Create();
	void Destroy();

	// Allocate memory pages
	void *pAllocPage(ITF_MEM_PageSize _ePageSize, u32 _uSize, u32 _uAlignment, bbool _bWriteCombine);
	
	// Free memory pages
	void FreePage(void *_pPage);

	// Realloc memory pages
	void *pReallocPage(void *_pOldPage,ITF_MEM_PageSize _ePageSize, u32 _uSize, u32 _uAlignment, bbool _bWriteCombine);

	// Get default page size
	u32 uGetDefaultPageSize();

	u32 uGetPageSize(void *_pPage) const ;

	bbool bIsValidPointer(void *_pPage) const ;

	u32 uGetPageNb();

	u32 uGetPageSize()	{return m_uPageSize;}

	u32 SizeWasted(u32 _uSize);

private:
	// Get the type and size of pages 
	void ComputeParameters(ITF_MEM_PageSize *_pPageType,u32 *_pSize,u32 *_pAlign);

	// Update stats during alloc and free.
	void AddPageStats(void *_pPage,u32 _uSize);
	void RemovePageStats(void *_pPage);

private:
	// Size taken by page allocs
	u32 m_uPageSize;	

	// Number of pages.
	u32 m_uPageNb;

	// Hash table whose key is address and value is alloc size. 
	// It is declared as a simple buffer here to avoid to include the hash table header. 
	u8 m_PageStatsBuffer[ITF_MEM_PageHashSize];
};

#endif // !defined(ITF_WII) && !defined(ITF_CTR)

} // namespace ITF

#endif //ENABLE_ALLOCATOR
#endif //_ITF_PAGE_H_

