#include "precompiled_GFXAdapter_GCM.h"

#include "GCM_API.h"

#include "VRAMRelocHdl.h"
#include "VRAMAllocator/VRAMMemoryTypes.h"
#include "VRAMAllocator/ChunkContainer.h"
#include "VRAMAllocator/PoolContainer.h"

namespace ITF
{
namespace GCM
{

    #define VRAM_RELOC_HDL_AUTO_LOCK() ScopeLockMutex autoLock(VRAMAllocator::PoolContainer::getptr()->m_mutex)

    //////////////////////////////////////////////////////////////////////////
    // Reloc handle Management
    //////////////////////////////////////////////////////////////////////////

    VRAMRelocHandle::VRAMRelocHandle(const VRAMRelocHandle & hdl)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        next = 0;
        prev = 0;
        if (hdl.IsValid())
        {
            next = hdl.next;
            prev = &hdl;
            hdl.next = this;
            if (next!=0)
            {
                VRAM_ALLOC_ASSERT(next->prev == &hdl);
                next->prev = this;
            }
        }
    }


    VRAMRelocHandle::~VRAMRelocHandle()
    {
        if (IsValid())
        {
            UnlinkHandle();
        }
    }

    void VRAMRelocHandle::operator = (const VRAMRelocHandle & hdl)
    {
        if (IsValid())
        {
            UnlinkHandle();
        }
        if (!hdl.IsValid()) // ask to invalidate
        {
            next = 0;
            prev = 0;
            RelocateAddress(VRAMAllocator::InvalidAddress, VRAMAllocator::InvalidOffset, CELL_GCM_LOCATION_LOCAL);
        }
        else
        {
            {
                VRAM_RELOC_HDL_AUTO_LOCK();
                next = hdl.next;
                prev = &hdl;
                hdl.next = this;
                if (next!=0)
                {
                    VRAM_ALLOC_ASSERT(next->prev == &hdl);
                    next->prev = this;
                }
            }
            VRAMAddress vramEA;
            VRAMOffset vramOffset;
            VRAMLocation location;
            hdl.ResolveAddress(vramEA, vramOffset, location);
            RelocateAddress(vramEA, vramOffset, location);
        }
    }

    void VRAMRelocHandle::operator += (u32 offset)
    {
        VRAMAddress vramEA;
        VRAMOffset vramOffset;
        VRAMLocation location;
        ResolveAddress(vramEA, vramOffset, location);
        vramEA += offset;
        vramOffset += offset;
        RelocateAddress(vramEA, vramOffset, location);
    }

    template <void (VRAMAllocator::PoolContainer::* AllocMethod)(VRAMChunkSize, VramMemoryType, VRAMAllocator::ChunkContainer * &, VRAMAllocator::ChunkIdx &)>
    void VRAMRelocHandle::TplAlloc(VRAMChunkSize size, VramMemoryType type)
    {
        if (IsValid())
        {
            UnlinkHandle();
        }
        VRAM_RELOC_HDL_AUTO_LOCK();
        VRAMAllocator::ChunkIdx idx = VRAMAllocator::ChunkContainer::InvalidIndex;
        VRAMAllocator::ChunkContainer * chunkContainer = 0;
        (VRAMAllocator::PoolContainer::getptr()->*AllocMethod)(size, type, chunkContainer, idx);
        if (chunkContainer==0)
        {
            next = 0;
            prev = 0;
            RelocateAddress(VRAMAllocator::InvalidAddress, VRAMAllocator::InvalidOffset, CELL_GCM_LOCATION_LOCAL);
            return;
        }
        VRAM_ALLOC_ASSERT(chunkContainer->GetAddress(idx) == (VRAMAddress)BitTweak::GetAboveAlign((ChunkSize)chunkContainer->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        chunkContainer->LinkRelocHandle(*this, idx);
        VRAMAddress vramEA = chunkContainer->GetAddress(idx);
        VRAMOffset vramOffset = chunkContainer->GetOffset(idx);
        RelocateAddress(vramEA, vramOffset, chunkContainer->GetLocation());
    }

    template <void (VRAMAllocator::PoolContainer::* AllocAlignedMethod)(VRAMChunkSize, VRAMChunkSize align, VramMemoryType, VRAMAllocator::ChunkContainer * &, VRAMAllocator::ChunkIdx &)>
    void VRAMRelocHandle::TplAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type)
    {
        if (IsValid())
        {
            UnlinkHandle();
        }
        VRAM_RELOC_HDL_AUTO_LOCK();
        VRAMAllocator::ChunkIdx idx = VRAMAllocator::ChunkContainer::InvalidIndex;
        VRAMAllocator::ChunkContainer * chunkContainer = 0;
        (VRAMAllocator::PoolContainer::getptr()->*AllocAlignedMethod)(size, alignment, type, chunkContainer, idx);
        if (chunkContainer==0)
        {
            next = 0;
            prev = 0;
            RelocateAddress(VRAMAllocator::InvalidAddress, VRAMAllocator::InvalidOffset, CELL_GCM_LOCATION_LOCAL);
            return;
        }
        VRAM_ALLOC_ASSERT(chunkContainer->GetAddress(idx) == (VRAMAddress)BitTweak::GetAboveAlign((VRAMChunkSize)chunkContainer->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        chunkContainer->LinkRelocHandle(*this, idx);
        VRAMAddress vramEA = chunkContainer->GetAddress(idx);
        VRAMOffset vramOffset = chunkContainer->GetOffset(idx);
        RelocateAddress(vramEA, vramOffset, chunkContainer->GetLocation());
    }

    void VRAMRelocHandle::Alloc(VRAMChunkSize size, VramMemoryType type)
    {
        TplAlloc<&VRAMAllocator::PoolContainer::Alloc>(size, type);
    }

    void VRAMRelocHandle::AllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type)
    {
        TplAllocAligned<&VRAMAllocator::PoolContainer::AllocAligned>(size, alignment, type);
    }

    // specify the memory area
    void VRAMRelocHandle::LocalAlloc(VRAMChunkSize size, VramMemoryType type)
    {
        TplAlloc<&VRAMAllocator::PoolContainer::LocalAlloc>(size, type);
    }

    void VRAMRelocHandle::LocalAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type)
    {
        TplAllocAligned<&VRAMAllocator::PoolContainer::LocalAllocAligned>(size, alignment, type);
    }

    void VRAMRelocHandle::MainAlloc(VRAMChunkSize size, VramMemoryType type)
    {
        TplAlloc<&VRAMAllocator::PoolContainer::MainAlloc>(size, type);
    }

    void VRAMRelocHandle::MainAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type)
    {
        TplAllocAligned<&VRAMAllocator::PoolContainer::MainAllocAligned>(size, alignment, type);
    }

    void VRAMRelocHandle::MainReportAlloc(VRAMChunkSize size, VramMemoryType type)
    {
        TplAlloc<&VRAMAllocator::PoolContainer::MainReportAlloc>(size, type);
    }

    void VRAMRelocHandle::MainReportAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type)
    {
        TplAllocAligned<&VRAMAllocator::PoolContainer::MainReportAllocAligned>(size, alignment, type);
    }

    VRAMChunkSize VRAMRelocHandle::MemSize()
    {
        if (IsValid())
        {
            VRAM_RELOC_HDL_AUTO_LOCK();
            // get the pool from the first VRAMHandleLink
            const VRAMHandleLinks * containerAnchor = prev;
            while(containerAnchor->prev!=0)
            {
                containerAnchor = containerAnchor->prev;
            }
            
            VRAMAllocator::ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->FindPoolFromLink(*containerAnchor);
            return chunkContainer->MemSize(*containerAnchor);
        }
        return 0;
    }

    void VRAMRelocHandle::UnlinkHandle()
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        VRAM_ALLOC_ASSERT(prev != 0);
        prev->next = next; 
        if (next!=0)
        {
            VRAM_ALLOC_ASSERT(next->prev == this);
            next->prev = prev;
        }
        if (!prev->IsValid()) // chunk entry ?
        {
            VRAMAllocator::ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->FindPoolFromLink(*prev);
            chunkContainer->FreeRelocHandle(*prev);
            if (chunkContainer->GetLocation()==CELL_GCM_LOCATION_MAIN // we never delete a LOCAL pool
                && chunkContainer->IsEmpty()
                && !chunkContainer->IsStatic()) 
            {
                VRAMAllocator::PoolContainer::getptr()->EraseContainer(chunkContainer);
            }
        }
    }

    // Invalid constant
    const SimpleVRAMRelocHandle InvalidRelocHandle;

    // 
    SimpleVRAMRelocHandle::SimpleVRAMRelocHandle()
    : m_vramEA(VRAMAllocator::InvalidAddress), m_vramOffset(VRAMAllocator::InvalidOffset), m_location(CELL_GCM_LOCATION_LOCAL)
    {
    }

    SimpleVRAMRelocHandle::SimpleVRAMRelocHandle(const VRAMRelocHandle & hdl)
    : VRAMRelocHandle(hdl)
    , m_vramEA(VRAMAllocator::InvalidAddress)
    , m_vramOffset(VRAMAllocator::InvalidOffset)
    , m_location(CELL_GCM_LOCATION_LOCAL)
    {
        if (hdl.IsValid())
        {
            SimpleVRAMRelocHandle simpleHdl;
            simpleHdl = hdl;
            RelocateAddress(simpleHdl.m_vramEA, simpleHdl.m_vramOffset, simpleHdl.m_location);
        }
    }

    void SimpleVRAMRelocHandle::RelocateAddress(VRAMAddress vramEA, VRAMOffset vramOffset, VRAMLocation location)
    {
        m_vramEA = vramEA;
        m_vramOffset = vramOffset;
        m_location = location;
    }

    void SimpleVRAMRelocHandle::ResolveAddress(VRAMAddress & vramEA, VRAMOffset & vramOffset, VRAMLocation & location) const
    {
        vramEA = m_vramEA;
        vramOffset = m_vramOffset;
        location = m_location;
    }

    // 
    OffsetPtrRelocHandle::OffsetPtrRelocHandle() : m_pVramOffset(0), m_vramEA(VRAMAllocator::InvalidAddress), m_pLocation(0) {}

    OffsetPtrRelocHandle::OffsetPtrRelocHandle(VRAMOffset * pOffset, VRAMLocation * pLocation) : m_pVramOffset(pOffset), m_vramEA(VRAMAllocator::InvalidAddress), m_pLocation(pLocation) {}

    void OffsetPtrRelocHandle::SetRelocPtr(VRAMOffset * pOffset, VRAMLocation * pLocation)
    {
        m_pVramOffset = pOffset;
        m_pLocation = pLocation;

        if (m_pVramOffset!=0)
        {
            (*m_pVramOffset) = 0;
        }
        if (m_pLocation!=0)
        {
            (*m_pLocation) = CELL_GCM_LOCATION_LOCAL;
        }
    }

    void OffsetPtrRelocHandle::RelocateAddress(VRAMAddress vramEA, VRAMOffset vramOffset, VRAMLocation location)
    {
        if (m_pVramOffset)
        {
            (*m_pVramOffset) = vramOffset;
            m_vramEA = vramEA;
            if (m_pLocation)
            {
                (*m_pLocation) = location;
            }
        }
        else
        {
            VRAM_ALLOC_ASSERT(m_vramEA == VRAMAllocator::InvalidAddress);
        }
    }

    void OffsetPtrRelocHandle::ResolveAddress(VRAMAddress & vramEA, VRAMOffset & vramOffset, VRAMLocation & location) const
    {
        if (m_pVramOffset)
        {
            vramOffset = (*m_pVramOffset);
            vramEA = m_vramEA;
        }
        else
        {
            VRAM_ALLOC_ASSERT(m_vramEA == VRAMAllocator::InvalidAddress);
            vramEA = VRAMAllocator::InvalidAddress;
            vramOffset = VRAMAllocator::InvalidOffset;
        }
        if (m_pLocation)
        {
            location = (*m_pLocation);
        }
    }
} // GCM
} // ITF
