#include "precompiled_GFXAdapter_PS5.h"

#include "VRAMAllocatorBinSizes.h"

// This file is used to generate the bin sizes for the VRAM allocator.
#if 0

#include <agc/gnmp/constants.h>
#include <agc/gnmp/dataformats.h>
#include <agc/gnmp/texture.h>

#include "core/math/BitTweak.h"

namespace ITF::Gnmp::VRAMAllocator
{

    // code used to generate best bin size values according to textures sizes with their mipmaps
    using SetSize = std::set<ux>;

    ux computeSize(ux _w, ux _h, sce::Gnm::DataFormat _format)
    {
        sce::Gnm::TileMode tileMode = sce::Gnm::kTileModeDisplay_LinearAligned;
        sce::GpuAddress::computeSurfaceTileMode(&tileMode, sce::GpuAddress::kSurfaceTypeTextureFlat, _format, 1);

        sce::Gnm::Texture tex;
        sce::Gnm::SizeAlign textureSizeAlign = tex.initAs2d(_w, _h, 16, _format, tileMode, sce::Gnm::kNumSamples1);
        return textureSizeAlign.m_size;
    }

    SetSize getAllTexSize(ux _maxSize = 16*1024)
    {
        ux maxSizePow2 = BitTweak::GetCeilPow2(_maxSize);

        SetSize setOfSize;

        // DXT1
        for (ux i=1; i<maxSizePow2; i++)
        {
            for (ux j=1; j<maxSizePow2; j++)
            {
                setOfSize.insert(computeSize(1<<i, 1<<j, sce::Gnm::kDataFormatBc1UnormSrgb));
            }
        }

        // DXT5
        for (ux i=1; i<maxSizePow2; i++)
        {
            for (ux j=1; j<maxSizePow2; j++)
            {
                setOfSize.insert(computeSize(1<<i, 1<<j, sce::Gnm::kDataFormatBc3UnormSrgb));
            }
        }

        // RGBA8
        for (ux i=1; i<maxSizePow2; i++)
        {
            for (ux j=1; j<maxSizePow2; j++)
            {
                setOfSize.insert(computeSize(1<<i, 1<<j, sce::Gnm::kDataFormatBc3UnormSrgb));
            }
        }

        return std::move(setOfSize);
    }

    bbool printTexSizes()
    {
        for(auto size : getAllTexSize())
        {
            printf("%d,\n", size);
        }
        return btrue;
    }

    bbool g_printedTexSize = printTexSizes();

} // namespace ITF::Gnmp::VRAMAllocator
#endif
