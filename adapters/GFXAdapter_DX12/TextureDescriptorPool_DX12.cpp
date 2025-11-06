#include "precompiled_GFXAdapter_DX12.h"

#include "TextureDescriptorPool_DX12.h"

#include "DX12/Helpers.h"
#include "Texture_DX12.h"
#include "core/utility.h"

#include "core/system/Synchronize.h"

#include <numeric>

namespace ITF::DX12
{

    TextureDescriptorPool::TextureDescriptorPool(ComPtr<ID3D12Device> _device, u32 _nbMaxTexture)
        : m_device(std::move(_device))
        , m_srvDescriptorSize(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
    {

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = _nbMaxTexture;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        ITF_VERIFY_HR_CALL(m_device->CreateDescriptorHeap(&srvHeapDesc, DX12_IID_COMPTR_ARGS(m_srvHeap)));

        if (m_srvHeap != nullptr)
        {
            m_cpuSRVHeapHandle = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
            m_gpuSRVHeapHandle = m_srvHeap->GetGPUDescriptorHandleForHeapStart();

            m_unusedIndex.resize(_nbMaxTexture);
            // By putting all the indices in the unused list, it can be used at registering.
            // We put the indices in reverse order so that we can use push/back and start at the beginning of the vector.
            std::iota(std::make_reverse_iterator(m_unusedIndex.end()), std::make_reverse_iterator(m_unusedIndex.begin()), 0);

            // resize once for all the pool entries.
            m_texturePoolEntries.resize(_nbMaxTexture);
        }
    }

    ux TextureDescriptorPool::allocateDescriptorIndex()
    {
        // locking all along the registration process as a first implementation.
        // We probably just need to lock when we access m_unusedIndex.
        ScopeLock lock(m_mutex);

        ux idx = UX_INVALID;
        if (!m_unusedIndex.empty())
        {
            idx = m_unusedIndex.back();
            m_unusedIndex.pop_back();
        }

        ITF_ASSERT_MSG(idx < m_texturePoolEntries.size(), "Out of texture descriptors. Check there is no leaks, and increase the initial descriptor heap size.");
        return idx;
    }

    void TextureDescriptorPool::releaseDescriptorIndex(ux idx)
    {
        ITF_ASSERT(idx < m_texturePoolEntries.size());

        if (idx >= m_texturePoolEntries.size())
            return;

        {
            ScopeLock lock(m_mutex);
            ITF_ASSERT(std::find(m_waitingUnregisteredIndex.begin(), m_waitingUnregisteredIndex.end(), idx) == m_waitingUnregisteredIndex.end());
            m_waitingUnregisteredIndex.push_back(idx);
            ITF_ASSERT(std::find(m_unusedIndex.begin(), m_unusedIndex.end(), idx) == m_unusedIndex.end());
        }
        TexturePoolEntry& entry = m_texturePoolEntries[idx];
        entry = {};
    }

    D3D12_CPU_DESCRIPTOR_HANDLE TextureDescriptorPool::getCPUDescriptorHandle(ux _descriptorIdx) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE result = m_cpuSRVHeapHandle;
        result.ptr += _descriptorIdx * m_srvDescriptorSize;
        return result;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE TextureDescriptorPool::getGPUDescriptorHandle(ux _descriptorIdx) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE result = m_gpuSRVHeapHandle;
        result.ptr += _descriptorIdx * m_srvDescriptorSize;
        return result;
    }

    TextureDescriptorPool::Handle TextureDescriptorPool::registerTexture(ID3D12Resource* res, const char * _debugName)
    {
        ITF_ASSERT(res != nullptr);
        if (res == nullptr)
            return {};

        auto texDesc = res->GetDesc();
        if (texDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || texDesc.DepthOrArraySize != 1)
        {
            ITF_ASSERT_MSG(0, "Not a valid simple 2D texture");
            return {};
        }

        ux textureIdxInPool = allocateDescriptorIndex();
        if (textureIdxInPool >= m_texturePoolEntries.size())
            return {}; // no more space in the pool

        TexturePoolEntry& entry = m_texturePoolEntries[textureIdxInPool];
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        entry.textureName = _debugName;
    #endif
        entry.lastSubmittedAtFrame = -1;

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        m_device->CreateShaderResourceView(res, &srvDesc, getCPUDescriptorHandle(textureIdxInPool));

        return Handle(this, textureIdxInPool);
    }

    void TextureDescriptorPool::recycleUnused(u64 _nbRenderedFrames)
    {
        ScopeLock lock(m_mutex);

        auto newEnd = std::remove_if(m_waitingUnregisteredIndex.begin(), m_waitingUnregisteredIndex.end(),
            [&](i32 idx)
            {
                TexturePoolEntry& entry = m_texturePoolEntries[idx];
                if (entry.lastSubmittedAtFrame < i64(_nbRenderedFrames))
                {
                    m_unusedIndex.push_back(idx);
                    return true;
                }
                return false;
            }
        );

        m_waitingUnregisteredIndex.erase(newEnd, m_waitingUnregisteredIndex.end());
    }

}
