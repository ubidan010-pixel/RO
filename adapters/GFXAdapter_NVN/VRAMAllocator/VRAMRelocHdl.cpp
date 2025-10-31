#include "precompiled_GFXAdapter_NVN.h"

#include "Core/math/BitTweak.h"

#include "VRAMRelocHdl.h"
#include "VRAMMemoryTypes.h"
#include "ChunkContainer.h"
#include "PoolContainer.h"

#include "VRAMAllocator.h"

namespace ITF::NVN
{

#define VRAM_RELOC_HDL_AUTO_LOCK() ScopeLockMutex autoLock(VRAMAllocator::PoolContainer::getptr()->m_mutex)

    //////////////////////////////////////////////////////////////////////////
    // Reloc handle Management
    //////////////////////////////////////////////////////////////////////////

    VRAMRelocHandle::VRAMRelocHandle(const VRAMRelocHandle& hdl)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        next = nullptr;
        prev = nullptr;
        if (hdl.isValid())
        {
            next = hdl.next;
            prev = &hdl;
            hdl.next = this;
            if (next != nullptr)
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

    VRAMRelocHandle& VRAMRelocHandle::operator = (const VRAMRelocHandle& hdl)
    {
        if (this == &hdl)
            return *this; // self assignment

        if (!hdl.isValid()) // ask to invalidate
        {
            if (isValid())
            {
                unlinkHandle();
            }
            next = nullptr;
            prev = nullptr;

            relocateAddress(VRAMAllocator::GPUStorage{});
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
                if (next != 0)
                {
                    VRAM_ALLOC_ASSERT(next->prev == &hdl);
                    next->prev = this;
                }
            }
            relocateAddress(hdl.resolveAddress());
        }
        return *this;
    }

    VRAMRelocHandle & VRAMRelocHandle::operator += (VRAMChunkSize offset)
    {
        VRAMAllocator::GPUStorage storage = resolveAddress();
        storage.offset += offset;
        relocateAddress(storage);
        return *this;
    }

    template <VRAMResourceType resType>
    void VRAMRelocHandle::alloc(VRAMChunkSize size, VramMemoryCategory cat)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = VRAMAllocator::ChunkContainer::InvalidIndex;
        VRAMAllocator::ChunkContainer* chunkContainer = 0;
        VRAMAllocator::PoolContainer::getptr()->alloc<resType>(size, cat, chunkContainer, idx);
        if (chunkContainer == 0)
        {
            next = nullptr;
            prev = nullptr;
            relocateAddress(VRAMAllocator::GPUStorage{});
            return;
        }
        VRAM_ALLOC_ASSERT(chunkContainer->GetAddress(idx) == BitTweak::GetAboveAlign(chunkContainer->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        chunkContainer->LinkRelocHandle(*this, idx);
        relocateAddress(chunkContainer->GetStorage(idx));
    }

    template <VRAMResourceType resType>
    void VRAMRelocHandle::allocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = VRAMAllocator::ChunkContainer::InvalidIndex;
        VRAMAllocator::ChunkContainer* chunkContainer = 0;
        VRAMAllocator::PoolContainer::getptr()->allocAligned<resType>(size, alignment, cat, chunkContainer, idx);
        if (chunkContainer == 0)
        {
            next = nullptr;
            prev = nullptr;
            relocateAddress(VRAMAllocator::GPUStorage{});
            return;
        }
        VRAM_ALLOC_ASSERT(chunkContainer->GetAddress(idx) == BitTweak::GetAboveAlign(chunkContainer->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        chunkContainer->LinkRelocHandle(*this, idx);
        relocateAddress(chunkContainer->GetStorage(idx));
    }

    // explicit instantiations
    template void VRAMRelocHandle::alloc<VRAMResourceType::RENDER_TARGET>(VRAMChunkSize size, VramMemoryCategory cat);
    template void VRAMRelocHandle::alloc<VRAMResourceType::RESOURCE>(VRAMChunkSize size, VramMemoryCategory cat);
    template void VRAMRelocHandle::alloc<VRAMResourceType::TEXTURE>(VRAMChunkSize size, VramMemoryCategory cat);
    template void VRAMRelocHandle::alloc<VRAMResourceType::SHADER>(VRAMChunkSize size, VramMemoryCategory cat);
    template void VRAMRelocHandle::allocAligned<VRAMResourceType::RENDER_TARGET>(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat);
    template void VRAMRelocHandle::allocAligned<VRAMResourceType::RESOURCE>(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat);
    template void VRAMRelocHandle::allocAligned<VRAMResourceType::TEXTURE>(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat);
    template void VRAMRelocHandle::allocAligned<VRAMResourceType::SHADER>(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryCategory cat);

    void VRAMRelocHandle::allocInContainer(VRAMChunkSize size, VRAMAllocator::ChunkContainer* _container, VramMemoryCategory type)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = _container->Alloc(size, type);
        if (idx == VRAMAllocator::ChunkContainer::InvalidIndex)
        {
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::GPUStorage{});
            return;
        }
        VRAM_ALLOC_ASSERT(_container->GetAddress(idx) == BitTweak::GetAboveAlign(_container->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        _container->LinkRelocHandle(*this, idx);
        relocateAddress(_container->GetStorage(idx));
    }

    void VRAMRelocHandle::allocAlignedInContainer(VRAMChunkSize size, VRAMChunkSize alignment, VRAMAllocator::ChunkContainer* _container, VramMemoryCategory type)
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        if (isValid())
        {
            unlinkHandle();
        }
        VRAMAllocator::ChunkIdx idx = _container->AllocAligned(size, alignment, type);
        if (idx == VRAMAllocator::ChunkContainer::InvalidIndex)
        {
            next = 0;
            prev = 0;
            relocateAddress(VRAMAllocator::GPUStorage{});
            return;
        }
        VRAM_ALLOC_ASSERT(_container->GetAddress(idx) == BitTweak::GetAboveAlign(_container->GetAddress(idx), VRAMAllocator::MinimalAlignment));
        _container->LinkRelocHandle(*this, idx);
        relocateAddress(_container->GetStorage(idx));
    }

    VRAMChunkSize VRAMRelocHandle::memSize() const
    {
        if (isValid())
        {
            return resolveAddress().size;
        }
        return 0;
    }

    void VRAMRelocHandle::unlinkHandle()
    {
        VRAM_RELOC_HDL_AUTO_LOCK();
        VRAM_ALLOC_ASSERT(prev != nullptr);
        prev->next = next;
        if (next != nullptr)
        {
            VRAM_ALLOC_ASSERT(next->prev == this);
            next->prev = prev;
        }
        if (!prev->isValid()) // chunk entry ?
        {
            VRAMAllocator::ChunkContainer* chunkContainer = VRAMAllocator::PoolContainer::getptr()->findPoolFromLink(*prev);
            chunkContainer->FreeRelocHandle(*prev);
            if (chunkContainer->IsEmpty()
                && !chunkContainer->IsStatic())
            {
                VRAMAllocator::PoolContainer::getptr()->eraseContainer(chunkContainer);
            }
        }
    }


    // SimpleVRAMRelocHandle

    SimpleVRAMRelocHandle::SimpleVRAMRelocHandle(const VRAMRelocHandle& hdl)
        : VRAMRelocHandle(hdl)
    {
        if (hdl.isValid())
        {
            // We could just call relocateAddress(hdl.resolveAddress()),
            // but the resolveAddress is protected.
            SimpleVRAMRelocHandle simpleHdl;
            simpleHdl = hdl;
            relocateAddress(simpleHdl.m_storage);
        }
    }

    void SimpleVRAMRelocHandle::relocateAddress(VRAMAllocator::GPUStorage _storage)
    {
        m_storage = _storage;
    }

    VRAMAllocator::GPUStorage SimpleVRAMRelocHandle::resolveAddress() const
    {
        return m_storage;
    }

    BufferVRAMRelocHandle::BufferVRAMRelocHandle(const VRAMRelocHandle& hdl)
        : VRAMRelocHandle(hdl)
    {
        if (hdl.isValid())
        {
            BufferVRAMRelocHandle newHdl;
            newHdl = hdl;
            relocateAddress(newHdl.resolveAddress());
        }
    }

    void BufferVRAMRelocHandle::relocateAddress(VRAMAllocator::GPUStorage _storage)
    {
        m_buffer.reset();
        if (_storage.memoryPool != nullptr)
        {
            nvn::BufferBuilder builder;
            builder.SetDefaults()
                .SetDevice(VRAMAllocator::getDevice())
                .SetStorage(_storage.memoryPool, _storage.offset, _storage.size);
                
            m_buffer = makeUnique<nvn::Buffer>();
            m_buffer->Initialize(&builder);
        }
    }

    VRAMAllocator::GPUStorage BufferVRAMRelocHandle::resolveAddress() const
    {
        if (m_buffer)
            return VRAMAllocator::GPUStorage{ m_buffer->GetMemoryPool(), VRAMOffset(m_buffer->GetMemoryOffset()), VRAMChunkSize(m_buffer->GetSize())};
        else
            return VRAMAllocator::GPUStorage{};
    }

} // namespace ITF::NVN
