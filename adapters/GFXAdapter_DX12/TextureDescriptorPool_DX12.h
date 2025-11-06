#pragma once

#include "DX12/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

#include "core/utility.h"
#include "core/container/extendedVector.h"

#include "core/system/Synchronize.h"

namespace ITF::DX12
{
    class Texture;

    class TextureDescriptorPool
    {
    public:
        explicit TextureDescriptorPool(ComPtr<ID3D12Device> _device, u32 nbMaxTexture = 10000);
        TextureDescriptorPool(const TextureDescriptorPool&) = delete;
        ~TextureDescriptorPool() = default;

        TextureDescriptorPool& operator = (const TextureDescriptorPool&) = delete;

        struct Handle;
        [[nodiscard]] Handle registerTexture(ID3D12Resource* res, const char* _debugName);

        [[nodiscard]] ID3D12DescriptorHeap* getHeap() const
        {
            return m_srvHeap.Get();
        }

        void recycleUnused(u64 _nbRenderedFrames);

        struct Handle
        {
        public:
            constexpr Handle() = default;
            Handle(const Handle&) = delete;
            Handle & operator = (const Handle&) = delete;

            Handle(Handle&& other) noexcept
                : m_texPool(other.m_texPool)
                , m_idx(other.m_idx)
            {
                other.m_texPool = nullptr;
                other.m_idx = UX_INVALID;
            }

            ~Handle() { reset(); }

            Handle & operator = (Handle&& other) noexcept
            {
                if (this != &other)
                {
                    reset();

                    m_texPool = other.m_texPool;
                    m_idx = other.m_idx;

                    other.m_texPool = nullptr;
                    other.m_idx = UX_INVALID;
                }
                return *this;
            }

            void reset()
            {
                if (m_texPool != nullptr && m_idx != UX_INVALID)
                {
                    m_texPool->releaseDescriptorIndex(m_idx);
                    m_texPool = nullptr;
                    m_idx = UX_INVALID;
                }
            }

            void set(ID3D12GraphicsCommandList * _cmdList, u32 _slot) const
            {
                _cmdList->SetGraphicsRootDescriptorTable(_slot, m_texPool->getGPUDescriptorHandle(m_idx));
            }

            void markAsUsedAtFrame(u64 _cpuFrameCount)
            {
                ITF_ASSERT(m_texPool != nullptr);
                ITF_ASSERT(m_idx < m_texPool->m_texturePoolEntries.size());
                m_texPool->m_texturePoolEntries[m_idx].lastSubmittedAtFrame = _cpuFrameCount;
            }

        private:
            friend class TextureDescriptorPool;
            explicit Handle(TextureDescriptorPool* _texPool, ux _idx)
                : m_texPool(_texPool)
                , m_idx(_idx)
            {}

            TextureDescriptorPool* m_texPool = nullptr;
            ux m_idx = UX_INVALID;
        };

    private:
        ux allocateDescriptorIndex();
        void releaseDescriptorIndex(ux idx);

        D3D12_CPU_DESCRIPTOR_HANDLE getCPUDescriptorHandle(ux _descriptorIdx) const;
        D3D12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandle(ux _descriptorIdx) const;

        ComPtr<ID3D12Device> m_device{};
        ComPtr<ID3D12DescriptorHeap> m_srvHeap{};
        ux m_srvDescriptorSize = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuSRVHeapHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE m_gpuSRVHeapHandle{};

        struct TexturePoolEntry
        {
        #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
            String8 textureName{}; // for debugging
        #endif
            i64 lastSubmittedAtFrame = -1;
        };
        Vector<TexturePoolEntry> m_texturePoolEntries; // fixed size vector after init.

        // Registering/unregistering can be done in a loading thread.
        // The mutex protect the management of 
        Mutex m_mutex;
        Vector<ux> m_unusedIndex; // Used at registering to get an unused id. 
        Vector<ux> m_waitingUnregisteredIndex; // Filled at unregister with unRegisteredTextures idx.
        // At recycleUnused, the indices of m_waitingUnregisteredIndex that are no more used are sent to m_unusedIds.
    };
}
