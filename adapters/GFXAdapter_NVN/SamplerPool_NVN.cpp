#include "precompiled_GFXAdapter_NVN.h"

#include "NVN/Helpers.h"
#include "ITFToNVN_Helpers.h"

#include "SamplerPool_NVN.h"

#include "VRAMAllocator/VRAMAllocator.h"

namespace ITF
{

    NVNSamplerPool::NVNSamplerPool(nvn::Device& _nvnDevice)
    {
        m_firstSamplerIndex = NVN::getDeviceProperty(_nvnDevice, nvn::DeviceInfo::RESERVED_SAMPLER_DESCRIPTORS);
        i32 samplerDescSize = NVN::getDeviceProperty(_nvnDevice, nvn::DeviceInfo::SAMPLER_DESCRIPTOR_SIZE);

        i32 nbDescriptors = m_firstSamplerIndex + NB_SAMPLERS;

        m_samplerMemory.allocAligned(nbDescriptors * samplerDescSize, samplerDescSize);
        m_samplerPool = NVN::makeInitialized<nvn::SamplerPool>(m_samplerMemory.getStorage().memoryPool, m_samplerMemory.getStorage().offset, nbDescriptors);
        initializeSamplers(_nvnDevice);
    }

    static nvn::SamplerBuilder createSamplerBuilder(nvn::Device& _nvnDevice, GFX_TEXADRESSMODE _clampX, GFX_TEXADRESSMODE _clampY, bool _isBilinear)
    {
        nvn::SamplerBuilder samplerBuilder;
        samplerBuilder.SetDefaults()
            .SetDevice(&_nvnDevice)
            .SetMinMagFilter(itfToNVNMinFilter(_isBilinear), itfToNVNMagFilter(_isBilinear))
            .SetWrapMode(itfToNVN(_clampX), itfToNVN(_clampY), nvn::WrapMode::CLAMP);
        return samplerBuilder;
    }

    void NVNSamplerPool::initializeSamplers(nvn::Device& _nvnDevice)
    {
        for (i32 iClampX = 0; iClampX < GFXTADDRESS_COUNT; ++iClampX)
        {
            GFX_TEXADRESSMODE clampX = static_cast<GFX_TEXADRESSMODE>(iClampX);
            for (i32 iClampY = 0; iClampY < GFXTADDRESS_COUNT; ++iClampY)
            {
                GFX_TEXADRESSMODE clampY = static_cast<GFX_TEXADRESSMODE>(iClampY);

                auto registerSampler = [&](bool _isBilinear)
                {
                    i32 idx = computeSamplerIndex(clampX, clampY, _isBilinear);
                    SamplerDesc& currSampler = m_samplersDesc[idx - m_firstSamplerIndex];
                    currSampler.samplerBuilder = createSamplerBuilder(_nvnDevice, clampX, clampY, _isBilinear);
                    m_samplerPool->RegisterSamplerBuilder(idx, &currSampler.samplerBuilder);
                    currSampler.samplerHandle = _nvnDevice.GetSeparateSamplerHandle(idx);
                };
                registerSampler(false);
                registerSampler(true);
            }
        }
    }



}
