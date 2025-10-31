#pragma once

#include "VRAMMemoryTypes.h"
#include "VRAMHandleLinks.h"
#include "PoolContainer.h"

#include "NVN/Helpers.h"

namespace ITF::NVN
{
    struct SizeAlign
    {
        u64 m_size;
        u64 m_align;
    };

    namespace VRAMAllocator
    {
        class ChunkContainer;
        class PoolContainer;
    }

    class VRAMRelocHandle : public VRAMHandleLinks
    {
    public:
        VRAMRelocHandle() { next = nullptr; prev = nullptr; }

        virtual ~VRAMRelocHandle();

        VRAMRelocHandle & operator = (const VRAMRelocHandle & hdl);
        VRAMRelocHandle & operator += (VRAMChunkSize offset);

        template <VRAMResourceType = VRAMResourceType::RESOURCE>
        void alloc(VRAMChunkSize _size, VramMemoryCategory _category = VRAM_Unknown);

        template <VRAMResourceType = VRAMResourceType::RESOURCE>
        void allocAligned(VRAMChunkSize _size, VRAMChunkSize _alignment, VramMemoryCategory _category = VRAM_Unknown);

        // Specify the container
        void allocInContainer(VRAMChunkSize size, VRAMAllocator::ChunkContainer* _container, VramMemoryCategory type = VRAM_Unknown);
        void allocAlignedInContainer(VRAMChunkSize size, VRAMChunkSize alignment, VRAMAllocator::ChunkContainer* _container, VramMemoryCategory type = VRAM_Unknown);

        VRAMChunkSize memSize() const;

    protected:
        virtual void relocateAddress(VRAMAllocator::GPUStorage _storage) = 0;
        virtual VRAMAllocator::GPUStorage resolveAddress() const = 0;

        VRAMRelocHandle(const VRAMRelocHandle & hdl);

    private:
        void unlinkHandle();
    };

    // Simpler version. Keep internally the storage for immediate access
    class SimpleVRAMRelocHandle : public VRAMRelocHandle
    {
    public:
        SimpleVRAMRelocHandle() = default;
        SimpleVRAMRelocHandle(const VRAMRelocHandle & hdl);
        virtual ~SimpleVRAMRelocHandle() override = default;

        SimpleVRAMRelocHandle& operator = (const SimpleVRAMRelocHandle& hdl) { VRAMRelocHandle::operator = (hdl); return *this; }
        SimpleVRAMRelocHandle& operator += (VRAMChunkSize offset) { VRAMRelocHandle::operator += (offset); return *this; }

        VRAMAllocator::GPUStorage getStorage() const { return m_storage; }

    protected:
        virtual void relocateAddress(VRAMAllocator::GPUStorage _storage) override;
        virtual VRAMAllocator::GPUStorage resolveAddress() const override;

    private:
        VRAMAllocator::GPUStorage m_storage{};
    };

    class BufferVRAMRelocHandle : public VRAMRelocHandle
    {
    public:
        BufferVRAMRelocHandle() = default;
        BufferVRAMRelocHandle(const VRAMRelocHandle& hdl);
        virtual ~BufferVRAMRelocHandle() override = default;

        BufferVRAMRelocHandle& operator = (const BufferVRAMRelocHandle& hdl) { VRAMRelocHandle::operator = (hdl); return *this; }
        BufferVRAMRelocHandle& operator += (VRAMChunkSize offset) { VRAMRelocHandle::operator += (offset); return *this; }

        nvn::Buffer * getBuffer() const { return m_buffer.get(); }

    protected:
        virtual void relocateAddress(VRAMAllocator::GPUStorage _storage) override;
        virtual VRAMAllocator::GPUStorage resolveAddress() const override;

    private:
        UniquePtr<nvn::Buffer> m_buffer;
    };

} // namespace ITF::NVN
