#include "precompiled_GFXAdapter_PS5.h"

#include "Core/math/BitTweak.h"

#include "VRAMRelocHdl.h"
#include "VRAMMemoryTypes.h"
#include "ChunkContainer.h"
#include "PoolContainer.h"

namespace ITF::Gnmp
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
        if (hdl.isValid())
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
        if (isValid())
        {
            unlinkHandle();
        }
    }

    void VRAMRelocHandle::operator = (const VRAMRelocHandle & hdl)
    {
        if (!hdl.isValid()) // ask to invalidate
        {
            if (isValid())
            {
                unlinkHandle();
            }
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::InvalidAddress);
        }
        else
        {
            {
                VRAM_RELOC_HDL_AUTO_LOCK();
                if (isValid())
                {
                    unlinkHandle();
                }
                next = hdl.next;
                prev = &hdl;
                hdl.next = this;
                if (next!=0)
                {
                    VRAM_ALLOC_ASSERT(next->prev == &hdl);
                    next->prev = this;
                }
            }
            relocateAddress(hdl.resolveAddress());
        }
    }

    void VRAMRelocHandle::operator += (VRAMChunkSize offset)
    {
        VRAMAddress vramAddr = resolveAddress();
        vramAddr += offset;
        relocateAddress(vramAddr);
    }

    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    void VRAMRelocHandle::alloc(VRAMChunkSize size, VramMemoryCategory cat)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = VRAMAllocator::ChunkContainer::InvalidIndex;
        VRAMAllocator::ChunkContainer * chunkContainer = 0;
        VRAMAllocator::PoolContainer::getptr()->alloc<resType, memBus>(size, cat, chunkContainer, idx);
        if (chunkContainer==0)
        {
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::InvalidAddress);
            return;
        }
        VRAM_ALLOC_ASSERT(chunkContainer->GetAddress(idx) == BitTweak::GetAboveAlign(chunkContainer->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        chunkContainer->LinkRelocHandle(*this, idx);
        VRAMAddress vramAddr = chunkContainer->GetAddress(idx);
        relocateAddress(vramAddr);
    }

    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    void VRAMRelocHandle::allocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = VRAMAllocator::ChunkContainer::InvalidIndex;
        VRAMAllocator::ChunkContainer * chunkContainer = 0;
        VRAMAllocator::PoolContainer::getptr()->allocAligned<resType, memBus>(size, alignment, cat, chunkContainer, idx);
        if (chunkContainer==0)
        {
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::InvalidAddress);
            return;
        }
        VRAM_ALLOC_ASSERT(chunkContainer->GetAddress(idx) == BitTweak::GetAboveAlign(chunkContainer->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        chunkContainer->LinkRelocHandle(*this, idx);
        VRAMAddress vramAddr = chunkContainer->GetAddress(idx);
        relocateAddress(vramAddr);
    }

    // explicit instantiations
    template void VRAMRelocHandle::alloc<READ_WRITE, VRAMMemoryBus::DIRECT>(VRAMChunkSize size, VramMemoryCategory cat);
    template void VRAMRelocHandle::alloc<READ_ONLY, VRAMMemoryBus::DIRECT>(VRAMChunkSize size, VramMemoryCategory cat);
    template void VRAMRelocHandle::allocAligned<READ_WRITE, VRAMMemoryBus::DIRECT>(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat);
    template void VRAMRelocHandle::allocAligned<READ_ONLY, VRAMMemoryBus::DIRECT>(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat);

    void VRAMRelocHandle::allocInContainer(VRAMChunkSize size, VRAMAllocator::ChunkContainer * _container, VramMemoryCategory type)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = _container->Alloc(size, type);
        if (idx==VRAMAllocator::ChunkContainer::InvalidIndex)
        {
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::InvalidAddress);
            return;
        }
        VRAM_ALLOC_ASSERT(_container->GetAddress(idx) == BitTweak::GetAboveAlign(_container->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        _container->LinkRelocHandle(*this, idx);
        VRAMAddress vramAddr = _container->GetAddress(idx);
        relocateAddress(vramAddr);        
    }

    void VRAMRelocHandle::allocAlignedInContainer(VRAMChunkSize size, VRAMChunkSize alignment, VRAMAllocator::ChunkContainer * _container, VramMemoryCategory type)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = _container->AllocAligned(size, alignment, type);
        if (idx==VRAMAllocator::ChunkContainer::InvalidIndex)
        {
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::InvalidAddress);
            return;
        }
        VRAM_ALLOC_ASSERT(_container->GetAddress(idx) == BitTweak::GetAboveAlign(_container->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        _container->LinkRelocHandle(*this, idx);
        VRAMAddress vramAddr = _container->GetAddress(idx);
        relocateAddress(vramAddr);
    }

    VRAMChunkSize VRAMRelocHandle::memSize()
    {
        if (isValid())
        {
            VRAM_RELOC_HDL_AUTO_LOCK();
            // get the pool from the first VRAMHandleLink
            const VRAMHandleLinks * containerAnchor = prev;
            while(containerAnchor->prev!=0)
            {
                containerAnchor = containerAnchor->prev;
            }
            
            VRAMAllocator::ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->findPoolFromLink(*containerAnchor);
            return chunkContainer->MemSize(*containerAnchor);
        }
        return 0;
    }

    void VRAMRelocHandle::unlinkHandle()
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        VRAM_ALLOC_ASSERT(prev != 0);
        prev->next = next; 
        if (next!=0)
        {
            VRAM_ALLOC_ASSERT(next->prev == this);
            next->prev = prev;
        }
        if (!prev->isValid()) // chunk entry ?
        {
            VRAMAllocator::ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->findPoolFromLink(*prev);
            chunkContainer->FreeRelocHandle(*prev);
            if (chunkContainer->IsEmpty()
                && !chunkContainer->IsStatic()) 
            {
                VRAMAllocator::PoolContainer::getptr()->eraseContainer(chunkContainer);
            }
        }
    }

    // Invalid constant
    const SimpleVRAMRelocHandle InvalidRelocHandle;

    // 
    SimpleVRAMRelocHandle::SimpleVRAMRelocHandle()
    : m_vramAddr(VRAMAllocator::InvalidAddress)
    {
    }

    SimpleVRAMRelocHandle::SimpleVRAMRelocHandle(const VRAMRelocHandle & hdl)
    : VRAMRelocHandle(hdl)
    , m_vramAddr(VRAMAllocator::InvalidAddress)
    {
        if (hdl.isValid())
        {
            SimpleVRAMRelocHandle simpleHdl;
            simpleHdl = hdl;
            relocateAddress(simpleHdl.m_vramAddr);
        }
    }

    void SimpleVRAMRelocHandle::relocateAddress(VRAMAddress vramAddr)
    {
        m_vramAddr = vramAddr;
    }

    VRAMAddress SimpleVRAMRelocHandle::resolveAddress() const
    {
        return m_vramAddr;
    }

    // 
    OffsetPtrRelocHandle::OffsetPtrRelocHandle()
        : m_pAddrToPatch(nullptr)
        , m_vramAddr(VRAMAllocator::InvalidAddress)
    {}

    OffsetPtrRelocHandle::OffsetPtrRelocHandle(VRAMAddress * pAddrToPatch)
        : m_pAddrToPatch(pAddrToPatch)
        , m_vramAddr(VRAMAllocator::InvalidAddress)
    {}

    void OffsetPtrRelocHandle::setRelocPtr(VRAMAddress * pAddrToPatch)
    {
        m_pAddrToPatch = pAddrToPatch;

        if (m_pAddrToPatch!=nullptr)
        {
            (*m_pAddrToPatch) = nullptr;
        }
    }

    void OffsetPtrRelocHandle::relocateAddress(VRAMAddress vramAddr)
    {
        if (m_pAddrToPatch)
        {
            m_vramAddr = vramAddr;
            (*m_pAddrToPatch) = vramAddr;
        }
        else
        {
            VRAM_ALLOC_ASSERT(m_vramAddr == VRAMAllocator::InvalidAddress);
        }
    }

    VRAMAddress OffsetPtrRelocHandle::resolveAddress() const
    {
        ITF_ASSERT(!m_pAddrToPatch || m_vramAddr == *m_pAddrToPatch);
        return m_vramAddr;
    }
} // namespace ITF::Gnm
