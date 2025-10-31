#include "precompiled_core.h"

#ifdef ENABLE_ALLOCATOR
#ifndef _ITF_PAGE_H_
#include "core/memory/allocator/page.h"
#endif //_ITF_PAGE_H_

#ifndef _ITF_ALLOCATOR_CONSTS_H_
#include "core/memory/allocator/allocator_consts.h"
#endif //_ITF_ALLOCATOR_CONSTS_H_

#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_

#ifndef _ITF_HASH_H_
#include "core/container/Hash.h"
#endif //_ITF_HASH_H_

#ifdef ITF_PS3
#include <sys/memory.h>
#endif //ITF_PS3

namespace ITF
{

// m_PageStats is of type Hash<void *,u32>
typedef Hash<void *,u32> td_Hash;

#define m_PageStats (*((td_Hash *)(&m_PageStatsBuffer)))
//ITF_ASSERT_STATIC(sizeof ( Hash< void * , u32 > ) == ITF_MEM_PageHashSize,"Wrong size for ITF_MEM_PageHashSize, please set it to sizeof (td_Hash)");

const u32 PageSize[ITF_MEM_PageSizeMAX] = {
#if (defined ITF_WINDOWS)
		4096,
#endif 
#if (defined ITF_X360)
		4096, 65536
#endif 
#if (defined ITF_PS3)
		65536, 1048576
#endif 
	};

const u32 PageFlag[ITF_MEM_PageSizeMAX] = {
#if (defined ITF_WINDOWS)
	0,
#endif 
#if (defined ITF_X360)
	0, MEM_LARGE_PAGES
#endif 
#if (defined ITF_PS3)
	SYS_MEMORY_PAGE_SIZE_64K, SYS_MEMORY_PAGE_SIZE_1M
#endif 
};

void Page::Create()
{
	ITF_MemSet((void *)m_PageStatsBuffer,0,ITF_MEM_PageHashSize);
	ITF_MEM_PushExt(ITF_MEM_NOPAGE_ALLOC);
	m_PageStats.create(18,(void *)1); 
	ITF_MEM_PopExt();
}

void Page::Destroy()
{
	m_PageStats.destroy();
}

// Allocates a memory page
void *Page::pAllocPage(ITF_MEM_PageSize _ePageSize, u32 _uSize, u32 _uAlignment, bbool _bWriteCombine)
{
	ComputeParameters(&_ePageSize,&_uSize,&_uAlignment);

	void *result;
	u32 uFlag = PageFlag[_ePageSize];

#ifdef ITF_PS3
	int error = sys_memory_allocate(_uSize,uFlag,(sys_addr_t*)&result);
	if (error != CELL_OK)
		result = NULL;
#elif  (defined ITF_X360)
	if (_bWriteCombine)
		uFlag |= PAGE_WRITECOMBINE;
	result = XPhysicalAlloc(_uSize, MAXULONG_PTR, _uAlignment, PAGE_READWRITE|uFlag );
#elif  (defined  ITF_WINDOWS)
	result = VirtualAlloc(NULL,_uSize, MEM_COMMIT|uFlag, PAGE_READWRITE );
#endif 

	AddPageStats(result,_uSize);

	return result;
}

// Frees a memory page
void Page::FreePage(void *_pPage)
{
#ifdef ITF_PS3
	int error = sys_memory_free((sys_addr_t)_pPage);
	ITF_ASSERT_CRASH(CELL_OK == error,"FreePage: not a valid address");
#elif  (defined ITF_X360) 
	XPhysicalFree(_pPage);
#elif (defined  ITF_WINDOWS)
	BOOL bSuccess = VirtualFree(_pPage,0, MEM_RELEASE);
	ITF_ASSERT(bSuccess);
#endif 

	RemovePageStats(_pPage);
}

void *Page::pReallocPage(void *_pOldPage,ITF_MEM_PageSize _ePageSize, u32 _uSize, u32 _uAlignment, bbool _bWriteCombine)
{
	void *pNewPage = pAllocPage(_ePageSize, _uSize, _uAlignment, _bWriteCombine);
	if(!pNewPage)
		return NULL;

	u32 uNewPageSize = uGetPageSize(pNewPage);
	u32 uOldPageSize = uGetPageSize(_pOldPage);
	u32 uCopySize = ITF::Min<u32>(uNewPageSize,uOldPageSize);
	ITF_Memcpy(pNewPage,_pOldPage,uCopySize);
	FreePage(_pOldPage);
	return pNewPage;
}

// Get default page size
u32 Page::uGetDefaultPageSize()
{
	return PageSize[ITF_MEM_PageSizeDEFAULT];
}


void Page::ComputeParameters(ITF_MEM_PageSize *_pPageType, u32 *_pSize, u32 *_pAlign)
{
	ITF_MEM_PageSize ePageType = *_pPageType;
	u32 uSize = *_pSize;
	u32 uAlign = *_pAlign;
	if (ePageType == ITF_MEM_PageSizeBEST)
	{
#if (defined ITF_WINDOWS)
		ePageType = ITF_MEM_PageSize4kB;
		ITF_ASSERT_CRASH(PageSize[ePageType] >= uAlign,"Alignment too big for page alloc");
#elif (defined ITF_PS3)
		if (PageSize[ITF_MEM_PageSize64kB] < uAlign)
			ePageType = ITF_MEM_PageSize1MB;
		else
		{
			u32 uLargePageSize = PageSize[ITF_MEM_PageSize1MB];
			u32 uLeftOver = (uSize) % uLargePageSize;
			ePageType = (uLeftOver == 0) ? ITF_MEM_PageSize1MB : ITF_MEM_PageSize64kB;
		}
		ITF_ASSERT_CRASH(PageSize[ePageType] >= uAlign,"Alignment too big for page alloc");
#elif  (defined ITF_X360)
		u32 uLargePageSize = PageSize[ITF_MEM_PageSize64kB];
		u32 uLeftOver = (uSize) % uLargePageSize;
		ePageType = (uLeftOver == 0) ? ITF_MEM_PageSize64kB : ITF_MEM_PageSize4kB;
#endif 
	}

#ifdef ITF_X360
	if (PageSize[ePageType] >= uAlign)
		*_pAlign = 0;
#endif //ITF_X360

	u32 uPageSize = PageSize[ePageType];
	u32 uPageNb = 1 + ((uSize-1)/uPageSize);
	*_pSize = uPageSize * uPageNb;
	*_pPageType = ePageType;
}

void Page::AddPageStats(void *_pPage,u32 _uSize)
{
	static bbool bNoResize = bfalse;
	if (!bNoResize)
	{
		// If m_PageStats.ForceResize allocates a page, this function will be called again. 
		// We avoid a stack overflow by not calling ForceResize twice. 
		bNoResize = btrue;

		// Ensure that the hash table is big enough for next AddPageStats, so that no alloc is done in bAddValue.
		if (m_PageStats.bSoonNeedResize())
		{
			ITF_MEM_PushExt(ITF_MEM_NOPAGE_ALLOC);
			m_PageStats.ForceResize();
			ITF_MEM_PopExt();
		}

		bNoResize = bfalse;
	}

	m_uPageSize+=_uSize;
	m_uPageNb++;
	m_PageStats.bAddValue(_pPage,_uSize);
}

void Page::RemovePageStats(void *_pPage)
{
	HashIterator it = m_PageStats.itFindValue(_pPage);
	ITF_ASSERT_CRASH(HashIteratorInvalid != it,"Page was not found, was it already freed ?")
	u32 uSize = m_PageStats.GetValue(it);
	m_uPageSize-=uSize;
	m_uPageNb--;
	m_PageStats.RemoveValue(it);
}

u32 Page::uGetPageSize(void *_pPage) const 
{
	return *(m_PageStats.getPtrValue(_pPage));
}

bbool Page::bIsValidPointer(void *_pPage) const 
{
	HashIterator it;
	for(it = m_PageStats.GetFirstIterator(); it != HashIteratorInvalid; it = m_PageStats.GetNextIterator(it))
	{
	     u32 uSize = m_PageStats.GetValue(it);
	     void *pAddress = m_PageStats.GetKey(it);

		 if(_pPage >= pAddress &&  _pPage < (u8*) pAddress + uSize)
			 return btrue;
	}

	return bfalse;
}

u32 Page::uGetPageNb()
{
	return m_PageStats.uGetElementNb();
}

u32 Page::SizeWasted(u32 _uSize)
{
	u32 uMinPage = PageSize[0];
	return (1+(_uSize-1)/uMinPage)*uMinPage - _uSize;
}

} // namespace ITF
#endif //ENABLE_ALLOCATOR
