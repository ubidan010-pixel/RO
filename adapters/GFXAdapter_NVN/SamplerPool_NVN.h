#pragma once

#include "NVN/Helpers.h"
#include "core/utility.h"

#include "engine/AdaptersInterfaces/GFXAdapter.h"

#include "VRAMAllocator/VRAMRelocHdl.h"

namespace ITF
{

    class NVNSamplerPool
    {
    public:

        explicit NVNSamplerPool(nvn::Device& _nvnDevice);
        ~NVNSamplerPool() = default;

        nvn::SeparateSamplerHandle getSamplerHandle(GFX_TEXADRESSMODE _clampX, GFX_TEXADRESSMODE _clampY, bool isBilinear) const
        {
            i32 samplerIndex = computeSamplerIndex(_clampX, _clampY, isBilinear);
            ITF_ASSERT_MSG(samplerIndex >= m_firstSamplerIndex && samplerIndex < m_firstSamplerIndex + NB_SAMPLERS,
                           "Sampler index out of bounds: %d", samplerIndex);
            return m_samplersDesc[samplerIndex - m_firstSamplerIndex].samplerHandle;
        }

        nvn::SamplerPool& getSamplerPool() // should be set in the command buffer
        {
            return *m_samplerPool;
        }

    private:

        void initializeSamplers(nvn::Device& _nvnDevice);
        i32 computeSamplerIndex(GFX_TEXADRESSMODE _clampX, GFX_TEXADRESSMODE _clampY, bool isBilinear) const
        {
            return m_firstSamplerIndex + (isBilinear ? 1 : 0) * GFXTADDRESS_COUNT * GFXTADDRESS_COUNT + (toUnderlying(_clampX) * GFXTADDRESS_COUNT) + toUnderlying(_clampY);
        }

        static const u32 NB_SAMPLERS = 2 * GFXTADDRESS_COUNT * GFXTADDRESS_COUNT;

        struct SamplerDesc
        {
            nvn::SamplerBuilder samplerBuilder;
            nvn::SeparateSamplerHandle samplerHandle;
        };
        SamplerDesc m_samplersDesc[NB_SAMPLERS];
        NVN::SimpleVRAMRelocHandle m_samplerMemory;
        NVN::UniquePtr<nvn::SamplerPool> m_samplerPool;
        i32 m_firstSamplerIndex = 0; // first sampler ID in the pool, should be 256 after initialization.
        // The first samplers are reserved for the system, so we can use the next ones.
    };
}
