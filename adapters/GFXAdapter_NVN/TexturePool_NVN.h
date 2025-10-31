#pragma once

#include "NVN/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

#include "core/utility.h"

#include "core/system/Synchronize.h"

namespace ITF
{
    class NVNTexture;

    class NVNTexturePool
    {
    public:
        explicit NVNTexturePool(nvn::Device & _device, u32 nbMaxTexture = 10000);
        NVNTexturePool(const NVNTexturePool&) = delete;
        ~NVNTexturePool() = default;

        NVNTexturePool & operator = (const NVNTexturePool&) = delete;

        struct Handle;
        [[nodiscard]] Handle registerTexture(NVNTexture& _texture);

        [[nodiscard]] nvn::TexturePool& getTexturePool()
        {
            return *m_nvnTexturePool;
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
                other.m_idx = -1;
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
                    other.m_idx = -1;
                }
                return *this;
            }

            void reset()
            {
                if (m_texPool != nullptr && m_idx >= 0)
                {
                    m_texPool->unRegisterIndex(m_idx);
                    m_texPool = nullptr;
                    m_idx = -1;
                }
            }

            [[nodiscard]] nvn::SeparateTextureHandle getNVNHandle() const
            {
                ITF_ASSERT(m_texPool != nullptr && m_idx >= 0);
                ITF_ASSERT(m_idx < i32(m_texPool->m_texturePoolEntries.size()));
                return m_texPool->m_texturePoolEntries[m_idx].textureHandle;
            }

            void markAsUsedAtFrame(u64 _cpuFrameCount)
            {
                ITF_ASSERT(m_texPool != nullptr && m_idx >= 0);
                ITF_ASSERT(m_idx < i32(m_texPool->m_texturePoolEntries.size()));
                m_texPool->m_texturePoolEntries[m_idx].lastSubmittedAtFrame = _cpuFrameCount;
            }

        private:
            friend class NVNTexturePool;
            explicit Handle(NVNTexturePool* texPool, i32 _idx)
                : m_texPool(texPool)
                , m_idx(_idx)
            {}


            NVNTexturePool* m_texPool = nullptr;
            i32 m_idx = -1;
        };

    private:
        void unRegisterIndex(i32 idx);

        nvn::Device& m_nvnDevice;
        i32 m_nbDriverReservedTexDescriptors = 0;

        struct TexturePoolEntry
        {
            NVNTexture* texture = nullptr; // for debugging ? not really useful.
            i64 lastSubmittedAtFrame = -1;
            nvn::SeparateTextureHandle textureHandle{};
        };
        Vector<TexturePoolEntry> m_texturePoolEntries; // fixed size vector after init.

        NVN::SimpleVRAMRelocHandle m_texPoolMemory;
        NVN::UniquePtr<nvn::TexturePool> m_nvnTexturePool;

        // Registering/unregistering can be done in a loading thread.
        // The mutex protect the management of 
        Mutex m_mutex;
        Vector<i32> m_unusedIndex; // Used at registering to get an unused id. 
        Vector<i32> m_waitingUnregisteredIndex; // Filled at unregister with unRegisteredTextures idx.
        // At recycleUnused, the indices of m_waitingUnregisteredIndex that are no more used are sent to m_unusedIds.
    };
}
