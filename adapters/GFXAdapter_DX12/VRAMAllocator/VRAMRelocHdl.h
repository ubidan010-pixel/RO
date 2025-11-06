#pragma once

#include "core/types.h"

#include "VRAMMemoryTypes.h"
#include "VRAMHandleLinks.h"
#include "PoolContainer.h"

#include "DX12/Helpers.h"

namespace ITF::DX12
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
        VRAMRelocHandle & operator = (VRAMRelocHandle&& hdl);
        VRAMRelocHandle & operator += (VRAMChunkSize offset);

        template <VRAMResourceType>
        void alloc(VRAMChunkSize _size, VramMemoryCategory _category = VRAM_Unknown);

        template <VRAMResourceType>
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

    class ResourceVRAMRelocHandle : public VRAMRelocHandle
    {
    public:
        ResourceVRAMRelocHandle() = default;
        ResourceVRAMRelocHandle(const VRAMRelocHandle& hdl);
        ResourceVRAMRelocHandle(const ResourceVRAMRelocHandle& hdl);
        ResourceVRAMRelocHandle(ResourceVRAMRelocHandle&& hdl);

        template <VRAMResourceType _resType>
        void alloc(VRAMChunkSize _size, D3D12_RESOURCE_DESC _desc, VramMemoryCategory _category = VRAM_Unknown)
        {
            m_desc = _desc;
            VRAMRelocHandle::alloc<_resType>(_size, _category);
        }

        template <VRAMResourceType _resType>
        void allocAligned(VRAMChunkSize _size, VRAMChunkSize _alignment, D3D12_RESOURCE_DESC _desc, VramMemoryCategory _category = VRAM_Unknown)
        {
            m_desc = _desc;
            VRAMRelocHandle::allocAligned<_resType>(_size, _alignment, _category);
        }

        virtual ~ResourceVRAMRelocHandle() override = default;

        ResourceVRAMRelocHandle& operator=(const VRAMRelocHandle& hdl);
        ResourceVRAMRelocHandle& operator=(const ResourceVRAMRelocHandle& hdl);
        ResourceVRAMRelocHandle& operator=(ResourceVRAMRelocHandle&& hdl);

        ID3D12Resource* getD3DResource() const { return m_resource.Get(); }
        VRAMAllocator::GPUStorage getStorage() const { return m_storage; }
        const D3D12_RESOURCE_DESC& getResourceDesc() { return m_desc; }

    private:
        virtual VRAMAllocator::GPUStorage resolveAddress() const override;
        virtual void relocateAddress(VRAMAllocator::GPUStorage _storage) override;

        ComPtr<ID3D12Resource> m_resource{};
        D3D12_RESOURCE_DESC m_desc{};
        VRAMAllocator::GPUStorage m_storage{};
    };

    class UploadBufferVRAMRelocHandle : public ResourceVRAMRelocHandle
    {
    public:
        using Parent = ResourceVRAMRelocHandle;
        using ResourceVRAMRelocHandle::ResourceVRAMRelocHandle;

        void alloc(VRAMChunkSize _size, VramMemoryCategory _category = VRAM_Unknown)
        {
            Parent::alloc<VRAMResourceType::RESOURCE_UPLOAD>(_size, CD3DX12_RESOURCE_DESC::Buffer(_size), _category);
        }

        void allocAligned(VRAMChunkSize _size, VRAMChunkSize _alignment, VramMemoryCategory _category = VRAM_Unknown)
        {
            Parent::allocAligned<VRAMResourceType::RESOURCE_UPLOAD>(_size, _alignment, CD3DX12_RESOURCE_DESC::Buffer(_size, D3D12_RESOURCE_FLAG_NONE, _alignment), _category);
        }
    };

    class DefaultResourceBufferVRAMRelocHandle : public ResourceVRAMRelocHandle
    {
    public:
        using Parent = ResourceVRAMRelocHandle;
        using ResourceVRAMRelocHandle::ResourceVRAMRelocHandle;

        void alloc(VRAMChunkSize _size, D3D12_RESOURCE_DESC _desc, VramMemoryCategory _category = VRAM_Unknown)
        {
            Parent::alloc<VRAMResourceType::RESOURCE>(_size, _desc, _category);
        }

        void allocAligned(VRAMChunkSize _size, VRAMChunkSize _alignment, D3D12_RESOURCE_DESC _desc, VramMemoryCategory _category = VRAM_Unknown)
        {
            Parent::allocAligned<VRAMResourceType::RESOURCE>(_size, _alignment, _desc, _category);
        }

        void alloc(VRAMChunkSize _size, VramMemoryCategory _category = VRAM_Unknown)
        {
            Parent::alloc<VRAMResourceType::RESOURCE>(_size, CD3DX12_RESOURCE_DESC::Buffer(_size), _category);
        }

        void allocAligned(VRAMChunkSize _size, VRAMChunkSize _alignment, VramMemoryCategory _category = VRAM_Unknown)
        {
            Parent::allocAligned<VRAMResourceType::RESOURCE>(_size, _alignment, CD3DX12_RESOURCE_DESC::Buffer(_size, D3D12_RESOURCE_FLAG_NONE, _alignment), _category);
        }
    };

} // namespace ITF::DX12
