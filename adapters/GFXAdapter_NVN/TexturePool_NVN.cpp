#include "precompiled_GFXAdapter_NVN.h"

#include "NVN/Helpers.h"
#include "Texture_NVN.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

#include "core/utility.h"

#include "core/system/Synchronize.h"

#include <numeric>

namespace ITF
{

    NVNTexturePool::NVNTexturePool(nvn::Device& _device, u32 nbMaxTexture)
        : m_nvnDevice(_device)
    {
        m_nbDriverReservedTexDescriptors = NVN::getDeviceProperty(_device, nvn::DeviceInfo::RESERVED_TEXTURE_DESCRIPTORS);
        ITF_ASSERT(nbMaxTexture + m_nbDriverReservedTexDescriptors < NVN::getDeviceProperty(_device, nvn::DeviceInfo::MAX_TEXTURE_POOL_SIZE));
        i32 texDescriptorSize = NVN::getDeviceProperty(_device, nvn::DeviceInfo::TEXTURE_DESCRIPTOR_SIZE);

        i32 nbTotalDescriptors = nbMaxTexture + m_nbDriverReservedTexDescriptors;
        i32 totalMemoryPoolSize = nbTotalDescriptors * texDescriptorSize;

        m_texPoolMemory.allocAligned(totalMemoryPoolSize, texDescriptorSize);
        m_nvnTexturePool = NVN::makeInitialized<nvn::TexturePool>(m_texPoolMemory.getStorage().memoryPool, m_texPoolMemory.getStorage().offset, nbTotalDescriptors);

        m_unusedIndex.resize(nbMaxTexture);
        // By putting all the indices in the unused list, it can be used at registering.
        // We put the indices in reverse order so that we can use push/back and start at the beginning of the vector.
        std::iota(std::make_reverse_iterator(m_unusedIndex.end()), std::make_reverse_iterator(m_unusedIndex.begin()), 0);

        // resize once for all the pool entries.
        m_texturePoolEntries.resize(nbMaxTexture);
    }


    NVNTexturePool::Handle NVNTexturePool::registerTexture(NVNTexture& _texture)
    {
        // locking all along the registration process as a first implementation.
        // We probably just need to lock when we access m_unusedIndex.
        ScopeLock lock(m_mutex); 

        i32 idx = -1;

        if (!m_unusedIndex.empty())
        {
            idx = m_unusedIndex.back();
            m_unusedIndex.pop_back();
        }

        ITF_ASSERT(idx!=-1);
        if (idx == -1) return {}; // no more space in the pool

        i32 textureIdxInPool = idx + m_nbDriverReservedTexDescriptors;
        m_nvnTexturePool->RegisterTexture(textureIdxInPool, &_texture.getPlatfomTexture(), nullptr);
        TexturePoolEntry & entry = m_texturePoolEntries[idx];
        entry.textureHandle = m_nvnDevice.GetSeparateTextureHandle(textureIdxInPool);
        entry.texture = &_texture; // $GB: todo remove
        return Handle(this, idx);
    }

    void NVNTexturePool::unRegisterIndex(i32 idx)
    {
        ITF_ASSERT(idx >= 0 && idx < i32(m_texturePoolEntries.size()));

        if (idx < 0 || idx >= i32(m_texturePoolEntries.size())) return;

        {
            ScopeLock lock(m_mutex);
            ITF_ASSERT(std::find(m_waitingUnregisteredIndex.begin(), m_waitingUnregisteredIndex.end(), idx) == m_waitingUnregisteredIndex.end());
            m_waitingUnregisteredIndex.push_back(idx);
            ITF_ASSERT(std::find(m_unusedIndex.begin(), m_unusedIndex.end(), idx) == m_unusedIndex.end());

            TexturePoolEntry& entry = m_texturePoolEntries[idx];
            entry.texture = nullptr;
            entry.textureHandle = nvn::SeparateTextureHandle{}; // reset the handle to avoid using it after unregistering.
        }
    }

    void NVNTexturePool::recycleUnused(u64 _nbRenderedFrames)
    {
        ScopeLock lock(m_mutex);

        auto newEnd = std::remove_if(m_waitingUnregisteredIndex.begin(), m_waitingUnregisteredIndex.end(),
            [&](i32 idx)
            {
                TexturePoolEntry& entry = m_texturePoolEntries[idx];
                if (entry.lastSubmittedAtFrame < _nbRenderedFrames)
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
