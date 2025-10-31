#ifndef ITF_MEMMNGPAGE_PS3_H_
#define ITF_MEMMNGPAGE_PS3_H_

#include <sys/memory.h>
#include "Core/math/BitTweak.h"

namespace ITF
{

class MemoryPageManager
{
public:

    enum PageSizeType
    {
        // PS3 -> 64K or 1M pages
        // VERY_SMALL_PAGE_SIZE = 4u*1024u,
        SMALL_PAGE_SIZE = 64u*1024u,
        BIG_PAGE_SIZE = 1024u*1024u,
    };

    enum
    {
        VIRTUAL_SPACE_MINIMAL_PAGE = 256u*1024u*1024u,
        VIRTUAL_SPACE_PAGE_ALIGNMENT = 256u*1024u*1024u
    };

	MemoryPageManager()
		: m_pAreaBegin(0)
		, m_pAreaEnd(0)
		, m_pPeak(0)
		, m_pHighMark(0)
		, m_pageSize(SMALL_PAGE_SIZE)
	{
	}

    void Init(ux _areaSize, PageSizeType _pageSizeType)
    {
		// align size on page
        _areaSize = BitTweak::GetAboveAlign(_areaSize, _pageSizeType);

		m_pageSize = _pageSizeType;
        m_pAreaBegin = ReserveVirtualSpace(_areaSize);
        m_pAreaEnd = m_pAreaBegin + _areaSize;

        m_pHighMark = m_pPeak = m_pAreaBegin;
    }

    void Close()
    {
        ReleaseVirtualSpace(m_pAreaBegin);
    }

    ux GetPageSize() const { return m_pageSize; } 
    u8 * GetAreaBegin() const { return m_pAreaBegin; } 
    u8 * GetAreaEnd() const { return m_pAreaEnd; } 

    bool isInRange(u8 * pBlock) const
    {
        return pBlock>=m_pAreaBegin && pBlock<m_pHighMark;
    }

    // commit/decommitPages work in unit of page sizes
    // It has to be used in pair, when the decommit size is not necessarily the same size as the commit
    bool CommitPages(u8 * _pageBegin, u8 * _pageEnd)
    {
        ux size = (_pageEnd - _pageBegin);

        size = BitTweak::GetAboveAlign(size, ux(m_pageSize));
        u8 * currentPageAddress = _pageBegin;

        while(size!=0)
        {
            // 1 - allocate physical
            sys_memory_t memId;
            int retError = sys_mmapper_allocate_memory(m_pageSize, (m_pageSize==SMALL_PAGE_SIZE)?SYS_MEMORY_GRANULARITY_64K:SYS_MEMORY_GRANULARITY_1M, &memId);
            if (retError!=CELL_OK)
            {
                return false;
            }

            // 2 - map to the given address
            retError = sys_mmapper_map_memory((sys_addr_t)currentPageAddress, memId, SYS_MEMORY_PROT_READ_WRITE);
            if (retError!=CELL_OK)
            {
                sys_mmapper_free_memory(memId);
                return false;
            }

            size -= m_pageSize;
            currentPageAddress += m_pageSize;
        }
        return true;
    }

    bool DecommitPages(u8 * _pageBegin, u8 * _pageEnd)
    {
        ux size = _pageEnd - _pageBegin;
        size = BitTweak::GetBelowAlign(size, ux(m_pageSize));
        u8 * currentPageAddress = _pageBegin;

        while(size!=0)
        {
            // 1 - unmap virtual memory
            sys_memory_t memId;
            int retError = sys_mmapper_unmap_memory((sys_addr_t)currentPageAddress, &memId);
            MEMMNG_ASSERT(retError==CELL_OK);

            // 2 - free physical memory
            if (retError==CELL_OK)
            {
                // 2 - free physical memory
                retError = sys_mmapper_free_memory(memId);
                MEMMNG_ASSERT(retError==CELL_OK);
            }

            size -= m_pageSize;
            currentPageAddress += m_pageSize;
        }
        return true;
    }
    
    // commit/decommitArea is letting area management to the system
    // It has to be used in pair, when the decommit size is the same size as the commit
    // and the address is not known
    u8 * CommitArea(ux _size)
    {
        _size = BitTweak::GetAboveAlign(_size, ux(m_pageSize));
        // 1 - allocate physical
        sys_memory_t memId;
        int retError = sys_mmapper_allocate_memory(_size, (m_pageSize==SMALL_PAGE_SIZE)?SYS_MEMORY_GRANULARITY_64K:SYS_MEMORY_GRANULARITY_1M, &memId);
        if (retError!=CELL_OK)
        {
            return NULL;
        }

        // 2 - map to the given address
        sys_addr_t allocatedAddr = NULL;
        retError = sys_mmapper_search_and_map((sys_addr_t)m_pAreaBegin, memId, SYS_MEMORY_PROT_READ_WRITE, &allocatedAddr);
        if (retError!=CELL_OK)
        {
            sys_mmapper_free_memory(memId);
            return NULL;
        }
        return (u8 *)allocatedAddr;
    }

    u8 * CommitArea(u8 * _areaToCommit, ux _size)
    {
        _size = BitTweak::GetAboveAlign(_size, ux(m_pageSize));
        // 1 - allocate physical
        sys_memory_t memId;
        int retError = sys_mmapper_allocate_memory(_size, (m_pageSize==SMALL_PAGE_SIZE)?SYS_MEMORY_GRANULARITY_64K:SYS_MEMORY_GRANULARITY_1M, &memId);
        if (retError!=CELL_OK)
        {
            return NULL;
        }

        // 2 - map to the given address
        retError = sys_mmapper_map_memory((sys_addr_t)_areaToCommit, memId, SYS_MEMORY_PROT_READ_WRITE);
        if (retError!=CELL_OK)
        {
            sys_mmapper_free_memory(memId);
            return NULL;
        }
        return (u8 *)_areaToCommit;
    }

    bool DecommitArea(u8 * _areaCommitedBegin)
    {
        // 1 - unmap virtual memory
        sys_memory_t memId;
        int retError = sys_mmapper_unmap_memory((sys_addr_t)_areaCommitedBegin, &memId);
        MEMMNG_ASSERT(retError==CELL_OK);
        if (retError==CELL_OK)
        {
            // 2 - free physical memory
            retError = sys_mmapper_free_memory(memId);
            MEMMNG_ASSERT(retError==CELL_OK);
            return true;
        }
        return false;
    }


protected:
    u8*					m_pAreaBegin;
    u8*					m_pAreaEnd;
    // (m_pAreaEnd-m_pAreaBegin) should be multiple of 256M

    u8*					m_pPeak; // allocated up to
    u8*					m_pHighMark; // current limit
    // m_pPeak>=m_pHighMark and both in range [m_pAreaBegin, m_pAreaEnd)

    PageSizeType m_pageSize;

    u8 * ReserveVirtualSpace(ux _size)
    {
        sys_addr_t pRet = 0;
        int retError = sys_mmapper_allocate_address(BitTweak::GetAboveAlign(_size, VIRTUAL_SPACE_MINIMAL_PAGE),
            ((m_pageSize==SMALL_PAGE_SIZE)?SYS_MEMORY_PAGE_SIZE_64K:SYS_MEMORY_PAGE_SIZE_1M)
            | SYS_MEMORY_ACCESS_RIGHT_PPU_THR
            | SYS_MEMORY_ACCESS_RIGHT_SPU_THR
            | SYS_MEMORY_ACCESS_RIGHT_RAW_SPU,
            VIRTUAL_SPACE_PAGE_ALIGNMENT,
            &pRet);
        MEMMNG_ASSERT(pRet); // Out of virtual space !!
        MEMMNG_ASSERT(retError==CELL_OK); // Problem reported by OS
        return (u8 *)pRet;
    }

    void ReleaseVirtualSpace(u8 * pageAdressBegin)
    {
        int retError = sys_mmapper_free_address((sys_addr_t)pageAdressBegin);
        MEMMNG_ASSERT(retError==CELL_OK); // Problem reported by OS
    }

};

} // namespace ITF

#endif // ITF_MEMMNGPAGE_PS3_H_
